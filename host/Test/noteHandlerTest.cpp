#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include "NoteHandler.h"
#include "WProgram.h"
#include <helpers.h>

struct NoteHandlerFixture {
  NoteHandlerFixture() {
  }

  NoteHandler noteHandler;
  uint8_t pitches[8];
  uint8_t velocities[8];
};

TEST_F (NoteHandlerFixture, EmptyNoteHandler) {
  uint8_t num = noteHandler.getLastPressedNotes(pitches, velocities, countof(pitches));
  CHECK_EQUAL(0, num);
}
