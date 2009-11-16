#ifndef ELEKTRON_PATTERN_H__
#define ELEKTRON_PATTERN_H__

class ElektronPattern {
public:
	uint8_t maxParams;
	uint8_t maxTracks;
	uint8_t maxSteps;
	uint8_t maxLocks;

	ElektronPattern() {
		maxParams = 0;
		maxTracks = 0;
		maxSteps = 0;
		maxLocks = 0;
	}
	
	virtual void init() {
	}

	virtual uint8_t getPosition() { return 0;	}
	virtual void setPosition(uint8_t position) { }

	virtual uint8_t getLength() {	return 0;	}
	virtual void setLength(uint8_t length) { }

	virtual uint8_t getKit() { return 0; }
	virtual void setKit(uint8_t kit) { }

	/* editing functions */
	virtual bool isTrackEmpty(uint8_t track);
	virtual bool isTrigSet(uint8_t track, uint8_t step);

	virtual void clearPattern();
	virtual void clearParamLocks(uint8_t track, uint8_t param);

	virtual void setTrig(uint8_t track, uint8_t step);
	virtual void setNote(uint8_t track, uint8_t step, uint8_t pitch);
	virtual void clearTrig(uint8_t track, uint8_t step);

	virtual bool addLock(uint8_t track, uint8_t trig, uint8_t param, uint8_t value);
	virtual void clearLock(uint8_t track, uint8_t trig, uint8_t param);
	virtual uint8_t getLock(uint8_t track, uint8_t trig, uint8_t param);
};

#endif /* ELEKTRON_PATTERN_H__ */
