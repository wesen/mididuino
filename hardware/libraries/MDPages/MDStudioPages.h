/* Copyright (c) 2010 - http://ruinwesen.com/ */

#ifndef MD_STUDIO_PAGES_H__
#define MD_STUDIO_PAGES_H__

#include <GUI.h>
#include <MD.h>
#include <MDStudio.h>

/**
 * \addtogroup MD Elektron MachineDrum
 *
 * @{
 *
 * \addtogroup md_pages MachineDrum Pages
 *
 * @{
 **/

/**
 * \addtogroup md_swap_track_page MachineDrum Swap Track Page
 *
 * @{
 **/

/**
 * This page is used to swap tracks inside a MachineDrum pattern.
 **/

class MDSwapTrackPage : public EncoderPage {
 public:
  MDTrackFlashEncoder srcEncoder;
  MDTrackFlashEncoder dstEncoder;
  MDPattern *pattern;

 MDSwapTrackPage(MDPattern *_pattern) :
  pattern(_pattern), srcEncoder("SRC"), dstEncoder("DST")
  {
    setEncoders(&srcEncoder, &dstEncoder);
  }

  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      uint8_t src = srcEncoder.getValue();
      uint8_t dst = dstEncoder.getValue();
      if (src != dst) {
	pattern->swapTracks(src, dst);
	ElektronDataToSysexEncoder encoder(&MidiUart);
	pattern->toSysex(encoder);
	GUI.flash_p_strings_fill(PSTR("SWAPPED TRACKS"), PSTR(""));
      }
    }
  }
  
};

/* @} @} @} */

#endif /* MD_STUDIO_PAGES__ */
