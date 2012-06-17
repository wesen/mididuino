#include "Midi.h"

class ForwardPrgChangeSketch : public Sketch, public MidiCallback {
  CCEncoder encs[4];
  EncoderPage page;

public:
  ForwardPrgChangeSketch() :
    page(&encs[0], &encs[1], &encs[2], &encs[3]) {

  }

  void setup() {
    Midi.addOnProgramChangeCallback(this, (MidiCallback::callback_ptr_t)&ForwardPrgChangeSketch::onProgramChange);
    setPage(&page);
  }

  void onProgramChange(uint8_t *msg) {
    MidiUart.sendProgramChange(MIDI_VOICE_CHANNEL(msg[0]), msg[1]);
  }
};


ForwardPrgChangeSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}
