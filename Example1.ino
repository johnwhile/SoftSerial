#include "Serial.h"

SoftSerial mSerial;


void setup() {
  mSerial.Begin(2, 3, 9600, SERIAL_8N1);
  Serial.begin(9600, SERIAL_8N1);
}


//white 127 bytes with a BUFFERSIZE or 64
void loop() {

  // optional: flush all wrinting byte
  while (mSerial.RequestToWrite())
    delay(mSerial.RequestToWrite() * mSerial.MilliSecondForByte);

  for (byte b = 0; b < 127; b++) {
    //optional: wait at least 10 free space in writing buffer
    while (mSerial.AvailableToWrite() < 10)
      delay(10 - mSerial.AvailableToWrite() * mSerial.MilliSecondForByte);

    mSerial.Write_Async(b);
  }
}