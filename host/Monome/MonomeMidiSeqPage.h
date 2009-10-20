#ifndef MONOME_MIDI_SEQ_PAGE_H__
#define MONOME_MIDI_SEQ_PAGE_H__

#include "WProgram.h"
#include "Monome.h"
#include "Midi.h"

extern void switchPage(uint8_t page);

class MonomeMidiSeqPage : public MonomePage, public ClockCallback {
 public:
  uint8_t tracks[6];
	uint8_t start;
  
	MonomeMidiSeqPage(MonomeParentClass *_monome, uint8_t _start): MonomePage(_monome) {
    y = 0;
    height = 7;
    for (uint8_t i = 0; i < countof(tracks); i++) {
      tracks[i] = 0;
    }
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
			TOGGLE_BIT(tracks[y], evt->x);
			setLED(evt->x, evt->y, IS_BIT_SET(tracks[y], evt->x) ? 1 : 0);
			return true;
			//			printf("evt x: %d y: %d: %d\n", evt->x, evt->y, evt->state);
		}

		return false;
  }

  void on16Note(uint32_t pos) {
		uint16_t pos2 = pos % 32;
		// XXX IGITT
		if (start == 0 && pos2 == 16 && monome->currentPage() == this) {
			
			setLED(7, 0, 0);
			switchPage(4);
			return;
		}
		if (start == 16 && pos2 == 0 && monome->currentPage() == this) {
			setLED(7, 0, 0);
			switchPage(3);
		}
		if ((pos2 >= start) && (pos2 < (start + 16))) {
			if ((pos % 2) == 0) {
				uint8_t previdx = ((pos - 1) / 2) % 8;
				uint8_t idx = (pos / 2) % 8;
				
				//      for (uint8_t i = 0; i < 8; i++) {
				//				setLED(i, 0, i == idx ? 1 : 0);
				//      }
				setLED(previdx, 0, 0);
				setLED(idx, 0, 1);
				
				for (uint8_t i = 0; i < countof(tracks); i++) {
					if (IS_BIT_SET(tracks[i], previdx)) {
						MidiUart.sendNoteOn(60 + i, 0);
					}
					if (IS_BIT_SET(tracks[i], idx)) {
						MidiUart.sendNoteOn(60 + i, 100);
					}
				}
			}
		}
	}
};

#endif /* MONOME_MIDI_SEQ_PAGE_H__ */
