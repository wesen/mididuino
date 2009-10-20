#include "Monome.h"

MonomeParentClass::MonomeParentClass() {
	activePage = NULL;
	for (uint8_t i = 0; i < 8; i++) {
		buf[i] = 0;
	}
	parse_state = MONOME_BYTE_1;
}

MonomeParentClass::~MonomeParentClass() {
}

void MonomeParentClass::setBufLED(uint8_t x, uint8_t y, uint8_t status) {
	if (status) {
		SET_BIT(buf[y], x);
	} else {
		CLEAR_BIT(buf[y], x);
	}
}

uint8_t MonomeParentClass::getBufLED(uint8_t x, uint8_t y) {
	if (IS_BIT_SET(buf[y], x)) {
		return 1;
	} else {
		return 0;
	}
}
	
void MonomeParentClass::setLED(uint8_t x, uint8_t y, uint8_t status) {
	if (getBufLED(x, y) != status) {
		setBufLED(x, y, status);
		sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED, status), MONOME_GEN_XY(x, y));
	}
}

void MonomeParentClass::toggleLED(uint8_t x, uint8_t y) {
	if (getBufLED(x, y)) {
		clearLED(x, y);
	} else {
		setLED(x, y);
	}
}

void MonomeParentClass::clearLED(uint8_t x, uint8_t y) {
	setLED(x, y, 0);
}

void MonomeParentClass::setRow(uint8_t row, uint8_t leds) {
	for (uint8_t x = 0; x < 8; x++) {
		if (IS_BIT_SET(leds, x)) {
			setBufLED(x, row, 1);
		} else {
			setBufLED(x, row, 0);
		}
	}
	sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED_ROW, row), leds);
}

void MonomeParentClass::setColumn(uint8_t column, uint8_t leds) {
	for (uint8_t y = 0; y < 8; y++) {
		if (IS_BIT_SET(leds, y)) {
			setBufLED(column, y, 1);
		} else {
			setBufLED(column, y, 0);
		}
	}
	sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED_COLUMN, column), leds);
}

void MonomeParentClass::setBuffer() {
	for (uint8_t i = 0; i < 8; i++) {
		setRow(i, buf[i]);
	}
}

void MonomeParentClass::shutdown(uint8_t state) {
	sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_SHUTDOWN, 0), state);
}

void MonomeParentClass::handleByte(uint8_t byte) {
	static uint8_t lastByte = 0;
	switch (parse_state) {
	case MONOME_BYTE_1:
		parse_state = MONOME_BYTE_2;
		lastByte = byte;
		break;
		
	case MONOME_BYTE_2:
		{
			parse_state = MONOME_BYTE_1;
			monome_event_t event;
			switch (MONOME_ADDRESS(lastByte)) {
			case MONOME_CMD_PRESS:
				event.x = MONOME_X(byte);
				event.y = MONOME_Y(byte);
				event.state = MONOME_STATE(lastByte);
				callbacks.call(&event);
				if (activePage != NULL) {
					activePage->handleEvent(&event);
				}
				break;
			}
		}
		break;
	}
}

void MonomeParentClass::setActivePage(MonomePage *_page) {
	activePage = _page;
}

bool MonomeParentClass::isActivePage(MonomePage *_page) {
	return activePage == _page;
}

void MonomeParentClass::drawPage(MonomePage *page) {
	for (uint8_t y = 0; y < 8; y++) {
		setRow(y, page->buf[y]);
	}
	page->needsRefresh = false;
}

void MonomeParentClass::updateGUI() {
	if (activePage && activePage->needsRefresh) {
		drawPage(activePage);
	}
}

