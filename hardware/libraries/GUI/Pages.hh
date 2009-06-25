#ifndef PAGES_H__
#define PAGES_H__


#include "Encoders.hh"
#include "ModalGui.hh"

class Page {
 public:
  char name[17];
  char shortName[4];
  bool redisplay;
  Sketch *sketch;

  Page(char *_name = NULL, char *_shortName = NULL) {
    sketch = NULL;
    redisplay = false;
    setName(_name);
    setShortName(_shortName);
  }

  void setName(char *_name = NULL) {
    if (_name != NULL) {
      m_strncpy(name, _name, 17);
    } else {
      name[0] = '\0';
    }
  }

  void setShortName(char *_shortName = NULL) {
    if (_shortName != NULL) {
      m_strncpy(shortName, _shortName, 4);
    } else {
      shortName[0] = '\0';
    }
  }
  
  virtual void handle() { }
  virtual void update();
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

class SwitchPage : public Page {
public:
  Page *pages[4];

  SwitchPage(char *_name = "SELECT PAGE",
	     Page *p1 = NULL, Page *p2 = NULL, Page *p3 = NULL, Page *p4 = NULL) :
    Page(_name) {
    initPages(p1, p2, p3, p4);
  }

  void initPages(Page *p1 = NULL,  Page *p2 = NULL, Page *p3 = NULL, Page *p4 = NULL) {
    pages[0] = p1;
    pages[1] = p2;
    pages[2] = p3;
    pages[3] = p4;
  }

  virtual void display();
  virtual bool handleEvent(gui_event_t *event);
};


#endif /* PAGES_H__ */
