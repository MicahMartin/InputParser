#pragma once
#include <cstdint>
#include <vector>
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
// TODO: unary and binary
//  "MP & *F",
//  "MP & *B",
//  "@F & !D, N, F"
//  "@B & !D, N, B",
//  "~D, DF, @F & !D, LK | ~LK",
//  "~D, DB, @B & !D, LP | ~LP",
//  TODO: load from file
//   "~D, 20DF, 20F, 8LP | 8~LP", // 214P
// First, we define our simple bytecode instruction set.
//
enum CommandOp : uint8_t {
  OP_PRESS,      // Check if a button is pressed (default)
  OP_RELEASE,    // Check if a button was released (modifier '~')
  OP_HOLD,       // Check if a button is held (modifier '*')
  OP_DELAY,      // Enforce a timing constraint (e.g., "8" frames)
  OP_AND,        // Logical AND operator
  OP_OR,         // Logical OR operator
  OP_END         // End of command marker
};

struct CommandIns {
  CommandOp opcode;
  uint32_t operand; // Represents an input bitmask or delay
};

// The compiled command, as a contiguous sequence of instructions.
struct CommandCode {
  std::vector<CommandIns> instructions;
  bool clears; // Indicates whether the command clears the input buffer upon execution.
};

// Modifier flag constants (pick bits that do not conflict with your input masks)
constexpr uint32_t ANY_FLAG = 0x80000000; // set by '@'
constexpr uint32_t NOT_FLAG = 0x40000000; // set by '!'
constexpr uint32_t OP_MASK = 0x3FFFFFFF;

