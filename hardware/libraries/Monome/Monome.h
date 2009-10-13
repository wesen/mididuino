#ifndef MONOME_H__
#define MONOME_H__

#include "WProgram.h"
#include <inttypes.h>
#include "Callback.hh"

typedef struct {
	uint8_t x, y;
	uint8_t state;
} monome_event_t;

class MonomeCallback {
};

typedef void(MonomeCallback::*monome_callback_ptr_t)(monome_event_t *evt);

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

class MonomeParentClass {
 public:
	enum {
		MONOME_BYTE_1 = 0,
		MONOME_BYTE_2
	} parse_state;
	uint8_t buf[8]; // blitting buffer

	MonomeParentClass() {
		for (uint8_t i = 0; i < 8; i++) {
			buf[i] = 0;
		}
		parse_state = MONOME_BYTE_1;
	}
	~MonomeParentClass() { }

	virtual void sendBuf(uint8_t *data, uint8_t len) = 0;
	virtual void sendMessage(uint8_t byte1, uint8_t byte2) = 0;
	
	void setLED(uint8_t x, uint8_t y, uint8_t status = 1) {
		sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED, status), MONOME_GEN_XY(x, y));
	}
	void clearLED(uint8_t x, uint8_t y) {
		setLED(x, y, 0);
	}
	void setRow(uint8_t row, uint8_t leds) {
		sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED_ROW, row), leds);
	}
	void setColumn(uint8_t column, uint8_t leds) {
		sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_LED_COLUMN, column), leds);
	}
	void setBuffer() {
		for (uint8_t i = 0; i < 8; i++) {
			setRow(i, buf[i]);
		}
	}

	void shutdown(uint8_t state = 0) {
		sendMessage(MONOME_GEN_ADDRESS(MONOME_CMD_SHUTDOWN, 0), state);
	}

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

	void handleByte(uint8_t byte) {
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
					break;
				}
			}
			break;
		}
	}
};

#endif /* MONOME_H__ */
