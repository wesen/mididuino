const static int LFO_SPD_SCALE = 16;

char *lfoTypes[] = {
  "NO ",
  "TRI",
  "SQR",
  "RND"
};

EnumEncoder typeEncoder(lfoTypes, 4, "TYP");
RangeEncoder amtEncoder(0, 127, "AMT");
RangeEncoder spdEncoder(0, 127, "SPD");


class LFOConfigPage : public EncoderPage {
public:
  LFO *currentLFO;

  LFOConfigPage() {
    currentLFO = NULL;
    encoders[0] = &typeEncoder;
    encoders[1] = &amtEncoder;
    encoders[2] = &spdEncoder;
  }

  virtual void display(bool redisplay = true) {
    EncoderPage::display(redisplay);
    if (typeEncoder.hasChanged()) {
      currentLFO->type = (lfo_type_t)typeEncoder.getValue();
    }
    if (amtEncoder.hasChanged()) {
      currentLFO->amount = amtEncoder.getValue();
    }
    if (spdEncoder.hasChanged()) {
      currentLFO->setSpeed(spdEncoder.getValue() * LFO_SPD_SCALE);
    }
  }
  
  void editLFO(LFO *lfo, Encoder *encoder) {
    currentLFO = lfo;
    typeEncoder.setValue((int)lfo->type);
    amtEncoder.setValue(lfo->amount);
    spdEncoder.setValue(lfo->inc / LFO_SPD_SCALE);
    encoders[3] = encoder;
  }

  virtual bool handleGui() {
    if (BUTTON_RELEASED(Buttons.BUTTON2)) {
      GUI.setPage(&pages[currentCCPage]);
    }
    return true;
  }  
};

LFOConfigPage lfoConfigPage;

void showLFOPage(LFO *lfo, Encoder *encoder) {
  lfoConfigPage.editLFO(lfo, encoder);
  GUI.setPage(&lfoConfigPage);
}

void initLFO(LFO *lfo) {
  lfo->setSpeed(0);
  lfo->acc = 0;
  lfo->amount = 0;
  lfo->type = LFO_NONE;
}

void on32Callback() {
  for (int page = 0; page < 4; page++) {
    for (int encoder = 0; encoder < 4; encoder++) {
      lfos[page][encoder].tick();
      if (lfos[page][encoder].hasChanged()) {
        uint8_t value = constrain(ccEncoders[page][encoder].realCur + lfos[page][encoder].getScaledValue(), 0, 127);
        ccEncoders[page][encoder].setValue(value, true);
        updateCCEncoder(ccs[page][encoder], channels[page][encoder], value);
      }
      lfos[page][encoder].update();
    }
  }
}

void initLFOs() {
  MidiClock.mode = MidiClock.EXTERNAL;

//  MidiClock.mode = MidiClock.INTERNAL;
//  MidiClock.setTempo(100);
//  MidiClock.transmit = true;
  MidiClock.setOn32Callback(on32Callback);
  MidiClock.start();
  for (int page = 0; page < 4; page++) {
    for (int i = 0; i < 4; i++) {
      initLFO(&lfos[page][i]);
    }
  }
}


