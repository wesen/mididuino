#include <GUI.h>
#include <MD.h>

RangeEncoder flfEncoder(0, 127, "FLF");
RangeEncoder flwEncoder(0, 127, "FLW");
RangeEncoder fbEncoder(0, 127, "FB");
RangeEncoder levEncoder(0, 127, "LEV");

RangeEncoder timEncoder(0, 127, "TIM");
RangeEncoder frqEncoder(0, 127, "FRQ");
RangeEncoder modEncoder(0, 127, "MOD");

EncoderPage page;
EncoderPage page2;

void on16Callback() {
}

void setup() {
  page.encoders[0] = &flfEncoder;
  page.encoders[1] = &flwEncoder;
  page.encoders[2] = &fbEncoder;
  page.encoders[3] = &levEncoder;

  page2.encoders[0] = &timEncoder;
  page2.encoders[1] = &frqEncoder;
  page2.encoders[2] = &modEncoder;
  page2.encoders[3] = &fbEncoder;

  flfEncoder.setValue(0);
  timEncoder.setValue(24);
  modEncoder.setValue(0);
  frqEncoder.setValue(0);
  flwEncoder.setValue(127);
  fbEncoder.setValue(32);
  levEncoder.setValue(100);

  GUI.setPage(&page);
  
  MidiClock.mode = MidiClock.EXTERNAL;
  MidiClock.transmit = false;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();
}

void loop() {
  page.handle();
  
  if (flfEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FLTF, flfEncoder.getValue());
  if (flwEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FLTW, flwEncoder.getValue());
  if (fbEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_FB, fbEncoder.getValue());
  if (levEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_LEV, levEncoder.getValue());
  if (timEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_TIME, timEncoder.getValue());
  if (frqEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_MFRQ, frqEncoder.getValue());
  if (modEncoder.hasChanged())
    MD::setEchoParam(MD_ECHO_MOD, modEncoder.getValue());
  
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    GUI.setPage(&page);
  } else if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    GUI.setPage(&page2);
  }
}
