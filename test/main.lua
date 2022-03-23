vosk = require('vosk')
vosk.init('libvosk.dll')
model = vosk.model('vosk-model-small-ru-0.22')
print('Model loaded', model)
recog = model:recognizer(16000.0)
print('Recognizer created', recog)
local f = io.open('rec.wav', 'rb')
f:seek('set', 44)

while true do
	local data = f:read(1024)
	if not data then break end
	if recog:push(data) then
		local result = recog:result()
		result = result:match('^{.+"text".+:.+"(.+)".+}$')
		if result then io.write(result, ' ') end
	end
end
