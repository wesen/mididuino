#include "WProgram.h"
#include "helpers.h"

#include "Elektron.hh"
#include "MNMMessages.hh"
#include "MNMPattern.hh"
#include "MNMParams.hh"

void MNMPattern::init() {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 64; j++) {
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
    uint16_t l = ElektronHelper::to16Bit(udata[0x14d8 + i * 2], udata[0x14d8 + i * 2 + 1]);
    midiNotes[i].note = ((l >> 9) & 0x7f);
    midiNotes[i].track = (l >> 6) & 0x7;
    midiNotes[i].position = l & 0x3f;
  }
  for (int i = 0; i < 384; i++) {
    uint16_t l = ElektronHelper::to16Bit(udata[0x17f8 + i * 2], udata[0x17f8 + i * 2 + 1]);
    chordNotes[i].note = ((l >> 9) & 0x7f);
    chordNotes[i].track = (l >> 6) & 0x7;
    chordNotes[i].position = l & 0x3f;
  }

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 64; j++) {
      if (IS_BIT_SET64(lockPatterns[i], j)) {
	paramLocks[i][j] = numRows;
	lockTracks[numRows] = i;
	lockParams[numRows] = j;
	numRows++;
      }
    }
  }

  return true;
}

#ifdef HOST_MIDIDUINO

void print64(uint64_t trigs) {
  for (int i = 0; i < 64; i++) {
    if (IS_BIT_SET64(trigs, i)) {
      printf("X");
    } else {
      printf(".");
    }
  }
  printf("\n");
}

void MNMPattern::print() {
  for (int i = 0; i < 6; i++) {
    printf("track %d\n", i);
    printf("amp     : ");
    print64(ampTrigs[i]);
    printf("filter  : ");
    print64(filterTrigs[i]);
    printf("lfo     : ");
    print64(lfoTrigs[i]);
    printf("off     : ");
    print64(offTrigs[i]);
    printf("trigless: ");
    print64(triglessTrigs[i]);
    printf("chord   : ");
    print64(chordTrigs[i]);
    printf("locks   : ");
    print64(lockPatterns[i]);
    for (int j = 0; j < 64; j++) {
      printf("%d ", noteNBR[i][j]);
    }
    printf("\n");
  }
  for (int i = 0; i < 6; i++) {
    printf("midi track %d\n", i);
    printf("note on  : ");
    print64(midiNoteOnTrigs[i]);
    printf("note off : ");
    print64(midiNoteOffTrigs[i]);
    printf("trigless : ");
    print64(midiTriglessTrigs[i]);
  }
}
#endif

bool MNMPattern::isLockPatternEmpty(uint8_t idx) {
  for (int i = 0; i < 64; i++) {
    if (locks[idx][i] != 255)
      return false;
  }
  return true;
}

bool MNMPattern::isLockPatternEmpty(uint8_t idx, uint64_t trigs) {
  for (int i = 0; i < 64; i++) {
    if (locks[idx][i] != 255 || !IS_BIT_SET64(trigs, i))
      return false;
  }

  return true;
}

bool MNMPattern::isParamLocked(uint8_t track, uint8_t param) {
  return paramLocks[track][param] != -1;
}

void MNMPattern::clearLockPattern(uint8_t lock) {
  if (lock >= 62)
    return;

  for (int i = 0; i < 64; i++) {
    locks[lock][i] = 255;
  }
  if (lockTracks[lock] != -1 && lockParams[lock] != -1) {
    paramLocks[lockTracks[lock]][lockParams[lock]] = -1;
  }
  lockTracks[lock] = -1;
  lockParams[lock] = -1;
}

void MNMPattern::cleanupLocks() {
  for (int i = 0; i < 64; i++) {
    if (lockTracks[i] != -1) {
      if (isLockPatternEmpty(i, ampTrigs[lockTracks[i]])) { // trigs
	if (lockParams[i] != -1) {
	  paramLocks[lockTracks[i]][lockParams[i]] = -1;
	}
	lockTracks[i] = -1;
	lockParams[i] = -1;
      }
    } else {
      lockParams[i] = -1;
    }
  }
}

void MNMPattern::clearPattern() {
  init();
}

void MNMPattern::clearTrack(uint8_t track) {
  if (track >= 6) {
    return;
  }
  // clear trigs XXX
  clearTrackLocks(track);
}

void MNMPattern::clearParamLocks(uint8_t track, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx != -1) {
    clearLockPattern(idx);
    paramLocks[track][param] = -1;
  }
}

void MNMPattern::clearTrackLocks(uint8_t track) {
  for (int i = 0; i < 62; i++) {
    clearParamLocks(track, i);
  }
}

void MNMPattern::clearTrig(uint8_t track, uint8_t trig,
			   bool ampTrig, bool filterTrig, bool lfoTrig,
			   bool triglessTrig, bool chordTrig) {
  if (ampTrig)
    CLEAR_BIT64(ampTrigs[track], trig);
  if (filterTrig)
    CLEAR_BIT64(filterTrigs[track], trig);
  if (lfoTrig)
    CLEAR_BIT64(lfoTrigs[track], trig);
  if (triglessTrig)
    CLEAR_BIT64(triglessTrigs[track], trig);
  if (chordTrig)
    CLEAR_BIT64(chordTrigs[track], trig);
}

void MNMPattern::setTrig(uint8_t track, uint8_t trig,
			 bool ampTrig, bool filterTrig, bool lfoTrig,
			 bool triglessTrig, bool chordTrig) {
  if (ampTrig)
    SET_BIT64(ampTrigs[track], trig);
  if (filterTrig)
    SET_BIT64(filterTrigs[track], trig);
  if (lfoTrig)
    SET_BIT64(lfoTrigs[track], trig);
  if (triglessTrig)
    SET_BIT64(triglessTrigs[track], trig);
  if (chordTrig)
    SET_BIT64(chordTrigs[track], trig);
}

int8_t MNMPattern::getNextEmptyLock() {
  for (int i = 0; i < 62; i++) {
    if (lockTracks[i] == -1 && lockParams[i] == -1) {
      return i;
    }
  }
  return -1;
}

void MNMPattern::recalculateLockPatterns() {
  for (int track = 0; track < 6; track++) {
    lockPatterns[track] = 0;
    for (int param = 0; param < 64; param++) {
      if (paramLocks[track][param] != -1) {
	SET_BIT64(lockPatterns[track], param);
      }
    }
  }
}

bool MNMPattern::addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1) {
    idx = getNextEmptyLock();
    if (idx == -1)
      return false;
    paramLocks[track][param] = idx;
    lockTracks[idx] = track;
    lockParams[idx] = param;
    for (int i = 0; i < 64; i++) {
      locks[idx][i] = 255;
    }
  }
  locks[idx][trig] = value;
  return true;
}

void MNMPattern::clearLock(uint8_t track, uint8_t trig, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1)
    return;
  locks[idx][trig] = 255;

  if (isLockPatternEmpty(idx, ampTrigs[track])) {
    paramLocks[track][param] = -1;
    lockTracks[idx] = -1;
    lockParams[idx] = -1;
  }
}

uint8_t MNMPattern::getLock(uint8_t track, uint8_t trig, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1)
    return 255;
  return locks[idx][trig];
}

uint16_t MNMPattern::toSysex(uint8_t *data, uint16_t len) {
  uint8_t udata[0x2a7];

  data[0] = 0xF0;
  m_memcpy(data + 1, monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
  data[6] = MNM_PATTERN_MESSAGE_ID;
  data[7] = 0x05;
  data[8] = 0x01;

  uint16_t cksum = 0;
  udata[0] = data[9] = origPosition;
  cksum += data[9];

  for (int i = 0; i < 6; i++) {
    ElektronHelper::from64Bit(ampTrigs[i],          udata + 1 + 8 * i);
    ElektronHelper::from64Bit(filterTrigs[i],       udata + 0x31 + 8 * i);
    ElektronHelper::from64Bit(lfoTrigs[i],          udata + 0x61 + 8 * i);
    ElektronHelper::from64Bit(offTrigs[i],          udata + 0x91 + 8 * i);
    ElektronHelper::from64Bit(midiNoteOnTrigs[i],   udata + 0xc1 + 8 * i);
    ElektronHelper::from64Bit(midiNoteOffTrigs[i],  udata + 0xf1 + 8 * i);
    ElektronHelper::from64Bit(triglessTrigs[i],     udata + 0x121 + 8 * i);
    ElektronHelper::from64Bit(chordTrigs[i],        udata + 0x151 + 8 * i);
    ElektronHelper::from64Bit(midiTriglessTrigs[i], udata + 0x181 + 8 * i);
    ElektronHelper::from64Bit(slidePatterns[i],        udata + 0x1b1 + 8 * i);
    ElektronHelper::from64Bit(swingPatterns[i],        udata + 0x1e1 + 8 * i);
    ElektronHelper::from64Bit(midiSlidePatterns[i],    udata + 0x211 + 8 * i);
    ElektronHelper::from64Bit(midiSwingPatterns[i],    udata + 0x241 + 8 * i);
    ElektronHelper::from64Bit(lockPatterns[i],      udata + 0x275 + 8 * i);
  }
  ElektronHelper::from32Bit(swingAmount, udata + 0x271);

  MNMDataToSysexEncoder encoder(data + 10, len - 10);
  for (int i = 1; i < 0x2a5; i++) {
    encoder.pack(udata[i]);
  }
  for (int i = 0; i < 6 ; i++) {
    for (int j = 0; j < 64; j++) {
      encoder.pack(noteNBR[i][j]);
    }
  }

  uint8_t *ptr = udata - 0x425;
  ptr[0x425] = length;
  ptr[0x426] = doubleTempo ? 1 : 0;
  ptr[0x427] = kit;
  ptr[0x428] = patternTranspose;
  for (int i = 0; i < 6; i++) {
    ptr[0x429 + i] = transpose[i].transpose;
    ptr[0x42f + i] = transpose[i].scale;
    ptr[0x435 + i] = transpose[i].key;

    ptr[0x43b + i] = midiTranspose[i].transpose;
    ptr[0x441 + i] = midiTranspose[i].scale;
    ptr[0x447 + i] = midiTranspose[i].key;

    ptr[0x44d + i] = arp[i].play;
    ptr[0x453 + i] = arp[i].mode;
    ptr[0x459 + i] = arp[i].octaveRange;
    ptr[0x45f + i] = arp[i].multiplier;
    ptr[0x465 + i] = arp[i].destination;
    ptr[0x46b + i] = arp[i].length;
    m_memcpy(&ptr[0x471 + i * 16], arp[i].pattern, 16);

    ptr[0x4d1 + i] = midiArp[i].play;
    ptr[0x4d7 + i] = midiArp[i].mode;
    ptr[0x4dd + i] = midiArp[i].octaveRange;
    ptr[0x4e3 + i] = midiArp[i].multiplier;
    ptr[0x4e9 + i] = midiArp[i].length;
    m_memcpy(&ptr[0x4ef + i * 16], midiArp[i].pattern, 16);
  }
  ElektronHelper::from16Bit(midiNotesUsed, ptr + 0x553);
  ptr[0x555] = chordNotesUsed;
  ptr[0x557] = locksUsed;

  for (int i = 0; i < 4; i++) {
    ptr[0x54f + i] = 0xFF;
  }
  
  for (int i = 0x425; i < 0x558; i++) {
    encoder.pack(ptr[i]);
  }

  for (int i = 0; i < 62; i++) {
    for (int j = 0; j < 64; j++) {
      encoder.pack(locks[i][j]);
    }
  }
  for (int i = 0; i < 400; i++) {
    uint16_t x = ((uint16_t)midiNotes[i].note << 9) |
      ((uint16_t)midiNotes[i].track << 6) | (midiNotes[i].position);
    encoder.pack(x >> 8);
    encoder.pack(x & 0xFF);
  }
  for (int i = 0; i < 192; i++) {
    uint16_t x = ((uint16_t)chordNotes[i].note << 9) | ((uint16_t)chordNotes[i].track << 6) | (chordNotes[i].position);
    encoder.pack(x >> 8);
    encoder.pack(x & 0xFF);
  }

  encoder.pack(0xFF); // >??

  uint16_t enclen = encoder.finish();
  for (uint16_t i = 0; i < enclen; i++) {
    //    printf("cksum: %.4x, data: %.2x\n", cksum, data[10 + i]);
    cksum += data[10 + i];
 }
  
  data[10 + enclen] = (cksum >> 7) & 0x7F;
  data[10 + enclen + 1] = cksum & 0x7F;
  data[10 + enclen + 2] = ((enclen + 5) >> 7) & 0x7F;
  data[10 + enclen + 3] = (enclen + 5) & 0x7F;
  data[10 + enclen + 2 + 2] = 0xF7;
  
  return enclen + 10 + sizeof(monomachine_sysex_hdr);
}
