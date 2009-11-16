#include <MidiTools.h>

RangeEncoder lengthEncoder(1, 16, "LEN");
uint8_t melodyNotes[16] = { 0 };

void randomizeNotes() {
	for (int i = 0; i < countof(melodyNotes); i++) {
		melodyNotes[i] = scalePitch(48 + random(24), 0, majorScale);
	}
}

uint8_t prevNote = 0;
void on16Callback() {
	MidiUart.sendNoteOff(prevNote);
	prevNote = melodyNotes[MidiClock.div16th_counter %
				   lengthEncoder.getValue()];
	MidiUart.sendNoteOn(prevNote, 100);
}

EncoderPage page(&lengthEncoder);

void setup() {
	randomizeNotes();
	lengthEncoder.setValue(8);
	MidiClock.mode = MidiClock.INTERNAL;
	MidiClock.setTempo(100);
	MidiClock.setOn16Callback(on16Callback);
	MidiClock.start();
        GUI.setPage(&page);
}

void loop() {
	GUI.updatePage();
	GUI.update();
}

void handleGui() {
	if (BUTTON_PRESSED(Buttons.BUTTON1)) {
		randomizeNotes();
	}
}
