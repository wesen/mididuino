#ifndef MD_DATA_ENCODER_H__
#define MD_DATA_ENCODER_H__

#include "DataEncoder.hh"

class MDDataToSysexEncoder : public DataEncoder {
  uint16_t retLen;
  uint16_t cnt7;

public:
	MDDataToSysexEncoder(DATA_ENCODER_INIT(uint8_t *_sysex = NULL, uint16_t _sysexLen = 0)) {
		init(DATA_ENCODER_INIT(_sysex, _sysexLen));
	}

	virtual void init(DATA_ENCODER_INIT(uint8_t *_sysex, uint16_t _sysexLen));
	DATA_ENCODER_RETURN_TYPE encode7Bit(uint8_t inb);
	virtual DATA_ENCODER_RETURN_TYPE pack8(uint8_t inb);
	virtual uint16_t finish();
};

class MDSysexToDataEncoder : public DataEncoder {
  uint16_t retLen;
  uint8_t cnt7;
  uint8_t bits;
  uint8_t tmpData[7];
	uint16_t cnt;

public:
	MDSysexToDataEncoder(uint8_t *_sysex = NULL, uint16_t _sysexLen = 0) {
		init(DATA_ENCODER_INIT(_sysex, _sysexLen));
	}

	virtual void init(DATA_ENCODER_INIT(uint8_t *_sysex, uint16_t _sysexLen));
	virtual DATA_ENCODER_RETURN_TYPE pack8(uint8_t inb);
	DATA_ENCODER_RETURN_TYPE unpack8Bit();
	virtual uint16_t finish();
};

class MDSysexDecoder : public DataDecoder {
  uint8_t cnt7;
  uint8_t bits;
  uint8_t tmpData[7];
	bool in7Bit;
	
public:
	MDSysexDecoder(DATA_ENCODER_INIT(uint8_t *_data = NULL, uint16_t _maxLen = 0)) {
		init(DATA_ENCODER_INIT(_data, _maxLen));
	}

	void start7Bit() {
		in7Bit = true;
		cnt7 = 0;
	}

	void stop7Bit() {
		in7Bit = false;
	}
	
	virtual void init(DATA_ENCODER_INIT(uint8_t *_data, uint16_t _maxLen));
	virtual DATA_ENCODER_RETURN_TYPE get8(uint8_t *c);
};

#endif /* MD_DATA_ENCODER_H__ */
