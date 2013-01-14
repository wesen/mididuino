/*
 * MidiCtrl - Octatrack Encoders
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */
 
#ifndef OT_ENCODERS_H__
#define OT_ENCODERS_H__

#include <GUI.h>
#include <OT.h>

#ifndef HOST_MIDIDUINO
class OTEncoder : public CCEncoder {
public:
  uint8_t track;
  uint8_t param;

  virtual uint8_t getCC();
  virtual uint8_t getChannel();
  virtual void initCCEncoder(uint8_t channel, uint8_t cc);
//  virtual void displayAt(int i);
//  virtual void update();

  void initOTEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0);

  OTEncoder(uint8_t _track = 0, uint8_t _param = 0, char *_name = NULL, uint8_t init = 0);
};

#endif /* !HOST_MIDIDUINO */

#endif /* OT_ENCODERS_H__ */
