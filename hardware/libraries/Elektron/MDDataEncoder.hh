#ifndef MD_DATA_ENCODER_H__
#define MD_DATA_ENCODER_H__

#include "DataEncoder.hh"


class MDDataToSysexEncoder : public DataEncoder {
  uint16_t retLen;
  uint16_t cnt7;

public:
	MDDataToSysexEncoder(uint8_t *_sysex = NULL, uint16_t _sysexLen = 0) {
		init(_sysex, _sysexLen);
	}

	virtual void init(uint8_t *_sysex, uint16_t _sysexLen);
	bool encode7Bit(uint8_t inb);
	virtual bool pack8(uint8_t inb);
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
		init(_sysex, _sysexLen);
	}

	virtual void init(uint8_t *_sysex, uint16_t _sysexLen);
	virtual bool pack8(uint8_t inb);
	bool unpack8Bit();
	virtual uint16_t finish();
};

class MDSysexDecoder : public DataDecoder {
  uint8_t cnt7;
  uint8_t bits;
  uint8_t tmpData[7];
	uint16_t cnt;
	
public:
	MDSysexDecoder(uint8_t *_data = NULL, uint16_t _maxLen = 0) {
		init(_data, _maxLen);
	}
	
	virtual void init(uint8_t *_data, uint16_t _maxLen);
	virtual bool get8(uint8_t *c);
};

#endif /* MD_DATA_ENCODER_H__ */
