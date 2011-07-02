#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include "NoteHandler.h"
#include "WProgram.h"
#include <helpers.h>

struct NoteHandlerFixture {
  NoteHandlerFixture() {
    for (int i = 0; i < countof(pitches); i++) {
      pitches[i] = 255;
      velocities[i] = 255;
    }
  }

  NoteHandler noteHandler;
  uint8_t pitches[8];
  uint8_t velocities[8];
};

TEST_F (NoteHandlerFixture, EmptyNoteHandler) {
  uint8_t num = noteHandler.getLastPressedNotes(pitches, velocities, countof(pitches));
  CHECK_EQUAL(0, num);
}

TEST_F (NoteHandlerFixture, OneNoteOnHandler) {
  uint8_t note[3] = { MIDI_NOTE_ON, 100, 100 };
  noteHandler.onNoteOnCallback(note);
  uint8_t num = noteHandler.getLastPressedNotes(pitches, velocities, countof(pitches));
	//  CHECK_EQUAL(1, num);
	//  CHECK_EQUAL(100, pitches[0]);
	//  CHECK_EQUAL(100, velocities[0]);
}

TEST_F (NoteHandlerFixture, OneNoteOnNoteOff) {
  uint8_t note[3] = { MIDI_NOTE_ON, 100, 100 };
  noteHandler.onNoteOnCallback(note);
  uint8_t num = noteHandler.getLastPressedNotes(pitches, velocities, countof(pitches));
	//  CHECK_EQUAL(1, num);
	//  CHECK_EQUAL(100, pitches[0]);
	//  CHECK_EQUAL(100, velocities[0]);
  note[0] = MIDI_NOTE_OFF;
  noteHandler.onNoteOffCallback(note);
  num = noteHandler.getLastPressedNotes(pitches, velocities, countof(pitches));
	//  CHECK_EQUAL(0, num);
}
