#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "Monome.h"

MidiUartOSXClass MidiUart;
MidiClass Midi, Midi2;

class MonomeHost : public MonomeParentClass {
protected:
	int fd;
public:
	MonomeHost(const char *file) {
		fd = open(file, O_RDWR);
		if (fd < 0)
			throw "could not open Monome file";
	}

	~MonomeHost() {
		if (fd >= 0)
			close(fd);
	}
	
	virtual void sendBuf(uint8_t *data, uint8_t len) {
		int ret = write(fd, data, len);
		if (ret < 0) {
			throw "error while writing";
		} else if (ret != len) {
			throw "could not write all necessary bytes";
		}
	}

	virtual void sendMessage(uint8_t byte1, uint8_t byte2) {
		uint8_t buf[2] = {byte1, byte2};
		sendBuf(buf, 2);
	}

	bool isAvailable() {
		
	}
};

int main(int argc, const char *argv[]) {
	MonomeHost monome(argv[1]);

	monome.setBuffer();
	monome.setLED(1, 1);
	monome.setLED(1, 2);
	monome.setLED(1, 3);
	
	return 0;
}
