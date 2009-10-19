#ifndef MDPATTERN_H__
#define MDPATTERN_H__

#include <inttypes.h>

class MDPattern {
public:
  uint8_t origPosition;
  uint64_t trigPatterns[16];
  uint64_t lockPatterns[16];

  uint64_t accentPattern;
  uint64_t slidePattern;
  uint64_t swingPattern;
  uint64_t swingAmount;
  uint8_t accentAmount;
  uint8_t patternLength;

  bool doubleTempo;
  uint8_t scale;

  uint8_t kit;
  uint8_t numLockedRows; // unused
  uint8_t locks[64][64];

  bool accentEditAll;
  bool slideEditAll;
  bool swingEditAll;
  uint64_t accentPatterns[16];
  uint64_t slidePatterns[16];
  uint64_t swingPatterns[16];

  uint8_t numRows;
  int8_t paramLocks[16][24];
  int8_t lockTracks[64];
  int8_t lockParams[64];

  bool isExtraPattern;

  MDPattern() {
    isExtraPattern = false;
  }

  /* XXX TODO extra pattern 64 */

  void init();
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);

  bool isLockPatternEmpty(uint8_t idx);
  bool isLockPatternEmpty(uint8_t idx, uint64_t trigs);

	bool isTrackEmpty(uint8_t track);
	
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

#endif /* MDPATTERN_H__ */

