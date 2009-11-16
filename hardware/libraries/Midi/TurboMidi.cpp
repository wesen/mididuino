#include "WProgram.h"

#include "Midi.h"
#include "MidiSysex.hh"
#include "TurboMidi.hh"

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
	
}
