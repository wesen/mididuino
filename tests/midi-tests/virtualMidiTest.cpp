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

int main(uint8_t argc, char *argv[]) {
  VirtualMidi midi;
  MidiCallbacks callbacks;

  midi.MidiClass::addOnNoteOnCallback(&callbacks, (midi_callback_ptr_t)&MidiCallbacks::onNoteOn);
  midi.MidiClass::addOnNoteOffCallback(&callbacks, (midi_callback_ptr_t)&MidiCallbacks::onNoteOff);

  midi.sendNoteOn(MIDI_NOTE_A0, 100);
  midi.sendNoteOff(MIDI_NOTE_A0);

  return 0;
}
