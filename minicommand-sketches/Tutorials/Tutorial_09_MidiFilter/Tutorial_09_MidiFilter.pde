#include <MidiTools.h>

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
	Midi.setOnNoteOnCallback(onNoteOnCallback);
	Midi.setOnNoteOffCallback(onNoteOffCallback);
}

void loop() {
	GUI.updatePage();
	GUI.update();
}

void handleGui() {
}
