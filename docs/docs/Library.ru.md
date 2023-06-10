# Таблица библиотеки

!!! note
	Вам может быть интересен [пример использования библиотеки](https://github.com/igor725/lua-vosk/blob/main/test/main.lua)
	находящийся в репозитории библиотеки. Для запуска этого примера вам понадобится [скомпилированный](./index.md#building) lua-vosk, сама библиотека Vosk
	и [заранее натренированная языковая модель](https://alphacephei.com/vosk/models) всё это должно находиться в папке, из которой запускается скрипт.


```lua
local vosk = require('luavosk') -- Загружает luavosk.so/luavosk.dll как Lua модуль
```

## vosk.init(path)

Эта функция подключает саму библиотеку Vosk (libvosk.so или libvosk.dll).
Функция Init должна быть вызвана ПЕРЕД любым другим вызовам к функциям библиотеки.

### Аргументы
- `path` - Путь к файлу `libvosk.[so/dll]` [опционально]

### Возвращает
- Ничего

### Возможные ошибки
- Failed to initialize vosk library: `<library or function name>` was not found

---

## vosk.wavguess(file)

Читает заголовок RIFF файла и устанавливает курсор на чанк с данными

```lua
local file = io.open('voice.wav', 'rb')
if file ~= nil then
	local samprate, isfloat = vosk.wavgues(file)
	-- ...
end
```

### Аргументы
- file - Файл, открытый с помощью [io.open(...)](https://lua.org.ru/contents_ru.html#pdf-io.open)

### Возвращает
- Частота звукового файла
- Булево значение, определяеющее формат семплов - плавающие или целые.

### Возможные ошибки
- Ошибка Lua типов
- Not a wave file
- WAVE id was not found
- No format sub-chunk found
- Invalid format sub-chunk size
- Unsupported data format
- Not a mono sound file
- Failed to determine sample rate
- Unexpected EOF
- Something wrong with wave extension block
- Failed to read chunk info

---

## vosk.model(path, batched)

Загружает указанную модель в память

```lua
local model = vosk.model('vosk-model-small-ru-0.22', false)
```

### Аргументы
- `path` - Путь до папки, содержащей модель
- `batched` - Укажите "true" для того чтобы создать batched модель (Пока незадокументировано)

### Возвращает
- Объект [Model](./Classes/Model.md)

### Возможные ошибки
- Ошибка Lua типов
- Please call vosk.init() first
- Failed to initialize a [batched] model

---

## vosk.spkmodel(path)

Loads the specified speaker model to memory

```lua
local spkmodel = vosk.spkmodel('vosk-model-spk-0.4')
```

### Аргументы
- `path` - Path to the folder with the pre-trained model

### Возвращает
- Объект [SpeakerModel](./Classes/SpeakerModel.md)

### Возможные ошибки
- Ошибка Lua типов
- Please call vosk.init() first
- There is no such function (vlib.spkmodel_new) in the loaded vosk library
- Failed to initialize a speaker model
