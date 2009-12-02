#include <SDCard.h>

void setup() {
  uint8_t ret = SDCard.init();
  if (ret != 0) {
    GUI.printf_fill("FAT %b", ret);
    return;
  }
  uint8_t buf[] = "foo";
  if (!SDCard.writeFile("/test", buf, 4)) {
    GUI.printf_fill("WRITE ERROR");
    return;
  }
  uint8_t buf2[4];
  ret = SDCard.readFile("/test", buf2, 4);
  if (ret != 4) {
    GUI.printf_fill("READ ERROR");
    return;
  }
  bool correct = true;
  for (uint8_t i = 0; i < 4; i++) {
    if (buf[i] != buf2[i]) {
      GUI.printf_fill("READ AT %b", i);
      return;
    }
  }
  
  GUI.printf_fill("SDCARD OK");
}
