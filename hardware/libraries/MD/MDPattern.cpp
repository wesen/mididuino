#include "MDPattern.hh"
#include "Elektron.hh"
#include "MDMessages.hh"
#include "helpers.h"
#include "MDParams.hh"
#include "GUI.h"

#ifdef HOST_MIDIDUINO
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
    for (int j = 0; j < 64; j++) {
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
  accentEditAll = 1;
  swingEditAll = 1;
  slideEditAll = 1;
	doubleTempo = 0;

  patternLength = 16;
  swingAmount = 50 << 14;
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
  for (uint16_t i = 9 - 6; i < (len - 4); i++) {
    cksum += data[i];
  }
  cksum &= 0x3FFF;
	uint16_t realcksum = ElektronHelper::to16Bit7(data[len - 4], data[len - 3]);
  if (cksum != realcksum) {
#ifdef HOST_MIDIDUINO
		printf("wrong checksum, %x should be %x\n", cksum, realcksum);
#else
    GUI.flash_string_fill("WRONG CKSUM");
#endif
    // wrong checksum
    return false;
  }
    
  origPosition = data[9 - 6];
	MDSysexDecoder decoder(data + 0xA - 6, 74);
  for (int i = 0; i < 16; i++) {
		uint32_t trigs;
		decoder.get32(&trigs);
		trigPatterns[i] = trigs;
  }

	decoder.init(data + 0x54 - 6, 74);
  for (int i = 0; i < 16; i++) {
		decoder.get32(lockPatterns + i);
  }

	decoder.init(data + 0x9e - 6, 19);
	decoder.get32(&accentPattern);
	decoder.get32(&slidePattern);
	decoder.get32(&swingPattern);
	decoder.get32(&swingAmount);

  accentAmount = data[0xB1 - 6];
  patternLength = data[0xB2 - 6];
  doubleTempo = (data[0xb3 - 6] == 1);
  scale = data[0xb4 - 6];
  kit = data[0xb5 - 6];
  numLockedRows = data[0xb6 - 6];

	decoder.init(data + 0xb7 - 6, 2341);
	for (uint8_t i = 0; i < 64; i++) {
		decoder.get(locks[i], 32);
	}

	decoder.init(data + 0x9DC - 6, 234);
	{
		uint32_t tmp;
		decoder.get32(&tmp);
		accentEditAll = (tmp == 1);
		decoder.get32(&tmp);
		slideEditAll = (tmp == 1);
		decoder.get32(&tmp);
		swingEditAll = (tmp == 1);
	}
  for (int i = 0; i < 16; i++) {
		decoder.get32(accentPatterns + i);
  }
  for (int i = 0; i < 16; i++) {
		decoder.get32(slidePatterns + i);
  }
  for (int i = 0; i < 16; i++) {
		decoder.get32(swingPatterns + i);
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

  if (isExtraPattern) {
		decoder.init(data + 0xac6 - 6, 2647);
    for (int i = 0; i < 16; i++) {
			decoder.get32hi(trigPatterns + i);
    }
		decoder.get32hi(&accentPattern);
		decoder.get32hi(&slidePattern);
		decoder.get32hi(&swingPattern);
		for (uint8_t i = 0; i < 64; i++) {
			decoder.get(locks[i] + 32, 32);
		}
		for (int i = 0; i < 16; i++) {
			decoder.get32hi(accentPatterns + i);
		}
		for (int i = 0; i < 16; i++) {
			decoder.get32hi(slidePatterns + i);
		}
		for (int i = 0; i < 16; i++) {
			decoder.get32hi(swingPatterns + i);
		}
  }

  return true;
}

uint16_t MDPattern::toSysex(uint8_t *data, uint16_t len) {
	if (patternLength > 32) {
		isExtraPattern = true;
	}

	uint16_t sysexLength = isExtraPattern ? 0x151d : 0xac6;
	
  // XXX check extrapattern
  if (len < (sysexLength + 5))
    return 0;

  m_memclr(data, sysexLength + 5);
  
  data[0] = 0xF0;
  m_memcpy(data + 1, machinedrum_sysex_hdr, sizeof(machinedrum_sysex_hdr));

  data[6] = MD_PATTERN_MESSAGE_ID;
  data[7] = 0x03; // version
  data[8] = 0x01;
  data[9] = origPosition;

	MDDataToSysexEncoder encoder(data + 0xA, 74);
	
  for (int i = 0; i < 16; i++) {
		encoder.pack32(trigPatterns[i]);
  }
	encoder.finish();

  recalculateLockPatterns();
	
	encoder.init(data + 0x54, 74);
  for (int i = 0; i < 16; i++) {
		encoder.pack32(lockPatterns[i]);
  }
	encoder.finish();

	encoder.init(data + 0x9e, 19);
	encoder.pack32(accentPattern);
	encoder.pack32(slidePattern);
	encoder.pack32(swingPattern);
	encoder.pack32(swingAmount);
	encoder.finish();
    
  data[0xB1] = accentAmount;
  data[0xB2] = patternLength;
  data[0xb3] = doubleTempo ? 1 : 0;
  data[0xb4] = scale;
  data[0xb5] = kit;
  data[0xb6] = numLockedRows;

  uint16_t cnt = 0;

	encoder.init(data + 0xB7, 2341);
	uint8_t lockData[64][32];
  for (int i = cnt; i < 64; i++) {
    m_memclr(lockData[i], 32);
  }
  for (int track = 0; track < 16; track++) {
    for (int param = 0; param < 24; param++) {
      int8_t lock = paramLocks[track][param];
      if (lock != -1) {
#if 0
				printf("track %d, param %d, lock %d\n", track, param, lock);
				for (uint8_t i = 0; i < 32; i++) {
					printf("%.2d ", locks[lock][i]);
				}
				printf("\n");
#endif
				
				m_memcpy(lockData[lock], locks[lock], 32);
				cnt++;
      }
    }
  }
	for (int i = cnt; i < 64; i++) {
		encoder.pack((uint8_t *)lockData, (64 * 32));
	}
	encoder.finish();
  
	encoder.init(data + 0x9dc, 234);
	encoder.pack32(accentEditAll ? 1 : 0);
	encoder.pack32(slideEditAll ? 1 : 0);
  encoder.pack32(swingEditAll ? 1 : 0);

  for (int i = 0; i < 16; i++) {
		encoder.pack32(accentPatterns[i]);
  }
  for (int i = 0; i < 16; i++) {
		encoder.pack32(slidePatterns[i]);
  }
  for (int i = 0; i < 16; i++) {
		encoder.pack32(swingPatterns[i]);
  }
	encoder.finish();

	if (isExtraPattern) {
		encoder.init(data + 0xac6, 2647);
		for (int i = 0; i < 16; i++) {
			encoder.pack32hi(trigPatterns[i]);
		}
		encoder.pack32hi(accentPattern);
		encoder.pack32hi(slidePattern);
		encoder.pack32hi(swingPattern);
    
		for (int i = cnt; i < 64; i++) {
			m_memclr(lockData[i], 32);
		}
		for (int track = 0; track < 16; track++) {
			for (int param = 0; param < 24; param++) {
				int8_t lock = paramLocks[track][param];
				if (lock != -1) {
					
					m_memcpy(lockData[lock], locks[lock] + 32, 32);
					cnt++;
				}
			}
		}
		encoder.pack((uint8_t *)lockData, 64 * 32);
		encoder.finish();
	}
	
  uint16_t checksum = 0;
  for (int i = 9; i < sysexLength; i++)
    checksum += data[i];
  data[sysexLength] = (uint8_t)((checksum >> 7) & 0x7F);
  data[sysexLength + 1] = (uint8_t)(checksum & 0x7F);
  uint16_t length = sysexLength + 5 - 7 - 3;
  data[sysexLength + 2] = (uint8_t)((length >> 7) &0x7F);
  data[sysexLength + 3 ] = (uint8_t)(length & 0x7F);
  data[sysexLength + 4] = 0xF7;

  return sysexLength + 5;
}

bool MDPattern::isTrackEmpty(uint8_t track) {
	return ((trigPatterns[track] == 0) &&
					(lockPatterns[track] == 0));
}

bool MDPattern::isLockPatternEmpty(uint8_t idx) {
  for (int i = 0; i < 64; i++) {
    if (locks[idx][i] != 255)
      return false;
  }
  return true;
}


bool MDPattern::isLockPatternEmpty(uint8_t idx, uint64_t trigs) {
  for (int i = 0; i < 64; i++) {
    if (locks[idx][i] != 255 && IS_BIT_SET64(trigs, i))
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
  for (int i = 0; i < 64; i++) {
    clearTrig(track, i);
  }
  clearTrackLocks(track);
}

void MDPattern::clearLockPattern(uint8_t lock) {
  if (lock >= 64)
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
  CLEAR_BIT64(trigPatterns[track], trig);
  for (int i = 0; i < 24; i++) {
    clearLock(track, trig, i);
  }
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
    for (int i = 0; i < 64; i++) {
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
			 
