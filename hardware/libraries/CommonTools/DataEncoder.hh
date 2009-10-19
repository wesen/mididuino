#ifndef DATA_ENCODER_H__
#define DATA_ENCODER_H__

class DataEncoder {
public:
  uint8_t *data;
  uint8_t *ptr;
  uint16_t maxLen;

  virtual void init(uint8_t *_data, uint16_t _maxLen) {
    data = _data;
    maxLen = _maxLen;
    ptr = data;
  }

	virtual bool pack(uint8_t *inb, uint16_t len) {
		for (uint16_t i = 0; i < len; i++) {
			if (!pack8(inb[i]))
				return false;
		}
		return true;
	}
  virtual bool pack8(uint8_t inb) {
    return false;
  }
	virtual bool pack16(uint16_t inw) {
		return pack8((inw >> 8) & 0xFF) && 
			pack8(inw & 0xFF);
		
	}

	virtual bool pack32(uint32_t inw) {
		return pack8((inw >> 24) & 0xFF) &&
			pack8((inw >> 16) & 0xFF) &&
			pack8((inw >> 8) & 0xFF) &&
			pack8(inw & 0xFF);
	}

	virtual bool pack32hi(uint64_t inw) {
		return pack32(inw >> 32);
	}

	
  virtual uint16_t finish() {
    return 0;
  }
#ifdef HOST_MIDIDUINO
  virtual ~DataEncoder() { };
#endif
};

class DataDecoder {
public:
	uint8_t *data;
	uint8_t *ptr;
	uint16_t maxLen;

	DataDecoder() {
	}

	virtual void init(uint8_t *_data, uint16_t _maxLen) {
		data = _data;
		maxLen = _maxLen;
		ptr = data;
	}

	virtual bool get8(uint8_t *c) = 0;
	virtual bool get16(uint16_t *c) {
		uint8_t b1, b2;
		bool ret = get8(&b1) && get8(&b2);
		if (ret) {
			*c = (((uint16_t)b1) << 8) | b2;
		}
		return ret;
	}
	virtual bool get32(uint32_t *c) {
		uint16_t b1, b2;
		bool ret = get16(&b1) && get16(&b2);
		if (ret) {
			*c = (((uint32_t)b1) << 16) | b2;
		}
		return ret;
	}
	virtual bool get32(uint64_t *c) {
		uint32_t c2;
		if (!get32(&c2)) {
			return false;
		}
		*c = c2;
		return true;
	}
	virtual bool get32hi(uint64_t *c) {
		uint32_t c2;
		if (!get32(&c2)) {
			return false;
		}
		*c |= ((uint64_t)c2) << 32;
		return true;
	}
	virtual uint16_t get(uint8_t *data, uint16_t len) {
		uint16_t i;
		for (i = 0; i < len; i++) {
			if (!get8(data + i))
				break;
		}
		return i;
	}

#ifdef HOST_MIDIDUINO
  virtual ~DataDecoder() { };
#endif
};

#endif /* DATA_ENCODER_H__ */
