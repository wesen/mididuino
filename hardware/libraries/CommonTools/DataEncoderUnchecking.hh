#ifndef DATA_ENCODER_UNCHECKING_H__
#define DATA_ENCODER_UNCHECKING_H__

#define DATA_ENCODER_RETURN_TYPE void
#define DATA_ENCODER_CHECK(condition) { condition; }
#define DATA_ENCODER_TRUE()  return
#define DATA_ENCODER_FALSE() return

#define DATA_ENCODER_UNCHECKING 1

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

	virtual DATA_ENCODER_RETURN_TYPE pack(uint8_t *inb, uint16_t len) {
		for (uint16_t i = 0; i < len; i++) {
			pack8(inb[i]);
		}
	}
	
  virtual DATA_ENCODER_RETURN_TYPE pack8(uint8_t inb) {
  }

virtual DATA_ENCODER_RETURN_TYPE pack16(uint16_t inw) {
		pack8((inw >> 8) & 0xFF);
		pack8(inw & 0xFF);
	}

	virtual DATA_ENCODER_RETURN_TYPE pack32(uint32_t inw) {
		pack8((inw >> 24) & 0xFF);
		pack8((inw >> 16) & 0xFF);
		pack8((inw >> 8) & 0xFF);
		pack8(inw & 0xFF);
	}

	virtual DATA_ENCODER_RETURN_TYPE pack32(uint32_t *addr, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			pack32(addr[i]);
		}
	}

	virtual DATA_ENCODER_RETURN_TYPE pack32(uint64_t *addr, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			pack32(addr[i]);
		}
	}

	virtual DATA_ENCODER_RETURN_TYPE pack32hi(uint64_t inw) {
		pack32(inw >> 32);
	}

	virtual DATA_ENCODER_RETURN_TYPE pack64(uint64_t inw) {
		pack32(inw & 0xFFFFFFFF);
		pack32hi(inw);
	}

	virtual DATA_ENCODER_RETURN_TYPE pack64(uint64_t *addr, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			pack64(addr[i]);
		}
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

	virtual DATA_ENCODER_RETURN_TYPE get8(uint8_t *c) {
	}
	
	virtual DATA_ENCODER_RETURN_TYPE get16(uint16_t *c) {
		uint8_t b1, b2;
		get8(&b1);
		get8(&b2);
		*c = (((uint16_t)b1) << 8) | b2;
	}
	virtual DATA_ENCODER_RETURN_TYPE get32(uint32_t *c) {
		uint16_t b1, b2;
		get16(&b1);
		get16(&b2);
		*c = (((uint32_t)b1) << 16) | b2;
	}

	virtual DATA_ENCODER_RETURN_TYPE get32(uint32_t *c, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			get32(&c[i]);
		}
	}
	virtual DATA_ENCODER_RETURN_TYPE get32(uint64_t *c) {
		uint32_t c2;
		get32(&c2);
		*c = c2;
	}
	virtual DATA_ENCODER_RETURN_TYPE get32(uint64_t *c, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			get32(&c[i]);
		}
	}
	virtual DATA_ENCODER_RETURN_TYPE get32hi(uint64_t *c) {
		uint32_t c2;
		get32(&c2);
		*c |= ((uint64_t)c2) << 32;
	}
	virtual DATA_ENCODER_RETURN_TYPE get32hi(uint64_t *c, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			get32hi(&c[i]);
		}
	}

	virtual DATA_ENCODER_RETURN_TYPE get64(uint64_t *c) {
		get32(c);
		get32hi(c);
	}

	virtual DATA_ENCODER_RETURN_TYPE get64(uint64_t *c, uint16_t cnt) {
		for (uint16_t i = 0; i < cnt; i++) {
			get64(&c[i]);
		}
	}
	
	virtual uint16_t get(uint8_t *data, uint16_t len) {
		uint16_t i;
		for (i = 0; i < len; i++) {
			get8(data + i);
		}
		return i;
	}

#ifdef HOST_MIDIDUINO
  virtual ~DataDecoder() { };
#endif
};

#endif /* DATA_ENCODER_UNCHECKING_H__ */
