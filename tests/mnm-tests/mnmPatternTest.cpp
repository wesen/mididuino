#include <unistd.h>
#include <stdlib.h>

#include "Platform.h"
#include <MNM.h>

bool compareMonoSysex(uint8_t *buf, uint8_t *buf2, uint16_t len) {
  uint8_t buftmp[8192];
  uint8_t buftmp2[8192];
  MNMSysexDecoder decoder(DATA_ENCODER_INIT(buf + 9, len - 11));
  decoder.get(buftmp + 1, len - 10);
  MNMSysexDecoder decoder2(DATA_ENCODER_INIT(buf2 + 10, len - 10));
  decoder2.get(buftmp2 + 1, len - 10);
  for (uint16_t i = 1; i < len - 10 ; i++) {
    if (buftmp[i] != buftmp2[i]) {
      printf("mono sysex different at 0x%x, %x != %x\n", i + 10, buftmp[i], buftmp2[i]);
      return false;
    } else {
      //      printf("%x: %x == %x\n", i + 10, buftmp[i], buftmp2[i]);
    }
  }
  return true;
}

class MNMHandler : public MNMCallback {
public:
  void onMNMKit() {
    MNMKit kit;
    printf("received\n");
    hexdump(MidiSysex.data, MidiSysex.recordLen);

    if (kit.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      printf("parsed mnm kit sysex\n");
      kit.print();
      uint8_t buf[8192];
      uint16_t retlen = kit.toSysex(buf, sizeof(buf));
      if (compareMonoSysex(MidiSysex.data, buf, 0x2ba)) {
        printf("Buffer comparison successful!\n");
      }
      retlen = 0;
      //      hexdump(buf+1, retlen - 2);
    } else {
      printf("error parsing mnm kit message\n");
    }
  }

  void onMNMGlobal() {
    MNMGlobal global;
    printf("received\n");
    hexdump(MidiSysex.data, MidiSysex.recordLen);

    if (global.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      printf("parsed mnm global sysex\n");
      global.print();
      uint8_t buf[8192];
      uint16_t retlen = global.toSysex(buf, sizeof(buf));
      if (compareMonoSysex(MidiSysex.data, buf, 0x110)) {
        printf("Buffer comparison successful!\n");
      }
      hexdump(buf+1, retlen - 2);
    } else {
      printf("error parsing mnm kit message\n");
    }
    MNM.requestPattern(0);
  }
  
  void onMNMPattern() {
    MNMPattern pattern;
    printf("received\n");
    hexdump(MidiSysex.data, MidiSysex.recordLen);
		
    if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
#if 0
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

      uint8_t buf[8192];
      uint16_t retlen = pattern.toSysex(buf, sizeof(buf));
      if (compareMonoSysex(MidiSysex.data, buf, 0x1978)) {
        printf("Buffer comparison successful!\n");
        //        MidiUart.sendRaw(buf, retlen);
      }
      printf("retlen: %d\n", retlen);
      hexdump(buf + 1, retlen - 2);
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

    MNM.requestKit(2);
  }
};

int main(void) {
  MNMSysexListener.setup();

  MNMHandler mnmHandler;
  MNMSysexListener.addOnPatternMessageCallback(&mnmHandler, (mnm_callback_ptr_t)&MNMHandler::onMNMPattern);
  MNMSysexListener.addOnKitMessageCallback(&mnmHandler, (mnm_callback_ptr_t)&MNMHandler::onMNMKit);
  MNMSysexListener.addOnGlobalMessageCallback(&mnmHandler, (mnm_callback_ptr_t)&MNMHandler::onMNMGlobal);

  //  MidiUart.init("Elektron TM-1", "Elektron TM-1");
  MidiUart.init("Port 1 - Generic", "Port 1 - Generic");
  MNM.requestGlobal(0);

  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }
    usleep(1000);
  }
}
