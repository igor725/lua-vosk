# lua-vosk
This library uses [vosk-api](https://github.com/alphacep/vosk-api) to perform offline speech recognition.

## Dependencies
1. [LuaJIT](https://luajit.org/)/[Lua](https://lua.org/) >= 5.1
2. [Vosk library](https://github.com/alphacep/vosk-api/releases/latest)

## Building

- On Windows: Run `./build.bat` in Microsoft Visual Studio Developer Command Prompt
- On Linux: Run `./build.sh`

## Getting started
This guide assumes that you have a compiled lua-vosk, vosk library and a [pre-trained model](https://alphacephei.com/vosk/models) in the Lua script's root directory.

First of all, we need to load the library and initialize the disired language model:
```lua
local vosk = require('vosk') -- Loads vosk.so/vosk.dll as Lua module
-- Returns: lua-vosk library table
--[[
	  This function loads the vosk main library (libvosk.so or libvosk.dll),
	  you can also specify the path to this library as a function argument.
	  Init MUST be called before any other call to the lua-vosk library.
]]
vosk.init()
-- Returns nothing or propagates an Lua error

local model = vosk.model(
	  'vosk-model-small-ru-0.22', -- Path to the folder with the pre-trained model
	  false -- Use "true" to create a batched model (Not documented yet)
) -- Loads the specified model
-- Returns: Model object

local recog = model:recognizer(
	  16000.0 -- Recognizer sample rate
) -- Creates a recognizer for language `model` with the specified sample rate
-- Returns: Recongizer object

local retcode = recog:push(
	'<audio data in PCM 16-bit mono format as Lua string>',
	false -- This parameter must be set to "true" if your samples data is a float array
) -- Pushes the specified data into the Recoginzer
-- Returns: 1 on success, 0 on fail and -1 on C++ exception.

-- If you use LuaJIT, you can pass FFI array pointer instead of Lua string
local arrsize = 1024
local data = require('ffi').new('char[?]', arrsize)

-- .... read data here

local retcode = recog:pushptr(
	data, arrsize, false
) -- This function does the same thing as "push" but accepts pointers instead

recog:setspk(
	spk
) -- Adds speaker model to the Recognizer
-- Returns nothing

recog:alts(
	10
) -- Configures Recognizer to output n-best results
-- Returns nothing

recog:grammar(
	'["word1", "word2", "word3", "word..."]'
) -- Configures Recognizer to use grammar

recog:timings(
	true, -- Enables words with times in the output
	true -- Same but for partial results
)
-- Returns nothing

recog:nlsml(
	true
) -- Enables NLSML output
-- Returns nothing

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
