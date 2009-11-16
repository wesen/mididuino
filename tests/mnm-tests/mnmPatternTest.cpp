#include <unistd.h>
#include <stdlib.h>

#include <WProgram.h>
#include <MNM.h>

class MNMHandler : public MNMCallback {
public:
	void onMNMPattern() {
		MNMPattern pattern;
		if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
			printf("parsed mnm pattern sysex\n");
			pattern.print();
			pattern.setTrig(2, 0);
			uint8_t buf[8192];
			uint16_t retlen = pattern.toSysex(buf, sizeof(buf));
			printf("retlen: %d\n", retlen);
			hexdump(buf, retlen);
			MidiUart.sendRaw(buf, retlen);
		} else {
			printf("error parsing mnm sysex message\n");
		}
	}
};

int main(void) {
	MNMSysexListener.setup();

	MNMHandler mnmHandler;
	MNMSysexListener.addOnPatternMessageCallback(&mnmHandler, (mnm_callback_ptr_t)&MNMHandler::onMNMPattern);

	MidiUart.init(0, 0);
	MNM.requestPattern(0);

	for (;;) {
		MidiUart.runLoop();
		while (MidiUart.avail()) {
			uint8_t c = MidiUart.getc();
			Midi.handleByte(c);
		}
		usleep(1000);
	}
}
