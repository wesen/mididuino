/*
 * MidiCtrl - Octatrack encoders
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "OTEncoders.h"

#ifndef HOST_MIDIDUINO
OTEncoder::OTEncoder(uint8_t _track, uint8_t _param, char *_name, uint8_t init) :
CCEncoder(0, 0, _name, init) {
  initOTEncoder(_track, _param);
  handler = CCEncoderHandle;
}

uint8_t OTEncoder::getCC() {
  return param;
}

uint8_t OTEncoder::getChannel() {
  return OT.getTrackChannel(track);
}

void OTEncoder::initCCEncoder(uint8_t _channel, uint8_t _cc) {
  if (OT.parseCC(_channel, _cc, &track, &param)) {
    initOTEncoder(track, param);
  }
}

void OTEncoder::initOTEncoder(uint8_t _track, uint8_t _param, char *_name, uint8_t init) {
  track = _track;
  param = _param;
  if (_name == NULL) {
    PGM_P name = NULL;
    name = OT.getParamName(track, param);
    if (name != NULL) {
      char myName[4];
      m_strncpy_p(myName, name, 4);
      setName(myName);
      GUI.redisplay();
    } else {
      setName("XXX");
      GUI.redisplay();
    }
  } else {
    setName(_name);
    GUI.redisplay();
  }

  setValue(init);
}


#endif /* !HOST_MIDIDUINO */
