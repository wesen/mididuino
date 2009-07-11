#ifndef MNMPATTERN_H__
#define MNMPATTERN_H__

#include <inttypes.h>

class MNMTranspose {
  int8_t transpose;
  
  static const uint8_t TRANSPOSE_CHROMATIC = 0;
  static const uint8_t TRANSPOSE_MAJOR = 1;
  static const uint8_t TRANSPOSE_MINOR = 2;
  static const uint8_t TRANSPOSE_FIXED = 3;
  uint8_t scale;
  uint8_t key;
};

class MNMArpeggiator {
  static const uint8_t ARP_PLAY_TRUE = 0;
  static const uint8_t ARP_PLAY_UP   = 1;
  static const uint8_t ARP_PLAY_DOWN = 2;
  static const uint8_t ARP_PLAY_CYCLIC = 3;
  static const uint8_t ARP_PLAY_RND = 4;
  uint8_t play[6];
  static const uint8_t ARP_MODE_OFF = 0;
  static const uint8_t ARP_MODE_KEY = 1;
  static const uint8_t ARP_MODE_SID  = 2;
  static const uint8_t ARP_MODE_ADD  = 3;
  uint8_t mode[6];
  uint8_t octaveRange[6];
  uint8_t multiplier;
  uint8_t destination;
  uint8_t length;
  uint8_t pattern;
};

typedef struct mnm_note_s {
  unsigned note : 7;
  unsigned track : 3;
  unsigned position : 6;
} mnm_note_t;

class MNMPattern {
public:
  uint8_t origPosition;

  uint64_t ampTrigs[6];
  uint64_t filterTrigs[6];
  uint64_t lfoTrigs[6];
  uint64_t offTrigs[6];
  uint64_t triglessTrigs[6];
  uint64_t chordTrigs[6];

  uint64_t slidePatterns[6];
  uint64_t swingPatterns[6];

  uint64_t midiNoteOnTrigs[6];
  uint64_t midiNoteOffTrigs[6];
  uint64_t midiTriglessTrigs[6];
  uint64_t midiSlidePatterns[6];
  uint64_t midiSwingPatterns[6];

  uint32_t swingAmount;
  uint64_t lockPatterns[6];
  uint8_t noteNBR[6][64];

  uint8_t length;
  bool doubleTempo;

  uint8_t kit;
  int8_t patternTranspose;

  MNMTranspose transpose[6];
  MNMTranspose midiTranspose[6];

  MNMArpeggiator arp[6];
  MNMArpeggiator midiArp[6];

  uint16_t midiNotesUsed;
  uint8_t chordNotesUsed;
  uint8_t locksUsed;
  uint8_t locks[62][64];
  
  mnm_note_t midiNotes[400];
  mnm_note_t chordNotes[192];

  MNMPattern() {
  }
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);

  void init();

  bool isLockPatternEmpty(uint8_t idx);
  bool isLockPatternEmpty(uint8_t idx, uint64_t trigs);
  bool isParamLocked(uint8_t track, uint8_t param);
  void clearLockPattern(uint8_t lock);
  void cleanupLocks();

  void clearPattern();
  void clearTrack(uint8_t track);

  void clearParamLocks(uint8_t track, uint8_t param);
  void clearTrackLocks(uint8_t track);

  void clearTrig(uint8_t track, uint8_t trig);
  void setTrig(uint8_t track, uint8_t trig);

  int8_t getNextEmptyLock();
  void recalculateLockPatterns();

  bool addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value);
  void clearLock(uint8_t track, uint8_t trig, uint8_t param);
  uint8_t getLock(uint8_t track, uint8_t trig, uint8_t param);
};



#endif /* MNMPATTERN_H__ */