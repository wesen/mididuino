#include <GUI.h>
#include <MD.h>

RangeEncoder flfEncoder(0, 127);
RangeEncoder flwEncoder(0, 127);
RangeEncoder fbEncoder(0, 127);
RangeEncoder levEncoder(0, 127);

EncoderPage page;

void on16Callback() {
}

void setup() {
  page.encoders[0] = &flfEncoder;
  page.encoders[1] = &flwEncoder;
  page.encoders[2] = &fbEncoder;
  page.encoders[3] = &levEncoder;
  flfEncoder.setValue(0);
  flwEncoder.setValue(127);
  fbEncoder.setValue(32);
  levEncoder.setValue(100);
  
  LCD.line1("FLF FLW FB  LEV ");
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  if (flfEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FLTF, flfEncoder.getValue());
  if (flwEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FLTW, flwEncoder.getValue());
  if (fbEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FB, fbEncoder.getValue());
  if (levEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_LEV, levEncoder.getValue());
  
  GUI.setLine(GUI.LINE2);  
  page.display();

  cli();
  page.handle();
  sei();

  GUI.update();
}

void handleGui() {
  page.update();
}
