#line 1 "C:\\Users\\User1\\AppData\\Local\\Arduino15\\RemoteSketchbook\\ArduinoCloud\\112600715189585168977\\Uart\\Queue.h"
#pragma once
#include "Arduino.h"
#include "Common.h"


// FIFO list
template<class ETYPE> class Queue {

private:
  int front = 0;  //buffer[front] not contain data, front is next empty index
  int back = 0;   //buffer[back] contain data
  ETYPE* buffer = NULL;

public:
  int size = 0;
  int capacity = 0;
  

  Queue(int capacity) {
    this->capacity = capacity;
    buffer = (ETYPE*)malloc(capacity * sizeof(ETYPE));
  }

  ~Queue()
  {
    free(buffer);
  }

  // Add an element to the buffer
  bool push(ETYPE val) {
    if (full()) return false;
    
    buffer[front] = val;
    front = (front + 1) % capacity;
    size++;
    return true;
  }

  // Remove an element from the buffer
  ETYPE pop() {
    if (empty()) return 0;
    ETYPE val = buffer[back];
    back = (back + 1) % capacity;
    size--;
    return val;
  }

  bool empty() const {
    return size == 0;
  }

  bool full() const {
    return size == capacity;
  }
};