#ifndef MONOME_MIDI_SEQ_PAGE_H__
#define MONOME_MIDI_SEQ_PAGE_H__

#include "WProgram.h"
#include "Monome.h"
#include "Midi.h"

#include "MonomeSequencer.h"

extern void switchPage(uint8_t page);

class MonomeMidiSeqPage : public MonomePage, public ClockCallback {
 public:
	uint8_t start;
	MonomeSequencer *sequencer;
  
	MonomeMidiSeqPage(MonomeParentClass *_monome, MonomeSequencer *_sequencer, uint8_t _start):
	MonomePage(_monome), sequencer(_sequencer) {
    y = 0;
    height = 7;
		start = _start;
  }

  virtual void setup() {
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)(&MonomeMidiSeqPage::on16Note));
  }

  virtual bool handleEvent(monome_event_t *evt) {
		if (evt->y == 0)
			return true;
		
		if (IS_BUTTON_PRESSED(evt)) {
			uint8_t y = evt->y - 1;
			if (sequencer != NULL) {
				sequencer->toggleTrackTrig(y, evt->x + start);
				setLED(evt->x, evt->y, sequencer->isTrackTrig(y, evt->x + start) ? 1 : 0);
			}
			return true;
		}

		return false;
  }

  void on16Note(uint32_t pos) {
		uint16_t pos2 = pos % 32;
		// XXX IGITT
		if (start == 0 && pos2 == 16 && monome->currentPage() == this) {
			setLED(7, 0, 0);
			switchPage(2);
			return;
		}
		if (start == 16 && pos2 == 0 && monome->currentPage() == this) {
			setLED(7, 0, 0);
			switchPage(1);
		}

		if ((pos2 >= start) && (pos2 < (start + 16))) {
			if ((pos % 2) == 0) {
				uint8_t previdx = ((pos - 1) / 2) % 8;
				uint8_t idx = (pos / 2) % 8;
				
				setLED(previdx, 0, 0);
				setLED(idx, 0, 1);
			}
		}
	}
};

#endif /* MONOME_MIDI_SEQ_PAGE_H__ */
