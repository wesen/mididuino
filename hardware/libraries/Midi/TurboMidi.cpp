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
	currentSpeed = TURBOMIDI_SPEED_1x;
	state = tm_state_normal;
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
				state = tm_master_req_answer_recvd;
			}
		}
		break;

	case TURBOMIDI_SPEED_ACK_SLAVE:
		if (state == tm_master_wait_speed_ack) {
			state = tm_master_speed_ack_recvd;
		}
		break;

	case TURBOMIDI_SPEED_RESULT_SLAVE:
		if (state == tm_master_wait_test_1) {
			state = tm_master_test_1_recvd;
		}
		break;

	case TURBOMIDI_SPEED_RESULT_SLAVE_2:
		if (state == tm_master_wait_test_2) {
			state = tm_master_test_2_recvd;
		}
		break;

	default:
		break;
	}
}

void TurboMidiSysexListenerClass::startTurboMidi() {
	state = tm_master_wait_req_answer;
	slaveSpeeds = 0;
	certifiedSlaveSpeeds = 0;

	sendSpeedRequest();
	bool ret = blockForState(tm_master_req_answer_recvd);
}

static void sendTurbomidiHeader(uint8_t cmd) {
	MidiUart.puts(turbomidi_sysex_header, sizeof(turbomidi_sysex_header));
	MidiUart.putc(cmd);
}

bool TurboMidiSysexListenerClass::sendSpeedRequest() {
	sendTurbomidiHeader(TURBOMIDI_SPEED_REQUEST);
	MidiUart.putc(0xF7);
}

void TurboMidiSysexListenerClass::sendSpeedNegotiationRequest(uint8_t speed1, uint8_t speed2) {
	state = tm_master_wait_speed_ack;

	sendTurbomidiHeader(TURBOMIDI_SPEED_NEGOTIATION_MASTER);
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

static uint8_t getHighestBit(uint16_t b) {
	for (uint8_t i = 15; i >= 0; i--) {
		if (IS_BIT_SET(b, i)) {
			return i;
		}
	}
	return 0;
}

uint32_t TurboMidiSysexListenerClass::tmSpeeds[11] = {
	31250,
	62500,
	104062,
	125000,
	156250,
	208125,
	250000,
	312500,
	415625,
	500000,
	625000
};
	
void TurboMidiSysexListenerClass::setSpeed(uint8_t speed) {
	currentSpeed = speed;
	MidiUart.setSpeed(tmSpeeds[speed]);
}

void TurboMidiSysexListenerClass::sendSpeedTest1(uint8_t speed1) {
	state = tm_master_wait_test_1;

	setSpeed(getHighestBit(speeds & slaveSpeeds));
	
	for (uint8_t i = 0; i < 16; i++) {
		MidiUart.putc(0x0C);
	}
	
	sendTurbomidiHeader(TURBOMIDI_SPEED_TEST_MASTER);	
	sendSpeedPattern();
	MidiUart.putc(0xF7);
}
	
void TurboMidiSysexListenerClass::sendSpeedTest2(uint8_t speed2) {
	state = tm_master_wait_test_2;

	setSpeed(getHighestBit(certifiedSpeeds & certifiedSlaveSpeeds));
	
	sendTurbomidiHeader(TURBOMIDI_SPEED_TEST_MASTER_2);	
	MidiUart.putc(0xF7);
}
	
void TurboMidiSysexListenerClass::sendSpeedAnswer() {
	sendTurbomidiHeader(TURBOMIDI_SPEED_ANSWER);	
	MidiUart.putc(speeds & 0x7F);
	MidiUart.putc((speeds >> 7) & 0x7F);
	MidiUart.putc(certifiedSpeeds & 0x7F);
	MidiUart.putc((certifiedSpeeds >> 7) & 0x7F);
	MidiUart.putc(0xF7);
}

void TurboMidiSysexListenerClass::sendSpeedNegotiationAck() {
	sendTurbomidiHeader(TURBOMIDI_SPEED_ACK_SLAVE);	
	MidiUart.putc(0xF7);
}

void TurboMidiSysexListenerClass::sendSpeedTest1Result() {
	sendTurbomidiHeader(TURBOMIDI_SPEED_RESULT_SLAVE);	
	sendSpeedPattern();
	MidiUart.putc(0xF7);
}

void TurboMidiSysexListenerClass::sendSpeedTest2Result() {
	sendTurbomidiHeader(TURBOMIDI_SPEED_RESULT_SLAVE_2);	
	MidiUart.putc(0xF7);
}

bool TurboMidiSysexListenerClass::blockForState(tm_state_t _state, uint16_t timeout) {
  uint16_t start_clock = read_slowclock();
  uint16_t current_clock = start_clock;
  do {
    current_clock = read_slowclock();
    handleIncomingMidi();
		GUI.display();
  } while ((clock_diff(start_clock, current_clock) < timeout) && (state != _state));
	return (state == _state);
}
