#include <MNM.h>

size_t readFile(const char *name, uint8_t *buf, uint32_t len) {
	FILE *f = fopen(name, "r");
	if (f == NULL) {
		return 0;
	}
	size_t ret = fread(buf, 1, len, f);
	fclose(f);
	return ret;
}


int main(int argc, char *argv[]) {
	MNMPattern pattern;
	if (argc > 1) {
		uint8_t buf[8192];
		size_t size = readFile(argv[1], buf, sizeof(buf));
		hexdump(buf, size);
		MNMPattern pattern;
		pattern.init();
		if (pattern.fromSysex(buf, size)) {
			pattern.print();
		} else {
			printf("pattern error\n");
			MNMSysexDecoder decoder(buf + 10);
			uint8_t buf2[8192];
			m_memset(buf2, sizeof(buf2), 0x00);
			decoder.get(buf2, 0x1978);
			hexdump(buf2, 0x1978);
		}
	}
}
	
