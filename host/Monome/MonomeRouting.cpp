#include <vector>
using namespace std;

#include "Monome.h"
#include "MonomeHost.h"
#include "MonomeTrigPage.h"

extern uint8_t standardDrumMapping[16];

void addOffTrig(uint8_t pitch);

uint8_t routing[16][6] = { { 0 } };
MonomeTrigPage *trigPages[3] = { 0 };

void trigRoutingOn(uint8_t routing);
void trigRoutingOff(uint8_t routing);

void trigRoutingOn(uint8_t trig) {
}

void trigRoutingOff(uint8_t trig) {
}

void trigNoteOn(uint8_t trig) {
	int column = -1;
	for (uint8_t i = 0; i < sizeof(standardDrumMapping); i++) {
		if (standardDrumMapping[i] == trig) {
			column = i;
			break;
		}
	}
	for (uint8_t i = 0; i < countof(trigPages); i++) {
		if (trigPages[i] != NULL) {
			trigPages[i]->onTrigNote(trig);
		}
	}

	if (column != -1) {
		for (int i = 0; i < 6; i++) {
			if (routing[column][i]) {
				//				printf("TRIGGER VIDEO EFFECT ON %d\n", i);
				MidiUart.sendNoteOn(60 + i, 100);
			}
		}
	}

	addOffTrig(trig);
}

void trigNoteOff(uint8_t trig) {
	for (uint8_t i = 0; i < countof(trigPages); i++) {
		if (trigPages[i] != NULL) {
			trigPages[i]->onTrigNoteOff(trig);
		}
	}
	int column = -1;
	for (uint8_t i = 0; i < sizeof(standardDrumMapping); i++) {
		if (standardDrumMapping[i] == trig) {
			column = i;
			break;
		}
	}
	if (column != -1) {
		for (int i = 0; i < 6; i++) {
			if (routing[column][i]) {
				//				printf("TRIGGER VIDEO EFFECT OFF %d\n", i);
				MidiUart.sendNoteOn(60 + i, 0);
			}
		}
	}
}

typedef struct {
public:
	int duration;
	uint8_t trig;
} offtrig_t;

vector<offtrig_t *> offTrigs;

void addOffTrig(uint8_t trig) {
	offtrig_t *offtrig = new offtrig_t;
	offtrig->duration = 10;
	offtrig->trig = trig;
	offTrigs.push_back(offtrig);
}

void tickOffTrig() {
	for (vector<offtrig_t *>::iterator it = offTrigs.begin();
			 it != offTrigs.end();
			 ) {
		offtrig_t *trig = *it;
		//		printf("%x, %d\n", trig->trig, trig->duration);
		if (--trig->duration == 0) {
			trigNoteOff(trig->trig);
			it = offTrigs.erase(it);
			delete trig;
		} else {
			it++;
		}
	}
}

class MonomeHandler : public MonomeCallback, public MidiCallback {
public:
	MonomeParentClass *monome;

	MonomeHandler(MonomeParentClass *_monome) {
		monome = _monome;
	}

	void setup() {
		Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&MonomeHandler::onNoteOn);
	}
	
	void onNoteOn(uint8_t *msg) {
		trigNoteOn(msg[1]);
	}
};

