uint8_t notes[] = { 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60 };

class FilterSketch : public Sketch, MidiCallback {
  public:

  EncoderPage page1;
  EncoderPage page2;

RangeEncoder ins[4];
RangeEncoder outs[4];

  FilterSketch() {
    for (int i = 0; i < 4; i++) {
      char *buf = "IN_";
      char *obuf = "OU_";
      obuf[2] = buf[2] = '1'  + i;
      ins[i].initRangeEncoder(0, 15, buf);
      outs[i].initRangeEncoder(0, 7, obuf);
    }
    page1.setEncoders(&ins[0], &ins[1], &ins[2], &ins[3]);
    page2.setEncoders(&outs[0], &outs[1], &outs[2], &outs[3]);
    setPage(&page1);
  }
  
  uint8_t inputTrack(uint8_t pitch) {
    for (int i = 0; i < 16; i++) {
      if (notes[i] == pitch)
      return i;
    }
    return 255;
  }
  
  void onNoteCallback(uint8_t *msg) {
    uint8_t input = inputTrack(msg[1]);
    if (input == 255)
    return;
    for (int i = 0; i < 4; i++) {
      if (page1.encoders[i]->getValue() == input) {
        uint8_t newPitch = page2.encoders[i]->getValue() + 30;
        msg[1] = newPitch;
        if (msg[2] > 0) {
          MidiUart.puts(msg, 3);
        } else {
          delay(30);
          MidiUart.puts(msg, 3);
        }
      }
    }
  }

  void onNoteOffCallback(uint8_t *msg) {
    msg[2] = 0;
    onNoteCallback(msg);
  }
  
  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&page2);
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&page2);
    }
    
    for (int i = Buttons.ENCODER1; i < Buttons.ENCODER4; i++) {
      if (EVENT_PRESSED(event, i)) {
        MidiUart.sendNoteOn(page2.encoders[i - Buttons.ENCODER1]->getValue() + 30, 100);
      } else if (EVENT_RELEASED(event, i)) {
        MidiUart.sendNoteOn(page2.encoders[i - Buttons.ENCODER1]->getValue() + 30, 0);
      }
    }
  }
  
  void setup() {
    Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&FilterSketch::onNoteCallback);
    Midi.addOnNoteOffCallback(this, (midi_callback_ptr_t)&FilterSketch::onNoteOffCallback);
  }
};

FilterSketch filterSketch;

void setup() {
  filterSketch.setup();
  GUI.setSketch(&filterSketch);
}

void loop() {
}
