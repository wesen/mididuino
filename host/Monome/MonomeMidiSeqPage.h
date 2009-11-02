#ifndef MONOME_MIDI_SEQ_PAGE_H__
#define MONOME_MIDI_SEQ_PAGE_H__

#include "WProgram.h"
#include "Monome.h"
#include "Midi.h"

#include "MonomeSequencer.h"

extern void switchPage(uint8_t page);

extern void clearTrack(uint8_t track);

class MonomeMidiSeqPage : public MonomePage, public ClockCallback {
 public:
	uint8_t start;
	MonomeSequencer *sequencer;

	bool clearTrackMode;
	bool muteMode;

	MonomeMidiSeqPage(MonomeParentClass *_monome, MonomeSequencer *_sequencer, uint8_t _start):
	MonomePage(_monome), sequencer(_sequencer) {
    y = 0;
    height = 7;
		start = _start;
		muteMode = false;
		clearTrackMode = false;
  }

  virtual void setup() {
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)(&MonomeMidiSeqPage::on16Note));
  }

	virtual void hide() {
		muteMode = false;
		clearTrackMode = false;
	}
	
	virtual void show() {
		muteMode = false;
		clearTrackMode = false;
		for (uint8_t i = 0; i < 6; i++) {
			drawTrack(i);
		}
	}

	void muteTrack(uint8_t track) {
		sequencer->toggleMuteTrack(track);
		drawTrack(track);
	}

	void clearPage() {
		for (uint8_t i = 0; i < 6; i++) {
			clearTrack(i);
		}
	}
	
	void clearTrack(uint8_t track) {
		::clearTrack(track);
		drawTrack(track);
	}

	void drawTrack(uint8_t track) {
		if (!sequencer->isTrackMuted(track)) {
			for (uint8_t i = 0; i < 8; i++) {
				setLED(i, track + 1, sequencer->isTrackTrig(track, i + start) ? 1 : 0);
			}
		} else {
			for (uint8_t i = 0; i < 8; i++) {
				setLED(i, track + 1, sequencer->isTrackTrig(track, i + start) ? 0 : 1);
			}
		}
	}

  virtual bool handleEvent(monome_event_t *evt) {
		if (evt->y == 0) {
			switch(evt->x) {
			case 0:
				clearTrackMode = evt->state;
				break;

			case 1:
				clearPage();
				break;

			case 7:
				muteMode = evt->state;
				break;
			}

			return true;
		}

		if (IS_BUTTON_PRESSED(evt)) {
			if (clearTrackMode) {
				//				printf("clear %d\n", evt->y - 1);
				clearTrack(evt->y - 1);
			} else if (muteMode) {
				//				printf("mute %d\n", evt->y - 1);	
				//				muteTrack(evt->y - 1);
			} else {
				uint8_t y = evt->y - 1;
				if (sequencer != NULL) {
					sequencer->toggleTrackTrig(y, evt->x + start);
					drawTrack(y);
				}
			}
			
			return true;
		}

		return false;
  }

  void on16Note(uint32_t pos) {
		uint16_t pos2 = (pos / 2) % 16;

		if ((pos % 2) != 0)
			return;
		
		// XXX IGITT
		if (start == 0 && pos2 == 8 && monome->currentPage() == this) {
			setLED(7, 0, 0);
			switchPage(2);
			return;
		}
		if (start == 8 && pos2 == 0 && monome->currentPage() == this) {
			setLED(7, 0, 0);
			switchPage(1);
		}

		//		printf("page: %p pos2 %d\n", this, pos2);
		
		if ((pos2 >= start) && (pos2 < (start + 16))) {
			uint8_t previdx = (pos2 - 1) % 8;
			if (pos2 == 0) {
				previdx = 7;
			}
			uint8_t idx = pos2 % 8;

			//			printf("previdx: %d idx %d\n", previdx, idx);
			setLED(previdx, 0, 0);
			setLED(idx, 0, 1);
		}
	}
};

#endif /* MONOME_MIDI_SEQ_PAGE_H__ */
