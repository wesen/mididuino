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
			noteNBR[i][j] = -1;
    }
  }
  locksUsed = 0;
  for (int i = 0; i < 62; i++) {
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
    swingPatterns[i] = 0xaaaaaaaa;

    midiNoteOnTrigs[i] = 0;
    midiNoteOffTrigs[i] = 0;
    midiTriglessTrigs[i] = 0;
    midiSlidePatterns[i] = 0;
    midiSwingPatterns[i] = 0xaaaaaaaa;

    lockPatterns[i] = 0;
  }

  //  accentAmount = 0;

	swingAmount = 50 << 14;
  patternLength = 16;
  kit = 0;
  origPosition = 0;
  //  scale = 0;
}

bool MNMPattern::fromSysex(uint8_t *data, uint16_t len) {
	if (!ElektronHelper::checkSysexChecksum(data, len)) {
    return false;
  }

  origPosition = data[3];
	MNMSysexDecoder decoder(DATA_ENCODER_INIT(data + 4, len - 4));

	decoder.get64(ampTrigs, 6);
	decoder.get64(filterTrigs, 6);
	decoder.get64(lfoTrigs, 6);
	decoder.get64(offTrigs, 6);
	decoder.get64(midiNoteOnTrigs, 6);
	decoder.get64(midiNoteOffTrigs, 6);
	decoder.get64(triglessTrigs, 6);
	decoder.get64(chordTrigs, 6);
	decoder.get64(midiTriglessTrigs, 6);
	decoder.get64(slidePatterns, 6);
	decoder.get64(swingPatterns, 6);
	decoder.get64(midiSlidePatterns, 6);
	decoder.get64(midiSwingPatterns, 6);

	decoder.get32(&swingAmount);

	decoder.get64(lockPatterns, 6);
	decoder.get((uint8_t *)noteNBR, 6 * 64);
	decoder.get8(&patternLength);
	decoder.get8(&doubleTempo);
	decoder.get8(&kit);
	decoder.get8((uint8_t *)&patternTranspose);

	decoder.get((uint8_t *)transpose, 6);
	decoder.get(scale, 6);
	decoder.get(key, 6);
	decoder.get((uint8_t *)midiTranspose, 6);
	decoder.get(midiScale, 6);
	decoder.get(midiKey, 6);

	decoder.get(arpPlay, 6);
	decoder.get(arpMode, 6);
	decoder.get(arpOctaveRange, 6);
	decoder.get(arpMultiplier, 6);
	decoder.get(arpDestination, 6);
	decoder.get(arpLength, 6);
	decoder.get((uint8_t *)arpPattern, 6 * 16);

	decoder.get(midiArpPlay, 6);
	decoder.get(midiArpMode, 6);
	decoder.get(midiArpOctaveRange, 6);
	decoder.get(midiArpMultiplier, 6);
	decoder.get(midiArpLength, 6);
	decoder.get((uint8_t *)midiArpPattern, 6 * 16);

	decoder.get(unused, 4);

	decoder.get16(&midiNotesUsed);
	decoder.get8(&chordNotesUsed);
	decoder.get8(unused + 4);
	decoder.get8(&locksUsed);
	decoder.get((uint8_t *)locks, 62 * 64);

  for (int i = 0; i < 400; i++) {
    uint16_t l;
		decoder.get16(&l);
    midiNotes[i].note = ((l >> 9) & 0x7f);
    midiNotes[i].track = (l >> 6) & 0x7;
    midiNotes[i].position = l & 0x3f;
  }
  for (int i = 0; i < 192; i++) {
    uint16_t l;
		decoder.get16(&l);
    chordNotes[i].note = ((l >> 9) & 0x7f);
    chordNotes[i].track = (l >> 6) & 0x7;
    chordNotes[i].position = l & 0x3f;
  }

	uint8_t foobar;
	decoder.get8(&foobar);

	numRows = 0;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 64; j++) {
      if (IS_BIT_SET64(lockPatterns[i], j)) {
				//				printf("lock track %d param %d to lock idx %d\n", i, j, numRows);
				paramLocks[i][j] = numRows;
				lockTracks[numRows] = i;
				lockParams[numRows] = j;
				numRows++;
      } else {
				paramLocks[i][j] = -1;
			}
    }
  }

  return true;
}

#ifdef HOST_MIDIDUINO
#define DEBUG(note, vel)
#else
#define DEBUG(note, vel) MidiUart.sendNoteOn(note, vel); delay(300); delay(300);
#endif

uint16_t MNMPattern::toSysex(uint8_t *data, uint16_t len) {
	DEBUG(10, 0);
	cleanupLocks();
	DEBUG(10, 10);
	recalculateLockPatterns();
	DEBUG(10, 20);
	
  m_memcpy(data + 1, monomachine_sysex_hdr, sizeof(monomachine_sysex_hdr));
	
  data[6] = MNM_PATTERN_MESSAGE_ID;
  data[7] = 0x05;
  data[8] = 0x01;
  data[9] = origPosition;

	MNMDataToSysexEncoder encoder(DATA_ENCODER_INIT(data + 10, len - 10));
	encoder.totalCnt++;

	encoder.pack64(ampTrigs, 6);
	encoder.pack64(filterTrigs, 6);
	encoder.pack64(lfoTrigs, 6);
	encoder.pack64(offTrigs, 6);
	encoder.pack64(midiNoteOnTrigs, 6);
	encoder.pack64(midiNoteOffTrigs, 6);
	encoder.pack64(triglessTrigs, 6);
	encoder.pack64(chordTrigs, 6);
	encoder.pack64(midiTriglessTrigs, 6);
	encoder.pack64(slidePatterns, 6);
	encoder.pack64(swingPatterns, 6);
	encoder.pack64(midiSlidePatterns, 6);
	encoder.pack64(midiSwingPatterns, 6);

	DEBUG(10, 30);
	
	encoder.pack32(swingAmount);

	encoder.pack64(lockPatterns, 6);
	encoder.pack((uint8_t *)noteNBR, 6 * 64);
	encoder.pack8(patternLength);
	encoder.pack8(doubleTempo);
	encoder.pack8(kit);
	encoder.pack8(patternTranspose);

	encoder.pack((uint8_t *)transpose, 6);
	encoder.pack(scale, 6);
	encoder.pack(key, 6);
	encoder.pack((uint8_t *)midiTranspose, 6);
	encoder.pack(midiScale, 6);
	encoder.pack(midiKey, 6);

	encoder.pack(arpPlay, 6);
	encoder.pack(arpMode, 6);
	encoder.pack(arpOctaveRange, 6);
	encoder.pack(arpMultiplier, 6);
	encoder.pack(arpDestination, 6);
	encoder.pack(arpLength, 6);
	encoder.pack((uint8_t *)arpPattern, 6 * 16);

	DEBUG(10, 40);
	
	encoder.pack(midiArpPlay, 6);
	encoder.pack(midiArpMode, 6);
	encoder.pack(midiArpOctaveRange, 6);
	encoder.pack(midiArpMultiplier, 6);
	encoder.pack(midiArpLength, 6);
	encoder.pack((uint8_t *)midiArpPattern, 6 * 16);

	encoder.pack(unused, 4);

	encoder.pack16(midiNotesUsed);
	encoder.pack8(chordNotesUsed);
	encoder.pack8(unused[4]);
	encoder.pack8(locksUsed);

	uint8_t lockData[62][64];
	uint8_t lockIdx = 0;
	m_memset(lockData, 62 * 64, 0xFF);
	for (int track = 0; track < 6; track++) {
		for (int param = 0; param < 64; param++) {
			int8_t lock = paramLocks[track][param];
			if (lock != -1) {
				m_memcpy(lockData[lockIdx], locks[lock], 64);
				lockIdx++;
			}
		}
	}
	encoder.pack((uint8_t *)lockData, 62 * 64);

  for (int i = 0; i < 400; i++) {
    uint16_t x = ((uint16_t)midiNotes[i].note << 9) |
      ((uint16_t)midiNotes[i].track << 6) | (midiNotes[i].position);
		encoder.pack16(x);
  }
	
  for (int i = 0; i < 192; i++) {
    uint16_t x =
			((uint16_t)chordNotes[i].note << 9) |
			((uint16_t)chordNotes[i].track << 6) |
			(chordNotes[i].position);
		encoder.pack16(x);
  }

	encoder.pack8(0xff);
	
  uint16_t enclen = encoder.finish();
	ElektronHelper::calculateSysexChecksum(data, enclen + 10);

  return enclen + 10 + 5;
}

#ifdef HOST_MIDIDUINO

void print64(uint64_t trigs, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (IS_BIT_SET64(trigs, i)) {
      printf("X  ");
    } else {
      printf(".  ");
    }
  }
  printf("\n");
}

void MNMPattern::print() {
  for (int i = 0; i < 6; i++) {
    printf("track %d\n", i);
    printf("amp     : ");
		printf("%llx\n", ampTrigs[i]);
    print64(ampTrigs[i], patternLength);
    for (int j = 0; j < patternLength; j++) {
      printf("%.2d ", noteNBR[i][j]);
    }
    printf("\n");
    printf("filter  : ");
    print64(filterTrigs[i], patternLength);
    printf("lfo     : ");
    print64(lfoTrigs[i], patternLength);
    printf("off     : ");
    print64(offTrigs[i], patternLength);
    printf("trigless: ");
    print64(triglessTrigs[i], patternLength);
    printf("chord   : ");
    print64(chordTrigs[i], patternLength);
    printf("locks   : ");
    print64(lockPatterns[i], patternLength);
		printf("lockIdx : ");
    for (int j = 0; j < 64; j++) {
			if (isParamLocked(i, j)) {
				printf("%d: %.d ", j, paramLocks[i][j]);
			}
    }
		printf("\n");
		for (int j = 0; j < 64; j++) {
			if (isParamLocked(i, j)) {
				printf("lock %d : ", j);
				for (int step = 0; step < patternLength; step++) {
					printf("%.3d ", getLock(i, step, j));
				}
				printf("\n");
			}
		}
    printf("\n");
		
  }
  for (int i = 0; i < 6; i++) {
    printf("midi track %d\n", i);
    printf("note on  : ");
    print64(midiNoteOnTrigs[i], patternLength);
    printf("note off : ");
    print64(midiNoteOffTrigs[i], patternLength);
    printf("trigless : ");
    print64(midiTriglessTrigs[i], patternLength);
  }
}
#endif

bool MNMPattern::isTrackEmpty(uint8_t track) {
	return ((ampTrigs[track] == 0) &&
					(filterTrigs[track] == 0) &&
					(lfoTrigs[track] == 0) &&
					(offTrigs[track] == 0) &&
					(triglessTrigs[track] == 0) &&
					(chordTrigs[track] == 0));
}

bool MNMPattern::isMidiTrackEmpty(uint8_t track) {
	return ((midiNoteOnTrigs[track] == 0) &&
					(midiNoteOffTrigs[track] == 0) &&
					(midiTriglessTrigs[track] == 0));
}

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
			uint8_t lockTrack = lockTracks[i];
      if (isLockPatternEmpty(i, ampTrigs[lockTrack]) &&
					isLockPatternEmpty(i, filterTrigs[lockTrack]) &&
					isLockPatternEmpty(i, lfoTrigs[lockTrack]) &&
					isLockPatternEmpty(i, triglessTrigs[lockTrack])) { // trigs
				if (lockParams[i] != -1) {
					paramLocks[lockTrack][lockParams[i]] = -1;
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
	ampTrigs[track] = 0;
	filterTrigs[track] = 0;
	lfoTrigs[track] = 0;
	offTrigs[track] = 0;
	triglessTrigs[track] = 0;
	chordTrigs[track] = 0;
	slidePatterns[track] = 0;
	// XXX	swingPatterns[track] = 0;
  clearTrackLocks(track);
}

void MNMPattern::clearMidiTrack(uint8_t track) {
	if (track >= 6) {
		return;
	}
	midiNoteOnTrigs[track] = 0;
	midiNoteOffTrigs[track] = 0;
	midiTriglessTrigs[track] = 0;
	midiSlidePatterns[track] = 0;
	// XXX	midiSwingPatterns[track] = 0;
}
 

void MNMPattern::clearParamLocks(uint8_t track, uint8_t param) {
  int8_t idx = paramLocks[track][param];
  if (idx != -1) {
    clearLockPattern(idx);
    paramLocks[track][param] = -1;
  }
}

void MNMPattern::clearTrackLocks(uint8_t track) {
  for (int i = 0; i < 64; i++) {
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

  if (isLockPatternEmpty(idx, ampTrigs[track]) && 
			isLockPatternEmpty(idx, filterTrigs[track]) &&
			isLockPatternEmpty(idx, lfoTrigs[track]) &&
			isLockPatternEmpty(idx, triglessTrigs[track])) { // trigs
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

