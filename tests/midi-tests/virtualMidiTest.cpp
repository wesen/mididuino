#include "Platform.h"

#include <inttypes.h>
#include <unistd.h>

#include "VirtualMidi.h"

class MidiCallbacks : public MidiCallback {
public:
  void onNoteOn(uint8_t *msg) {
    printf("on note on\n");
  }

  void onNoteOff(uint8_t *msg) {
    printf("on note off\n");
  }
};

int main(int argc, char *argv[]) {
  VirtualMidi midi;
  MidiCallbacks callbacks;

  ADD_CALLBACK(midi.MidiClass::addOnNoteOnCallback, callbacks, onNoteOn);
  ADD_CALLBACK(midi.MidiClass::addOnNoteOffCallback, callbacks, onNoteOff);

  midi.sendNoteOn(MIDI_NOTE_A0, 100);
  midi.sendNoteOff(MIDI_NOTE_A0);

  return 0;
}
