#ifndef SKETCH_H__
#define SKETCH_H__

#include "Pages.hh"
#include "Stack.h"

class Sketch {
public:

  Stack<Page *, 4> pageStack;
  char *name;

  Sketch(char *_name = NULL) {
    name = _name;
  }

  void setPage(Page *page) {
    pageStack.reset();
    pushPage(page);
  }
  
  void pushPage(Page *page) {
    page->redisplayPage();
    pageStack.push(page);
  }

  void popPage() {
    pageStack.pop(NULL);
    Page *page = currentPage();
    if (page != NULL)
      page->redisplayPage();
  }

  Page *currentPage() {
    Page *page = NULL;
    pageStack.peek(&page);
    return page;
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
  virtual void close() {
  }
};


#endif /* SKETCH_H__ */
