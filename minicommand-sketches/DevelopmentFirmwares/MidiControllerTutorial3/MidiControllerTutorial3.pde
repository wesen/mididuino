CCEncoder encoders[4];
EncoderPage encoderPage(&encoders[0], &encoders[1], &encoders[2], &encoders[3]);
CCEncoder encoders2[4];
EncoderPage encoderPage2(&encoders2[0], &encoders2[1], &encoders2[2], &encoders2[3]);

SwitchPage switchPage;

void setup() {
  encoders[0].initCCEncoder(0, 0);
  encoders[0].setName("P1");
  encoders[1].initCCEncoder(0, 1);
  encoders[1].setName("P2");
  encoders[2].initCCEncoder(0, 2);
  encoders[2].setName("P3");
  encoders[3].initCCEncoder(0, 3);
  encoders[3].setName("P4");

  encoders2[0].initCCEncoder(1, 0);
  encoders2[0].setName("P5");
  encoders2[1].initCCEncoder(1, 1);
  encoders2[1].setName("P6");
  encoders2[2].initCCEncoder(1, 2);
  encoders2[2].setName("P7");
  encoders2[3].initCCEncoder(1, 3);
  encoders2[3].setName("P8");

  encoderPage.setup();
  encoderPage.setShortName("PG1");
  encoderPage2.setup();
  encoderPage2.setShortName("PG2");
  
  switchPage.initPages(&encoderPage, &encoderPage2);

  GUI.setPage(&encoderPage);
}

bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    GUI.pushPage(&switchPage);
    return true;
  } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
    GUI.popPage(&switchPage);
    return true;
  }
  
  return false;
}


