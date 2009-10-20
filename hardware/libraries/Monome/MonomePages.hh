#ifndef MONOME_PAGES_H__
#define MONOME_PAGES_H__

#include "WProgram.h"
#include <inttypes.h>
#include "Callback.hh"
#include "Stack.h"

class MonomeParentClass;
class MonomePageContainer;

class MonomePage {
public:
	bool isSetup;
	uint8_t buf[8];
	bool needsRefresh;
	MonomeParentClass *monome;
  MonomePageContainer *parent;

	MonomePage(MonomeParentClass *_monome);

	virtual bool handleEvent(monome_event_t *evt) {
		return false;
	}
	virtual void setup() {
		isSetup = true;
	}
	virtual void show() {
	}
	virtual void redisplayPage() {
		needsRefresh = true;
	}
	virtual void hide() {
	}
	virtual void loop() {
	}
	
	uint8_t getBufLED(uint8_t x, uint8_t y) {
		if (IS_BIT_SET(buf[y], x)) {
			return 1;
		} else {
			return 0;
		}
	}
	void setLED(uint8_t x, uint8_t y, uint8_t status = 1);
	void toggleLED(uint8_t x, uint8_t y);
	void clearLED(uint8_t x, uint8_t y) {
		setLED(x, y, 0);
	}
};

class MonomePageContainer {
 public:
  Stack<MonomePage *, 8> pageStack;

  virtual bool handleTopEvent(monome_event_t *event) {
    return false;
  }

  void setPage(MonomePage *page) {
    pageStack.reset();
    pushPage(page);
  }
  
  void pushPage(MonomePage *page) {
    page->parent = this;
    if (!page->isSetup) {
      page->setup();
      page->isSetup = true;
    }
		page->redisplayPage();
		page->show();
    pageStack.push(page);
  }

  void popPage(MonomePage *page) {
    if (currentPage() == page) {
      popPage();
    }
  }
  
  void popPage() {
    MonomePage *page;
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

  virtual MonomePage *currentPage() {
    MonomePage *page = NULL;
    pageStack.peek(&page);
    return page;
  }

};



#endif /* MONOME_PAGES_H__ */
