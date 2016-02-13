#include "Platform.h"
#include "Midi.h"
#include "MidiUartHost.h"

int main(void) {
  MidiUart.init(1, 1);

  for (int i = 0; i < 20; i++) {
    MidiUart.sendProgramChange(i);
    MidiUart.runLoop();
    delay(100);
  }

  for (int i = 0; i < 20; i++) {
    MidiUart.runLoop();
    delay(200);
  }


  return 0;
}
