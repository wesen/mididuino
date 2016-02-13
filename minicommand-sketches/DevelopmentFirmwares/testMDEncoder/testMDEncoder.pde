#include <MD.h>

MDEncoder testEncoder(0, 0, "P00");
EncoderPage page(&testEncoder);

void setup() {
  GUI.setPage(&page);
}
