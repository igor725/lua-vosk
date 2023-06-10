# Recognizer

## recog:setspk(spkmdl)

Adds speaker model to the Recognizer

```lua
recog:setspk(spkmdl)
```

### Arguments
- `spkmdl` - Speaker identification model ([SpeakerModel](./SpeakerModel.md))

### Returns
- Nothing

### Possible errors
- Lua type error

---

## recog:grammar(grm)

Configures Recognizer to use specified grammar

!!! note
	Using this function with a `grm` argument that is neither a table
	nor a JSON string will remove the previously set grammar

### Arguments
- `grm` - Your grammar data (table, JSON string)

```lua
recog:grammar({'word1', 'word2', 'word3', 'word...'})
-- or with JSON
recog:grammar('["word1", "word2", "word3", "word..."]')
```

### Returns
- Nothing

### Possible errors
- Invalid value (...) at index ... in the grammar table
- There is no such function (vlib.recog_setgrm) in the loaded vosk library

---

## recog:push(data, isfloat)

Pushes the specified data into the Recoginzer

!!! note
	If you use LuaJIT, you can pass FFI array pointer instead of Lua string:

	```lua
	local arrsize = 1024
	local data = require('ffi').new('char[?]', arrsize)

	-- .... read data here

	local retcode = recog:pushptr(
		data, arrsize, false
	) -- This function does the same thing as "push" but accepts pointers instead

	if retcode < 1 then
		-- Retry, maybe?
	end
	```

### Arguments
- `data` - audio data in PCM 16-bit (or 32-bit float) mono format (string)
- `isfloat` - this parameter must be set to "true" if your samples data is a float array (boolean)

```lua
local retcode = recog:push(wavchunk, false)
if retcode < 1 then
	-- Retry, maybe?
end
```

### Returns
- `1` on success, `0` on fail and `-1` on C++ exception (number)

### Possible errors
- Lua type error

---

## recog:alts(altcnt)

Configures Recognizer to output n-best results

### Arguments
- `altcnt` - the number of n-best results (number)

```lua
recog:alts(10)
```

### Returns
- Nothing

### Possible errors
- Lua type error

---

## recog:timings(tm, tm_part)

Enables or disables words with times in the output

### Arguments
- `tm` - Set timings state for the output (boolean)
- `tm_part` - Same but for partial results (boolean)

```lua
recog:timings(true, true)
```

### Returns
- Nothing

---

## recog:nlsml(state)

Enables NLSML output

```lua
recog:nlsml(true)
```

### Returns
- Nothing

### Possible errors
- There is no such function (vlib.recog_nlsml) in the loaded vosk library

---

## recog:result()

Returns speech recognition result

### Returns
- Recognition result (string)

---

## recog:partial()

Returns partial speech recognition text which is not yet finalized

### Returns
- Partial recognition text (string)

---

## recog:final()
Returns speech recognition result. Same as :result(), but doesn't wait
for silence. You usually call it in the end of the stream to get final
bits of audio. It flushes the feature pipeline, so all remaining audio
chunks got processed

### Returns
- Final result of recognition (string)

---

## recog:reset()

Resets current results so the recognition can continue from scratch

### Returns
- Nothing
