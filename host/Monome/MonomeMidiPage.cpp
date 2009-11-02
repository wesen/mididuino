#include "WProgram.h"

#include "MidiUartHost.h"

#include "MonomeMidiPage.h"

extern MidiUartHostClass MidiUart;

void switchPage(uint8_t page);

MonomeMidiPage::MonomeMidiPage(MonomeParentClass *monome, MonomeSequencer *_sequencer) :
	MonomeTrigPage(monome, 0) {
	sequencer = _sequencer;
	y = 0;
	height = 7;
	for (uint8_t i = 0; i < 6; i++) {
		trackPressed[i] = false;
	}
	clearTrackMode = false;
	muteMode = false;
}

void MonomeMidiPage::show() {
	clearTrackMode = false;
	muteMode = false;
	for (uint8_t i = 0; i < 6; i++) {
		trackPressed[i] = false;
	}
}

void MonomeMidiPage::hide() {
	clearTrackMode = false;
	muteMode = false;
	for (uint8_t i = 0; i < 6; i++) {
		trackPressed[i] = false;
	}
}

extern void clearTrack(uint8_t track);

void MonomeMidiPage::clearTrack(uint8_t track) {
	::clearTrack(track);
}

void MonomeMidiPage::clearPage() {
		for (uint8_t i = 0; i < 6; i++) {
			clearTrack(i);
		}
}

bool MonomeMidiPage::handleEvent(monome_event_t *evt) {
	if (evt->y == 0) {
		switch(evt->x) {
		case 0:
			clearTrackMode = evt->state;
			break;
			
		case 1:
			clearPage();
				break;
				
		case 7:
			muteMode = evt->state;
			break;
		}
		
		return true;
	}

	uint8_t track = evt->y - 1;

	if (evt->state == 1) {
		if (clearTrackMode) {
			//				printf("clear %d\n", evt->y - 1);
			clearTrack(evt->y - 1);
			return true;
		} else {
			uint32_t pos = MidiClock.div16th_counter;
			MidiUart.sendNoteOn(60 + track, 100);
			setLED(evt->x, evt->y);
			sequencer->setTrackTrig(track, (pos / 2) % sequencer->len);
			trackPressed[track] = true;
			trackStartPoint[track] = evt->x;
			trackStartTime[track] = pos;
		}
	} else {
		MidiUart.sendNoteOn(60 + track, 0);
		trackPressed[track] = false;
		for (uint8_t i = 0; i < 8; i++) {
			clearLED(i, evt->y);
		}
	}
	return true;
}

void MonomeMidiPage::onTrigNote(uint8_t pitch) {
}

void MonomeMidiPage::onTrigNoteOff(uint8_t pitch) {
}
		
void MonomeMidiPage::on16Note(uint32_t pos) {
	uint8_t interval = 4;
	
	if ((pos % interval) == 0) {
		uint16_t pos2 = (pos / interval) % 8;
		
		uint8_t previdx = ((pos - 1) / interval) % 8;
		uint8_t idx = (pos / interval) % 8;
		
		setLED(previdx, 0, 0);
		setLED(idx, 0, 1);

	}
	if ((pos % 2) == 0) {
		for (uint8_t i = 0; i < 6; i++) {
			if (trackPressed[i]) {
				uint8_t len = (pos - trackStartTime[i]) / 2;
				if ((len + trackStartPoint[i]) < 8) {
					setLED(len + trackStartPoint[i], i + 1);
				}
			}
		}
		for (uint8_t i = 0; i < 6; i++) {
			if (trackPressed[i]) {
				sequencer->setTrackTrig(i, (pos / 2) % sequencer->len);
			}
		}
	}
}

void MonomeMidiPage::setup() {
	MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)(&MonomeMidiPage::on16Note));
}

