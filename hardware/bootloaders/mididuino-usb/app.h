#ifndef APP_H__
#define APP_H__

#define F_CPU 16000000UL
//#define F_CPU 8000000UL

#define D12_EP2_IN_CALLBACK midi_ep_in_callback
#define D12_EP2_OUT_CALLBACK midi_ep_out_callback

#include "common.h"

void midi_ep_in_callback(uint8_t ep);
void midi_ep_out_callback(uint8_t  ep);

#define A0_PORT      D
#define A0_PIN       5

#define WR_PORT      D
#define WR_PIN       7

#define RD_PORT      D
#define RD_PIN       6

#define SUSPEND_PORT D
#define SUSPEND_PIN  4

#define IRQ_PORT     D
#define IRQ_PIN      3

#define D12_WRITE(data) { \
  DDRA = 0xFF; PORTA = (data); \
}

#define D12_READ() PINA

#define D12_SET_INPUT() { \
  DDRA = 0x00; PORTA = 0xFF; \
}

#endif /* APP_H__ */
