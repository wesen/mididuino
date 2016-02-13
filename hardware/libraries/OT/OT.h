/*
 * MidiCtrl - Interface to the Elektron MachineDrum
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */
 
 #ifndef OT_H__
 #define OT_H__
 
#include "Platform.h"
#include "Elektron.hh"

#define OT_NUM_TRACKS 8
#define OT_NO_TRACK 255

struct OTTrack {
  uint8_t model;
  uint8_t fx1;
  uint8_t fx2;
  uint8_t channel;
};

#define IS_OT_FX_PARAM(cc) (((cc) >= 34) && ((cc) <= 45))
#define GET_OT_FX_PARAM(cc) (((cc) >= 40) ? ((cc) - 40) : ((cc) - 34))

#include "OTParams.h"
#include "OTEncoders.h"


class OTClass {
public:
  OTClass();

  /** assume that all channels follow sequentially, set the base channel. **/
  void setBaseChannel(uint8_t channel);

  OTTrack tracks[OT_NUM_TRACKS];

  inline uint8_t getTrackChannel(uint8_t track) {
    return tracks[track].channel;
  }

  uint8_t getChannelTrack(uint8_t channel) {
    for (uint8_t i = 0; i < OT_NUM_TRACKS; i++) {
      if (tracks[i].channel == channel) {
        return i;
      }
    }
    return OT_NO_TRACK;
  }

  /*
  void triggerTrack(uint8_t track);
  void triggerSample(uint8_t track);
  void triggerMidi(uint8_t track);
  void triggerRecording(uint8_t track);
  */

  /** Parse an incoming CC into a track and a parameter **/
  bool parseCC(uint8_t _channel, uint8_t cc, uint8_t *track, uint8_t *param);

  PGM_P getParamName(uint8_t track, uint8_t param) {
    return ot_track_param_name(tracks + track, param);
  }

};

extern OTClass OT;
 
 #endif /* OT_H__ */
 
