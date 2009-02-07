#include <MD.h>

void onStatusResponseCallback(uint8_t type, uint8_t value) {
  LCD.line1("STATUS");
  LCD.line2();
  LCD.putnumberx(type);
  LCD.puts(" ");
  LCD.putnumber(value);
}

void onKitMessageCallback() {
  LCD.line1("KIT   ");
}

void onGlobalMessageCallback() {
  LCD.line1("GLOBAL");
}

void setup() {
  MDSysex.setup();
  MDSysex.setOnStatusResponseCallback(onStatusResponseCallback);
  MDSysex.setOnKitMessageCallback(onKitMessageCallback);
  MDSysex.setOnGlobalMessageCallback(onGlobalMessageCallback);
}

void loop() {
}

void handleGui() {
  if (BUTTON_PRESSED(Buttons.BUTTON1))
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
  if (BUTTON_PRESSED(Buttons.BUTTON2))
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
  if (BUTTON_PRESSED(Buttons.BUTTON4)) 
    MD.sendRequest(MD_KIT_REQUEST_ID, MD.currentKit);
  if (BUTTON_PRESSED(Buttons.BUTTON3))
    MD.sendRequest(MD_GLOBAL_REQUEST_ID, MD.currentGlobal);
}
