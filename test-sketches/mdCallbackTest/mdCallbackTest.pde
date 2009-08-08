#include <MD.h>
#include "Callback.hh"

class CBTestSketch : public Sketch, public Callback {
  public:
  CBTestSketch() {
  }
  
  virtual void onCallback(uint16_t type, void *obj) {
    GUI.flash_strings_fill("CB", "");
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, type);
  }
  
  virtual void setup() {
    MDTask.setup();
    MDTask.kitChangeHandler.add(this);
  }
  
  virtual void destroy() {
  }
  
  virtual void loop() {
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    return false;
  }
};

CBTestSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
