#include "MDPattern.hh"
#include "Elektron.hh"
#include "MDMessages.hh"
#include "helpers.h"
#include "MDParams.hh"

#ifdef AVR
#include "GUI.h"
#else
#include <stdio.h>
#endif

// #include "GUI.h"

void MDPattern::init() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      paramLocks[i][j] = -1;
    }
  }

  numRows = 0;
  for (int i = 0; i < 64; i++) {
    lockTracks[i] = -1;
    lockParams[i] = -1;
    for (int j = 0; j < 32; j++) {
      locks[i][j] = 255;
    }
  }

  for (int i = 0; i < 16; i++) {
    trigPatterns[i] = 0;
    lockPatterns[i] = 0;
    accentPatterns[i] = 0;
    slidePatterns[i] = 0;
    swingPatterns[i] = 0;
  }

  accentPattern = 0;
  slidePattern = 0;
  swingPattern = 0;
  accentAmount = 0;
  patternLength = 16;
  swingAmount = 50 << 14;
  accentEditAll = 1;
  swingEditAll = 1;
  slideEditAll = 1;
  origPosition = 0;
  kit = 0;
  scale = 0;
}

bool MDPattern::fromSysex(uint8_t *data, uint16_t len) {
  init();
  
  if ((len != (0xACA - 6)) && (len != (0x1521 - 6)))  {
#ifdef AVR
    GUI.flash_string_fill("WRONG LENGTH");
#else
    printf("WRONG LENGTH: %x\n", len);
#endif
    return false;
  }

  if (len == (0x1521 - 6)) {
    isExtraPattern = true;
  } else {
    isExtraPattern = false;
  }
  

  uint16_t cksum = 0;
  for (int i = 9 - 6; i < (len - 4); i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
  if (cksum != to16Bit(data[len - 4], data[len - 3])) {
    // wrong checksum
    return false;
  }
    

  origPosition = data[9 - 6];
  uint8_t data2[204];
  sysex_to_data_elektron(data + 0xA - 6, data2, 74);
  uint8_t *ptr = data2;
  for (int i = 0; i < 16; i++) {
    trigPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
    
  sysex_to_data_elektron(data + 0x54 - 6, data2, 74);
  ptr = data2;
  for (int i = 0; i < 16; i++) {
    lockPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }

  sysex_to_data_elektron(data + 0x9e - 6, data2, 19);
  accentPattern = to32Bit(data2);
  slidePattern = to32Bit(data2 + 4);
  swingPattern = to32Bit(data2 + 8);
  swingAmount = to32Bit(data2 + 12);

  accentAmount = data[0xB1 - 6];
  patternLength = data[0xB2 - 6];
  doubleTempo = (data[0xb3 - 6] == 1);
  scale = data[0xb4 - 6];
  kit = data[0xb5 - 6];
  numLockedRows = data[0xb6 - 6];
  sysex_to_data_elektron(data + 0xB7 - 6, (uint8_t *)locks, 2341);
  sysex_to_data_elektron(data + 0x9DC - 6, data2, 234);

  accentEditAll = (to32Bit(data2) == 1);
  slideEditAll = (to32Bit(data2 + 4) == 1);
  swingEditAll = (to32Bit(data2 + 8) == 1);

  ptr = data2 + 12;
  for (int i = 0; i < 16; i++) {
    accentPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    slidePatterns[i] = to32Bit(ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    swingPatterns[i] = to32Bit(ptr);
    ptr += 4;
  }

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 24; j++) {
      if (IS_BIT_SET32(lockPatterns[i], j)) {
	paramLocks[i][j] = numRows;
	lockTracks[numRows] = i;
	lockParams[numRows] = j;
	numRows++;
      }
    }
  }
  

  return true;
  
}

uint8_t lockData[64][32];

uint16_t MDPattern::toSysex(uint8_t *data, uint16_t len) {
  // XXX check extrapattern
  if (len < 0xacb)
    return 0;

  m_memclr(data, 0xACB);
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_PATTERN_MESSAGE_ID;
  data[7] = 0x03; // version
  data[8] = 0x01;
  data[9] = origPosition;

  uint8_t data2[204];
  uint8_t *ptr = data2;

  for (int i = 0; i < 16; i++) {
    from32Bit(trigPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0xA, 64);

  recalculateLockPatterns();
  ptr = data2;
  for (int i = 0; i < 16; i++) {
    from32Bit(lockPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0x54, 64);

  from32Bit(accentPattern, data2);
  from32Bit(slidePattern, data2 + 4);
  from32Bit(swingPattern, data2 + 8);
  from32Bit(swingAmount, data2 + 12);
  data_to_sysex_elektron(data2, data + 0x9e, 16);
    
  data[0xB1] = accentAmount;
  data[0xB2] = patternLength;
  data[0xb3] = doubleTempo ? 1 : 0;
  data[0xb4] = scale;
  data[0xb5] = kit;
  data[0xb6] = numLockedRows;

  uint16_t retlen = 0;
  uint16_t cnt = 0;

  for (int track = 0; track < 16; track++) {
    for (int param = 0; param < 24; param++) {
      int8_t lock = paramLocks[track][param];
      if (lock != -1) {
	uint8_t *data3 = locks[lock];
	m_memcpy(lockData[lock], locks[lock], 32);
	cnt++;
      }
    }
  }
  for (int i = cnt; i < 64; i++) {
    m_memclr(lockData[i], 32);
  }
  data_to_sysex_elektron((uint8_t*)lockData, data + 0xB7, 64 * 32);
  
  ptr = data + 0xB7;
#if 0  
  {
    uint16_t cnt7 = 0;
      
    ptr[0] = 0;

    for (int track = 0; track < 16; track++) {
      for (int param = 0; param < 24; param++) {
	uint8_t lock = paramLocks[track][param];
	if (lock != -1) {
	  uint8_t *data3 = locks[lock];
	    
	  for (int i = 0; i < 32; i++) {
	    uint8_t c = data3[i] & 0x7F;
	    uint8_t msb = data3[i] >> 7;
	    ptr[0] |= msb << (6 - cnt7);
	    ptr[1 + cnt7] = c;
	      
	    if (cnt7++ == 6) {
	      ptr += 8;
	      retlen += 8;
	      ptr[0] = 0;
	      cnt7 = 0;
	    }
	      
	    cnt++;
	  }
	}
      }
    }

    for (uint16_t i = cnt; i < 64 * 32; i++) {
      uint8_t c = 0 & 0x7F;
      uint8_t msb = 0 >> 7;
      ptr[0] |= msb << (6 - cnt7);
      ptr[1 + cnt7] = c;
	
      if (cnt7++ == 6) {
	ptr += 8;
	retlen += 8;
	ptr[0] = 0;
	cnt7 = 0;
      }
	
      cnt++;
    }

    retlen += cnt7 + (cnt7 != 0 ? 1 : 0);
  }
#endif

  from32Bit(accentEditAll ? 1 : 0, data2);
  from32Bit(slideEditAll ? 1 : 0, data2 + 4);
  from32Bit(swingEditAll ? 1 : 0, data2 + 8);

  ptr = data2 + 12;
  for (int i = 0; i < 16; i++) {
    from32Bit(accentPatterns[i], ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    from32Bit(slidePatterns[i], ptr);
    ptr += 4;
  }
  for (int i = 0; i < 16; i++) {
    from32Bit(swingPatterns[i], ptr);
    ptr += 4;
  }
  data_to_sysex_elektron(data2, data + 0x9DC, 204);

  uint16_t checksum = 0;
  for (int i = 9; i < 0xac6; i++)
    checksum += data[i];
  data[0xac6] = (uint8_t)((checksum >> 7) & 0x7F);
  data[0xac7] = (uint8_t)(checksum & 0x7F);
  uint16_t length = 0xacb - 7 - 3;
  data[0xac8] = (uint8_t)((length >> 7) &0x7F);
  data[0xac9] = (uint8_t)(length & 0x7F);
  data[0xaca] = 0xF7;

  return 0xacb;
}

bool MDPattern::isLockPatternEmpty(uint8_t idx) {
  for (int i = 0; i < 32; i++) {
    if (locks[idx][i] != 255)
      return false;
  }
  return true;
}


bool MDPattern::isLockPatternEmpty(uint8_t idx, uint32_t trigs) {
  for (int i = 0; i < 32; i++) {
    if (locks[idx][i] != 255 || !IS_BIT_SET32(trigs, i))
      return false;
  }

  return true;
}

void MDPattern::cleanupLocks() {
  for (int i = 0; i < 64; i++) {
    if (lockTracks[i] != -1) {
      if (isLockPatternEmpty(i, trigPatterns[lockTracks[i]])) {
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

void MDPattern::clearPattern() {
  init();
}

void MDPattern::clearTrack(uint8_t track) {
  if (track >= 16)
    return;
  for (int i = 0; i < 32; i++) {
    clearTrig(track, i);
  }
  clearTrackLocks(track);
}

void MDPattern::clearLockPattern(uint8_t lock) {
  if (lock >= 64)
    return;
  
  for (int i = 0; i < 32; i++) {
    locks[lock][i] = 255;
    if (lockTracks[lock] != -1 && lockParams[lock] != -1) {
      paramLocks[lockTracks[lock]][lockParams[lock]] = -1;
    }
    lockTracks[lock] = -1;
    lockParams[lock] = -1;
  }
}

bool MDPattern::isParamLocked(uint8_t track, uint8_t param) {
  return paramLocks[track][param] != -1;
}

void MDPattern::clearParamLocks(uint8_t track, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx != -1) {
    clearLockPattern(idx);
    paramLocks[track][param] = -1;
  }
}

void MDPattern::clearTrackLocks(uint8_t track) {
  for (int i = 0; i < 24; i++) {
    clearParamLocks(track, i);
  }
}

void MDPattern::clearTrig(uint8_t track, uint8_t trig) {
  CLEAR_BIT32(trigPatterns[track], trig);
  for (int i = 0; i < 24; i++) {
    clearLock(track, trig, i);
  }
}

void MDPattern::setTrig(uint8_t track, uint8_t trig) {
  SET_BIT32(trigPatterns[track], trig);
}

int8_t MDPattern::getNextEmptyLock() {
  for (int i = 0; i < 64; i++) {
    if (lockTracks[i] == -1 && lockParams[i] == -1) {
      return i;
    }
  }
  return -1;
}

bool MDPattern::addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1) {
    idx = getNextEmptyLock();
    if (idx == -1)
      return false;
    paramLocks[track][param] = idx;
    lockTracks[idx] = track;
    lockParams[idx] = param;
    for (int i = 0; i < 32; i++) {
      locks[idx][i] = 255;
    }
  }
  locks[idx][trig] = value;
  return true;
}

void MDPattern::clearLock(uint8_t track, uint8_t trig, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1)
    return;
  locks[idx][trig] = 255;

  if (isLockPatternEmpty(idx, trigPatterns[track])) {
    paramLocks[track][param] = -1;
    lockTracks[idx] = -1;
    lockParams[idx] = -1;
  }
}

void MDPattern::recalculateLockPatterns() {
  for (int track = 0; track < 16; track++) {
    lockPatterns[track] = 0;
    for (int param = 0; param < 24; param++) {
      if (paramLocks[track][param] != -1) {
	SET_BIT32(lockPatterns[track], param);
      }
    }
  }
}

uint8_t MDPattern::getLock(uint8_t track, uint8_t trig, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx == -1)
    return 255;
  return locks[idx][trig];
}
			 
