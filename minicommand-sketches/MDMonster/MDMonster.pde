#include <MD.h>
#include <Sequencer.h>
#include <MidiTools.h>

class SubSketch {
  public:
    SubSketch() { };
    virtual void setup() { };
    virtual void loop() { };
    virtual void handleGui() { };
    virtual void destroy() { };
    virtual PGM_P getName() { return PSTR(""); };
};

SubSketch *currentSketch = NULL;

void loop() {
  if (currentSketch != NULL) 
    currentSketch->loop();
}

void handleGui() {
  bool everyDown = true;
  bool onePressed = false;
  for (int i = Buttons.BUTTON1; i <= Buttons.BUTTON4; i++) {
    if (BUTTON_PRESSED(i))
      onePressed = true;
    if (BUTTON_UP(i))
      everyDown = false;
  }
  if (everyDown && onePressed)
    setup();
  if (currentSketch != NULL)
    currentSketch->handleGui();
}
