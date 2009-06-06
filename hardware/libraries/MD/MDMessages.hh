#ifndef MDMESSAGES_H__
#define MDMESSAGES_H__

class MachineDrumGlobal {
public:
  uint8_t origPosition;
  uint8_t drumRouting[16];
  uint8_t baseChannel;
  uint16_t tempo;
  bool extendedMode;
  bool clockIn;
  bool clockOut;
  bool transportIn;
  bool transportOut;
  bool localOn;

  uint8_t drumLeft;
  uint8_t drumRight;
  uint8_t gateLeft;
  uint8_t gateRight;
  uint8_t senseLeft;
  uint8_t senseRight;
  uint8_t minLevelRight;
  uint8_t minLevelLeft;
  uint8_t maxLevelRight;
  uint8_t maxLevelLeft;

  uint8_t programChange;
  uint8_t trigMode;

  uint8_t keyMap[128];

  MachineDrumGlobal() {
  }

  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

class MachineDrumLFO {
public:
  uint8_t destinationTrack;
  uint8_t destinationParam;
  uint8_t shape1;
  uint8_t shape2;
  uint8_t type;
};

class MachineDrumMachine {
public:
  uint8_t params[24];
  uint8_t track;
  uint8_t level;
  uint32_t model;
  MachineDrumLFO lfo;
  uint8_t trigGroup;
  uint8_t muteGroup;
};

class MachineDrumKit {
public:
  uint8_t origPosition;
  char name[17];
  uint8_t reverb[8];
  uint8_t delay[8];
  uint8_t eq[8];
  uint8_t dynamics[8];

  MachineDrumMachine machines[16];

  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};



class MachineDrumPattern {
public:
  uint8_t origPosition;
  uint32_t trigPatterns[16];
  uint32_t lockPatterns[16];

  uint32_t accentPattern;
  uint32_t slidePattern;
  uint32_t swingPattern;
  uint32_t swingAmount;
  uint8_t accentAmount;
  uint8_t patternLength;

  bool doubleTempo;
  uint8_t scale;

  uint8_t kit;
  uint8_t numLockedRows; // unused
  uint8_t locks[64][32];

  bool accentEditAll;
  bool slideEditAll;
  bool swingEditAll;
  uint32_t accentPatterns[16];
  uint32_t slidePatterns[16];
  uint32_t swingPatterns[16];

  uint8_t numRows;
  int8_t paramLocks[16][24];

  /* XXX TODO extra pattern 64 */
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

class MachineDrumRow {
public:
  uint8_t pattern;
  uint8_t kit;
  uint8_t loopTimes;
  uint8_t jump;
  uint16_t mutes;
  uint16_t tempo;
  uint8_t startPosition;
  uint8_t endPosition;
};

class MachineDrumSong {
public:
  uint8_t origPosition;
  char name[17];
  MachineDrumRow rows[256];
  uint8_t numRows;
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

#endif /* MDMESSAGES_H__ */
