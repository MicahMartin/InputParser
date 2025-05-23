#pragma once
#include <string>
#include <vector>
#include "CommandScanner.h"
#include "CommandVm.h"

struct CommandStringObj {
  std::string command;
  bool clears;
};

struct CommandJson {
  std::string command;
  std::string name;
  bool clears;
  int num;
};

struct RootJson {
  std::vector<CommandJson> commands;
};

class CommandCompiler {
public:
  CommandCompiler();
  ~CommandCompiler();

  void init(const char* path);
  const CommandCode* getCommand(int index) const;
  std::string opcodeToString(CommandOp opcode);

private:
  void compile(const char* inputString, bool clears);
  CommandCode compileNode();
  void printCode(const CommandCode& code);

  std::vector<CommandCode> commands;
  CommandScanner commandScanner;
  CommandToken* currentToken;
};
