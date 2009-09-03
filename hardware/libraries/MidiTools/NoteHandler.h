#ifndef NOTEHANDLER_H__
#define NOTEHANDLER_H__

#include "Midi.h"
#include "ListPool.hh"

typedef struct incoming_note_s {
  uint8_t channel;
  uint8_t pitch;
  uint8_t velocity;
} incoming_note_t;

class NoteHandler {
 public:
  ListPool<incoming_note_t, 8> incomingNotes;

  NoteHandler() {
  }

  void setup();
  void destroy();
  uint8_t getLastPressedNotes(uint8_t *pitches, uint8_t *velocities, uint8_t maxNotes = 8);
  uint8_t getLastPressedNotesOrderPitch(uint8_t *pitches, uint8_t *velocities, uint8_t maxNotes = 8);
  void onNoteOnCallback(uint8_t *msg);
  void onNoteOffCallback(uint8_t *msg);
};

#endif /* NOTEHANDLER_H__ */
