#ifndef APP_H__
#define APP_H__

#define F_CPU 16000000UL
//#define F_CPU 8000000UL

#define D12_EP2_IN_CALLBACK midi_ep_in_callback
#define D12_EP2_OUT_CALLBACK midi_ep_out_callback

#include "common.h"

void midi_ep_in_callback(uint8_t ep);
void midi_ep_out_callback(uint8_t  ep);

#endif /* APP_H__ */
