#include <inttypes.h>
#include <unistd.h>

#include "MidiUartOSX.h"
#include "MNM.h"

MidiUartOSXClass MidiUart;
MidiClass Midi;

void onCCCallback(uint8_t *msg) {
  printf("cc %d: %d received\n", msg[1], msg[2]);
}

void onKitChanged() {
  printf("kit changed: %s\n", MNM.kit.name);
}

int main(void) {
  MidiUartOSXClass::listInputMidiDevices();
  MidiUartOSXClass::listOutputMidiDevices();

  MidiUart.init(0, 0);

  Midi.setOnControlChangeCallback(onCCCallback);

  MNMTask.setup();
  MNMTask.autoLoadKit = true;
  MNMTask.autoLoadGlobal = true;
  MNMTask.reloadGlobal = true;
  MNMTask.addOnKitChangeCallback(onKitChanged);

  MNM.requestKit(0);
  
  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }
    //    read_clock();
    usleep(1000);
  }

  return 0;
}
