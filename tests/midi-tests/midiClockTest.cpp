#include <unistd.h>
#include <stdio.h>

#include "WProgram.h"

#include "Midi.h"

class ClockHandler : public ClockCallback {
public:
	void on16th(uint32_t count) {
		printf("count: %lu\n", count);
		fflush(stdout);
	}
};

ClockHandler handler;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		MidiUartHostClass::listInputMidiDevices();
		MidiUartHostClass::listOutputMidiDevices();
		return 1;
	}

	MidiUart.init(atoi(argv[1]), atoi(argv[2]));
	MidiClock.mode = MidiClock.EXTERNAL_MIDI;
	MidiClock.addOn16Callback(&handler, (midi_clock_callback_ptr_t)&ClockHandler::on16th);
	MidiClock.start();

  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
			//			printf("%x\n", c);
      Midi.handleByte(c);
    }
    //    read_clock();
    usleep(1000);
  }

	return 0;
								
}
