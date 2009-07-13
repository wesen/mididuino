#include <inttypes.h>
#include <unistd.h>

#include "MidiUartOSX.h"

MidiUartOSXClass MidiUart;
MidiClass Midi;

void onCCCallback(uint8_t *msg) {
  printf("cc %d: %d received\n", msg[1], msg[2]);
}

int main(void) {
  MidiUartOSXClass::listInputMidiDevices();
  MidiUartOSXClass::listOutputMidiDevices();

  MidiUart.init(0, 0);
  MidiUart.sendCC(0, 100, 100);
  MidiUart.midiSendShort(0xB0, 100, 100);

  Midi.setOnControlChangeCallback(onCCCallback);
  
  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }
    usleep(1000);
  }

  return 0;
}
