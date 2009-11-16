#include <unistd.h>
#include <stdlib.h>

#include <WProgram.h>
#include <MNM.h>

bool compareMonoSysex(uint8_t *buf, uint8_t *buf2) {
	uint8_t buftmp[8192];
	uint8_t buftmp2[8192];
	uint16_t len = 0x1978;
	MNMSysexDecoder decoder(DATA_ENCODER_INIT(buf + 9, len - 11));
	decoder.get(buftmp + 1, len - 10);
	MNMSysexDecoder decoder2(DATA_ENCODER_INIT(buf2 + 10, len - 10));
	decoder2.get(buftmp2 + 1, len - 10);
	for (uint16_t i = 1; i < len - 10 ; i++) {
		if (buftmp[i] != buftmp2[i]) {
			printf("mono sysex different at 0x%x, %x != %x\n", i + 10, buftmp[i], buftmp2[i]);
			return false;
		} else {
			printf("%x: %x == %x\n", i + 10, buftmp[i], buftmp2[i]);
		}
	}
	return true;
}


class MNMHandler : public MNMCallback {
public:
	void onMNMPattern() {
		MNMPattern pattern;
		printf("received\n");
		hexdump(MidiSysex.data, MidiSysex.recordLen);
		
		if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
#if 1
			printf("parsed mnm pattern sysex\n");
			pattern.print();
			pattern.setAllTrig(2, 0);
			pattern.noteNBR[2][0] = 10;
			pattern.setAmpTrig(2, 0);
			pattern.setFilterTrig(2, 0);
			pattern.setLFOTrig(2, 0);
			pattern.setTriglessTrig(2, 0);
			printf("after setting\n");
			pattern.print();
#endif

			printf("chordNotes[191] %p\n", &pattern.chordNotes[191]);
			uint8_t buf[8192];
			printf("buf %p chordNotes[191] %p\n", buf, &pattern.chordNotes[191]);
			uint16_t retlen = pattern.toSysex(buf, sizeof(buf));
			compareMonoSysex(MidiSysex.data, buf);
			printf("retlen: %d\n", retlen);
			hexdump(buf + 1, retlen - 2);
			MidiUart.sendRaw(buf, retlen);
#if 0
			if (pattern.fromSysex(buf + 6, retlen - 7)) {
				printf("reimported\n");
				pattern.print();
			} else {
				printf("reimport error\n");
			}
#endif
			
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
