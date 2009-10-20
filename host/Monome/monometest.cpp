#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
using namespace std;

#include "WProgram.h"
#include "MidiUartHost.h"

#include "Monome.h"
#include "MonomeHost.h"
#include "MonomeTrigPage.h"
#include "MonomeMidiPage.h"

uint8_t standardDrumMapping[16] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

void addOffTrig(uint8_t pitch);
void switchPage(uint8_t page);

uint8_t routing[16][6] = { { 0 } };

void trigRoutingOn(uint8_t routing);
void trigRoutingOff(uint8_t routing);

MonomeTrigPage *pages[3] = { 0 };
MonomeParentClass *gMonome;

void switchPage(uint8_t page) {
	if ((page < countof(pages)) && (pages[page] != NULL)) {
		pages[page]->needsRefresh = true;
		gMonome->setActivePage(pages[page]);
		printf("switch page %d\n", page);
		for (uint8_t i = 0; i < countof(pages); i++) {
			if (pages[i] != NULL) {
				for (uint8_t j = 0; j < countof(pages); j++) {
					pages[i]->setLED(j, 7, j == page ? 1 : 0);
				}
			}
		}
	}
}

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
	for (uint8_t i = 0; i < countof(pages); i++) {
		if (pages[i] != NULL) {
			pages[i]->onTrigNote(trig);
		}
	}

	if (column != -1) {
		for (int i = 0; i < 6; i++) {
			if (routing[column][i]) {
				printf("TRIGGER VIDEO EFFECT ON %d\n", i);
				MidiUart.sendNoteOn(60 + i, 100);
			}
		}
	}
	
}

void trigNoteOff(uint8_t trig) {
	for (uint8_t i = 0; i < countof(pages); i++) {
		if (pages[i] != NULL) {
			pages[i]->onTrigNoteOff(trig);
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
				printf("TRIGGER VIDEO EFFECT OFF %d\n", i);
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
		monome->addCallback(this, (monome_callback_ptr_t)(&MonomeHandler::onEvent));
		Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&MonomeHandler::onNoteOn);
	}
	
	void onEvent(monome_event_t *evt) {
		//		monome->setLED(evt->x, evt->y, evt->state);
	}

	void onNoteOn(uint8_t *msg) {
		trigNoteOn(msg[1]);
	}
};

int main(int argc, const char *argv[]) {
	int input = -1;
	int output = -1;

#ifdef apple
	if (argc != 4) {
		printf("Usage: ./monome inputdevice outputdevice monomedevice\n\n");
		printf("input devices\n");
		MidiUartOSXClass::listInputMidiDevices();
		printf("output devices\n");
		MidiUartOSXClass::listOutputMidiDevices();
		return 0;
	} else {
		input = atoi(argv[1]);
		output = atoi(argv[2]);
		MidiUart.init(input, output);
	}
#endif
	
	MonomeHost monome(argv[3]);
	MonomeHandler handler(&monome);
	MonomeTrigPage page(&monome, 0), page2(&monome, 8), page3(&monome, 16);
	MonomeMidiPage midiPage(&monome);

	pages[0] = &page;
	pages[1] = &page2;
	pages[2] = &midiPage;

	for (uint8_t i = 0; i < countof(pages); i++) {
		if (pages[i] != NULL) {
			for (uint8_t j = 0; j < countof(pages); j++) {
				if (pages[j] != NULL) {
					//					pages[i]->setLED(j, 7);
				}
			}
		}
	}

	handler.setup();


	gMonome = &monome;
 	monome.setBuffer();
	monome.setActivePage(&page);

	switchPage(0);

	

	for (;;) {
		MidiUart.runLoop();
		while (MidiUart.avail()) {
			uint8_t c = MidiUart.getc();
			Midi.handleByte(c);
		}

		monome.updateGUI();
		if (monome.isAvailable()) {
			monome.handle();
		}

		tickOffTrig();
	}

	return 0;
}
