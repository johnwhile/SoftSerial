#pragma once
#include "Arduino.h"
#include "Common.h"
#include "Timer.h"

//static class, you can use only one instance
class SoftSerial {
private:
	sbyte tx, rx;
	uint ticks_correction = 64;
	uint us_correction = 6;

public:
	int MilliSecondForByte;


	SoftSerial() {
		tx = -1;
		rx = -1;
	}

	// Only write mode. If you prefer not to use timer1, modify the code yourself
	bool Begin(byte tx, uint baud = 9600, uint config = SERIAL_8N1) {

		this->tx = tx;
		pinMode(tx, OUTPUT);
		//uart set high voltage when idle
		digitalWrite(tx, HIGH);

		TIMER_SETUP();
		i_config.set(baud, config);
		i_ticks = i_config.ticks_per_bit - ticks_correction;
		i_tx = tx;
		i_packet_tx = 0;

		MilliSecondForByte = i_config.uS_per_bit * i_config.tot / 1000;

		return true;
	}

	// Since reading must use timer1, writing will also be enabled
	bool Begin(byte tx, byte rx, uint baud = 9600, uint config = SERIAL_8N1) {

		if (!Begin(tx, baud, config)) return false;

		this->rx = rx;
		pinMode(rx, INPUT);
		i_targhet_rx = 1 << (i_config.tot - 1);
		i_cursor_rx = 0;
		i_packet_rx = 0;
		i_rx = rx;
		attachInterrupt(digitalPinToInterrupt(rx), Interrupt_rx, FALLING);
		return true;
	}

	~SoftSerial() {
		End();
	}

	End() {
		noInterrupts();
		TIMER_STOP_RX();
		if (rx>-1) detachInterrupt(digitalPinToInterrupt(rx));
		interrupts();
		return true;
	}

	// Return the queue size of writing buffer.
	byte RequestToWrite() {
		return i_buffer_tx.size;
	}

	// Return the queue free space of writing buffer.
	byte AvailableToWrite() {
		return i_buffer_tx.capacity - i_buffer_tx.size;
	}

	// Return the queue size of reading buffer.
	// If the buffer will be not emptied, it will fill up until overflows, all new read data will be lost
	byte AvailableToRead() {
		return i_buffer_rx.size;
	}

	// Return and remove the First data from queue reading buffer and check if it's not corrupted
	// return zero if queue is empty
	byte Read(bool &valid) {
		valid = false;
		return AvailableToRead() ? getdata(i_buffer_rx.pop(), i_config.bits, i_config.parity, valid) : 0;	
	}

	// Return and remove the First data from queue reading buffer
	// return zero if queue is empty
	byte Read() {
		bool valid = false;
		return Read(valid);
	}

	// Non-blocking function but blocking until the previous data is written
	Write_WaitAsync(byte data) {
		//wait for the previous outgoing data to be sent
		while (i_packet_tx) {}

		noInterrupts();
		i_packet_tx = getpacket(data, i_config.bits, i_config.parity, i_config.stop);
		
		TIMER_BEGIN_TX();
		interrupts();
	}

	// Non-blocking function
	Write_Async(byte data) {

		ushort packet = getpacket(data, i_config.bits, i_config.parity, i_config.stop);

		noInterrupts();
		if (!i_buffer_tx.size) TIMER_BEGIN_TX(); //writing mode already started
		i_buffer_tx.push(packet);
		interrupts();
	}
	
	// blocking function, not use TIMER1 but delayMicroseconds(). 
	Write(byte data) {
		ushort packet = getpacket(data, i_config.bits, i_config.parity, i_config.stop);
		
		while (packet) {
			digitalWrite(tx, packet & 1);
			delayMicroseconds(i_config.uS_per_bit - us_correction);
			packet >>= 1;
		}
		digitalWrite(tx, HIGH);
		
	}
};
