#include "VirtualController.h"
#include "CommandVm.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/types.h>

VirtualController::VirtualController(){
  commandCompiler.init("./char_def/commands.json");
};

VirtualController::~VirtualController(){};

void VirtualController::update(uint32_t input){
  prevState = currentState;
  currentState = cleanSOCD(input);

  const uint32_t prevButtons = prevState & Input::BTN_MASK;
  const uint32_t currButtons = currentState & Input::BTN_MASK;
  const uint32_t changedButtons = prevButtons ^ currButtons;

  const uint32_t prevStick = prevState & Input::DIR_MASK;
  const uint32_t currStick = currentState & Input::DIR_MASK;

  InputFrame currentFrame;
  currentFrame.pressedBits = changedButtons & currButtons;
  currentFrame.releasedBits = changedButtons & prevButtons;

  if (prevStick != currStick) {
    currentFrame.pressedBits  |= currStick == 0 ? Input::NOINPUT : currStick;
    currentFrame.releasedBits |= prevStick == 0 ? Input::NOINPUT : prevStick;
  }

  inputBuffer.push(currentFrame);
}

bool VirtualController::isPressed(uint32_t input, bool strict) {
  return strict ? strictMatch(currentState, input) : (currentState & input) != 0;
}

bool VirtualController::wasPressed(uint32_t input, bool strict, bool pressed, int offset) {
  if (offset >= MAX_HISTORY || offset < 0) return false;

  const InputFrame& currentFrame = inputBuffer[offset];
  const uint32_t targetMask = pressed ? currentFrame.pressedBits : currentFrame.releasedBits;
  return strict ? strictMatch(targetMask, input) : (targetMask & input) != 0;
}

bool VirtualController::wasPressedBuffer(uint32_t input, bool strict, bool pressed, int buffLen){
  for (int i = 0; i < buffLen; i++) {
    if (wasPressed(input,strict,pressed,i)) return true;
  }
  return false;
}

bool VirtualController::evalPrefix(const std::vector<CommandIns>& code, int &ip, int &frameOffset){
  const CommandIns& ins = code[ip++];
  uint32_t operand = ins.operand & OP_MASK;

  bool negated = (ins.operand & NOT_FLAG) != 0;
  bool any = (ins.operand & ANY_FLAG) != 0;

  bool val = false;
  switch (ins.opcode) {
    case OP_PRESS: {
      int matchedFrame = findMatchingFrame(operand, !any, true, frameOffset);
      val = (matchedFrame >= 0);
      if (val) frameOffset = matchedFrame;
      break;
    }
    case OP_RELEASE: {
      int matchedFrame = findMatchingFrame(operand, !any, false, frameOffset);
      val = (matchedFrame >= 0);
      if (val) frameOffset = matchedFrame;
      break;
    }
    case OP_HOLD: {
      val = isPressed(operand, !any);
      break;
    }
    case OP_AND: {
      bool left = evalPrefix(code, ip, frameOffset);
      if (!left) return false;
      bool right = evalPrefix(code, ip, frameOffset);
      val = left && right;
      break;
    }
    case OP_OR: {
      bool left  = evalPrefix(code, ip, frameOffset);
      bool right = evalPrefix(code, ip, frameOffset);
      val = left || right;
      break;
    }

    default:
      val = false;  // unknown opcode
      printf("wtF?\n");
      break;
  }
  return negated ? !val : val;
}

bool VirtualController::checkCommand(int index, bool faceRight) {
  const auto &code = commandCompiler.getCommand(index)->instructions;
  int frameOffset = 0;
  int ip = 0;

  // Evaluate *each* top‑level clause (comma‑separated) in turn
  // until we hit an implicit OP_END or run out of code.
  while (ip < (int)code.size() && code[ip].opcode != OP_END) {
    // remember: Frameoffset is being modified by evalprefix
    bool clause = evalPrefix(code, ip, frameOffset);
    if (!clause) 
    return false;
  }
  return true;
}

VCState VirtualController::save(){
  VCState state;

  state.currentState = currentState;
  state.prevState = prevState;
  state.inputBuffNext = inputBuffer.next;
  std::memcpy(state.inputBuff, inputBuffer.buffer, sizeof (state.inputBuff));
  
  return state;
}

void VirtualController::load(VCState const& state){

  currentState = state.currentState;
  prevState = state.prevState;
  inputBuffer.next = state.inputBuffNext;

  std::memcpy(inputBuffer.buffer, state.inputBuff, sizeof (inputBuffer.buffer));
}

std::string VirtualController::printHistory(){
  std::string retString;
  for (int i = 0; i < 8; i++) {
    retString += std::to_string(inputBuffer[i].pressedBits);
  }
  return retString;
}

uint32_t VirtualController::cleanSOCD(uint32_t input){
  const uint32_t horizontal = input & Input::HORIZONTAL_SOCD;
  if (horizontal == Input::HORIZONTAL_SOCD)
    input &= ~Input::HORIZONTAL_SOCD;

  const uint32_t vertical = input & Input::VERTICAL_SOCD;
  if (vertical == Input::VERTICAL_SOCD)
    input &= ~Input::VERTICAL_SOCD;

  return input;
}

bool VirtualController::strictMatch(uint32_t bitsToCheck, uint32_t query) {
  // Extract the directional and button parts from the query.
  const uint32_t queryDir = query & Input::DIR_MASK;
  const uint32_t queryBtn = query & Input::BTN_MASK;

  // Check directional bits if any were provided.
  bool dirMatch = (queryDir == 0) || ((bitsToCheck & Input::DIR_MASK) == queryDir);
  // Check button bits if any were provided.
  bool btnMatch = (queryBtn == 0) || ((bitsToCheck & Input::BTN_MASK) == queryBtn);

  return dirMatch && btnMatch;
}

int VirtualController::findMatchingFrame(uint32_t operand, bool strict, bool pressed, int startOffset, int buffLen){
  for (int i = startOffset; i < buffLen; ++i) {
    if (wasPressed(operand, strict, pressed, i)) 
      return i;
  }
  return -1;
}
