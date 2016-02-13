#include <AutoEncoderPage.h>
#include <CCHandler.h>
#include <MidiClockPage.h>

AutoEncoderPage<CCEncoder> encoderPage[4];
SwitchPage switchPage;

void setup() {
  ccHandler.setup();
  
  uint8_t param_idx = 0;
  for (uint8_t page_idx = 0; page_idx < 4; page_idx++) {
    char pageName[4] = "PG ";
    pageName[2] = page_idx + '0';
    encoderPage[page_idx].setShortName(pageName);
    encoderPage[page_idx].setup();

    for (uint8_t encoder_idx = 0; encoder_idx < 4; encoder_idx++) {
       char name[4] = "P00"; 
       name[1] = (param_idx / 10) + '0';
       name[2] = (param_idx % 10) + '0';

       CCEncoder *encoder = &encoderPage[page_idx].realEncoders[encoder_idx];
       encoder->setName(name);
       encoder->initCCEncoder(0, param_idx);
       
       param_idx++;
    }
    
    encoderPage[page_idx].isSetup = true;
    encoderPage[page_idx].redisplay = true;
  }
  switchPage.initPages(&encoderPage[0], &encoderPage[1], &encoderPage[2], &encoderPage[3]);

  GUI.setPage(&encoderPage[0]);
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


