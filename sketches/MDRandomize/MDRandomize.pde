#include <MD.h>
#include <GUI.h>

RangeEncoder trackEncoder(0, 15, "TRK");
RangeEncoder modelEncoder(0, 200, "MDL");
EncoderPage trackPage;

md_machine_t machine = {
  TRX_SD_MODEL,
  
  { 0, 64, 0, 64, 
    0, 64, 0, 64,
 
    0, 64, 0, 64, 
    0, 64, 0, 64,
 
    0, 64, 0, 64, 
    0, 64, 0, 64,
  },
  
  { 0, 64, 0, 64,
    0, 64, 0, 64 }
};

void setup() {
  trackPage.encoders[0] = &trackEncoder;
  trackPage.encoders[1] = &modelEncoder;
  GUI.setPage(&trackPage);
}

void loop() {
  GUI.updatePage();
  
  if (modelEncoder.hasChanged()) {
    PGM_P name = getMachineName(modelEncoder.getValue());
    if (name == NULL) {
      name = PSTR("XXX");
    }
    GUI.put_p_string_at_fill(8, name);
  }  
  GUI.update();
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    MD.assignMachine(trackEncoder.getValue(), modelEncoder.getValue());
  }
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    machine.model = modelEncoder.getValue();
    MD.setMachine(trackEncoder.getValue(), &machine);
  }
}
