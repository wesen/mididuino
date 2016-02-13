/*
 * MidiCtrl - Octatrack class
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "OT.h"

OTClass::OTClass() {
  setBaseChannel(0);
}

void OTClass::setBaseChannel(uint8_t channel) {
  for (uint8_t i = 0; i < OT_NUM_TRACKS; i++) {
    tracks[i].channel = channel + i;
  }
}

bool OTClass::parseCC(uint8_t _channel, uint8_t cc, uint8_t *track, uint8_t *param) {
  uint8_t i = 0;
  for (i = 0; i < OT_NUM_TRACKS; i++) {
    if (tracks[i].channel == _channel) {
      *track = i;
      break;
    }
  }

  if (i == OT_NUM_TRACKS) {
    /* no corresponding track found */
    return false;
  }

  if ((cc == 7) || (cc == 8) || (cc >= 16 && cc <= 51)) {
    *param = cc;
    return true;
  } else {
    return false;
  }
}

OTClass OT;
