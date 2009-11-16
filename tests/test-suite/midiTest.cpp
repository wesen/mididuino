#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <ListPool.hh>
#include <Midi.h>

class Foobar : public MidiCallback {
public:
  void midiCallback(uint8_t *msg) {
    printf("callbac\n");
  }
};

class MidiCallbackTester {
public:
	bool receivedCC;
	bool receivedNoteOn;
	bool receivedNoteOff;
	bool receivedAftertouch;
	bool receivedProgramChange;
	bool receivedChannelPressure;
	bool receivedPitchWheel;

	uint8_t data[3];

	void reset() {
		data[0] = 0x00;
		data[1] = 0x00;
		data[2] = 0x00;
		
		receivedCC = false;
		receivedNoteOn = false;
		receivedNoteOff = false;
		receivedAftertouch = false;
		receivedProgramChange = false;
		receivedChannelPressure = false;
		receivedPitchWheel = false;
	}

	void onCC(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onNoteOn(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onNoteOff(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onAftertouch(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onProgramChange(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onChannelPressure(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}

	void onPitchWheel(uint8_t *msg) {
		receivedCC = true;
		data[0] = msg[0];
		data[1] = msg[1];
		data[2] = msg[2];
	}
	
	void setup(MidiClass *midi) {
	}
};

struct MidiCallbackFixture {
  MidiCallbackFixture() {
  }
  
  CallbackList<MidiCallback, 4> callbackList;
  Foobar foobar;
};

TEST_F (MidiCallbackFixture, TestAdd) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
}

TEST_F (MidiCallbackFixture, TestAddRemove) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
  CHECK(callbackList.remove(&foobar));
  CHECK_EQUAL(0, callbackList.size());
}

TEST_F (MidiCallbackFixture, TestCall) {
  CHECK(callbackList.add(&foobar));
  CHECK_EQUAL(1, callbackList.size());
  //  callbackMidi(callbackList, NULL);
}
