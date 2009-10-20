#include "WProgram.h"

#include <inttypes.h>
#include <unistd.h>

#include "MidiUartHost.h"

int main(void) {
  MidiUartHostClass::listInputMidiDevices();
  MidiUartHostClass::listOutputMidiDevices();

  MidiUart.init(0, 0);
	MidiUart.putc(0x90);
	MidiUart.putc(0x10);
	MidiUart.putc(0x00);

  return 0;
}
