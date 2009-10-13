#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "Monome.h"
#include "select.hh"

MidiUartOSXClass MidiUart;
MidiClass Midi, Midi2;

class MonomeHost : public MonomeParentClass {
protected:
	int fd;
		FDSelect fds;

public:
	MonomeHost(const char *file) {
		fd = open(file, O_RDWR);
		if (fd < 0)
			throw "could not open Monome file";
		fds.add(fd);
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

	bool isAvailable(unsigned long timeout = 10) {
		fds.waitRead(timeout);
		return fds.isSet(fd);
	}

	void handle() {
		char buf[128];
		int ret = read(fd, buf, sizeof(buf));
		if (ret < 0) {
			throw "error while reading from monome";
		}
		for (uint8_t i = 0; i < ret; i++) {
			handleByte(buf[i]);
		}
	}
};

class MonomeHandler : public MonomeCallback {
public:
	MonomeParentClass *monome;

	MonomeHandler(MonomeParentClass *_monome) {
		monome = _monome;
	}

	void setup() {
		monome->addCallback(this, (monome_callback_ptr_t)(&MonomeHandler::onEvent));
	}
	
	void onEvent(monome_event_t *evt) {
		monome->setLED(evt->x, evt->y, evt->state);
	}
};

int main(int argc, const char *argv[]) {
	MonomeHost monome(argv[1]);
	MonomeHandler handler(&monome);

	handler.setup();

	monome.setBuffer();
	monome.setLED(1, 1);
	monome.setLED(1, 2);
	monome.setLED(1, 3);

	for (;;) {
		if (monome.isAvailable()) {
			monome.handle();
		}
	}

	return 0;
}
