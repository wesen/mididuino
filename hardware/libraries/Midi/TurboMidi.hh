#ifndef TURBOMIDI_H__
#define TURBOMIDI_H__

#include "WProgram.h"

#define TURBOMIDI_SPEED_REQUEST 0x10
#define TURBOMIDI_SPEED_ANSWER  0x11
#define TURBOMIDI_SPEED_NEGOTIATION_MASTER 0x12
#define TURBOMIDI_SPEED_ACK_SLAVE          0x13
#define TURBOMIDI_SPEED_TEST_MASTER        0x14
#define TURBOMIDI_SPEED_RESULT_SLAVE       0x15
#define TURBOMIDI_SPEED_TEST_MASTER_2      0x16
#define TURBOMIDI_SPEED_RESULT_SLAVE_2     0x17

#define TURBOMIDI_SPEED_1x    1
#define TURBOMIDI_SPEED_2x    2
#define TURBOMIDI_SPEED_3_33x 3
#define TURBOMIDI_SPEED_4x    4
#define TURBOMIDI_SPEED_5x    5
#define TURBOMIDI_SPEED_6_66x 6
#define TURBOMIDI_SPEED_8x    7
#define TURBOMIDI_SPEED_10x   8
#define TURBOMIDI_SPEED_13_3x 9
#define TURBOMIDI_SPEED_16x   10
#define TURBOMIDI_SPEED_20x   11

class TurboMidiSysexListenerClass : public MidiSysexListenerClass {
public:
	TurboMidiSysexListenerClass();

	bool isGenericMessage;
	
	virtual void handleByte(uint8_t byte);
	virtual void start() { isGenericMessage = false; }
	virtual void end();

	void    setupTurboMidiSlave();
	uint8_t getTurboMidiSpeed();
	bool    requestTurboMidi();
	void    stopTurboMidi();

	enum {
		tm_state_normal = 0,

		/* master states */
		tm_master_wait_req_answer,
		tm_master_wait_test_1,
		tm_master_wait_test_2,
		tm_master_ok,

		/* slave states */
		tm_slave_wait_test_1,
		tm_slave_wait_test_2,
		tm_slave_ok,
		
	} state;
	
#ifdef HOST_MIDIDUINO
	virtual ~TurboMidiSysexListenerClass() {
	}
#endif
};

extern TurboMidiSysexListenerClass TurboMidiSysexListener;

#endif /* TURBOMIDI_H__ */
