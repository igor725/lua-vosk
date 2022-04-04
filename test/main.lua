vosk = require('vosk')
vosk.init(--[[ При отсутствии параметра, указывающего путь, используется libvosk.so, либо libvosk.dll]])
model = vosk.model('vosk-model-small-ru-0.22') -- Директория, в которой обитает нейромодель
print('Model loaded', model)
recog = model:recognizer(16000.0) -- Создаём распознаватель с указанным битрейтом
print('Recognizer created', recog)
local f = io.open('rec.wav', 'rb')
f:seek('set', 44)

local function txt(s)
	return s:match('^{.+"text".+:.+"(.*)".+}$')
end

local data 
repeat
	data = f:read(1024)
	if data and recog:push(data) then -- Отправляем данные в распознаватель
		local result = txt(recog:result()) -- Если recog:push() вернула true, читаем результат
		if result and #result > 0 then io.write(result, ' ') end
	end
until data

print(txt(recog:final())) -- 
