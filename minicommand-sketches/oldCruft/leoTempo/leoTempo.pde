RangeEncoder tempoEncoder(30, 300, "TMP");

EncoderPage page(&tempoEncoder);

int tempos[128] = { 0 } ;

void initTempos() {
  for (int i = 0; i < 128; i++) {
    tempos[i] = -1;
  }
}

void on16Callback() {
  int i = MidiClock.div16th_counter % 128;
  if (BUTTON_DOWN(Buttons.BUTTON2)) {
    tempos[i] = tempoEncoder.getValue();
  }
  
    if (tempos[i] >= 30) {
      MidiClock.setTempo(tempos[i]);
      tempoEncoder.setValue(tempos[i]);
    }
}

void setup() {
  GUI.setPage(&page);
  MidiClock.mode = MidiClock.INTERNAL;
  tempoEncoder.setValue(100);
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.transmit = true;
  initTempos();
}

void loop() {
  GUI.updatePage();
  if (tempoEncoder.hasChanged()) {
    MidiClock.setTempo(tempoEncoder.getValue());
  }
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MidiClock.start();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("CLEAR RECORD");
    initTempos();
  }
}
