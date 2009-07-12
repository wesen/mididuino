#ifndef MNM_MESSAGES_H__
#define MNM_MESSAGES_H__

#include <inttypes.h>

class MNMMidiMap {
public:
  uint8_t range;
  uint8_t pattern;
  uint8_t offset;
  uint8_t length;
  int8_t transpose;
  uint8_t timing;
};

class MNMGlobal {
public:
  uint8_t origPosition;

  uint8_t autotrackChannel;
  uint8_t baseChannel;
  uint8_t channelSpan;
  uint8_t multitrigChannel;
  uint8_t multimapChannel;

  bool clockIn;
  bool clockOut;
  bool ctrlIn;
  bool ctrlOut;

  bool transportIn;
  bool transportOut;
  
  bool sequencerOut;
  bool arpOut;
  bool keyboardOut;
  bool midiClockOut;
  bool pgmChangeOut;
  bool pgmChangeIn;

  uint8_t note; /* not used */
  uint8_t gate; /* not used */
  uint8_t sense; /* not used */
  uint8_t minVelocity; /* not used */
  uint8_t maxVelocity; /* not used */

  uint8_t midiMachineChannels[6];
  uint8_t ccDestinations[6][4];

  bool midiSeqLegato[6]; /* always true */
  bool legato[6]; /* not unsed */
  
  MNMMidiMap maps[32];

  uint8_t globalRouting;

  uint32_t baseFreq;

  MNMGlobal() {
  }

  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

class MNMTrackModifier {
public:
  static const uint8_t DEST_POS_PITCH_BEND = 0;
  static const uint8_t DEST_NEG_PITCH_BEND = 1;
  static const uint8_t DEST_POS_MOD_WHEEL = 2;
  static const uint8_t DEST_NEG_MOD_WHEEL = 3;
  static const uint8_t DEST_VELOCITY = 4;
  static const uint8_t DEST_KEY_FOLLOW = 5;

  uint8_t destPage[6][2];
  uint8_t destParam[6][2];
  int8_t range[6][2];
};

class MNMMachine {
public:
  uint8_t params[72];
  uint8_t level;
  uint8_t track;
  uint8_t type;
  uint8_t model;
  uint8_t trigTrack;
  MNMTrackModifier modifier;
};

class MNMKit {
public:
  uint8_t origPosition;
  char name[17];
  MNMMachine machines[6];

  uint16_t patchBusIn;

  bool modifierMirrorLeftRight;
  bool modifierMirrorUpDown;
  bool modifierLPKeyTrack;
  bool modifierHPKeyTrack;

  bool trigPortamento;
  bool trigLegatoAmp;
  bool trigLegatoFilter;
  bool trigLegatoLFO;

  static const uint8_t MULTIMODE_ALL = 0;
  static const uint8_t MULTIMODE_SPLIT_KEY = 1;
  static const uint8_t MULTIMODE_SEQ_START = 2;
  static const uint8_t MULTIMODE_SEQ_TRANSPOSE = 3;
  uint8_t commonMultimode;
  uint8_t commonTiming;

  uint8_t splitKey;
  uint8_t splitRange;

  MNMKit() {
  }
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

class MNMRow {
public:
  uint8_t pattern;
  uint8_t kit;
  uint8_t loop;
  uint8_t jump;
  uint8_t mute;
  uint8_t muteMidi;
  uint8_t startPosition;
  uint8_t endPosition;
  int8_t patternTranspose;
  int8_t trackTranspose[6];
  int8_t midiTranspose[6];
  uint16_t tempo;
};

class MNMSong {
public:
  uint8_t origposition;
  char name[17];
  MNMRow rows[200];

  MNMSong() {
  }
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};



#endif /* MNM_MESSAGES_H__ */
