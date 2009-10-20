#include "Monome.h"

MonomePage::MonomePage(MonomeParentClass *_monome) {
	monome = _monome;
	for (uint8_t i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
		needsRefresh = true;
	}
}

void MonomePage::setLED(uint8_t x, uint8_t y, uint8_t status) {
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


