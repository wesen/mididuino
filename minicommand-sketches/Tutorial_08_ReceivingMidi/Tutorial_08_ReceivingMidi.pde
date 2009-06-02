RangeEncoder param1(0, 127, "P1");
RangeEncoder param2(0, 127, "P2");
EncoderPage page(&param1, &param2);

void onCCCallback(uint8_t *msg) {
	if (MIDI_VOICE_CHANNEL(msg[0]) == MidiUart.currentChannel) {
		if (msg[1] == 1) {
			param1.setValue(msg[2]);
		} else if (msg[1] == 2) {
			param2.setValue(msg[2]);
 		}
	}
}

void setup() {
	Midi.setOnControlChangeCallback(onCCCallback);
	GUI.setPage(&page);
}

void loop() {
	GUI.updatePage();
	if (param1.hasChanged()) {
		MidiUart.sendCC(1, param1.getValue());
	}
	if (param2.hasChanged()) {
		MidiUart.sendCC(2, param2.getValue());
	}
	GUI.update();
}

void handleGui() {
}
