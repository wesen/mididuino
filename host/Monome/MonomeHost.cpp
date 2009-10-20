#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "MonomeHost.h"

MonomeHost::MonomeHost(const char *file) {
	fd = open(file, O_RDWR | O_NONBLOCK | O_NOCTTY);
	if (fd < 0)
		throw "could not open Monome file";

	struct termios options;
	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	options.c_cflag |= (CLOCAL | CREAD);
	tcsetattr(fd, TCSANOW, &options);

	fds.add(fd);
	printf("fd: %d\n", fd);
}

MonomeHost::~MonomeHost() {
	if (fd >= 0)
		close(fd);
}
	
void MonomeHost::sendBuf(uint8_t *data, uint8_t len) {
	int ret = write(fd, data, len);
	if (ret < 0) {
		throw "error while writing";
	} else if (ret != len) {
		throw "could not write all necessary bytes";
	}
}

void MonomeHost::sendMessage(uint8_t byte1, uint8_t byte2) {
	uint8_t buf[2] = {byte1, byte2};
	sendBuf(buf, 2);
}

bool MonomeHost::isAvailable(unsigned long timeout) {
	fds.waitRead(timeout);
	return fds.isSet(fd);
}

void MonomeHost::handle() {
	char buf[128];
	int ret = read(fd, buf, sizeof(buf));
	if (ret < 0) {
		throw "error while reading from monome";
	}
	for (uint8_t i = 0; i < ret; i++) {
		handleByte(buf[i]);
	}
}

