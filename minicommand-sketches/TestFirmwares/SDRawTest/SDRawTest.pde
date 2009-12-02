#include <SDCard.h>

void setup() {
  if (!sd_raw_init()) {
    GUI.printf_fill("RAW ERROR");
  } else {
    GUI.printf_fill("RAW OK");
  }
}
