# Library table

!!! note
	You might be interested in the [use case of this library](https://github.com/igor725/lua-vosk/blob/main/test/main.lua)
	provided in my GitHub repository. This example assumes that you have a [compiled](./index.md#building) lua-vosk, Vosk
	library and a [pre-trained model](https://alphacephei.com/vosk/models) in the Lua script's directory. Note that the
	comments in this script are written in Russian because I'm lazy pos and I simply don't want to translate it.

```lua
local vosk = require('luavosk') -- Loads luavosk.so/luavosk.dll as Lua module
```

## vosk.init(path)

This function loads the vosk main library (libvosk.so or libvosk.dll).
Init MUST be called before any other call to the lua-vosk library.

### Arguments
- `path` - Path to the `libvosk.[so/dll]` file [optional]

### Returns
- Nothing

### Possible errors
- Failed to initialize vosk library: `<library or function name>` was not found

---

## vosk.wavguess(file)

Reads a wav's RIFF header and sets the file cursor to the beginning of the samples data

```lua
local file = io.open('voice.wav', 'rb')
if file ~= nil then
	local samprate, isfloat = vosk.wavgues(file)
	-- ...
end
```

### Arguments
- file - File opened using [io.open(...)](https://www.lua.org/pil/21.2.html)

### Returns
- Sample rate of a wav file
- Boolean value determines whether the sample format is float or not

### Possible errors
- Lua type error
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

Loads the specified model to memory

```lua
local model = vosk.model('vosk-model-small-en-us-0.15', false)
```

### Arguments
- `path` - Path to the folder with the pre-trained model
- `batched` - Use "true" to create a batched model (Not documented yet)

### Returns
- [Model](./Classes/Model.md) object

### Possible errors
- Lua type error
- Please call vosk.init() first
- Failed to initialize a [batched] model

---

## vosk.spkmodel(path)

Loads the specified speaker model to memory

```lua
local spkmodel = vosk.spkmodel('vosk-model-spk-0.4')
```

### Arguments
- `path` - Path to the folder with the pre-trained model

### Returns
- [SpeakerModel](./Classes/SpeakerModel.md) object

### Possible errors
- Lua type error
- Please call vosk.init() first
- There is no such function (vlib.spkmodel_new) in the loaded vosk library
- Failed to initialize a speaker model
