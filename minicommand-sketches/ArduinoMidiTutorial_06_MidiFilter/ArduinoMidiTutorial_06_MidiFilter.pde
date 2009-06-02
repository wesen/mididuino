#include <MidiUart.h>
#include <Midi.h>
#include <MidiTools.h>
MidiClass Midi;

void onNoteOnCallback(uint8_t *msg) {
  MidiUart.sendNoteOn(MIDI_VOICE_CHANNEL(msg[0]),
			scalePitch(msg[1], 0, majorScale),
			msg[2]);
}

void onNoteOffCallback(uint8_t *msg) {
  MidiUart.sendNoteOff(MIDI_VOICE_CHANNEL(msg[0]),
  			 scalePitch(msg[1], 0, majorScale),
			 msg[2]);
}

void setup() {
  MidiUart.init();  
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  Midi.setOnNoteOffCallback(onNoteOffCallback);
}

void loop() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
}
