#include <MDStudioSketch.h>
#include <MDStudioPages.h>

MDStudioSketch::MDStudioSketch() :
  swapTrackPage(&pattern),
  swapPatternPage(this) {
  patternCb = NULL;
  patternCbPtr = NULL;
}

void MDStudioSketch::setup() {
  /* XXX initialize pages here */
  swapTrackPage.setName("SWAP TRACKS");
  switchPage.addPage(&swapTrackPage);

  swapPatternPage.setup();
  swapPatternPage.setName("SWAP PATTERNS");
  switchPage.addPage(&swapPatternPage);

  switchPage.parent = this;
   
  MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&MDStudioSketch::onPatternChange);
  MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDStudioSketch::onKitChanged);
  MDSysexListener.addOnPatternMessageCallback(this,
					      (md_callback_ptr_t)&MDStudioSketch::onPatternMessage);
}

void MDStudioSketch::onKitChanged() {
}

void MDStudioSketch::show() {
  if (currentPage() == NULL) {
    // setPage(&defaultPage);
  }
}

void MDStudioSketch::doExtra(bool pressed) {
}

void MDStudioSketch::getName(char *n1, char *n2) {
  m_strncpy_p(n1, PSTR("MD  "), 5);
  m_strncpy_p(n2, PSTR("STD "), 5);
}

bool MDStudioSketch::handleEvent(gui_event_t *event) {
  if (currentPage() == &switchPage) {
    if (EVENT_RELEASED(event, Buttons.ENCODER4) || EVENT_RELEASED(event, Buttons.BUTTON4)) {
      if (!switchPage.setSelectedPage()) {
	popPage(&switchPage);
      }
      return true;
    }
  } else {
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      pushPage(&switchPage);
      return true;
    }
  }
  return false;
}

void MDStudioSketch::onPatternChange() {
  MD.requestPattern(MD.currentPattern);
}

void MDStudioSketch::onPatternMessage() {
  if ((patternCb != NULL) && (patternCbPtr != NULL)) {
    ((patternCb)->*(patternCbPtr))();
    patternCb = NULL;
    patternCbPtr = NULL;
  } else {
    MDPattern *pat = &pattern;
    if (pat->fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      char name[5];
      MD.getPatternName(pat->origPosition, name);
      GUI.flash_strings_fill("PATTERN", name);
    } else {
      GUI.flash_p_strings_fill(PSTR("SYSEX"), PSTR("ERROR"));
    }
  }
}

void MDStudioSketch::requestPattern(uint8_t pat, MDCallback *cb, md_callback_ptr_t ptr) {
  patternCb = cb;
  patternCbPtr = ptr;
  MD.requestPattern(pat);
}
