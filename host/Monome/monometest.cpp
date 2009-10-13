#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
using namespace std;

#include "Monome.h"
#include "select.hh"

MidiUartOSXClass MidiUart;
MidiClass Midi, Midi2;

class MonomeHost : public MonomeParentClass {
protected:
	int fd;
		FDSelect fds;

public:
	MonomeHost(const char *file) {
		fd = open(file, O_RDWR);
		if (fd < 0)
			throw "could not open Monome file";
		fds.add(fd);
	}

	~MonomeHost() {
		if (fd >= 0)
			close(fd);
	}
	
	virtual void sendBuf(uint8_t *data, uint8_t len) {
		int ret = write(fd, data, len);
		if (ret < 0) {
			throw "error while writing";
		} else if (ret != len) {
			throw "could not write all necessary bytes";
		}
	}

	virtual void sendMessage(uint8_t byte1, uint8_t byte2) {
		uint8_t buf[2] = {byte1, byte2};
		sendBuf(buf, 2);
	}

	bool isAvailable(unsigned long timeout = 10) {
		fds.waitRead(timeout);
		return fds.isSet(fd);
	}

	void handle() {
		char buf[128];
		int ret = read(fd, buf, sizeof(buf));
		if (ret < 0) {
			throw "error while reading from monome";
		}
		for (uint8_t i = 0; i < ret; i++) {
			handleByte(buf[i]);
		}
	}
};

	
uint8_t standardDrumMapping[16] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

void addOffTrig(uint8_t pitch);
void switchPage(uint8_t page);

uint8_t routing[16][6] = { { 0 } };

void trigRoutingOn(uint8_t routing);
void trigRoutingOff(uint8_t routing);

class MonomeTrigPage : public MonomePage {
public:
	uint8_t start;
	
	MonomeTrigPage(MonomeParentClass *monome, uint8_t _start) : MonomePage(monome) {
		start = _start;
	}

	~MonomeTrigPage() {
	}

	void toggleRouting(uint8_t x, uint8_t y) {
		uint8_t trig = x + start;
		uint8_t output = y - 1;
		if (routing[trig][output] == 0) {
			routing[trig][output] = 1;
			setLED(x, y, 1);
		} else {
			routing[trig][output] = 0;
			clearLED(x, y);
		}
		//		printf("set routing %d, %d, to %d\n", trig, output, routing[trig][output]);
	}

	virtual void handleEvent(monome_event_t *evt) {
		if ((evt->y == 7) && (evt->state == 1)) {
			switchPage(evt->x);
			return;
		}
		
		if ((evt->y >= 1) && (evt->state == 1)) {
			toggleRouting(evt->x, evt->y);
			//			printf("evt x: %d y: %d: %d\n", evt->x, evt->y, evt->state);
		}
	}

	bool isValidColumn(uint8_t column) {
		return ((column >= start) && (column < (start + 8)));
	}
	
	uint8_t getColumn(uint8_t pitch) {
		int column = -1;
		for (uint8_t i = 0; i < sizeof(standardDrumMapping); i++) {
			if (standardDrumMapping[i] == pitch) {
				column = i;
				break;
			}
		}
		if (isValidColumn(column)) {
			return column - start;
		} else {
			return -1;
		}
	}

	virtual void onTrigNote(uint8_t pitch) {
		int column = getColumn(pitch);
		uint8_t idx = column + start;
		if (column != -1) {
			//			trigRoutingOn(column + start);
			for (int i = 0; i < 6; i++) {
				if (routing[idx][i]) {
					setLED(column, i + 1, 0);
				} else {
					clearLED(column, i + 1);
				}
			}
			setLED(column, 0);
			addOffTrig(pitch);
		}
	}

	virtual void onTrigNoteOff(uint8_t pitch) {
		int column = getColumn(pitch);
		uint8_t idx = column + start;
		if (column != -1) {
			for (int i = 0; i < 6; i++) {
				if (routing[idx][i]) {
					setLED(column, i + 1);
				} else {
					clearLED(column, i + 1);
				}
			}
			//trigRoutingOff(column + start);
			clearLED(column, 0);
		}
	}
};

class MonomeMidiPage : public MonomeTrigPage {
public:
	MonomeMidiPage(MonomeParentClass *monome) : MonomeTrigPage(monome, 0) {
		
	}

	virtual void handleEvent(monome_event_t *evt) {
		if ((evt->y == 7) && (evt->state == 1)) {
			switchPage(evt->x);
			return;
		}
		
		if ((evt->y >= 1) && (evt->y < 7)) {
			if (evt->state == 1) {
				MidiUart.midiSendShort(0x90, 60 + evt->y - 1, 100);
				setLED(evt->x, evt->y);
			} else {
				MidiUart.midiSendShort(0x90, 60 + evt->y - 1, 0);
				clearLED(evt->x, evt->y);
			}
		}
	}


	virtual void onTrigNote(uint8_t pitch) {
	}

	virtual void onTrigNoteOff(uint8_t pitch) {
	}
		
};


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
				MidiUart.midiSendShort(0x90, 60 + i, 100);
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
				MidiUart.midiSendShort(0x90, 60 + i, 00);
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
	MonomeHost monome(argv[3]);
	MonomeHandler handler(&monome);
	MonomeTrigPage page(&monome, 0), page2(&monome, 8), page3(&monome, 16);
	MonomeMidiPage midiPage(&monome);

	pages[0] = &page;
	pages[1] = &page2;
	pages[2] = &midiPage;

	for (int i = 0; i < countof(pages); i++) {
		if (pages[i] != NULL) {
			for (int j = 0; j < countof(pages); j++) {
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
