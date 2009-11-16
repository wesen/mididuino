#include "Elektron.hh"
#include "MDDataEncoder.hh"

void MDDataToSysexEncoder::init(DATA_ENCODER_INIT(uint8_t *_sysex, uint16_t _sysexLen),
																MidiUartParent *_uart) {
	DataEncoder::init(DATA_ENCODER_INIT(_sysex, _sysexLen));
	uart = _uart;
	if (uart != NULL) {
		data = ptr = buf;
	}

	inChecksum = false;
	checksum = 0;
	retLen = 0;
	start7Bit();
}


void MDDataToSysexEncoder::start7Bit() {
	in7Bit = true;
	cnt7 = 0;
}

void MDDataToSysexEncoder::stop7Bit() {
	finish();
	in7Bit = false;
	cnt7 = 0;
}

void MDDataToSysexEncoder::reset() {
	finish();
	start7Bit();
}

void MDDataToSysexEncoder::startChecksum() {
	checksum = 0;
	inChecksum = true;
}

void MDDataToSysexEncoder::finishChecksum() {
	uint16_t len = retLen - 5;
	inChecksum = false;
	stop7Bit();
	pack8((checksum >> 7) & 0x7F);
	pack8(checksum & 0x7F);
	pack8((len >> 7) & 0x7F);
	pack8(len & 0x7F);
	pack8(0xF7);
}

uint16_t MDDataToSysexEncoder::finish() {
	uint8_t inc = ((cnt7 > 0) ? (cnt7 + 1) : 0);
	cnt7 = 0;
	if (inChecksum) {
		for (uint8_t i = 0; i < inc; i++) {
			checksum += ptr[i];
		}
	}
	ptr += inc;
	retLen += inc;
	return retLen;
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
		if (inChecksum) {
			for (uint8_t i = 0; i < 8; i++) {
				checksum += ptr[i];
			}
		}
		if (uart != NULL) {
			for (uint8_t i = 0; i < 8; i++) {
				uart->putc(data[i]);
			}
			ptr = data;
		} else {
			ptr += 8;
		}
    cnt7 = 0;
  }

	DATA_ENCODER_TRUE();
}

DATA_ENCODER_RETURN_TYPE MDDataToSysexEncoder::pack8(uint8_t inb) {
	if (in7Bit) {
		DATA_ENCODER_CHECK(encode7Bit(inb));
	} else {
#ifdef DATA_ENCODER_CHECKING
		DATA_ENCODER_CHECK((ptr + 1) < (data + maxLen));
#endif
		if (inChecksum) {
			checksum += inb;
		}
		if (uart != NULL) {
			uart->putc(inb);
		} else {
			*(ptr++) = inb;
		}
		retLen++;
	}
	DATA_ENCODER_TRUE();
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
