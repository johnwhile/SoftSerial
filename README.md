# SoftSerial
Arduino SoftwareSerial in Full Duplex Rx/Tx

The code was needed to provide a second serial port to the **ArduinoNano** and then use the ***HardwareSerial0*** to communicate with the PC.
> **Limitation:** Only one instance of ***SoftSerial*** can be created.
Use Timer1 CompareA and CompareB interrupts to independently manage the write and read flow.
