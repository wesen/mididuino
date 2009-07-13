#include "WProgram.h"
#include "helpers.h"

#include "Elektron.hh"
#include "MNMMessages.hh"
#include "MNMPattern.hh"

void MNMPattern::init() {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 72; j++) {
      paramLocks[i][j] = -1;
    }
  }
  locksUsed = 0;
  for (int i = 0; i < 64; i++) {
    lockTracks[i] = -1;
    lockParams[i] = -1;
    for (int j = 0; j < 64; j++) {
      locks[i][j] = 255;
    }
  }

  for (int i = 0; i < 6; i++) {
    ampTrigs[i] = 0;
    filterTrigs[i] = 0;
    lfoTrigs[i] = 0;
    offTrigs[i] = 0;
    triglessTrigs[i] = 0;
    chordTrigs[i] = 0;
    slidePatterns[i] = 0;
    swingPatterns[i] = 0;

    midiNoteOnTrigs[i] = 0;
    midiNoteOffTrigs[i] = 0;
    midiTriglessTrigs[i] = 0;
    midiSlidePatterns[i] = 0;
    midiSwingPatterns[i] = 0;

    lockPatterns[i] = 0;
  }

  //  accentAmount = 0;

  swingAmount = 50 << 14;
  length = 16;
  kit = 0;
  origPosition = 0;
  //  scale = 0;
}

bool MNMPattern::fromSysex(uint8_t *data, uint16_t len) {
  if (len < (0x1978 + 3)) {
#ifdef HOST_MIDIDUINO
    fprintf(stderr, "wrong len, %d should be %d bytes\n", len, 0x1978 + 3);
#endif
    return false;
  }

  uint8_t *udata = data + 3;
  origPosition = udata[0];
  for (int i = 0; i < 6; i++) {
    ampTrigs[i] = ElektronHelper::to64Bit(udata + 1 + i * 8);
    filterTrigs[i] = ElektronHelper::to64Bit(udata + 0x31 + i * 8);
    lfoTrigs[i] = ElektronHelper::to64Bit(udata + 0x61 + i * 8);
    offTrigs[i] = ElektronHelper::to64Bit(udata + 0x91 + i * 8);
    triglessTrigs[i] = ElektronHelper::to64Bit(udata + 0x121 + i * 8);
    chordTrigs[i] = ElektronHelper::to64Bit(udata + 0x151 + i * 8);
    slidePatterns[i] = ElektronHelper::to64Bit(udata + 0x1b1 + i * 8);
    swingPatterns[i] = ElektronHelper::to64Bit(udata + 0x1e1 + i * 8);

    midiTriglessTrigs[i] = ElektronHelper::to64Bit(udata + 0x181 + i * 8);
    midiNoteOnTrigs[i] = ElektronHelper::to64Bit(udata + 0xc1 + i * 8);
    midiNoteOffTrigs[i] = ElektronHelper::to64Bit(udata + 0xf1 + i * 8);
    midiSlidePatterns[i] = ElektronHelper::to64Bit(udata + 0x211 + i * 8);
    midiSwingPatterns[i] = ElektronHelper::to64Bit(udata + 0x241 + i * 8);

    lockPatterns[i] = ElektronHelper::to64Bit(udata + 0x275 + i * 8);

    m_memcpy(noteNBR[i], udata + 0x2a5 + i * 64, 64);

    transpose[i].transpose = udata[0x429 + i];
    transpose[i].scale = udata[0x42f + i];
    transpose[i].key = udata[0x435 + i];

    midiTranspose[i].transpose = udata[0x43b + i];
    midiTranspose[i].scale = udata[0x441 + i];
    midiTranspose[i].key = udata[0x447 + i];

    arp[i].play = udata[0x44d + i];
    arp[i].mode = udata[0x453 + i];
    arp[i].octaveRange = udata[0x459 + i];
    arp[i].multiplier = udata[0x45f + i];
    arp[i].destination = udata[0x465 + i];
    arp[i].length = udata[0x46b + i];

    m_memcpy(arp[i].pattern, &udata[0x471 + i * 16], 16);

    midiArp[i].play = udata[0x4d1 + i];
    midiArp[i].mode = udata[0x4d7 + i];
    midiArp[i].octaveRange = udata[0x4dd + i];
    midiArp[i].multiplier = udata[0x4e3 + i];
    midiArp[i].length = udata[0x4e9 + i];
    m_memcpy(midiArp[i].pattern, &udata[0x4ef + i * 16], 16);
  }
  swingAmount = ElektronHelper::to32Bit(udata + 0x271);
  length = udata[0x425];
  doubleTempo = (udata[0x426] == 1);
  kit = udata[0x427];
  patternTranspose = udata[0x428];
  midiNotesUsed = ElektronHelper::to16Bit(udata[0x553], udata[0x554]);
  chordNotesUsed = udata[0x555];
  locksUsed = udata[0x557];
  m_memcpy(locks, udata + 0x558, 62 * 64);
  for (int i = 0; i < 400; i++) {
    uint16_t l = ElektronHelper::to16Bit(udata[0x14d8 + i * 2], udata[0x1d48 + i * 2 + 1]);
    m_memcpy((uint8_t *)(&midiNotes[i]), (uint8_t *)&l, 2);
  }
  for (int i = 0; i < 384; i++) {
    uint16_t l = ElektronHelper::to16Bit(udata[0x17f8 + i * 2], udata[0x17f8 + i * 2 + 1]);
    m_memcpy((uint8_t *)(&chordNotes[i]), (uint8_t *)&l, 2);
  }

  return true;
}

bool MNMPattern::isLockPatternEmpty(uint8_t idx) {
  return false;
}

uint16_t MNMPattern::toSysex(uint8_t *sysex, uint16_t len) {
  return 0;
}
