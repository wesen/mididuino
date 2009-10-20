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
};

int main(int argc, const char *argv[]){
	signal(SIGINT, sigquit);
	
	try {
		MonomeHost monome(argv[1]);
		MonomePage page(&monome);

		monome.setActivePage(&page);
		page.setLED(1, 1);
		page.setLED(2, 2);
		
		for (;;) {
			monome.updateGUI();
			if (monome.isAvailable(100)) {
				printf("foo\n");
				monome.handle();
				printf("foo2\n");
			}
		}
	} catch (const char *s) {
		fprintf(stderr, "caught error %s\n", s);
	}

	return 0;
}
