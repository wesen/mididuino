#include "WProgram.h"

#include "MidiUartHost.h"

MidiClass Midi, Midi2;
MidiUartHostClass MidiUart;

void handleIncomingMidi() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
}

void MidiUartHostParent::onOutputMessage(uint8_t *msg, uint8_t len) {
	printf("output message %d bytes\n", len);
	if (len > 3) {
		midiSendLong(msg, len);
	} else {
		switch (len) {
		case 0:
			return;
		case 1:
			midiSendShort(msg[0], 0, 0);
			break;
		case 2:
			midiSendShort(msg[0], msg[1], 0);
			break;
		case 3:
			midiSendShort(msg[0], msg[1], msg[2]);
			break;
		default:
			return;
		}
	}
}

void MidiUartHostParent::init(int _inputDevice, int _outputDevice) {
	inputDevice = _inputDevice;
	outputDevice = _outputDevice;
    
	outputMidi.init();
	outputMidi.addOnMessageCallback(this,
																	(midi_callback_ptr2_t)&MidiUartHostParent::onOutputMessage);

}

