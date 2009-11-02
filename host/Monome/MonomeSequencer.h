#ifndef MONOME_SEQ_H__
#define MONOME_SEQ_H__

#include "WProgram.h"
#include "Midi.h"

class MonomeSequencer : public ClockCallback {
 public:
	uint8_t basePitch;
	uint8_t len;
	uint32_t tracks[6];
	bool tracksTriggered[6];

	MonomeSequencer(uint8_t _len, uint8_t _basePitch = 60) {
		len = _len;
		basePitch = _basePitch;
		for (uint8_t i = 0; i < countof(tracks); i++) {
			tracks[i] = 0;
			tracksTriggered[i] = false;
		}
	}

	void setup() {
		MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)(&MonomeSequencer::on16Callback));
	}

	void toggleTrackTrig(uint8_t track, uint8_t step) {
		TOGGLE_BIT(tracks[track], step);
		triggerTrack(track, step);
	}

	void triggerTrack(uint8_t track, uint8_t step) {
		if (isTrackTrig(track, step)) {
			tracksTriggered[track] = true;
			MidiUart.sendNoteOn(basePitch + track, 100);
		} else if (tracksTriggered[track]) {
			tracksTriggered[track] = false;
			MidiUart.sendNoteOn(basePitch + track, 0);
		}
	}

	void setTrackTrig(uint8_t track, uint8_t step) {
		SET_BIT(tracks[track], step);
		triggerTrack(track, step);
	}

	void clearTrackTrig(uint8_t track, uint8_t step) {
		CLEAR_BIT(tracks[track], step);
		triggerTrack(track, step);
	}

	bool isTrackTrig(uint8_t track, uint8_t step) {
		return IS_BIT_SET(tracks[track], step);
	}

	void on16Callback(uint32_t pos) {
		uint8_t step = pos % len;
		for (uint8_t i = 0; i < countof(tracks); i++) {
			triggerTrack(i, step);
		}
	}
};

#endif /* MONOME_SEQ_H__ */
