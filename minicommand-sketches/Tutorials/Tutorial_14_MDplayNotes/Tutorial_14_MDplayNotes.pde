#include <MD.h>

MDMelodicTrackFlashEncoder trackEncoder("TRK");
EncoderPage page(&trackEncoder);

void onNoteOn2(uint8_t *msg) {
  uint8_t track = trackEncoder.getValue();
  if (MD.isMelodicTrack(track)) {
    MD.sendNoteOn(track, msg[1], msg[2]);
  }
}

void setup() {
  initMDTask();
  GUI.setPage(&page);
}


