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

struct MNMDataToSysexFixture {
	MNMDataToSysexEncoder encoder;
};

TEST_F (MNMDataToSysexFixture, MNMDataToSysexByte) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	uint16_t len = encoder.finish();
	CHECK_EQUAL(2, len);
	CHECK_EQUAL(0, (int)data[0]);
	CHECK_EQUAL(0, (int)data[1]);
}

TEST_F (MNMDataToSysexFixture, MNMDataToSysexByte2) {
	uint8_t data[16];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	CHECK_DATA_ENCODE(encoder.pack8(0));
	uint16_t len = encoder.finish();
	CHECK_EQUAL(3, len);
	CHECK_EQUAL(64, (int)data[0]);
	CHECK_EQUAL(0x02, (int)data[1]);
	CHECK_EQUAL(0x00, (int)data[2]);
}

struct MNMDataBothFixture {
	MNMDataToSysexEncoder encoder;
	MNMSysexDecoder decoder;
};

TEST_F (MNMDataBothFixture, MNMDataBoth8Bit) {
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


TEST_F (MNMDataBothFixture, MNMDataBoth8BitNull) {
	uint8_t data[1024];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint16_t i = 0; i < 512; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(0));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = 0; i < 512; i++) {
		uint8_t tmp;
		CHECK_DATA_ENCODE(decoder.get8(&tmp));
		CHECK_EQUAL(0, (int)tmp);
	}
}

TEST_F (MNMDataBothFixture, MNMDataBoth8BitFF) {
	uint8_t data[1024];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint16_t i = 0; i < 512; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(0xFF));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = 0; i < 512; i++) {
		uint8_t tmp;
		CHECK_DATA_ENCODE(decoder.get8(&tmp));
		CHECK_EQUAL(0xFF, (int)tmp);
	}
}



TEST_F (MNMDataBothFixture, MNMDataBoth8BitMore) {
	uint8_t data[65000];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint16_t i = 0; i < 30000; i++) {
		CHECK_DATA_ENCODE(encoder.pack8(i & 0xFF));
	}
	uint16_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint16_t i = 0; i < 30000; i++) {
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


TEST_F (MNMDataBothFixture, MNMDataBoth16Bit) {
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

TEST_F (MNMDataBothFixture, MNMDataBoth32Bit) {
	uint8_t data[65535];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint32_t i = 0; i < 512; i++) {
		CHECK_DATA_ENCODE(encoder.pack32(i));
	}
	uint32_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint32_t i = 0; i < 512; i++) {
		uint32_t tmp;
		CHECK_DATA_ENCODE(decoder.get32(&tmp));
		CHECK_EQUAL(i, tmp);
	}
}


TEST_F (MNMDataBothFixture, MNMDataBoth64Bit) {
	uint8_t data[65535];
	encoder.init(DATA_ENCODER_INIT(data, countof(data)));
	for (uint64_t i = 0; i < 512; i++) {
		CHECK_DATA_ENCODE(encoder.pack64(i));
	}
	uint64_t len = encoder.finish();

	decoder.init(DATA_ENCODER_INIT(data, len));
	for (uint64_t i = 0; i < 512; i++) {
		uint64_t tmp;
		CHECK_DATA_ENCODE(decoder.get64(&tmp));
		CHECK_EQUAL(i, tmp);
	}
}


