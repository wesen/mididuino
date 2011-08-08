#include "Platform.h"

#include <MDStudioSketch.h>
#include <MDStudioPages.h>

/***************************************************************************
 *
 * MDSwapTrackPage
 *
 ***************************************************************************/

MDSwapTrackPage::MDSwapTrackPage(MDPattern *_pattern) :
  pattern(_pattern), srcEncoder("TR1"), dstEncoder("TR2") {
  setEncoders(&srcEncoder, &dstEncoder);
}

bool MDSwapTrackPage::handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    uint8_t src = srcEncoder.getValue();
    uint8_t dst = dstEncoder.getValue();
    if (src != dst) {
      pattern->swapTracks(src, dst);
      MD.kit.swapTracks(src, dst);
      {
        ElektronDataToSysexEncoder encoder(&MidiUart);
        pattern->toSysex(encoder);
      }
      {
        ElektronDataToSysexEncoder encoder(&MidiUart);
        MD.kit.toSysex(encoder);
      }
      GUI.flash_p_strings_fill(PSTR("SWAPPED TRACKS"), PSTR(""));
    }
    return true;
  }

  return false;
}


/***************************************************************************
 *
 * MDSwapPatternPage
 *
 ***************************************************************************/

void MDSwapPatternPage::setup() {
  MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&MDSwapPatternPage::onPatternChange);
  EncoderPage::setup();
}

MDSwapPatternPage::MDSwapPatternPage(MDStudioSketch *_sketch) :
  sketch(_sketch), srcEncoder("PT1"), dstEncoder("PT2") {
  setEncoders(&srcEncoder, &dstEncoder);
}

bool MDSwapPatternPage::handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1) && (swap_pattern_state == REQUEST_PATTERN_NONE)) {
    srcPattern = srcEncoder.getValue();
    dstPattern = dstEncoder.getValue();
    swapPattern();
    return true;
  }

  return false;
}

void MDSwapPatternPage::swapPattern() {
  switch (swap_pattern_state) {
  case REQUEST_PATTERN_NONE:
    if (sketch->pattern.origPosition == dstPattern) {
      uint8_t tmp = srcPattern;
      srcPattern = dstPattern;
      dstPattern = srcPattern;
    }
    if (sketch->pattern.origPosition == srcPattern) {
      swap_pattern_state = REQUEST_PATTERN_DST;
      swapPattern();
      return;
    } else {
      swap_pattern_state = REQUEST_PATTERN_SRC;
      GUI.flash_p_strings_fill(PSTR("REQUESTING"), PSTR("SRC PATTERN"));
      sketch->requestPattern(srcPattern, this, (md_callback_ptr_t)&MDSwapPatternPage::onPatternMessage);
    }
    break;

  case REQUEST_PATTERN_SRC:
    GUI.flash_p_strings_fill(PSTR("REQUESTING"), PSTR("DST PATTERN"));
    sketch->requestPattern(dstPattern, this, (md_callback_ptr_t)&MDSwapPatternPage::onPatternMessage);
    break;

  case REQUEST_PATTERN_DST:
    {
      sketch->pattern.origPosition = dstPattern;
      pattern2.origPosition = srcPattern;
      GUI.flash_p_strings_fill(PSTR("WRITING"), PSTR("SRC PATTERN"));
      {
	ElektronDataToSysexEncoder encoder(&MidiUart);
	sketch->pattern.toSysex(encoder);
      }
      GUI.flash_p_strings_fill(PSTR("WRITING"), PSTR("DST PATTERN"));
      {
	ElektronDataToSysexEncoder encoder(&MidiUart);
	pattern2.toSysex(encoder);
      }
      swap_pattern_state = REQUEST_PATTERN_NONE;
    }
    break;
  }
}

void MDSwapPatternPage::onPatternMessage() {
  switch (swap_pattern_state) {
  case REQUEST_PATTERN_SRC:
    if (sketch->pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      swapPattern();
    } else {
      GUI.flash_p_strings_fill(PSTR("ERROR IN"), PSTR("SRC PATTERN"));
      swap_pattern_state = REQUEST_PATTERN_NONE;
    }
    break;

  case REQUEST_PATTERN_DST:
    if (pattern2.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
      swapPattern();
    } else {
      GUI.flash_p_strings_fill(PSTR("ERROR IN"), PSTR("DST PATTERN"));
      swap_pattern_state = REQUEST_PATTERN_NONE;
    }
    break;

  default:
    break;
  }
}

void MDSwapPatternPage::onPatternChange() {
  srcEncoder.setValue(MD.currentPattern);
}

/***************************************************************************
 *
 * MDTransposePatternPage
 *
 ***************************************************************************/

MDTransposePatternPage::MDTransposePatternPage(MDPattern *_pattern) :
  pattern(_pattern), trackEncoder("TRK"), offsetEncoder(-48, 48, "OFF") {
  setEncoders(&trackEncoder);
}

bool MDTransposePatternPage::handleEvent(gui_event_t *event) {
}
