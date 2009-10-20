#include "WProgram.h"

#include <inttypes.h>
#include <unistd.h>

#include "MidiUartHost.h"

int main(void) {
  MidiUartHostClass::listInputMidiDevices();
  MidiUartHostClass::listOutputMidiDevices();

  MidiUart.init(0, 0);

	MidiUart.putc(0x90);
	MidiUart.putc(0x01);
	MidiUart.putc(0x02);
	
	MidiUart.putc(0xF0);
	MidiUart.putc(0x00);
	MidiUart.putc(0x01);
	MidiUart.putc(0x02);
	MidiUart.putc(0x03);
	MidiUart.putc(0x04);
	MidiUart.putc(0xF7);
	
  return 0;
}
