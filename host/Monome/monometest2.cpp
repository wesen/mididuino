#include <stdio.h>

#include "MidiUartHost.h"

#include "Monome.h"
#include "MonomeHost.h"

MidiUartHostClass MidiUart;
MidiClass Midi, Midi2;

int main(int argc, const char *argv[]){
	try {
		MonomeHost monome(argv[1]);
		
		for (;;) {
			monome.updateGUI();
			if (monome.isAvailable()) {
				monome.handle();
			}
		}
	} catch (const char *s) {
		fprintf(stderr, "caught error %s\n", s);
	}

	return 0;
}
