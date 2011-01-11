#include <MD.h>

MDTempoEncoder foo("TMP", 130);
MDTrackFlashEncoder foo2("TRK", 0, true);
EncoderPage page(&foo, &foo2);

void setup() {
  GUI.setPage(&page);
}
