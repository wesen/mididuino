#ifndef MD_RANDOMIZE_SKETCH_H__
#define MD_RANDOMIZE_SKETCH_H__

#include <MDRandomizePage.h>

class MDRandomizeSketch : 
  public Sketch, public MDCallback {

public:
  MDRandomizerClass MDRandomizer;
  MDRandomizePage randomizePage;

  MDRandomizeSketch() : randomizePage(&MDRandomizer) {
  }
  
  void setup() {
    MDRandomizer.setup();
    MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDRandomizeSketch::onKitChanged);
  }

  void onKitChanged() {
    MDRandomizer.onKitChanged();
  }


  virtual void show() {
    if (currentPage() == NULL)
      setPage(&randomizePage);
  }

  virtual void mute(bool pressed) {
  }

  virtual void doExtra(bool pressed) {
    randomizePage.randomize();
  }

  virtual Page *getPage(uint8_t i) {
    if (i == 0) {
      return &randomizePage;
    } else {
      return NULL;
    }
  }

  void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("MD  "), 5);
    m_strncpy_p(n2, PSTR("RND "), 5);
  }


  
  
};

#endif /* MD_RANDOMIZE_SKETCH_H__ */
