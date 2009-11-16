#ifndef ELEKTRON_PATTERN_H__
#define ELEKTRON_PATTERN_H__

class ElektronPattern {
public:
	uint8_t maxParams;
	uint8_t maxTracks;
	uint8_t maxSteps;
	uint8_t maxLocks;

	uint8_t locks[64][64];
	int8_t lockTracks[64];
	int8_t lockParams[64];

	ElektronPattern() {
		maxParams = 0;
		maxTracks = 0;
		maxSteps = 0;
		maxLocks = 0;
		clearPattern();
	}

	void init() {
		clearPattern();
	}
	
	virtual uint8_t getPosition()                 { return 0;	}
	virtual void    setPosition(uint8_t position) { }

	virtual uint8_t getLength()                   {	return 0;	}
	virtual void    setLength(uint8_t length)     { }

	virtual uint8_t getKit()                      { return 0; }
	virtual void    setKit(uint8_t kit)           { }

	/* editing functions */
	virtual bool isTrackEmpty(uint8_t track)                { return true; }
	virtual bool isLockEmpty(uint8_t track, uint8_t param)  { return true; }
	virtual bool isTrigSet(uint8_t track, uint8_t step)     { return false; }
	bool isParamLocked(uint8_t track, uint8_t param);
	bool isLockPatternEmpty(int8_t idx, uint64_t trigs);
	bool isLockEmpty(uint8_t idx);

	virtual void clearPattern()                         { }
	virtual void clearTrack(uint8_t track)              { }
	void clearParamLocks(uint8_t track, uint8_t param);
	void clearTrackLocks(uint8_t track);
	void clearLockPattern(int8_t lock);

	virtual void setTrig(uint8_t track, uint8_t step)                { }
	virtual void setNote(uint8_t track, uint8_t step, uint8_t pitch) { }
	virtual void clearTrig(uint8_t track, uint8_t step)              { }

	bool addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value);
	void clearLock(uint8_t track, uint8_t trig, uint8_t param);
	uint8_t getLock(uint8_t track, uint8_t trig, uint8_t param);

	virtual int8_t getLockIdx(uint8_t track, uint8_t param)             { return -1; }
	virtual void setLockIdx(uint8_t track, uint8_t param, int8_t value) { }
	virtual void recalculateLockPatterns()                              { }

	int8_t getNextEmptyLock();
	void cleanupLocks();

	/* sysex functions */
	virtual bool fromSysex(uint8_t *sysex, uint16_t len) { return false; }
	
};

#endif /* ELEKTRON_PATTERN_H__ */
