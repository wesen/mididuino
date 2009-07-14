#ifndef MNMPATTERN_H__
#define MNMPATTERN_H__

#include <inttypes.h>

class MNMTranspose {
public:
  int8_t transpose;
  
  static const uint8_t TRANSPOSE_CHROMATIC = 0;
  static const uint8_t TRANSPOSE_MAJOR = 1;
  static const uint8_t TRANSPOSE_MINOR = 2;
  static const uint8_t TRANSPOSE_FIXED = 3;
  uint8_t scale;
  uint8_t key;
};

class MNMArpeggiator {
public:
  static const uint8_t ARP_PLAY_TRUE = 0;
  static const uint8_t ARP_PLAY_UP   = 1;
  static const uint8_t ARP_PLAY_DOWN = 2;
  static const uint8_t ARP_PLAY_CYCLIC = 3;
  static const uint8_t ARP_PLAY_RND = 4;
  uint8_t play;
  static const uint8_t ARP_MODE_OFF = 0;
  static const uint8_t ARP_MODE_KEY = 1;
  static const uint8_t ARP_MODE_SID  = 2;
  static const uint8_t ARP_MODE_ADD  = 3;
  uint8_t mode;
  uint8_t octaveRange;
  uint8_t multiplier;
  uint8_t destination;
  uint8_t length;
  uint8_t pattern[16];
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
  uint8_t numRows;
  uint8_t locks[62][64];
  int8_t paramLocks[6][72];
  int8_t lockTracks[64];
  int8_t lockParams[64];
  
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

  void clearTrig(uint8_t track, uint8_t trig,
		 bool ampTrig = false, bool filterTrig = false, bool lfoTrig = false,
		 bool triglessTrig = false, bool chordTrig = false);
  void setTrig(uint8_t track, uint8_t trig,
	       bool ampTrig = false, bool filterTrig = false, bool lfoTrig = false,
	       bool triglessTrig = false, bool chordTrig = false);

  void clearAllTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, true, true, true, true, true);
  }
  void setAllTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, true, true, true);
  }
  void clearAmpTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, true);
  }
  void setAmpTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, true);
  }
  void clearFilterTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, false, true);
  }
  void setFilterTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, false, true);
  }
  void clearLFOTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, false, false, true);
  }
  void setLFOTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, false, false, true);
  }
  void clearTriglessTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, false, false, false, true);
  }
  void setTriglessTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, false, false, false, true);
  }
  void clearChordTrig(uint8_t track, uint8_t trig) {
    clearTrig(track, trig, false, false, false, false, true);
  }
  void setChordTrig(uint8_t track, uint8_t trig) {
    setTrig(track, trig, false, false, false, false, true);
  }

  int8_t getNextEmptyLock();
  void recalculateLockPatterns();

  void setNote(uint8_t track, uint8_t step, uint8_t note);
  void setChordNote(uint8_t track, uint8_t step, uint8_t note);
  void clearChordNote(uint8_t track, uint8_t step, uint8_t note);

  bool addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value);
  void clearLock(uint8_t track, uint8_t trig, uint8_t param);
  uint8_t getLock(uint8_t track, uint8_t trig, uint8_t param);

#ifdef HOST_MIDIDUINO
  void print();
#endif
};



#endif /* MNMPATTERN_H__ */
