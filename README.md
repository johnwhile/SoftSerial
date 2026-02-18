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

## FIFO Buffers
```
static volatile Queue<ushort> i_buffer_tx(BUFFERSIZE);
static volatile Queue<ushort> i_buffer_rx(BUFFERSIZE);
```
Since writes and reads are not synchronized with the code of loop(), to avoid overflows check buffers size with
```
byte RequestToWrite();
byte AvailableToWrite();
byte AvailableToRead();
```
## Write

There are 3 ways to write a byte and they must not be used simultaneously.
1. This function does not use the timer interrupts but a simple delay. It's blocking code, so the code inside the loop will resume when the byte is completely written. For example, for a SERIAL_8N1 and baud rate 9600 = 10 bits * 104.17uS = 1.042mS
```
void loop() {
  Write(byte data) //exist after 1.04mS
}
```
2. This function use timer interrupts, so the code inside the loop can continue in a nearly-asynchronous way, minus some few cycles used by the interrupt function.
However since it doesn't use the write buffer the function will block the code inside loop() until the previous byte is written.
```
void loop() {
  Write_WaitAsync(byte data); //exist after some micro seconds
}
```
3. Same function of 2. but use a write buffer. The bytes will be pusched in the buffer and consumed over time. Useful for writing many bytes without slowing down the code inside loop().
   It is necessary to control the overflow when the byte insertion speed is faster than the output flow.
```
void loop() {
    //exist after some micro seconds
    for (byte data = 0; data < BUFFERSIZE; data++)
    {
      while(!AvailableToWrite());  //check overflow
      Write_Async(byte data)
    }
}
```


