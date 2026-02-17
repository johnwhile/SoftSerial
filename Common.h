#pragma once
#include <arduino.h>
#include <stdint.h>

#define DebugOutA 13
#define DebugOutB 12

#define int int32_t
#define uint uint32_t
#define short int16_t
#define ushort uint16_t
#define sbyte int8_t

//01010... output
const ushort DEBUG_STREAM_8N1 = 0x2AA;
const byte DEBUF_DATA_8N1 = 0x55;

 struct SerialConfig {
  uint uS_per_bit = 1000000 / 9600;
  uint ticks_per_bit = F_CPU / 9600;

  //max 8
  byte bits = 8;
  //0 none, 2 even, 3 odd
  byte parity = 0;
  //1 or 2
  byte stop = 1;
  // total bit on packet stream
  byte tot = 10;
  
  void set(uint baud = 9600, uint config = SERIAL_8N1)
  {
    uS_per_bit = 1000000 / baud;   // 104.1667 uS per bit
    ticks_per_bit = F_CPU / baud;  // cycle per bit = 1666.66667 for 16Mhz, truncate at 1666
    config >>= 1;
    bits = (config & 3) + 5;
    config >>= 2;
    stop = (config & 1) + 1;
    config >>= 1;
    parity = config & 3;
    tot = 1 + bits + stop + (parity ? 1 : 0);  //1 for start bit0
  }

  SerialConfig(uint baud = 9600, uint config = SERIAL_8N1) {
    set(baud, config);
  }
};


static inline bool
is_even_parity(byte data) {
  data ^= data >> 4;
  data ^= data >> 2;
  data ^= data >> 1;
  return data & 1;
}

static ushort invert_bit_order(ushort stream, byte size) {
  if (size > 16) size = 16;
  ushort inverted = 0;
  while (size--) {
    inverted |= stream & 1;
    stream >>= 1;
    inverted <<= 1;
  }
  return inverted;
}

//Get byte data from a compatible packet data
static byte getdata(ushort packet, const byte bits, const byte parity, bool &valid) {
  // remove stop bit0
  packet >>= 1;
  ushort flag = 1 << bits;
  byte data = packet & (flag - 1);

  valid = packet & flag;
  switch (parity) {
      // None
    case 0:
      break;
      // Even
    case 2:
      valid &= !is_even_parity(data);
      break;
      // Odd
    case 3:
      valid &= is_even_parity(data);
      break;
  }
  return data;
}

//create the packet data to send of
static ushort getpacket(const byte data, const byte bits, const byte parity, byte stop) {
  ushort packet = data;
  ushort flag = 1 << bits;
  //apply mask to fix the byte
  packet &= (flag - 1);

  switch (parity) {
      //None
    case 0:
      break;

      //Even
    case 2:
      if (!is_even_parity(packet)) packet |= flag;
      flag <<= 1;
      break;

      //Odd
    case 3:
      if (is_even_parity(packet)) packet |= flag;
      flag <<= 1;
      break;
  }

  //add stop bit 1
  do {
    packet |= flag;
    flag <<= 1;

  } while (--stop);

  //add start bit 0
  packet <<= 1;
  return packet;
}
