#include <MidiTools.h>
#include <MD.h>

void setup() {
  LCD.line1("MD NOTE TEST");
  MD.kit.machines[0].model = EFM_SD_MODEL;
  MD.kit.machines[1].model = EFM_SD_MODEL;
  MD.kit.machines[2].model = EFM_SD_MODEL;
  MD.kit.machines[3].model = EFM_SD_MODEL;
}

void loop() {
}

void handleGui() {
  uint8_t i;
  for (i = 0; i < 4; i++) {
    if (BUTTON_PRESSED(i + 4))
      MD.sendNoteOn(i, scalePitch(random(47, 80), 47, majorScale), 100);
  }
}
