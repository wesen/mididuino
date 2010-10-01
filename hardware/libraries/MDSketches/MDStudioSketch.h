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

class MDStudioSketch :
public Sketch, MDCallback {
 public:
  MDPattern pattern;

  ScrollSwitchPage switchPage;

  /* XXX insert pages here */
  MDSwapTrackPage swapTrackPage;

 MDStudioSketch() :
  swapTrackPage(&pattern)
   // call page constructors here XXX
  {
  }

 void setup() {
   /* XXX initialize pages here */
   swapTrackPage.setName("SWAP TRACKS");
   switchPage.addPage(&swapTrackPage);
   switchPage.parent = this;
   
   MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&MDStudioSketch::onPatternChange);
   MDTask.addOnKitChangeCallback(this, (md_callback_ptr_t)&MDStudioSketch::onKitChanged);
   MDSysexListener.addOnPatternMessageCallback(this,
					       (md_callback_ptr_t)&MDStudioSketch::onPatternMessage);
 }

 virtual void onKitChanged() {
 }

 /** This method is called when the sketch is made active and
  ** displayed first. Use this to set the default page.
  **/
 virtual void show() {
   if (currentPage() == NULL) {
     // setPage(&defaultPage);
   }
 }

 /** This method is called when the extra button is pressed on a monster firmware page. **/
 virtual void doExtra(bool pressed) {
 }

 /** Return the name of the firmware for a MDMonster display page. **/
 void getName(char *n1, char *n2) {
   m_strncpy_p(n1, PSTR("MD  "), 5);
   m_strncpy_p(n2, PSTR("STD "), 5);
 }

 virtual bool handleEvent(gui_event_t *event) {
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

 void onPatternChange() {
   MD.requestPattern(MD.currentPattern);
 }

 void onPatternMessage() {
   if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
     char name[5];
     MD.getPatternName(pattern.origPosition, name);
     GUI.flash_strings_fill("PATTERN", name);
   } else {
     GUI.flash_p_strings_fill(PSTR("SYSEX"), PSTR("ERROR"));
   }
 }

};

/* @} @} @} */

#endif /* MD_STUDIO_SKETCH_H__ */
