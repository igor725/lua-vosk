local vosk = require('vosk')
vosk.init(--[[ При отсутствии параметра, указывающего путь до библиотеки, используется libvosk.so, либо libvosk.dll]])
vosk.loglevel(0)
local model = vosk.model('vosk-model-small-ru-0.22', false) -- Директория, в которой обитает нейромодель
print('Model loaded', model)
local recog = model:recognizer(16000.0) -- Создаём распознаватель с указанным битрейтом
print('Recognizer created', recog)
-- Открываем файл, который хотим перевести в текст
local f = io.open('rec.wav', 'rb')
if not f then io.stderr:write('Failed to open rec.wav') return end
-- Пропускаем заголовок wav файла, после него начинаются семплы
f:seek('set', 44)

local function txt(s)
	return s:match('^{.+"text".+:.+"(.*)".+}$')
end

local data
repeat
	-- Читаем файл блоками по 1024 байта
	data = f:read(1024)
	if data and recog:push(data) > 0 then -- Отправляем данные в распознаватель
		local result = txt(recog:result()) -- Если recog:push() вернула true, читаем результат
		if result and #result > 0 then io.write(result, ' ') end -- Показываем пользователю результат
	end
until data == nil -- Повторяем, пока есть данные

print(txt(recog:final())) -- Показываем остатки текста, если они есть
