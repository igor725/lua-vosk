local current_model = '../vosk-model-ru-0.42'
-- local current_model = '../vosk-model-small-en-us-0.15'
-- local current_model = '../vosk-model-small-ru-0.22'

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

local lib = ffi.load('sndfile')
local socket = require('socket.core')
local vosk = require('luavosk')

vosk.init({'../libvosk.dll', '../libvosk.so', 'vosk'})
vosk.loglevel(-999)

io.write('Loading vosk model...')
local model = vosk.model(current_model, false)
print('done!')

if ffi.string(lib.sf_version_string()) < 'libsndfile-1.0.29' then
	print('Warning! This version of libsndfile won\'t read ogg/opus!')
end

local sleep = socket.sleep
local clients = {}

local sv = socket.tcp()
sv:settimeout(0)
sv:bind('0.0.0.0', 8088)
sv:listen()

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

local function doThings(cl, data, size)
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
			error('Sound file must be mono')
		end

		local fsamp = ffi.new('short[128]')
		local recog = model:recognizer(sfi.samplerate)
		local recogtxt = ''
		local start = os.clock()
		local wait = os.clock()
		local res

		while true do
			local readed = lib.sf_read_short(sff, fsamp, 128)
			if readed == 0 then break end

			if recog:pushptr(fsamp, tonumber(readed) * 2, false) > 0 then
				res = recog:result()
				recogtxt = recogtxt .. res.text .. ' '
			end

			if os.clock() - wait > 1 then -- Не даём сопрограмме занимать поток дольше 1 секунды
				coroutine.yield()
				wait = os.clock()
			end
		end

		lib.sf_close(sff)
		res = recog:final()
		recogtxt = recogtxt .. res.text

		send(cl, '\r\n{"recognized": "')
		send(cl, recogtxt)
		send(cl, '", "model": "')
		send(cl, current_model)
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
	if reqline == nil then return end
	local met, path, httpver = reqline:match('^(%u+)%s(.-)%s(HTTP/%d%.%d)$')
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
			if path == '/model' then
				send(cl, ' 200 OK\r\nContent-Type: text/plain\r\n\r\n')
				send(cl, current_model)
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

				doThings(cl, data, size)
			end

			return
		end
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
				collectgarbage()
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
