#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(){
  for (auto i = 0; i < MAX_HISTORY; i++) {
    buffer[i] = InputFrame{};
  }
}

void CircularBuffer::push(const InputFrame& elem){
  buffer[next] = elem;
  next++;
  next = next % MAX_HISTORY;
}

const InputFrame& CircularBuffer::front() const {
  int i = (next == 0) ? MAX_HISTORY : next;
  return buffer[--i];
}

InputFrame& CircularBuffer::operator[](int index){
  if (index >= MAX_HISTORY) {
    printf("CircularBuffer Index out of bounds\n");
    exit(0);
  }
  int head = ((next == 0) ? MAX_HISTORY : next) - 1;
  if(head - index < 0){
    return buffer[MAX_HISTORY + (head-index)];
  }
  return buffer[head - index];
};
