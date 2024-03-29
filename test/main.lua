if package.cpath:find('%.dll') then
	os.execute('chcp 65001>nul')
	package.cpath = package.cpath .. ';../build/Debug/?.dll;../build/Release/?.dll'
else
	package.cpath = package.cpath .. ';../build/?.so'
end

local vosk = require('luavosk')
-- При отсутствии параметра, указывающего путь до библиотеки, используется libvosk.so, либо libvosk.dll
vosk.init({'../libvosk.dll', '../libvosk.so', 'libvosk.dll', 'libvosk.so', 'libvosk.so.1'})
vosk.loglevel(-999)
-- -- Открываем файл, который хотим перевести в текст
local f = io.open('rec.wav', 'rb')
if not f then io.stderr:write('Failed to open rec.wav') return end
local nsamp, isfloat = vosk.wavguess(f) -- Переносим курсор файла к данным, узнаём семпл рейт и формат данных
local mdinit_start = os.clock()
local model = vosk.model('vosk-model-small-ru-0.22', false) -- Директория, в которой обитает нейромодель
print('Transcription model:', model)
local spk = select(2, pcall(vosk.spkmodel, 'vosk-model-spk-0.4')) -- Модель для определения говорящего
print('Speaker model:', spk)
local recog = model:recognizer(nsamp, spk) -- Создаём распознаватель с указанным битрейтом
print('Recognizer:', recog)
print('* MDINIT time: ', os.clock() - mdinit_start)

local function txt(s)
	if vosk.hasjson then
		return s.text
	else
		return s:match('^{.+"text".+:.+"(.*)".+}$')
	end
end

local ts_start = os.clock()
local data
repeat
	-- Читаем файл блоками по 4096 байт
	data = f:read(4096)
	if data and recog:push(data, isfloat) > 0 then -- Отправляем данные в распознаватель
		local result = txt(recog:result()) -- Если recog:push() вернула true, читаем результат
		if result and #result > 0 then io.write(result, ' ') end -- Показываем пользователю результат
	end
until data == nil -- Повторяем, пока есть данные

print(txt(recog:final())) -- Показываем остатки текста, если они есть
print('* TS time: ', os.clock() - ts_start)
