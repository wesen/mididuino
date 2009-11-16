#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include <MD.h>

#include "hexdump.hh"
#include "../pattern-tests/PrintMDPattern.h"

class MDHandler : public MDCallback {
public:
	void onMDPattern() {
		PrintMDPattern pattern;
		if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
			printf("parsed pattern sysex\n");
			pattern.print();
			pattern.setTrig(2, 0);
			uint8_t buf[8192];
			uint16_t retlen = pattern.toSysex(buf, sizeof(buf));
			printf("retlen: %d\n", retlen);
			//			hexDump(buf, retlen);
			MidiUart.sendRaw(buf, retlen);
		} else {
			printf("error parsing sysex message\n");
		}
	}
};

int main(void) {
	PrintMDPattern pattern;
	pattern.print();
	MDSysexListener.setup();
	
	MDHandler mdHandler;
	MDSysexListener.addOnPatternMessageCallback(&mdHandler, (md_callback_ptr_t)&MDHandler::onMDPattern);
	
  MidiUart.init(0, 0);

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
