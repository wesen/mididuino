#include "Elektron.hh"
#include "MDDataEncoder.hh"

void MDDataToSysexEncoder::init(uint8_t *_sysex, uint16_t _sysexLen) {
	DataEncoder::init(_sysex, _sysexLen);
	cnt7 = 0;
	retLen = 0;
}

bool MDDataToSysexEncoder::encode7Bit(uint8_t inb) {
  uint8_t msb = inb >> 7;
  uint8_t c = inb & 0x7F;
	if ((ptr + cnt7 + 1) >= (data + maxLen)) {
		return false;
	}
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
  return true;
}

bool MDDataToSysexEncoder::pack8(uint8_t inb) {
	return encode7Bit(inb);
}

uint16_t MDDataToSysexEncoder::finish() {
	return retLen + ((cnt7 > 0) ? (cnt7 + 1) : 0);
}

void MDSysexToDataEncoder::init(uint8_t *_data, uint16_t _maxLen) {
	DataEncoder::init(_data, _maxLen);
	cnt7 = 0;
  cnt = 0;
	retLen = 0;
}

bool MDSysexToDataEncoder::pack8(uint8_t inb) {
	if ((cnt % 8) == 0) {
		bits = inb;
	} else {
		bits <<= 1;
		tmpData[cnt7++] = inb | (bits & 0x80);
	}
	cnt++;

	if (cnt7 == 7) {
		return unpack8Bit();
	}
	return true;
}

bool MDSysexToDataEncoder::unpack8Bit() {
	for (uint8_t i = 0; i < cnt7; i++) {
		*(ptr++) = tmpData[i];
		retLen++;
	}
	cnt7 = 0;
	return true;
}

uint16_t MDSysexToDataEncoder::finish() {
	if (!unpack8Bit())
		return 0;
	else
		return retLen;
}

void MDSysexDecoder::init(uint8_t *_data, uint16_t _maxLen) {
	DataDecoder::init(_data, _maxLen);
	cnt7 = 0;
  cnt = 0;
}

bool MDSysexDecoder::get8(uint8_t *c) {
	if ((cnt % 8) == 0) {
		bits = *(ptr++);
		cnt++;
	}
	bits <<= 1;
	*c = *(ptr++) | (bits & 0x80);
	cnt++;

	return true;
}
