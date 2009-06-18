#include <LFO.h>

LFOClass lfo1;

void on96Callback() {
  lfo1.tick();
  if (lfo1.hasChanged()) {
    MidiUart.sendCC(0, lfo1.getValue());
  }
  lfo1.update();
}

RangeEncoder minEncoder(0, 127, "MIN");
RangeEncoder maxEncoder(0, 127, "MAX");
RangeEncoder spdEncoder(0, 100, "SPD");

EncoderPage page(&minEncoder, &maxEncoder, &spdEncoder);

void setup() {
  MidiClock.mode = MidiClock.INTERNAL;
  MidiClock.setTempo(100);
  MidiClock.transmit = true;
  MidiClock.setOn96Callback(on96Callback);
  lfo1.setSpeed(10);
  lfo1.type = LFO_TRIANGLE;
  GUI.setPage(&page);
}

void loop() {
  GUI.updatePage();

  if (minEncoder.hasChanged()) {
  }
  if (maxEncoder.hasChanged()) {
  }
  if (spdEncoder.hasChanged()) {
    lfo1.setSpeed(spdEncoder.getValue() * 8);
  }
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    if (MidiClock.isStarted()) {
      MidiClock.stop();
    } 
    else {
      MidiClock.start();
    }
  }
}
