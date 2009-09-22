#ifndef PAGES_H__
#define PAGES_H__

#include "Stack.h"
#include "Encoders.hh"
#include "ModalGui.hh"

class Sketch;
class PageContainer;

class Page {
 public:
  char name[17];
  char shortName[4];
  bool redisplay;
  PageContainer *parent;
  bool isSetup;

  Page(const char *_name = NULL, const char *_shortName = NULL) {
    parent = NULL;
    redisplay = false;
    setName(_name);
    setShortName(_shortName);
    isSetup = false;
  }

  void setName(const char *_name = NULL) {
    if (_name != NULL) {
      m_strncpy(name, _name, 17);
    } else {
      name[0] = '\0';
    }
  }

  void setShortName(const char *_shortName = NULL) {
    if (_shortName != NULL) {
      m_strncpy(shortName, _shortName, 4);
    } else {
      shortName[0] = '\0';
    }
  }
  
  virtual void update();
  virtual void loop() { }
  virtual void display() { }
  virtual void finalize() { }
  
  virtual void clear()  { }
  virtual void show() { }
  virtual void hide() { }
  void redisplayPage();
  virtual bool handleEvent(gui_event_t *event) {
    return false;
  }
  virtual void setup() { }

#ifdef HOST_MIDIDUINO
  virtual ~Page() { }
#endif
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
  virtual void update();
  virtual void clear();
  virtual void display();
  virtual void finalize();
  
  void displayNames();
};

class SwitchPage : public Page {
public:
  Page *pages[4];

  SwitchPage(const char *_name = "SELECT PAGE:",
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

  virtual void loop() { };
  virtual void display();
  virtual bool handleEvent(gui_event_t *event);
};

class EncoderSwitchPage : public SwitchPage {
public:
  EncoderSwitchPage(Page *p1 = NULL, Page *p2 = NULL, Page *p3 = NULL, Page *p4 = NULL) :
    SwitchPage(NULL, p1, p2, p3, p4) {
  }

  virtual void display();
  virtual bool handleEvent(gui_event_t *event);
};


class ScrollSwitchPage : public EncoderPage {
public:
  Vector<Page *, 8> pages;
  RangeEncoder pageEncoder;

  ScrollSwitchPage() : pageEncoder(0, 0) {
    pageEncoder.pressmode = true;
    encoders[0] = &pageEncoder;
  }

  void addPage(Page *page);

  bool setSelectedPage();
  virtual void display();
  virtual void loop();
  virtual bool handleEvent(gui_event_t *event);
};

class PageContainer {
 public:
  Stack<Page *, 8> pageStack;

  virtual bool handleTopEvent(gui_event_t *event) {
    return false;
  }

  void setPage(Page *page) {
    pageStack.reset();
    pushPage(page);
  }
  
  void pushPage(Page *page) {
    page->parent = this;
    if (!page->isSetup) {
      page->setup();
      page->isSetup = true;
    }
    page->redisplayPage();
    page->show();
    pageStack.push(page);
  }

  void popPage(Page *page) {
    if (currentPage() == page) {
      popPage();
    }
  }
  
  void popPage() {
    Page *page;
    pageStack.pop(&page);
    if (page != NULL) {
      page->parent = NULL;
      page->hide();
    }

    page = currentPage();
    if (page != NULL) {
      page->redisplayPage();
    }
  }

  virtual Page *currentPage() {
    Page *page = NULL;
    pageStack.peek(&page);
    return page;
  }

#ifdef HOST_MIDIDUINO
  virtual ~PageContainer() { }
#endif
};

#endif /* PAGES_H__ */
