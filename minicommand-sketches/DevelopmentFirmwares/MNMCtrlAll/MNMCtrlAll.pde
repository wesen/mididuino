#include <Platform.h>
#include <MNM.h>

void onControlChange(uint8_t *msg) {
  uint8_t param;
  uint8_t track;

  if (BUTTON_DOWN(Buttons.BUTTON1)) {
    MNM.parseCC(msg[0] & 0xF, msg[1], &track, &param);
    for (uint8_t i = 0; i < 6; i++) {
      if (i != track) {
        MNM.setParam(i, param, msg[2]);
      }
    }
  }
}

void setup() {
  initMNMTask();
}
