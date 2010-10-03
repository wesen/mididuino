/* Copyright (c) 2010 - http://ruinwesen.com/ */

#ifndef MD_STUDIO_PAGES_H__
#define MD_STUDIO_PAGES_H__

#include <GUI.h>
#include <MD.h>

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

class MDStudioSketch;

class MDSwapTrackPage : public EncoderPage {
 public:
  MDTrackFlashEncoder srcEncoder;
  MDTrackFlashEncoder dstEncoder;
  MDPattern *pattern;

  MDSwapTrackPage(MDPattern *_pattern);
  virtual bool handleEvent(gui_event_t *event);
  
};

class MDSwapPatternPage : public EncoderPage, MDCallback {
 public:
  MDPatternSelectEncoder srcEncoder;
  MDPatternSelectEncoder dstEncoder;
  MDStudioSketch *sketch;
  MDPattern pattern2;

  uint8_t srcPattern;
  uint8_t dstPattern;

  MDSwapPatternPage(MDStudioSketch *_sketch);

  enum {
    REQUEST_PATTERN_NONE = 0,
    REQUEST_PATTERN_SRC,
    REQUEST_PATTERN_DST
  } swap_pattern_state;

  void setup();
  void onPatternChange();
  void onPatternMessage();
  virtual bool handleEvent(gui_event_t *event);
  void swapPattern();
};

/* @} @} @} */

#endif /* MD_STUDIO_PAGES__ */
