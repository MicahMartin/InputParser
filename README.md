# Virtual Controller Example

just a reference implementation for a modular virtual controller, suitable for use in a 2D / 3D fighting game engine with command sequences. 

Shows examples for:
- Evaluating user-defined command sequences (e.g. quarter circle forward) with a tiny DSL and some json
- storing inputs in a circular buffer + accessing it like any normal container
- Saving / loading virtual controller & input buffer state
- using bitfields & masks to handle inputs


```
// each 'commandString' is a descriptor for a sequence of bytecode instructions.
// P | ~P = ((wasPressed(LP)) || (wasReleased(LP)))
// @F & !D = ((wasPressed(F, strict = false)) && !(wasPressed(D)))
// MP & *D = ((wasPressed(MP)) && (isPressed(F)))
// DF = (wasPressed(DF))
// ~D = (wasReleased(D))

//  Forward, neutral, forward
//  "F, N, F",
//  back , neutral, back 
//  "B, N, B",
//  any down release, neutral, any down press, LP
//  "@~D, N, @D, LP",
//  any forward that doesnt include down, neutral, forward (lienent dash)
//  "@F & !D, N, F"
//  any back that doesnt include down, neutral, forward (lienent backdash)
//  "@B & !D, N, B",
//  release of down, downforward, anyforward that doesnt include down, lk or release of lk (236K)
//  "~D, DF, @F & !D, LK | ~LK",
//  release of down, downback, any back that doesnt include down, lp or release of lp (214P)
//  "~D, DB, @B & !D, LP | ~LP",
//  MP + forward IS pressed
//  "MP & *F",
//  MP + back IS pressed 
//  "MP & *B",
//
// input = N, F, B, U, D, UF, UB, DF, DB, LP, LK, MP, MK
// funcMods = ~, *, @
// unary = !
// binary = &, |
```

---
## 🔧 Components Overview

### 1. `VirtualController`

Handles:
- SOCD cleaning (Simultaneous Opposing Cardinal Directions)
- Tracking pressed & released inputs across frames
- Evaluating buffered commands against parsed DSL sequences

### 2. `CircularBuffer`

A simple fixed-size ring buffer used to hold a history of `InputFrame` objects.

### 3. `CommandCompiler` + `CommandScanner`

- Parses string-based commands into a custom bytecode
- Converts human-readable DSL input definitions into executable logic
- Supports operators like `&`, `|`, and modifiers like `@`, `~`, `*`, `!`

### 4. `CommandVm`

The virtual machine that:
- Executes command bytecode
- Supports logical conditions and temporal constraints
---

## 🕹 Input Encoding

Inputs are encoded as bitmasks:
- Directions: `UP`, `DOWN`, `LEFT`, `RIGHT`, and diagonals like `DF` (down-forward)
- Buttons: `LP`, `MP`, `HP`, `LK`, `MK`, `HK`
- Neutral: `N` (interpreted as "no direction")
- Special: `NOINPUT`, `START`, `SELECT`, etc.

`InputFrame` holds 3 fields per frame:

```cpp
struct InputFrame {
  uint32_t pressedBits;   // buttons that went down this frame
  uint32_t releasedBits;  // buttons that were released this frame
  uint32_t validBits;     // unused currently
};
```

P.S. I havent refacotred input de-validation yet! Some commands should mark their inputs as invalid when consumed. Imagine you input a command, and it bleeds into the next command. Im too lazy to think of a scenario rn but think of it like this: in sf4 if you did QCF -> FADC, if the QCF didn't invalidate itself in the input buffer, that QCF could bleed into your next commands. We dont want this, but sometimes we do! like imagine you're doing a micro dash into a special move, you wouldnt want the dash to invalidate it's forward inputs because you want to use it. 

