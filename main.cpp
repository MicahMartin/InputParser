#include "VirtualController.h"
#include <cstdint>

uint32_t fetchUserInput();

int main (int argc, char *argv[]) {
  VirtualController vc;
  // game loop
  while (true) {
    int userInput = fetchUserInput();
    vc.update(userInput);
    // do stuff!
  }
  
  return 0;
}
