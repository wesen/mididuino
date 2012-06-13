#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <ListPool.hh>
#include <Midi.h>
#include <VirtualMidi.h>

struct MidiCallbackFixture {
  VirtualMidi midi;

  MidiCallbackFixture() {
    midi.init();
    midi.initSerial();
  }
};


TEST_F (MidiCallbackFixture, TestNoteOn) {
  class CB : public MidiCallback {
  public:
    bool noteReceived;
    CB() : noteReceived(false) {
    };

    void onNoteOn(uint8_t *msg) {
      printf("on note on\n");
      noteReceived = true;
    }
  } cb;

  ADD_CALLBACK(midi.MidiClass::addOnNoteOnCallback, cb, onNoteOn);
  midi.sendNoteOn(MIDI_NOTE_A0, 100);
}
