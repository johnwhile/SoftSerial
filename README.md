# SoftSerial
Arduino SoftwareSerial in Full Duplex Rx/Tx

The code was needed to provide a second serial port to the **ArduinoNano** and then use the ***HardwareSerial0*** to communicate with the PC.
> [!IMPORTANT]
>  **Limitation:** Only one instance of ***SoftSerial*** can be created. It use Timer1 CompareA and CompareB interrupts to independently manage the write and read flow.

## Packet Functions
These functions create the byte data and the packet.
```
static byte getdata(ushort packet, const byte bits, const byte parity, bool &valid);
static ushort getpacket(const byte data, const byte bits, const byte parity, byte stop);
```
the streaming **packet** is the raw bits stream sent to interrupt functions. The most significant bit (MSB) must be always 1 and the least significant bit (LSB) must be always 0. For example for SERIAL_8E1. The packet maximum size is 16bit and can be modified at will. Maximum know config is SERIAL_8E2 withe a total of 12 bits

| stop | parity | b8 | b7 | b6 | b5 | b4 | b3 | b1 | start |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0 |

## FIFO Buffer
# SoftSerial
Arduino SoftwareSerial in Full Duplex Rx/Tx

The code was needed to provide a second serial port to the **ArduinoNano** and then use the ***HardwareSerial0*** to communicate with the PC.
> [!IMPORTANT]
>  **Limitation:** Only one instance of ***SoftSerial*** can be created. It use Timer1 CompareA and CompareB interrupts to independently manage the write and read flow.

## Packet Functions
These functions create the byte data and the packet.
```
static byte getdata(ushort packet, const byte bits, const byte parity, bool &valid);
static ushort getpacket(const byte data, const byte bits, const byte parity, byte stop);
```
the streaming **packet** is the raw bits stream sent to interrupt functions. The most significant bit (MSB) must be always 1 and the least significant bit (LSB) must be always 0. For example for SERIAL_8E1. The packet maximum size is 16bit and can be modified at will. Maximum know config is SERIAL_8E2 withe a total of 12 bits

| stop | parity | b8 | b7 | b6 | b5 | b4 | b3 | b1 | start |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0-1 | 0 |

## FIFO Buffer
```
template<class ETYPE> class Queue
```


