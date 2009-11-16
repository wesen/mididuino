#include <MNM.h>
#include <MidiEuclidSketch.h>
#include <MNMPatternEuclidSketch.h>

bool eventHandler(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    MNMPattern pattern;
    pattern.init();
    uint8_t buf[8192];
    uint16_t enclen = pattern.toSysex(buf, sizeof(buf));
    MidiUart.puts(buf, enclen);
    MidiUart.sendNoteOn(80, 80);
  }
}
void setup() {
  MidiUart.sendNoteOn(100, 100);
  delay(300); delay(300);
  GUI.addEventHandler(eventHandler);
}

void loop() {
}

