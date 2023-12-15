local modelspath = '..'
local loadedmodels = {}
local availmodels

if package.cpath:find('%.dll') then
	os.execute('chcp 65001>nul')
	package.cpath = package.cpath .. ';../../build/Debug/?.dll;../../build/Release/?.dll;../?.dll'
else
	package.cpath = package.cpath .. ';../../build/?.so;../?.so'
end

local ffi = require('ffi')
ffi.cdef[[
	typedef void *SNDFILE;
	typedef int64_t sf_count_t;

	typedef sf_count_t (*sf_vio_get_filelen)(void *ud);
	typedef sf_count_t (*sf_vio_seek)(sf_count_t offset, int whence, void *ud);
	typedef sf_count_t (*sf_vio_read)(void *ptr, sf_count_t count, void *ud);
	typedef sf_count_t (*sf_vio_write)(const void *ptr, sf_count_t count, void *ud);
	typedef sf_count_t (*sf_vio_tell)(void *ud);

	typedef struct {
		sf_vio_get_filelen get_filelen;
		sf_vio_seek seek;
		sf_vio_read read;
		sf_vio_write write;
		sf_vio_tell tell;
	} SF_VIRTUAL_IO;

	typedef struct {
		sf_count_t frames;
		int samplerate;
		int channels;
		int format;
		int sections;
		int seekable;
	} SF_INFO;

	const char *sf_version_string(void);
	SNDFILE *sf_open_virtual(SF_VIRTUAL_IO *sfv, int mode, SF_INFO *sfi, void *ud);
	sf_count_t sf_read_short(SNDFILE *sf, short *ptr, sf_count_t items);
	int sf_close(SNDFILE *sf);
]]

local lib
	do
		local succ

		succ, lib = pcall(ffi.load, 'libsndfile.so.1')
		if not succ then
			lib = ffi.load('sndfile')
		end
	end
local socket = require('socket.core')
local vosk = require('luavosk')
vosk.init({'../libvosk.dll', '../libvosk.so', 'libvosk.dll', 'libvosk.so', 'libvosk.so.1'})
vosk.loglevel(-999)

if ffi.string(lib.sf_version_string()) < 'libsndfile-1.0.29' then
	print('Warning! This version of libsndfile won\'t read ogg/opus!')
end

local sleep = socket.sleep
local clients = {}

local sv = socket.tcp()
sv:settimeout(0)
sv:bind('0.0.0.0', 8088)
sv:listen()

local function scanModels()
	local mt = {}

	if jit.os == 'Windows' then
		local p = io.popen('dir /b /ad "' .. modelspath .. '"', 'r')

		if p ~= nil then
			for li in p:lines() do
				local mdpath = modelspath .. '/' .. li
				local f = io.open(mdpath .. '/am/final.mdl', 'r')
				if f ~= nil then
					table.insert(mt, mdpath)
					f:close()
				end
			end
			p:close()
		end
	else
		local p = io.popen('find ' .. modelspath .. ' -type d -exec test -f {}/am/final.mdl \\; -print', 'r')

		if p ~= nil then
			for li in p:lines() do
				table.insert(mt, li)
			end
			p:close()
		end
	end

	return mt
end

availmodels = scanModels()

local function recvLine(cl)
	local data = ''
	while true do
		local recv, err, part = cl:receive('*l')
		if err == 'closed' then
			return error('Client closed the connection')
		elseif err == 'timeout' then
			coroutine.yield()
		elseif err ~= nil then
			return error('Socket error: ' .. err)
		end

		if part ~= nil and #part > 0 then
			data = data .. part
		elseif recv ~= nil then
			return data .. recv
		end
	end
end

local function send(cl, data)
	local mlen = #data
	local pos, err

	while true do
		local npos, sent
		npos, err, sent = cl:send(data, pos)
		if err ~= nil and err ~= 'timeout' then
			break
		elseif npos == mlen then
			return true
		elseif npos == nil or npos < mlen and err == 'timeout' then
			pos = sent + 1
			coroutine.yield()
		end
	end

	return false
end

local function doThings(cl, data, size, mdi)
	local pos = 0

	local funcs = {
		ffi.cast('sf_vio_get_filelen', function() return size end),
		ffi.cast('sf_vio_seek', function(offset, whence)
			if whence == 0 then
				pos = tonumber(offset) or 0
			elseif whence == 1 then
				pos = pos + (tonumber(offset) or 0)
			elseif whence == 2 then
				pos = size - (tonumber(offset) or 0)
			else
				return 1
			end

			return 0
		end),
		ffi.cast('sf_vio_read', function(ptr, cnt)
			if pos + tonumber(cnt) > size then
				cnt = size - pos
			end

			ffi.copy(ptr, data + pos, cnt)
			pos = pos + cnt

			return cnt
		end),
		ffi.cast('sf_vio_write', function() return 0 end),
		ffi.cast('sf_vio_tell', function() return pos end)
	}

	local sfv = ffi.new('SF_VIRTUAL_IO', funcs)
	local sfi = ffi.new('SF_INFO', {0, 0, 0, 0, 0})
	local sff = lib.sf_open_virtual(sfv, 0x10 --[[SFM_READ]], sfi, nil)

	if sff ~= nil then
		if sfi.channels > 1 then
			send(cl, '\r\n{"error": "Sound file must be mono"}')
			return
		end

		local model = loadedmodels[mdi]
		if model == nil then
			local succ, md = pcall(vosk.model, availmodels[mdi], false)
			if succ and md ~= nil then
				loadedmodels[mdi] = md
				model = md
			else
				send(cl, '\r\n{"error": "Failed to load specified vosk model"}')
				return
			end
		end
		local fsamp = ffi.new('short[128]')
		local recog = model:recognizer(sfi.samplerate)
		local recogtxt = ''
		local start = os.clock()
		local wait = start
		local res

		while true do
			local readed = lib.sf_read_short(sff, fsamp, 128)
			if readed == 0 then break end

			if recog:pushptr(fsamp, tonumber(readed) * 2, false) > 0 then
				res = recog:result()
				recogtxt = recogtxt .. res.text .. ' '
			end

			local curr = os.clock()
			if curr - wait > 1 then -- Не даём сопрограмме занимать поток дольше 1 секунды
				coroutine.yield()
				wait = curr
			end
		end

		lib.sf_close(sff)
		res = recog:final()
		recogtxt = recogtxt .. res.text

		send(cl, '\r\n{"recognized": "')
		send(cl, recogtxt)
		send(cl, '", "time": ')
		send(cl, ('%.3f'):format(os.clock() - start))
		send(cl, '}')
	end

	for i = 1, #funcs do
		funcs[i]:free()
	end

	if sff == nil then
		send(cl, ('Content-Length: 33\r\n\r\n{"error": "Transcription failed"}'))
	end
end

local function processClient(cl)
	local reqline = recvLine(cl)
	if reqline == nil or #reqline > 512 then
		send(cl, 'HTTP/1.1 400 Bad Request\r\n\r\nRequest exceeded the maximum size')
		return
	end
	local met, path, httpver = reqline:match('^(%u+)%s(.-)%s(HTTP/%d%.%d)$')
	if met == nil then
		send(cl, 'HTTP/1.1 400 Bad Request\r\n\r\nRequest is malformed')
		return
	end
	if httpver < 'HTTP/1.0' and httpver >= 'HTTP/2.0' then return end
	local headers = {}

	while true do
		local htxt = recvLine(cl)
		if htxt == '' or htxt == nil then break end
		key, value = htxt:match('^(.+): (.+)$')
		headers[key:lower()] = tonumber(value) or value
	end

	send(cl, httpver)

	if met == 'GET' then
		if path:byte(1, 1) == 47 then
			if path == '/models' then
				send(cl, ' 200 OK\r\nContent-Type: application/json\r\n\r\n[')
				for i = 1, #availmodels do
					if i > 1 then send(cl, ', ') end
					local mdl = availmodels[i]
					local ens = tostring(loadedmodels[i] ~= nil)
					send(cl, '["' .. mdl .. '", ' .. ens .. ']')
				end
				send(cl, ']')
				return
			end

			if path == '/' then
				path = path .. 'index.html'
			end

			local f = io.open('./dist/' .. path, 'rb')

			if f ~= nil then
				send(cl, (' 200 OK\r\nContent-Type: %s\r\n\r\n'):format(
					path:find('%.js$') ~= nil and 'text/javascript' or 'text/html'
				))
				send(cl, f:read('*a'))
				f:close()
				return
			end
		end
	elseif met == 'POST' then
		if path == '/rec' then
			send(cl, ' 200 OK\r\nContent-Type: application/json\r\n')

			local mdi = headers['model']

			if type(mdi) ~= 'number' then
				send(cl, ('Content-Length: 34\r\n\r\n{"error": "No model header found"}'))
				return
			elseif mdi < 0 or mdi >= #availmodels then
				send(cl, ('Content-Length: 32\r\n\r\n{"error": "Invalid model index"}'))
				return
			end

			local size = headers['content-length']
			if size ~= nil and size > 0 then
				if size > 6 * 1024 * 1024 then
					send(cl, ('Content-Length: 34\r\n\r\n{"error": "Recording is too long"}'))
					return
				end

				local data = ffi.new('char[?]', size)
				local dleft = size
				while dleft > 0 do
					local drecv, err, dpart = cl:receive(dleft)

					if err == 'timeout' then
						coroutine.yield()
						drecv = dpart
					elseif err ~= nil then
						error('Socket error: ' .. err)
					end

					ffi.copy(data + (size - dleft), drecv, #drecv)
					dleft = dleft - #drecv
				end

				doThings(cl, data, size, mdi + 1)
			end

			collectgarbage()
			return
		end
	elseif met == 'PUT' then
		local model = headers['model']

		if type(model) == 'number' and model >= 0 and model <= #availmodels then
			model = model + 1
			local mdname = availmodels[model]

			if path == '/on' then
				if loadedmodels[model] == nil then
					local succ, md = pcall(vosk.model, mdname, false)
					if succ then loadedmodels[model] = md end
					send(cl, ' 200 OK\r\nContent-Type: application/json\r\n\r\n{"ok": ')
					send(cl, tostring(succ))
					if not succ then
						send(cl, ', "error": "')
						send(cl, tostring(md))
						send(cl, '"')
					end
					send(cl, '}')
					collectgarbage()
					return
				end
			elseif path == '/off' then
				if loadedmodels[model] ~= nil then
					loadedmodels[model] = nil
					send(cl, ' 200 OK\r\nContent-Type: application/json\r\n\r\n{"ok": true}')
					collectgarbage()
					return
				end
			end

			collectgarbage()
		end
	else
		send(cl, ' 400 Bad Request\r\n\r\nBad request!')
		return
	end

	send(cl, ' 404 Not Found\r\n\r\nNot found: ')
	send(cl, reqline)
end
jit.off(doThings, true)

print('Running HTTP server on http://127.0.0.1:8088/')
while true do
	do
		local cl = sv:accept()
		if cl ~= nil then
			cl:settimeout(0)
			table.insert(clients, coroutine.create(function()
				local succ, err = pcall(processClient, cl)
				if not succ then
					send(cl, '\r\n' .. tostring(err))
				end
				cl:close()
			end))
		end
	end

	for i = #clients, 1, -1 do
		local co = clients[i]
		local res, err = coroutine.resume(co)
		if not res or coroutine.status(co) == 'dead' then
			if err then print(err) end
			table.remove(clients, i)
		end
	end

	sleep(0.01)
end
