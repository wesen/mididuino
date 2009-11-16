#include <CppUnitLite2.h>
#include <TestResultStdErr.h>

#include <Elektron.hh>

#ifdef DATA_ENCODER_CHECKING
#define CHECK_DATA_ENCODE(encode) { CHECK(encode); }
#define CHECK_DATA_ENCODE_NOT(encode) { CHECK(!(encode)); }
#else
#define CHECK_DATA_ENCODE(encode) { encode; }
#define CHECK_DATA_ENCODE_NOT(encode) { encode; }
#endif

struct MDDataToSysexFixture {
	MDDataToSysexEncoder encoder;
};

TEST_F (MDDataToSysexFixture, MDDataToSysexByte) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	uint16_t len = encoder.finish();
	CHECK_EQUAL(2, len);
	CHECK_EQUAL(0, (int)data[0]);
	CHECK_EQUAL(0, (int)data[1]);
}

TEST_F (MDDataToSysexFixture, MDDataToSysexByteHigh) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(128));
	uint16_t len = encoder.finish();
	CHECK_EQUAL(2, len);
	CHECK_EQUAL((1 << 6), (int)data[0]);
	CHECK_EQUAL(0, (int)data[1]);
}

TEST_F (MDDataToSysexFixture, MDDataToSysexBytes) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint8_t i = 0; i < 7; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(128));
	}
	uint16_t len = encoder.finish();
	CHECK_EQUAL(8, len);
	CHECK_EQUAL(0x7F, (int)data[0]);
	for (uint8_t i = 0; i < 7; i++) {
		CHECK_EQUAL(0, (int)data[1 + i]);
	}
}

#ifdef DATA_ENCODER_CHECKING
TEST_F (MDDataToSysexFixture, MDDataTestOverflow) {
	uint8_t data[2];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	CHECK_DATA_ENCODE_NOT(encoder.pack8(0));
}

TEST_F (MDDataToSysexFixture, MDDataTestOverflow2) {
	uint8_t data[8];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	CHECK_DATA_ENCODE(encoder.pack8(1));
	CHECK_DATA_ENCODE(encoder.pack8(2));
	CHECK_DATA_ENCODE(encoder.pack8(3));
	CHECK_DATA_ENCODE(encoder.pack8(4));
	CHECK_DATA_ENCODE(encoder.pack8(5));
	CHECK_DATA_ENCODE(encoder.pack8(6));
	CHECK_DATA_ENCODE_NOT(encoder.pack8(0));
}
#endif

TEST_F (MDDataToSysexFixture, MDDataToSysex32Bit) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	uint32_t tmp = 0x00;
	CHECK_DATA_ENCODE(encoder.pack32(tmp));
	uint16_t len = encoder.finish();
	CHECK_EQUAL(5, len);
	CHECK_EQUAL(0, (int)data[0]);
	CHECK_EQUAL(0, (int)data[1]);
	CHECK_EQUAL(0, (int)data[2]);
	CHECK_EQUAL(0, (int)data[3]);
	CHECK_EQUAL(0, (int)data[4]);
}

TEST_F (MDDataToSysexFixture, MDDataToSysex32Bit2) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	uint32_t tmp = 0x01;
	encoder.pack32(tmp);
	uint16_t len = encoder.finish();
	CHECK_EQUAL(5, len);
	CHECK_EQUAL(0, (int)data[0]);
	CHECK_EQUAL(0, (int)data[1]);
	CHECK_EQUAL(0, (int)data[2]);
	CHECK_EQUAL(0, (int)data[3]);
	CHECK_EQUAL(1, (int)data[4]);
}

struct MDDataBothFixture {
	MDDataToSysexEncoder encoder;
	MDSysexDecoder decoder;
};

TEST_F (MDDataBothFixture, MDDataBoth8Bit) {
	uint8_t data[1024];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint16_t i = 0; i < 512; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(i & 0xFF));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = 0; i < 512; i++) {
		uint8_t tmp;
		CHECK_DATA_ENCODE(decoder.get8(&tmp));
		CHECK_EQUAL(i & 0xFF, (int)tmp);
	}
}

TEST_F (MDDataBothFixture, MDDataBoth8BitMore) {
	uint8_t data[65000];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint16_t i = 0; i < 55000; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(i & 0xFF));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = 0; i < 55000; i++) {
		uint8_t tmp;
		CHECK_DATA_ENCODE(decoder.get8(&tmp));
		if (tmp != (i & 0xFF)) {
			printf("error at %d\n", i);
			//			printf("%p, %p: %d\n", encoder.data + encoder.maxLen - 8, encoder.ptr,
			//						 encoder.data + encoder.maxLen - 8 - encoder.ptr);
		}
		CHECK_EQUAL(i & 0xFF, (int)tmp);
	}
}


TEST_F (MDDataBothFixture, MDDataBoth16Bit) {
	uint8_t data[8192];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	uint16_t cnt = 1024;
	uint16_t start = 0;
	for (uint16_t i = start; i < start + cnt; i++) {
		CHECK_DATA_ENCODE(encoder.pack16(i));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = start; i < start + cnt; i++) {
		uint16_t tmp;
		CHECK_DATA_ENCODE(decoder.get16(&tmp));
		CHECK_EQUAL(i, (uint16_t)tmp);
	}
}

