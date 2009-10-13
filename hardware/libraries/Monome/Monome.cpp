#include "Monome.h"

MonomePage::MonomePage(MonomeParentClass *_monome) {
	monome = _monome;
	for (uint8_t i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
		needsRefresh = true;
	}
}

uint8_t MonomePage::getBufLED(uint8_t x, uint8_t y) {
	if (IS_BIT_SET(buf[y], x)) {
		return 1;
	} else {
		return 0;
	}
}

void MonomePage::setLED(uint8_t x, uint8_t y, uint8_t status) {
	//	printf("set led %d, %d: %d\n", x, y, status);
	
	if (status) {
		SET_BIT(buf[y], x);
	} else {
		CLEAR_BIT(buf[y], x);
	}

	if (monome->isActivePage(this)) {
		monome->setLED(x, y, status);
	} else {
		needsRefresh = true;
	}
}

void MonomePage::toggleLED(uint8_t x, uint8_t y) {
	if (getBufLED(x, y)) {
		clearLED(x, y);
	} else {
		setLED(x, y);
	}
}

void MonomePage::clearLED(uint8_t x, uint8_t y) {
	setLED(x, y, 0);
}

void MonomePage::handleEvent(monome_event_t *evt) {
}

