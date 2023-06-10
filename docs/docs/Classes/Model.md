# Model

## model:recognizer(nsamp, spkmdl)

Creates a new [Recognizer](./Recognizer.md) for language `model` with the specified sample rate

### Arguments
- `nsamp` - Recognizer sample rate
- `spkmdl` - [Speaker model](./SpeakerModel.md), will be ignored for batched models [optional]

### Returns
- [Recognizer](./Recognizer.md) object

### Possible errors
- Lua type error
- Failed to create a new [batched] recognizer

```lua
local recog = model:recognizer(16000, nil)
```

---

## model:find(word)

Check if a word can be recognized by the model

### Arguments
- `word` - Test word

### Returns
- The word number or `-1`

### Possible errors
- Lua type error

---

## model:wait()

Wait for the processing (only for batched models)

### Arguments
- None

### Returns
- Nothing
