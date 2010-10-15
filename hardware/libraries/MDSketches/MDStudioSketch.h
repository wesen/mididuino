/* Copyright (c) 2010 - http://ruinwesen.com/ */

#ifndef MD_STUDIO_SKETCH_H__
#define MD_STUDIO_SKETCH_H__

#include <MDStudioPages.h>

/**
 * \addtogroup MD Elektron MachineDrum
 *
 * @{
 * 
 * \addtogroup md_sketches MachineDrum Sketches
 * 
 * @{
 **/

/**
 * \addtogroup md_studio_sketch MachineDrum Studio Sketch
 *
 * @{
 **/

#include <MDStudioPages.h>

class MDStudioSketch :
public Sketch, MDCallback {
 public:
  MDPattern pattern;

  MDCallback *patternCb;
  md_callback_ptr_t patternCbPtr;

  ScrollSwitchPage switchPage;

  /* XXX insert pages here */
  MDSwapTrackPage swapTrackPage;
  MDSwapPatternPage swapPatternPage;
  MDTransposePatternPage transposePatternPage;

  MDStudioSketch();

  void setup();
  virtual void onKitChanged();

  /** This method is called when the sketch is made active and
   ** displayed first. Use this to set the default page.
   **/
  virtual void show();

  /** This method is called when the extra button is pressed on a monster firmware page. **/
  virtual void doExtra(bool pressed);

 /** Return the name of the firmware for a MDMonster display page. **/
  void getName(char *n1, char *n2);

  virtual bool handleEvent(gui_event_t *event);

  void requestPattern(uint8_t pattern, MDCallback *cb, md_callback_ptr_t ptr);
  
  void onPatternChange();

  void onPatternMessage();
};

/* @} @} @} */

#endif /* MD_STUDIO_SKETCH_H__ */
