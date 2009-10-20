#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include <MD.h>

class MDHandler : public MDCallback {
public:
	void onMDPattern() {
		MDPattern pattern;
		if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
			printf("parsed pattern sysex\n");
		} else {
			printf("error parsing sysex message\n");
		}
	}
};

int main(void) {
	MDSysexListener.setup();
	
	MDHandler mdHandler;
	MDSysexListener.addOnPatternMessageCallback(&mdHandler, (md_callback_ptr_t)&MDHandler::onMDPattern);
	
  MidiUart.init(3, 3);

	int i =MD.getCurrentKit(1000);
	printf("kit: %d\n", i);
	MD.requestPattern(0);

  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
			Midi.handleByte(c);
    }
		usleep(1000);
  }
	

}
