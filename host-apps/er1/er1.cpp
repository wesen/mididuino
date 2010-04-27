#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include "MidiSysex.hh"
#include "CCHandler.h"
#include "Encoders.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

typedef struct er1_param_name_s {
	char *name;
	uint16_t nrpn;
} er1_param_name_t;

// 0x200, 0x20a, 0x214, 0x1e
const char *synth_names[] = {
	"LOW BOOST",
	"PAN",
	"PITCH",
	"WAVE", // 0-63 / 64-127 : Sin / Tri
	"MOD TYPE", // 0-5 : Saw / Squ / Tri / S&H / Noise / Env
	"MOD SPEED",
	"MOD DEPTH",
	"LEVEL",
	"DECAY",
	"MOTION SEQ TYPE" // 0-2 : Off / Smooth / TrigHold
};

// 0x264
const char *effect_names[] = {
	"DELAY DEPTH",
	"DELAY TIME",
	"RING 1", // 0-63 / 64-127 : Off/On
	"RING 2", // 0-63 / 64-127 : Off/On
	"INPUT GAIN 1",
	"INPUT GAIN 2",
	"ACCENT LEVEL", 
	"DELAY TYPE", // 0-2 Off / MotionSeq / TempoDelay
	"MUTE 1", // Bit6 : Mute/Solo, Bit3-0 (Synth4 - 1) : Mute
	"MUTE 2", // Bit6 : Mute/Solo, Bit 5- 0 (A2, A1, P4-P1): Mute
};

class ER1 : public MidiCallback {
public:
	ER1() {
		nrpnReceived = false;
		lastNRPN = 0;
	}

	bool nrpnReceived;
	uint16_t lastNRPN;
	
	void onCCCallback(uint8_t *msg) {
		//		printf("CC: %x %x %x\n", msg[0], msg[1], msg[2]);

		switch (msg[1]) {
		case 0x62:
			lastNRPN |= msg[2];
			break;

		case 0x63:
			lastNRPN |= (msg[2]) << 8;
			break;

		case 0x06:
			onNRPNCallback(lastNRPN, msg[2]);
			lastNRPN = 0;
			break;
		}
	}

	void handleNRPNSynth(uint16_t nrpn, uint8_t value) {
		uint8_t synth = (nrpn - 0x200) / 10;
		uint8_t param = (nrpn - 0x200) % 10;
		printf("SYNTH %d %s: %d\n", synth, synth_names[param], value);
	}
	
	void handleNRPNSample(uint16_t nrpn, uint8_t value) {
		uint8_t synth = (nrpn - 0x228) / 10;
		uint8_t param = (nrpn - 0x228) % 10;
		printf("SAMPLE %d %s: %d\n", synth, synth_names[param], value);
	}
	
	void handleNRPNInput(uint16_t nrpn, uint8_t value) {
		uint8_t synth = (nrpn - 0x250) / 10;
		uint8_t param = (nrpn - 0x250) % 10;
		printf("INPUT %d %s: %d\n", synth, synth_names[param], value);
	}
	
	void handleNRPNEffect(uint16_t nrpn, uint8_t value) {
		uint8_t param = (nrpn - 0x264) / 10;
		printf("EFFECT %s: %d\n", effect_names[param], value);
	}
	
	void onNRPNCallback(uint16_t nrpn, uint8_t value) {
		printf("nrpn %x : %x\n", nrpn, value);
		
		if (!((nrpn >= 0x200) && (nrpn <= 0x26d))) {
			printf("unknown NRPN number: %x\n", nrpn);
			return;
		}

		if (nrpn >= 0x200) {
			if (nrpn < 0x228) {
				handleNRPNSynth(nrpn, value);
			} else if (nrpn < 0x250) {
				handleNRPNSample(nrpn, value);
			} else if (nrpn < 0x264) {
				handleNRPNInput(nrpn, value);
			} else {
				handleNRPNEffect(nrpn, value);
			}
		}
	}
	
	void onNoteOnCallback(uint8_t *msg) {
		printf("NOTE: %x %x %x\n", msg[0], msg[1], msg[2]);
	}
};

ER1 er1;

int main(int argc, char *argv[]) {
	MidiUart.listInputMidiDevices();
  MidiUart.init(0, 0);
  Midi.addOnControlChangeCallback(&er1, (midi_callback_ptr_t)&ER1::onCCCallback);
  Midi.addOnNoteOnCallback(&er1, (midi_callback_ptr_t)&ER1::onNoteOnCallback);

	uint8_t sysexCmd[] = { 0x7e, 0x0c, 0x06, 0x01	};
	MidiUart.sendSysex(sysexCmd, 4);
  
  for (;;) {
    handleIncomingMidi();
  }

  return 0;
}
