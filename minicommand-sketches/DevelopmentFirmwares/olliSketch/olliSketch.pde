#include <MD.h>

/* 
 * Track6 DEC, Ctrl-All Hipass, Ctrl-All REVERB, Ctrl-All Delay 
 * Track9 DEC, Ctrl-All Hipass, Ctrl-All REVERB, Ctrl-All Delay 
 */
 
#define MD_TRACK_6        5  /* on track 6 */
#define MD_TRACK_9        8  /* on track 9 */
#define MD_TRACK_CTRL_ALL 12 /* on track 13 */

MDEncoder track6Dec(MD_TRACK_6, MODEL_P2, "6DC");
MDEncoder track9Dec(MD_TRACK_9, MODEL_P2, "9DC");
MDEncoder allHipass(MD_TRACK_CTRL_ALL, MODEL_FLTF, "FLF");
MDEncoder allReverb(MD_TRACK_CTRL_ALL, MODEL_REV, "REV");
MDEncoder allDelay(MD_TRACK_CTRL_ALL, MODEL_DEL, "DEL");

EncoderPage page1(&track6Dec, &allHipass, &allReverb, &allDelay);
EncoderPage page2(&track9Dec, &allHipass, &allReverb, &allDelay);

void setup() {
  GUI.setPage(&page1);
}

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    GUI.setPage(&page1);
    return true;
  }
  if (EVENT_PRESSED(evt, Buttons.BUTTON2)) {
    GUI.setPage(&page2);
    return true;
  }
  return false;
}
