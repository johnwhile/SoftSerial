#pragma once
#include "Arduino.h"
#include "Common.h"
#include "Queue.h"

//buffer size for read and write queues 
#define BUFFERSIZE 64

//cycle for each bit at specified baud rate
static volatile ushort i_ticks = 1666;
//can be any digital output
static volatile byte i_tx = 2;
//require digital pins With Interrupts, usually 2 and 3
static volatile byte i_rx = 3;
//single packet raw data to transmit
static volatile ushort i_packet_tx = 0;
//single packet raw data to reading
static volatile ushort i_packet_rx = 0;
static volatile ushort i_cursor_rx = 0;
static volatile ushort i_targhet_rx = 1 << 10;


static volatile SerialConfig i_config;
static volatile Queue<ushort> i_buffer_tx(BUFFERSIZE);
static volatile Queue<ushort> i_buffer_rx(BUFFERSIZE);

//No prescale timer
#define TIMER_SETUP() \
  { \
    TCCR1A = 0; \
    TCCR1B = bit(CS10); \
    TIMSK1 = 0; \
    OCR1A = 0; \
    OCR1B = 0; \
  }

#define TIMER_STOP_TX() (TIMSK1 &= ~bit(OCIE1A))
#define TIMER_STOP_RX() (TIMSK1 &= ~bit(OCIE1B))
//require a +10 cycles to trigger interrupt just near
#define TIMER_BEGIN_TX() (TIMSK1 |= bit(OCIE1A), OCR1A = TCNT1 + 10)
//require a +1/2 bit size to get a clear level voltage
#define TIMER_BEGIN_RX() (TIMSK1 |= bit(OCIE1B), OCR1B = TCNT1 + i_ticks / 2 - 10)

/************************************************************
               Write using CompareA interrupt
*************************************************************/
static inline void set_digit_tx() {
  digitalWrite(i_tx, i_packet_tx & 1);
  i_packet_tx >>= 1;
}

ISR(TIMER1_COMPA_vect) {
  OCR1A += i_ticks;
  if (i_packet_tx) set_digit_tx();
  else if (i_buffer_tx.size) i_packet_tx = i_buffer_tx.pop();
  else TIMER_STOP_TX();
}

/************************************************************
               Read using CompareB interrupt
*************************************************************/
static inline void get_digit_rx() {
  int digit = digitalRead(i_rx);
  i_packet_rx |= digit ? i_cursor_rx : 0;
}

static inline void complete_rx() {
  i_buffer_rx.push(i_packet_rx);
  i_cursor_rx = 0;
  i_packet_rx = 0;
}

ISR(TIMER1_COMPB_vect) {
  OCR1B += i_ticks;

  if (i_cursor_rx) {
    if (i_cursor_rx <= i_targhet_rx) get_digit_rx();
    if (i_cursor_rx == i_targhet_rx) complete_rx();
    i_cursor_rx <<= 1;
  } else TIMER_STOP_RX();
}

// Interrupt Service Routine (ISR), trigger at the start bit
void Interrupt_rx() {
  if (!i_cursor_rx) {
    i_packet_rx = 0;
    i_cursor_rx = 1;
    TIMER_BEGIN_RX();
  }
}
