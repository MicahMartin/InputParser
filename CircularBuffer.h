#pragma once
#include <cstdio>
#include <cstdlib>
#include "Input.h"

class CircularBuffer {
public:
  CircularBuffer();

  void push(const InputFrame& elem);
  const InputFrame& front() const;
  InputFrame& operator[](int index);

  int next = 0;
  InputFrame buffer[MAX_HISTORY];

  ~CircularBuffer(){};

private:
};
