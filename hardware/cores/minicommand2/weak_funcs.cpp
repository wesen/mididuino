#include "WProgram.h"
#include "Midi.h"
#include <GUI.h>

void __attribute__((weak)) setup() {
}

void __attribute__((weak)) loop() {
}

void __attribute__((weak)) onNoteOn(uint8_t *msg) {
}

void __attribute__((weak)) onNoteOff(uint8_t *msg) {
}

void __attribute__((weak)) onControlChange(uint8_t *msg) {
}

void __attribute__((weak)) on16Callback(uint32_t pos) {
}

void __attribute__((weak)) on32Callback(uint32_t pos) {
}

bool __attribute__((weak)) handleEvent(gui_event_t *evt) {
	return false;
}

class DefaultCallbacks : public MidiCallback, public ClockCallback {
public:
	void onNoteOn(uint8_t *msg) {
		::onNoteOn(msg);
	}
	void onNoteOff(uint8_t *msg) {
		::onNoteOff(msg);
	}
	void onControlChange(uint8_t *msg) {
		::onControlChange(msg);
	}
	void on16Callback(uint32_t pos) {
		::on16Callback(pos);
	}
	void on32Callback(uint32_t pos) {
		::on32Callback(pos);
	}

	void setupMidiCallbacks() {
		Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOn);
		Midi.addOnNoteOffCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOff);
		Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onControlChange);
	}

	void setupClockCallbacks() {
		MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&DefaultCallbacks::on16Callback);
	}
	
};

DefaultCallbacks defaultCallbacks;

void __attribute__((weak)) setupEventHandlers() {
	GUI.addEventHandler(&handleEvent);
}

void __attribute__((weak)) setupMidiCallbacks() {
	defaultCallbacks.setupMidiCallbacks();
}

void __attribute__((weak)) setupClockCallbacks() {
	defaultCallbacks.setupClockCallbacks();
}
