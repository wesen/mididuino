#ifndef PAGES_H__
#define PAGES_H__


#include "Encoders.hh"
#include "ModalGui.hh"

class Page {
 public:
  Page() { }
  virtual void handle() { }
  virtual void update() { }
  virtual void clear()  { }
  virtual void display(bool redisplay = false) { }
  virtual bool handleGui() { return false; }
};

class Encoder;

class EncoderPage : public Page {
 public:
  Encoder *encoders[GUI_NUM_ENCODERS];
  
  EncoderPage(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL, Encoder *e4 = NULL) {
    for (uint8_t i; i < GUI_NUM_ENCODERS; i++) {
      encoders[i] = NULL;
    }
    encoders[0] = e1;
    encoders[1] = e2;
    encoders[2] = e3;
    encoders[3] = e4;
  }
  virtual void handle();
  virtual void update();
  virtual void clear();
  virtual void display(bool redisplay = false);
  void displayNames();
};


#endif /* PAGES_H__ */
