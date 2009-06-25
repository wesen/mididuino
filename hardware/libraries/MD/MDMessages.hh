#ifndef MDMESSAGES_H__
#define MDMESSAGES_H__

#include "MDPattern.hh"

extern uint8_t machinedrum_sysex_hdr[5];

uint16_t to16Bit(uint8_t b1, uint8_t b2);
uint32_t to32Bit(uint8_t *b);
void from16Bit(uint16_t num, uint8_t *b);
void from32Bit(uint32_t num, uint8_t *b);

class MDGlobal {
public:
  uint8_t origPosition;
  uint8_t drumRouting[16];
  int8_t drumMapping[16];
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

  MDGlobal() {
  }

  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

class MDLFO {
public:
  uint8_t destinationTrack;
  uint8_t destinationParam;
  uint8_t shape1;
  uint8_t shape2;
  uint8_t type;
  uint8_t speed;
  uint8_t depth;
  uint8_t mix;
};

class MDMachine {
public:
  uint8_t params[24];
  uint8_t track;
  uint8_t level;
  uint32_t model;
  MDLFO lfo;
  uint8_t trigGroup;
  uint8_t muteGroup;
};

class MDKit {
public:
  uint8_t origPosition;
  char name[17];
  uint8_t reverb[8];
  uint8_t delay[8];
  uint8_t eq[8];
  uint8_t dynamics[8];

  MDMachine machines[16];

  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};



class MDRow {
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

class MDSong {
public:
  uint8_t origPosition;
  char name[17];
  MDRow rows[256];
  uint8_t numRows;
  
  bool fromSysex(uint8_t *sysex, uint16_t len);
  uint16_t toSysex(uint8_t *sysex, uint16_t len);
};

#endif /* MDMESSAGES_H__ */
