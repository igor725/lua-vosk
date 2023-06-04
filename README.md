# lua-vosk
This library uses [vosk-api](https://github.com/alphacep/vosk-api) to perform offline speech recognition.

## Dependencies
1. [LuaJIT](https://luajit.org/)/[Lua](https://lua.org/) >= 5.1
2. [Vosk library](https://github.com/alphacep/vosk-api/releases/latest)

## Building

Use CMake to build the project:

- On Windows:
```batch
mkdir build
cd build
cmake ..
msbuild luavosk.sln
```

- On Linux/WSL:
```bash
mkdir build
cd build
cmake ..
make -j4
```

## Getting started
This guide assumes that you have a compiled lua-vosk, vosk library and a [pre-trained model](https://alphacephei.com/vosk/models) in the Lua script's directory.

First of all, we need to load the library and initialize the disired language model:
```lua
local vosk = require('luavosk') -- Loads luavosk.so/luavosk.dll as Lua module
-- Returns: lua-vosk library table

--[[
	  This function loads the vosk main library (libvosk.so or libvosk.dll),
	  you can also specify the path to this library as a function argument.
	  Init MUST be called before any other call to the lua-vosk library.
]]
vosk.init()
-- Returns nothing
-- Possible errors:
--   * Failed to initialize vosk library: <library or function name> was not found

vosk.wavguess(
	file -- File opened using io.open(...)
) -- Reads a wav's RIFF header and sets the file cursor to the beginning of the samples data
-- Returns: sample rate and a boolean value determines whether the sample format is float or not
-- Possible errors:
--  * Lua type error
--  * Not a wave file
--  * WAVE id was not found
--  * No format sub-chunk found
--  * Invalid format sub-chunk size
--  * Unsupported data format
--  * Not a mono sound file
--  * Failed to determine sample rate
--  * Unexpected EOF
--  * Something wrong with wave extension block
--  * Failed to read chunk info

local model = vosk.model(
	  'vosk-model-small-ru-0.22', -- Path to the folder with the pre-trained model
	  false -- Use "true" to create a batched model (Not documented yet)
) -- Loads the specified model
-- Returns: Model object
-- Possible errors:
--  * Lua type error
--  * Please call vosk.init() first
--  * Failed to initialize a [batched] model

local spkmodel = vosk.spkmodel(
	'vosk-model-spk-0.4'
) -- Loads the specified speaker model
-- Returns: Speaker model object
-- Possible errors:
--  * Lua type error
--  * Please call vosk.init() first
--  * There is no such function (vlib.spkmodel_new) in the loaded vosk library
--  * Failed to initialize a speaker model

local recog = model:recognizer(
	  16000.0, -- Recognizer sample rate
	  spkmdl, -- Speaker model, will be ignored for batched models [optional]
) -- Creates a recognizer for language `model` with the specified sample rate
-- Returns: Recongizer object
-- Possible errors:
--  * Lua type error
--  * Failed to create a new [batched] recognizer

recog:setspk(
	spkmdl
) -- Adds speaker model to the Recognizer
-- Returns nothing
-- Possible errors:
--  * Lua type error

recog:grammar(
	{'word1', 'word2', 'word3', 'word...'}
	-- JSON array also can be used: '["word1", "word2", "word3", "word..."]'
) -- Configures Recognizer to use specified grammar
-- Returns nothing
-- Possible errors:
--  * Invalid value (...) at index ... in the grammar table
--  * There is no such function (vlib.recog_setgrm) in the loaded vosk library

local retcode = recog:push(
	'<audio data in PCM 16-bit mono format as a Lua string>',
	false -- This parameter must be set to "true" if your samples data is a float array
) -- Pushes the specified data into the Recoginzer
-- Returns: 1 on success, 0 on fail and -1 on C++ exception.
-- Possible errors:
--  * Lua type error

-- If you use LuaJIT, you can pass FFI array pointer instead of Lua string
local arrsize = 1024
local data = require('ffi').new('char[?]', arrsize)

-- .... read data here

local retcode = recog:pushptr(
	data, arrsize, false
) -- This function does the same thing as "push" but accepts pointers instead

recog:alts(
	10
) -- Configures Recognizer to output n-best results
-- Returns nothing
-- Possible errors:
--  * Lua type error

recog:timings(
	true, -- Enables words with times in the output
	true -- Same but for partial results
)
-- Returns nothing

recog:nlsml(
	true
) -- Enables NLSML output
-- Returns nothing
--  * There is no such function (vlib.recog_nlsml) in the loaded vosk library

recog:result()
-- Returns: speech recognition result

recog:partial()
-- Returns: partial speech recognition text which is not yet finalized

recog:final()
-- Returns: speech recognition result. Same as :result(), but doesn't wait
-- for silence. You usually call it in the end of the stream to get final
-- bits of audio. It flushes the feature pipeline, so all remaining audio
-- chunks got processed

recog:reset()
-- Resets current results so the recognition can continue from scratch
-- Returns nothing
```
