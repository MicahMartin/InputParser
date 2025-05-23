#pragma once
#include <cstdint>
#include "CommandCompiler.h"
#include "CircularBuffer.h"
#include "Input.h"

struct VCState {
  uint32_t currentState{ 0 }, prevState{ 0 };
  InputFrame inputBuff[MAX_HISTORY];
  int inputBuffNext;
};

class VirtualController {
public:
  VirtualController();
  VirtualController(VirtualController &&) = default;
  VirtualController(const VirtualController &) = default;
  VirtualController &operator=(VirtualController &&) = default;
  VirtualController &operator=(const VirtualController &) = default;
  ~VirtualController();

  void update(uint32_t input);
  bool checkCommand(int index, bool faceRight);

  VCState save();
  void load(VCState const& state);

  std::string printHistory();

private:
  bool isPressed(uint32_t input, bool strict = true);
  bool wasPressed(uint32_t input, bool strict = true, bool pressed = true, int offset = 0);
  bool wasPressedBuffer(uint32_t input, bool strict = true, bool pressed = true, int buffLen = 2);
  bool evalPrefix(const std::vector<CommandIns>& code, int &ip, int &frameOffset);

  uint32_t cleanSOCD(uint32_t input);
  bool strictMatch(uint32_t bitsToCheck, uint32_t query);
  int findMatchingFrame(uint32_t operand, bool strict, bool pressed, int startOffset, int buffLen = 16);

  CommandCompiler commandCompiler;

  // stateful
  CircularBuffer inputBuffer;
  uint32_t currentState{ 0 }, prevState{ 0 };
};
