#include "WProgram.h"

#include "Midi.h"
#include "MidiSysex.hh"
#include "TurboMidi.hh"

static uint8_t turbomidi_sysex_header[] = {
	0xF0, 0x00, 0x20, 0x3c, 0x00, 0x00
};

TurboMidiSysexListenerClass::TurboMidiSysexListenerClass() :
	MidiSysexListenerClass() {
	ids[0] = 0x00;
	ids[1] = 0x20;
	ids[2] = 0x3c;
}

void TurboMidiSysexListenerClass::handleByte(uint8_t byte) {
	if (MidiSysex.len == 3) {
		if (byte == 0x00) {
			isGenericMessage = true;
		} else {
			isGenericMessage = false;
		}
	}
}

void TurboMidiSysexListenerClass::end() {
	if (!isGenericMessage)
		return;

	switch (MidiSysex.data[5]) {
		/* master requests (when we are slave) */
	case TURBOMIDI_SPEED_REQUEST:
		break;

	case TURBOMIDI_SPEED_NEGOTIATION_MASTER:
		break;

	case TURBOMIDI_SPEED_TEST_MASTER:
		break;

	case TURBOMIDI_SPEED_TEST_MASTER_2:
		break;


		/* slave answers (when we are master) */
	case TURBOMIDI_SPEED_ANSWER:
		{
			if (state == tm_master_wait_req_answer) {
				slaveSpeeds = MidiSysex.data[6] | (MidiSysex.data[7] << 7);
				certifiedSlaveSpeeds = MidiSysex.data[8] | (MidiSysex.data[9] << 7);
			}
		}
		break;

	case TURBOMIDI_SPEED_ACK_SLAVE:
		break;

	case TURBOMIDI_SPEED_RESULT_SLAVE:
		break;

	case TURBOMIDI_SPEED_RESULT_SLAVE_2:
		break;

	default:
		break;
	}
}

bool TurboMidiSysexListenerClass::requestTurboMidi() {
	if (state != tm_state_normal) {
	}

	state = tm_master_wait_req_answer;
	slaveSpeeds = 0;
	certifiedSlaveSpeeds = 0;
	
	MidiUart.puts(turbomidi_sysex_header, sizeof(turbomidi_sysex_header));
	MidiUart.putc(0x10);
	MidiUart.putc(0xF7);
}

void TurboMidiSysexListenerClass::requestSpeedNegotiation(uint8_t speed1, uint8_t speed2) {
	state = tm_master_wait_speed_ack;

	MidiUart.puts(turbomidi_sysex_header, sizeof(turbomidi_sysex_header));
	MidiUart.putc(0x12);
	MidiUart.putc(speed1);
	MidiUart.putc(speed2);
	MidiUart.putc(0xF7);
}

static void sendSpeedPattern() {
	for (uint8_t i = 0; i < 4; i++) {
		MidiUart.putc(0x00);
	}
	for (uint8_t i = 0; i < 4; i++) {
		MidiUart.putc(0x55);
	}
}

void TurboMidiSysexListenerClass::sendSpeedTest1(uint8_t speed1) {
	state = tm_master_wait_test_1;

	// XXX set speed
	
	for (uint8_t i = 0; i < 16; i++) {
		MidiUart.putc(0x0C);
	}
	
	MidiUart.puts(turbomidi_sysex_header, sizeof(turbomidi_sysex_header));
	MidiUart.putc(0x14);
	sendSpeedPattern();
	MidiUart.putc(0xF7);
}
	
void TurboMidiSysexListenerClass::sendSpeedTest2(uint8_t speed2) {
	state = tm_master_wait_test_2;

	// XXX set speed
	
	MidiUart.puts(turbomidi_sysex_header, sizeof(turbomidi_sysex_header));
	MidiUart.putc(0x16);
	MidiUart.putc(0xF7);
}
	
void TurboMidiSysexListenerClass::sendSpeedAnswer() {
}

void TurboMidiSysexListenerClass::sendSpeedNegotiationAck() {
}

void TurboMidiSysexListenerClass::sendSpeedTest1Result() {
}

void TurboMidiSysexListenerClass::sendSpeedTest2Result() {
}
