#ifndef DATA_ENCODER_H__
#define DATA_ENCODER_H__

#include "WProgram.h"
#include "Midi.h"

#ifdef HOST_MIDIDUINO
#include "DataEncoderUnchecking.hh"
//#include "DataEncoderChecking.hh"
#else
#include "DataEncoderUnchecking.hh"
#endif

class UartDataEncoder : public DataEncoder {
public:
	MidiUartParent *uart;
	
	UartDataEncoder(MidiUartParent *_uart) {
		uart = _uart;
	}

	DATA_ENCODER_RETURN_TYPE pack8(uint8_t inb) {
		uart->putc(inb);
		DATA_ENCODER_TRUE();
	}
};

#endif /* DATA_ENCODER_H__ */
