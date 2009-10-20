#include <stdio.h>

#include "WProgram.h"
#include "MidiUartHost.h"

#include "Monome.h"
#include "MonomeHost.h"

void sigquit(int isngal) {
	printf("sigquit\n");
	exit(1);
}

class TestMonomePage : public MonomePage {
public:
	TestMonomePage(MonomeParentClass *monome) : MonomePage(monome) {
		setLED(1, 1);
		setLED(2, 2);
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

class MonomePageSwitcher : public MonomeCallback {
public:
	MonomePage *pages[8];

	MonomePageSwitcher() {
	}
	
	bool handleEvent(monome_event_t *event) {
	}
};

int main(int argc, const char *argv[]){
	signal(SIGINT, sigquit);
	
	try {
		MonomeHost monome(argv[1]);
		TestMonomePage page(&monome);

		monome.setup();

		monome.setPage(&page);

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
