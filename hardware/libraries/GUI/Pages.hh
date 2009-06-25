#ifndef PAGES_H__
#define PAGES_H__


#include "Encoders.hh"
#include "ModalGui.hh"

class Page {
 public:
  char name[2][17];
  bool redisplay;
  Sketch *sketch;

  Page() { sketch = NULL; redisplay = false; }
  virtual void handle() { }
  virtual void update() { }
  virtual void clear()  { }
  virtual void display() { }
  virtual void show() { }
  virtual void hide() { }
  void redisplayPage() {
    GUI.setLine(GUI.LINE1);
    GUI.clearLine();
    GUI.setLine(GUI.LINE2);
    GUI.clearLine();
    redisplay = true;
  }
  virtual bool handleEvent(gui_event_t *event) {
    return false;
  }
};

class Encoder;

class EncoderPage : public Page {
 public:
  Encoder *encoders[GUI_NUM_ENCODERS];
  
  EncoderPage(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL, Encoder *e4 = NULL) {
    setEncoders(e1, e2, e3, e4);
  }
  
  void setEncoders(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL, Encoder *e4 = NULL) {
    encoders[0] = e1;
    encoders[1] = e2;
    encoders[2] = e3;
    encoders[3] = e4;
  }
  virtual void handle();
  virtual void update();
  virtual void clear();
  virtual void display();
  void displayNames();
};


#endif /* PAGES_H__ */
