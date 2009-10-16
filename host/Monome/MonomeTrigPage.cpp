#include "MonomeTrigPage.h"

extern uint8_t routing[16][6];
extern uint8_t standardDrumMapping[16];
void switchPage(uint8_t page);
void addOffTrig(uint8_t pitch);

MonomeTrigPage::MonomeTrigPage(MonomeParentClass *monome, uint8_t _start) : MonomePage(monome) {
	start = _start;
}

MonomeTrigPage::~MonomeTrigPage() {
}

void MonomeTrigPage::toggleRouting(uint8_t x, uint8_t y) {
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

void MonomeTrigPage::handleEvent(monome_event_t *evt) {
	if ((evt->y == 7) && (evt->state == 1)) {
		switchPage(evt->x);
		return;
	}
		
	if ((evt->y >= 1) && (evt->state == 1)) {
		toggleRouting(evt->x, evt->y);
		//			printf("evt x: %d y: %d: %d\n", evt->x, evt->y, evt->state);
	}
}

bool MonomeTrigPage::isValidColumn(uint8_t column) {
	return ((column >= start) && (column < (start + 8)));
}
	
uint8_t MonomeTrigPage::getColumn(uint8_t pitch) {
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

void MonomeTrigPage::onTrigNote(uint8_t pitch) {
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

void MonomeTrigPage::onTrigNoteOff(uint8_t pitch) {
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
