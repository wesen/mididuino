#ifndef SKETCH_H__
#define SKETCH_H__

#include "Pages.hh"
#include "Stack.h"

class Sketch : public PageContainer {
public:

  char *name;

  Sketch(char *_name = NULL) {
    name = _name;
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
};

extern Sketch _defaultSketch;

#endif /* SKETCH_H__ */
