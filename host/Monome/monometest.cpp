#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
using namespace std;

#include "WProgram.h"
#include "MidiUartHost.h"

#include "MonomeStraightPage.h"

#include "Monome.h"
#include "MonomeHost.h"
#include "MonomeMidiPage.h"
#include "MonomeMidiSeqPage.h"
#include "MonomeSequencer.h"

uint8_t standardDrumMapping[16] = {
  36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62
};

MonomePageSwitcher *gSwitcher = NULL;

void switchPage(uint8_t page) {
	if (gSwitcher != NULL) {
		gSwitcher->setPage(page);
	}
}

MonomeSequencer *gSequencer;

void clearTrack(uint8_t track) {
	for (uint8_t i = 0; i < gSequencer->len; i++) {
		gSequencer->clearTrackTrig(track, i);
	}
}	

bool handleMonomeEvent(monome_event_t *evt) {
	if (evt->y == 7) {
		MidiUart.sendNoteOn(15, evt->x, evt->state ? 100 : 0);
	}
	return false;
}

int main(int argc, const char *argv[]) {
	int input = -1;
	int output = -1;

	if (argc != 4) {
		printf("Usage: ./monome inputdevice outputdevice monomedevice\n\n");
		printf("input devices\n");
		MidiUartHostClass::listInputMidiDevices();
		printf("output devices\n");
		MidiUartHostClass::listOutputMidiDevices();
		return 0;
	} else {
		input = atoi(argv[1]);
		output = atoi(argv[2]);
		MidiUart.init(input, output);
	}
	MidiClock.mode = MidiClock.EXTERNAL_MIDI;
	MidiClock.start();
	MidiUart.currentChannel = 2;
	
	MonomeHost monome(argv[3]);
	monome.setup();

	MonomeSequencer sequencer(16);
	gSequencer = &sequencer;

	MonomeMidiPage midiPage(&monome, &sequencer);
	midiPage.setup();
	MonomeMidiSeqPage seqPage(&monome, &sequencer, 0), seqPage2(&monome, &sequencer, 8);
	seqPage.setup();
	seqPage2.setup();

	MonomeStraightPage paintPage1(&monome, 3);
	MonomeStraightPage paintPage2(&monome, 4);
	MonomeStraightPage paintPage3(&monome, 5);
	MonomeStraightPage paintPage4(&monome, 6);

	sequencer.setup(); // hack add sequencer as last in callback list

	MonomePageSwitcher switcher(&monome,
															&midiPage, &seqPage, &seqPage2,
															&paintPage1, &paintPage2, &paintPage3, &paintPage4);
	gSwitcher = &switcher;
	switcher.setup();
	switcher.setPage(0);
	monome.setBuffer();
	monome.addEventHandler(handleMonomeEvent);

	for (;;) {
		MidiUart.runLoop();
		while (MidiUart.avail()) {
			uint8_t c = MidiUart.getc();
			Midi.handleByte(c);
		}

		monome.loop();
		if (monome.isAvailable()) {
			monome.handle();
		}

		//		tickOffTrig();
	}

	return 0;
}
