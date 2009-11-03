#ifndef MONOME_STRAIGHT_PAGE_H__
#define MONOME_STRAIGHT_PAGE_H__

#include "Monome.h"
#include <inttypes.h>

class MonomeStraightPage : public MonomePage {
 public:
	uint8_t channel;
	MonomeStraightPage(MonomeParentClass *monome, uint8_t _channel = 3) : MonomePage(monome) {
		channel = _channel;
		y = 0;
		height = 7;
	}

	~MonomeStraightPage() { }

	virtual bool handleEvent(monome_event_t *evt) {
		if (evt->state == 1) {
			setLED(evt->x, evt->y);
			MidiUart.sendNoteOn(channel, evt->y * 8 + evt->x, 100);
		} else {
			clearLED(evt->x, evt->y);
			MidiUart.sendNoteOn(channel, evt->y * 8 + evt->x, 0);
		}
		return true;
	}
};

#endif /* MONOME_STRAIGHT_PAGE_H__ */
