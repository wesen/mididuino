#include "MidiUartParent.hh"
#include "MidiUartHost.h"

#include "MonomeMidiPage.h"

extern MidiUartHostClass MidiUart;

void switchPage(uint8_t page);

MonomeMidiPage::MonomeMidiPage(MonomeParentClass *monome) : MonomeTrigPage(monome, 0) {
}

void MonomeMidiPage::handleEvent(monome_event_t *evt) {
	if ((evt->y == 7) && (evt->state == 1)) {
		switchPage(evt->x);
		return;
	}
		
	if ((evt->y >= 1) && (evt->y < 7)) {
		if (evt->state == 1) {
			MidiUart.sendNoteOn(60 + evt->y - 1, 100);
			setLED(evt->x, evt->y);
		} else {
			MidiUart.sendNoteOn(60 + evt->y - 1, 0);
			clearLED(evt->x, evt->y);
		}
	}
}


void MonomeMidiPage::onTrigNote(uint8_t pitch) {
}

void MonomeMidiPage::onTrigNoteOff(uint8_t pitch) {
}
		
