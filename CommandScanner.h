#pragma once
#include "stdint.h"
#include <cstdlib>
#include <map>
#include <vector>
#include "Input.h"

enum CommandTokenType {
  CTOKEN_NEUTRAL,
  CTOKEN_FORWARD,
  CTOKEN_BACK,
  CTOKEN_UP,
  CTOKEN_DOWN,
  CTOKEN_UPFORWARD,
  CTOKEN_UPBACK,
  CTOKEN_DOWNFORWARD,
  CTOKEN_DOWNBACK,
  CTOKEN_LP,
  CTOKEN_LK,
  CTOKEN_MP,
  CTOKEN_MK,
  CTOKEN_NUMBER,

  CTOKEN_RELEASED,
  CTOKEN_HELD,

  CTOKEN_AND,
  CTOKEN_OR,

  CTOKEN_ANY,
  CTOKEN_NOT,
  CTOKEN_DELIM,
  CTOKEN_END,
};

struct CommandToken {
  CommandTokenType type;
  const char *start;
  uint8_t length;
};

static uint32_t parseInputMask(const CommandToken* token) {
  switch (token->type) {
    case CTOKEN_NEUTRAL: return Input::NOINPUT;
    case CTOKEN_FORWARD: return Input::RIGHT;
    case CTOKEN_DOWNFORWARD: return Input::DOWNRIGHT;
    case CTOKEN_UPFORWARD: return Input::UPRIGHT;
    case CTOKEN_BACK:    return Input::LEFT;
    case CTOKEN_DOWNBACK: return Input::DOWNLEFT;
    case CTOKEN_UPBACK: return Input::UPLEFT;
    case CTOKEN_UP:      return Input::UP;
    case CTOKEN_DOWN:    return Input::DOWN;
    case CTOKEN_LP:      return Input::LIGHT_P;
    case CTOKEN_LK:      return Input::LIGHT_K;
    case CTOKEN_MP:      return Input::MEDIUM_P;
    case CTOKEN_MK:      return Input::MEDIUM_K;
    default:             return 0;
  }
}

// Parses a number token into a uint32_t delay (number of frames).
static uint32_t parseNumber(const CommandToken* token) {
  return static_cast<uint32_t>(std::strtol(token->start, nullptr, 10));
}

class CommandScanner {
public:
  CommandScanner();
  ~CommandScanner();

  std::vector<CommandToken> scan(const char *inputString);

  CommandTokenType getInputType();
  CommandToken makeToken(CommandTokenType type);
  bool isAtEnd();
  char peek();
  char peekNext();
  char advance();
  bool match(char expected);
  void skipWhitespace();
  bool isAlpha(char c);
  bool isDigit(char c);
  CommandTokenType checkKeyword(int start, int end, const char *rest,
                                CommandTokenType type);

  std::map<CommandTokenType, const char *> tokenToString = {
      {CTOKEN_NEUTRAL, "NEUTRAL"},
      {CTOKEN_DOWN, "DOWN"},
      {CTOKEN_FORWARD, "FORWARD"},
      {CTOKEN_BACK, "BACK"},
      {CTOKEN_UP, "UP"},
      {CTOKEN_DOWNBACK, "DOWNBACK"},
      {CTOKEN_DOWNFORWARD, "DOWNFORWARD"},
      {CTOKEN_UPBACK, "UPBACK"},
      {CTOKEN_UPFORWARD, "UPFORWARD"},
      {CTOKEN_LP, "LP"},
      {CTOKEN_LK, "LK"},
      {CTOKEN_MP, "MP"},
      {CTOKEN_MK, "MK"},
      {CTOKEN_NUMBER, "NUMBER"},
      {CTOKEN_RELEASED, "RELEASED"},
      {CTOKEN_HELD, "HELD"},
      {CTOKEN_AND, "AND"},
      {CTOKEN_OR, "OR"},
      {CTOKEN_ANY, "ANY"},
      {CTOKEN_NOT, "NOT"},
      {CTOKEN_DELIM, "DELIM"},
      {CTOKEN_END, "END"},
  };

private:
  const char *scannerStart;
  const char *scannerCurrent;
};
