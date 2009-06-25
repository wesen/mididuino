#ifndef SKETCH_H__
#define SKETCH_H__

#include "Pages.hh"
#include "Stack.h"

class Sketch {
public:

  Stack<Page *, 4> pageStack;
  Page *modalPage;
  char *name;

  Sketch(char *_name = NULL) {
    name = _name;
  }

  void setModalPage(Page *_modalPage) {
    if (modalPage != NULL) {
      return;
    }
    modalPage = _modalPage;
    modalPage->sketch = this;
    modalPage->redisplayPage();
    modalPage->show();
  }

  void clearModalPage(Page *_modalPage = NULL) {
    if (modalPage == _modalPage) {
      modalPage->hide();
      modalPage = NULL;
      currentPage()->redisplayPage();
    }
  }
  
  void setPage(Page *page) {
    pageStack.reset();
    pushPage(page);
  }
  
  void pushPage(Page *page) {
    page->sketch = this;
    page->redisplayPage();
    page->show();
    pageStack.push(page);
  }

  void popPage() {
    Page *page;
    pageStack.pop(&page);
    if (page != NULL) {
      page->sketch = NULL;
      page->hide();
    }

    page = currentPage();
    if (page != NULL) {
      page->redisplayPage();
    }
  }

  Page *currentPage() {
    Page *page = NULL;
    if (modalPage != NULL)
      return modalPage;
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
  virtual void destroy() {
  }
};


#endif /* SKETCH_H__ */
