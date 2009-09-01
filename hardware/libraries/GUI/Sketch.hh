#ifndef SKETCH_H__
#define SKETCH_H__

#include "Pages.hh"
#include "Stack.h"

class Sketch : public PageContainer {
public:

  char *name;
  bool muted;
  bool monster;

  Sketch(char *_name = NULL) {
    name = _name;
    muted = false;
    monster = false;
  }

  virtual void setupMonster(bool _monster) {
    monster = _monster;
    if (monster) {
      muted = true;
    }
    setup();
  }
  
  virtual void setup() {
  }

  virtual bool handleEvent(gui_event_t *event) {
    return false;
  }

  bool handleTopEvent(gui_event_t *event) {
    Page *curPage = currentPage();
    if (curPage != NULL) {
      if (curPage->handleEvent(event)) {
	return true;
      }
    }
    return handleEvent(event);
  }
  
  virtual void loop() {
  }
  virtual void destroy() {
  }

  virtual void show() {
  }

  virtual void hide() {
  }

  virtual void doExtra(bool pressed) {
  }

  virtual void mute(bool pressed) {
    if (pressed) {
      muted = !muted;
    }
  }

  virtual Page *getPage(uint8_t i) {
    return NULL;
  }

  virtual bool handleSwitchEvent(bool pressed) {
    return false;
  }

  virtual void getName(char *n1, char *n2) {
    n1[0] = '\0';
    n2[0] = '\0';
  }

};

class SketchSwitchPage : public Page {
public:
  Sketch *sketches[4];
  Page *tmpPage;

  SketchSwitchPage(char *_name = "SELECT SKETCH:",
		   Sketch *s1 = NULL, Sketch *s2 = NULL, Sketch *s3 = NULL, Sketch *s4 = NULL) :
    Page(_name) {
    tmpPage = NULL;
    sketches[0] = s1;
    sketches[1] = s2;
    sketches[2] = s3;
    sketches[3] = s4;
  }

  virtual void loop() { };
  virtual void display();
  virtual bool handleEvent(gui_event_t *event);

  virtual bool handlePopEvent(gui_event_t *event);
  virtual bool handleGlobalEvent(gui_event_t *event);
};

extern Sketch _defaultSketch;

#endif /* SKETCH_H__ */
