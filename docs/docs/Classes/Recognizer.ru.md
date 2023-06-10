# Recognizer

## recog:setspk(spkmdl)

Добавляет модель распознавания говорящего к Распознователю

```lua
recog:setspk(spkmdl)
```

### Аргументы
- `spkmdl` - Модель распознования говорящего ([SpeakerModel](./SpeakerModel.md))

### Возвращает
- Ничего

### Возможные ошибки
- Ошибка Lua типов

---

## recog:grammar(grm)

Настраивает Распознователь на использование указанной грамматики

!!! note
	Вызов с аргументом `grm`, не являющимся таблицей или JSON строкой
	удалит предыдущую установленную грамматику

### Аргументы
- `grm` - Ваши данные грамматики (table, JSON string)

```lua
recog:grammar({'word1', 'word2', 'word3', 'word...'})
-- или через JSON
recog:grammar('["word1", "word2", "word3", "word..."]')
```

### Возвращает
- Ничего

### Возможные ошибки
- Invalid value (...) at index ... in the grammar table
- There is no such function (vlib.recog_setgrm) in the loaded vosk library

---

## recog:push(data, isfloat)

Отправляет переданные данные в Распознователь

!!! note
	Если вы используете LuaJIT, есть возможность передавать указатели на массивы вместо строк:

	```lua
	local arrsize = 1024
	local data = require('ffi').new('char[?]', arrsize)

	-- .... читаем данные здесь

	local retcode = recog:pushptr(
		data, arrsize, false
	) -- Эта функция делает то же самое, что и "push", но принимает только указатели

	if retcode < 1 then
		-- Обрабатываем ошибку
	end
	```

### Аргументы
- `data` - моно аудио данные в формате PCM 16-bit или 32-bit float (string)
- `isfloat` - этот параметр должен быть установлен на "true" если ваши данные в float формате (булево)

```lua
local retcode = recog:push(wavchunk, false)
if retcode < 1 then
	-- Обрабатываем ошибку
end
```

### Возвращает
- В случае успеха вернет `1`, иначе `0` или `-1` при C++ исключении (number)

### Возможные ошибки
- Ошибка Lua типов

---

## recog:alts(altcnt)

Configures Recognizer to output n-best results

### Аргументы
- `altcnt` - the number of n-best results (number)

```lua
recog:alts(10)
```

### Возвращает
- Ничего

### Возможные ошибки
- Ошибка Lua типов

---

## recog:timings(tm, tm_part)

Enables or disables words with times in the output

### Аргументы
- `tm` - Set timings state for the output (boolean)
- `tm_part` - Same but for partial results (boolean)

```lua
recog:timings(true, true)
```

### Возвращает
- Ничего

---

## recog:nlsml(state)

Включает/выключает вывод данных в формате NLSML

```lua
recog:nlsml(true)
```

### Возвращает
- Ничего

### Возможные ошибки
- There is no such function (vlib.recog_nlsml) in the loaded vosk library

---

## recog:result()

Returns speech recognition result

### Возвращает
- Результат распознования (string)

---

## recog:partial()

Returns partial speech recognition text which is not yet finalized

### Возвращает
- Частично распознанный текст (string)

---

## recog:final()
Returns speech recognition result. Same as :result(), but doesn't wait
for silence. You usually call it in the end of the stream to get final
bits of audio. It flushes the feature pipeline, so all remaining audio
chunks got processed

### Возвращает
- Финальный результат распознования (string)

---

## recog:reset()

Resets current results so the recognition can continue from scratch

### Возвращает
- Ничего
