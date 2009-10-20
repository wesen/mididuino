#include <stdio.h>

#include "WProgram.h"
#include "MidiUartHost.h"

#include "Monome.h"
#include "MonomeHost.h"

class TestMonomePage : public MonomePage {
public:
	TestMonomePage(MonomeParentClass *monome) : MonomePage(monome) {
		setLED(1, 1);
		setLED(2, 2);
		height = 7;
	}
	
	bool handleEvent(monome_event_t *event) {
		printf("handle event %d, %d, %d\n", event->x, event->y, event->state);
		if (IS_BUTTON_PRESSED(event)) {
			setLED(event->x, event->y);
		} else if (IS_BUTTON_RELEASED(event)) {
			clearLED(event->x, event->y);
		}
		return true;
	}

};


class TestMonomePage2 : public MonomePage {
public:
	TestMonomePage2(MonomeParentClass *monome) : MonomePage(monome) {
		setLED(4, 4);
		setLED(5, 4);
		height = 7;
	}
	
	bool handleEvent(monome_event_t *event) {
		printf("handle event %d, %d, %d\n", event->x, event->y, event->state);
		if (IS_BUTTON_PRESSED(event)) {
			toggleLED(event->x, event->y);
		}
		return true;
	}

};


int main(int argc, const char *argv[]){
	try {
		MonomeHost monome(argv[1]);
		monome.setup();

		TestMonomePage page(&monome);
		TestMonomePage2 page2(&monome);
		MonomePageSwitcher switcher(&monome, &page, &page2);

		switcher.setup();
		switcher.setPage(0);
		monome.setBuffer();

		for (;;) {
			monome.loop();
			if (monome.isAvailable(100)) {
				monome.handle();
			}
		}
	} catch (const char *s) {
		fprintf(stderr, "caught error %s\n", s);
	}

	return 0;
}
