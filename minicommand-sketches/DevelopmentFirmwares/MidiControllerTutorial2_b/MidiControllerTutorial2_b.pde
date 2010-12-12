#include <AutoEncoderPage.h>
#include <CCHandler.h>
#include <MidiClockPage.h>

AutoEncoderPage<AutoNameCCEncoder> encoderPage;

void setup() {
  ccHandler.setup();
  
  encoderPage.setup();
  encoderPage.realEncoders[0].initCCEncoder(0, 0);
  encoderPage.realEncoders[0].setName("P1");
  encoderPage.realEncoders[1].initCCEncoder(0, 1);
  encoderPage.realEncoders[1].setName("P2");
  encoderPage.realEncoders[2].initCCEncoder(0, 2);
  encoderPage.realEncoders[2].setName("P3");
  encoderPage.realEncoders[3].initCCEncoder(0, 3);
  encoderPage.realEncoders[3].setName("P4");
  
  encoderPage.isSetup = true;
  
  GUI.setPage(&encoderPage);
  
  initClockPage();
}


