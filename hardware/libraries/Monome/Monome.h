#ifndef MONOME_H__
#define MONOME_H__

#include "WProgram.h"
#include <inttypes.h>
#include "Callback.hh"

#define MONOME_GEN_ADDRESS(addr, data) ((((addr) & 0xF) << 4) | ((data) & 0xF))
#define MONOME_ADDRESS(data) (((data) >> 4) & 0xF)
#define MONOME_STATE(data) ((data) & 0xF)

#define MONOME_GEN_XY(x, y) ((((x) & 0xF) << 4) | ((y) & 0xF))
#define MONOME_X(data) ((data >> 4) & 0xF)
#define MONOME_Y(data) ((data) & 0xF)

#define MONOME_CMD_PRESS         0x00
#define MONOME_CMD_ADC_VAL       0x01
#define MONOME_CMD_LED           0x02
#define MONOME_CMD_LED_INTENSITY 0x03
#define MONOME_CMD_LED_TEST      0x04
#define MONOME_CMD_ADC_ENABLE    0x05
#define MONOME_CMD_SHUTDOWN      0x06
#define MONOME_CMD_LED_ROW       0x07
#define MONOME_CMD_LED_COLUMN    0x08

#include "MonomePages.hh"
class MonomePage;

class MonomeParentClass {
 public:
	enum {
		MONOME_BYTE_1 = 0,
		MONOME_BYTE_2
	} parse_state;
	uint8_t buf[8]; // blitting buffer

	MonomePage *activePage;

	MonomeParentClass();
	~MonomeParentClass();

	virtual void sendBuf(uint8_t *data, uint8_t len) = 0;
	virtual void sendMessage(uint8_t byte1, uint8_t byte2) = 0;

	void setBufLED(uint8_t x, uint8_t y, uint8_t status);
	uint8_t getBufLED(uint8_t x, uint8_t y);
	void setLED(uint8_t x, uint8_t y, uint8_t status = 1);
	void toggleLED(uint8_t x, uint8_t y);
	void clearLED(uint8_t x, uint8_t y);
	void setRow(uint8_t row, uint8_t leds);
	void setColumn(uint8_t column, uint8_t leds);
	void setBuffer();
	void shutdown(uint8_t state = 0);
	void handleByte(uint8_t byte);

	void setActivePage(MonomePage *_page);
	bool isActivePage(MonomePage *_page);
	void drawPage(MonomePage *page);
	void updateGUI();

	// callbacks
	CallbackVector1<MonomeCallback, 8, monome_event_t *> callbacks;
	void addCallback(MonomeCallback *obj, void (MonomeCallback::*func)(monome_event_t *)) {
		callbacks.add(obj, func);
	}
	void removeCallback(MonomeCallback *obj, void (MonomeCallback::*func)(monome_event_t *) = NULL) {
		if (func == NULL) {
			callbacks.remove(obj);
		} else {
			callbacks.remove(obj, func);
		}
	}
};


#endif /* MONOME_H__ */
