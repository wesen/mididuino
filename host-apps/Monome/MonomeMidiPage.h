/*
 * MidiCtrl - Monome Midi Page for visuals application
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */


#ifndef MONOME_MIDI_PAGE_H__
#define MONOME_MIDI_PAGE_H__

#include "PlatformConfig.h"
#include "Monome.h"
#include "Midi.h"


#include "MonomeTrigPage.h"
#include "MonomeSequencer.h"

class MonomeMidiPage : public MonomeTrigPage, public ClockCallback {
 public:
  MonomeSequencer *sequencer;
  bool trackPressed[6];
  uint8_t trackStartPoint[6];
  uint32_t trackStartTime[6];

  bool clearTrackMode;
  bool muteMode;

  void clearTrack(uint8_t track);
  void clearPage();
	
  MonomeMidiPage(MonomeParentClass *monome, MonomeSequencer *_sequencer);
  virtual bool handleEvent(monome_event_t *evt);
  virtual void onTrigNote(uint8_t pitch);
  virtual void onTrigNoteOff(uint8_t pitch);
  virtual void setup();
  void on16Note(uint32_t pos);

  virtual void show();
  virtual void hide();
};

#endif /* MONOME_MIDI_PAGE_H__ */

