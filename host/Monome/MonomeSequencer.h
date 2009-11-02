#ifndef MONOME_SEQ_H__
#define MONOME_SEQ_H__

#include "WProgram.h"
#include "Midi.h"

class MonomeSequencer : public ClockCallback {
 public:
	uint8_t basePitch;
	uint8_t len;
	uint32_t tracks[6];
	bool tracksMuted[6];
	bool tracksTriggered[6];

	MonomeSequencer(uint8_t _len, uint8_t _basePitch = 60) {
		len = _len;
		basePitch = _basePitch;
		for (uint8_t i = 0; i < countof(tracks); i++) {
			tracks[i] = 0;
			tracksTriggered[i] = false;
			tracksMuted[i] = false;
		}
	}

	void setup() {
		MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)(&MonomeSequencer::on16Callback));
	}

	void muteTrack(uint8_t track) {
		tracksMuted[track] = true;
	}

	void unmutedTrack(uint8_t track) {
		tracksMuted[track] = true;
	}

	void toggleMuteTrack(uint8_t track) {
		tracksMuted[track] = !tracksMuted[track];
	}

	bool isTrackMuted(uint8_t track) {
		return tracksMuted[track];
	}		

	void triggerOffTrack(uint8_t track, uint8_t step) {
		if (!isTrackTrig(track, step) && (tracksTriggered[track])) {
			tracksTriggered[track] = false;
			//			MidiUart.sendNoteOn(basePitch + track, 0); // keine noten fuer haye
			MidiUart.sendCC(2, basePitch + track, 0);
		}
	}

	void triggerTrack(uint8_t track, uint8_t step) {
		if (isTrackTrig(track, step) && !tracksMuted[track]) {
			if (!tracksTriggered[track]) {
				tracksTriggered[track] = true;
				//				MidiUart.sendNoteOn(basePitch + track, 100); // keine noten fuer haye
				MidiUart.sendCC(2, basePitch + track, 127);
			}
		} else {
			triggerOffTrack(track, step);
		}
	}

	void toggleTrackTrig(uint8_t track, uint8_t step) {
		//		printf("toggle track %d, step %d\n", track, step);
		TOGGLE_BIT(tracks[track], step);
		triggerOffTrack(track, step);
	}

	void setTrackTrig(uint8_t track, uint8_t step) {
		SET_BIT(tracks[track], step);
		triggerOffTrack(track, step);
	}

	void clearTrackTrig(uint8_t track, uint8_t step) {
		CLEAR_BIT(tracks[track], step);
		triggerOffTrack(track, step);
	}

	bool isTrackTrig(uint8_t track, uint8_t step) {
		return IS_BIT_SET(tracks[track], step);
	}

	void on16Callback(uint32_t pos) {
		if (pos % 2 != 0)
			return;

		uint8_t step = (pos / 2) % len;
		uint8_t step2 = step % 4;
		if (step2 == 0) {
			MidiUart.sendNoteOn(65, 100);
		} else if (step2 == 2) {
			MidiUart.sendNoteOn(65, 0);
		}
		
		//		printf("step %d\n", step);
		for (uint8_t i = 0; i < countof(tracks); i++) {
			triggerTrack(i, step);
		}
	}
};

#endif /* MONOME_SEQ_H__ */
