#include "Elektron.hh"
#include "MDDataEncoder.hh"

void MDDataToSysexEncoder::init(DATA_ENCODER_INIT(uint8_t *_sysex, uint16_t _sysexLen)) {
	DataEncoder::init(DATA_ENCODER_INIT(_sysex, _sysexLen));
	cnt7 = 0;
	retLen = 0;
}

DATA_ENCODER_RETURN_TYPE MDDataToSysexEncoder::encode7Bit(uint8_t inb) {
  uint8_t msb = inb >> 7;
  uint8_t c = inb & 0x7F;

#ifdef DATA_ENCODER_CHECKING
	DATA_ENCODER_CHECK((ptr + cnt7 + 1) < (data + maxLen));
#endif

	if (cnt7 == 0) {
		ptr[0] = 0;
	}
  ptr[0] |= msb << (6 - cnt7);
  ptr[cnt7 + 1] = c;
  if (++cnt7 == 7) {
    retLen += 8;
    ptr += 8;
    cnt7 = 0;
  }

	DATA_ENCODER_TRUE();
}

DATA_ENCODER_RETURN_TYPE MDDataToSysexEncoder::pack8(uint8_t inb) {
	DATA_ENCODER_CHECK(encode7Bit(inb));
	DATA_ENCODER_TRUE();
}

uint16_t MDDataToSysexEncoder::finish() {
	return retLen + ((cnt7 > 0) ? (cnt7 + 1) : 0);
}

void MDSysexToDataEncoder::init(DATA_ENCODER_INIT(uint8_t *_data, uint16_t _maxLen)) {
	DataEncoder::init(DATA_ENCODER_INIT(_data, _maxLen));
	cnt7 = 0;
  cnt = 0;
	retLen = 0;
}

DATA_ENCODER_RETURN_TYPE MDSysexToDataEncoder::pack8(uint8_t inb) {
	if ((cnt % 8) == 0) {
		bits = inb;
	} else {
		bits <<= 1;
		tmpData[cnt7++] = inb | (bits & 0x80);
	}
	cnt++;

	if (cnt7 == 7) {
		DATA_ENCODER_CHECK(unpack8Bit());
	}
	DATA_ENCODER_TRUE();
}

DATA_ENCODER_RETURN_TYPE MDSysexToDataEncoder::unpack8Bit() {
	for (uint8_t i = 0; i < cnt7; i++) {
		*(ptr++) = tmpData[i];
		retLen++;
	}
	cnt7 = 0;
	DATA_ENCODER_TRUE();
}

uint16_t MDSysexToDataEncoder::finish() {
#ifdef DATA_ENCODER_CHECKING
	if (!unpack8Bit())
		return 0;
	else
		return retLen;
#else
	unpack8Bit();
	return retLen;
#endif
}

 void MDSysexDecoder::init(DATA_ENCODER_INIT(uint8_t *_data, uint16_t _maxLen)) {
	 DataDecoder::init(DATA_ENCODER_INIT(_data, _maxLen));
	 start7Bit();
}

DATA_ENCODER_RETURN_TYPE MDSysexDecoder::get8(uint8_t *c) {
	if (in7Bit) {
		if ((cnt7 % 8) == 0) {
			bits = *(ptr++);
			cnt7++;
		}
		bits <<= 1;
		*c = *(ptr++) | (bits & 0x80);
		cnt7++;
	} else {
		*c = *(ptr++);
	}

	DATA_ENCODER_TRUE();
}
