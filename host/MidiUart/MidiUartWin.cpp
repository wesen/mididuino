//#ifdef WINDOWS

#include "WProgram.h"
#include "MidiUartWin.h"
#include "Midi.h"

void MidiUartWinClass::listInputMidiDevices() {
  MIDIINCAPS     mic;
  unsigned long   iNumDevs, i;
  
  iNumDevs = midiInGetNumDevs();

  for (i = 0; i < iNumDevs; i++) {
		if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))) {
			printf("%lu) %s\r\n", i, mic.szPname);
		}
	}  
}

void MidiUartWinClass::listOutputMidiDevices() {
  MIDIOUTCAPS     moc;
  unsigned long   iNumDevs, i;
  
  iNumDevs = midiOutGetNumDevs();

  for (i = 0; i < iNumDevs; i++) {
		if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS))){
			printf("%lu) %s\r\n", i, moc.szPname);
		}
	}  
}

MidiUartWinClass::MidiUartWinClass(int _inputDevice, int _outputDevice) {
	inputDevice = -1;
	outputDevice = -1;
	if ((_inputDevice >= 0) && (_outputDevice >= 0)) {
		init(_inputDevice, _outputDevice);
	}
}

MidiUartWinClass::~MidiUartWinClass() {
	if (midiInStop(inHandle) != MMSYSERR_NOERROR) {
		throw "Could not stop MIDI input";
	}
	if (midiInReset(inHandle) != MMSYSERR_NOERROR) {
		throw "Could reset MIDI input";
	}
	if (midiInClose(inHandle) != MMSYSERR_NOERROR) {
		throw "Could not close MIDI input";
	}

	midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));

	midiOutClose(outHandle);
}

void MidiUartWinClass::sendSysex(uint8_t *data, uint8_t cnt) {
}

void MidiUartWinClass::putc(uint8_t c) {
}

bool MidiUartWinClass::avail() {
	return false;
}

uint8_t MidiUartWinClass::getc() {
	return 0;
}


void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
													 DWORD dwParam1, DWORD dwParam2) {
	printf("dwInstance %p\n", (void *)dwInstance);

	switch (uMsg) {
	case MIM_DATA:
		break;

	case MIM_LONGDATA:
		break;

	case MIM_MOREDATA:
		printf("midi driver is throwing away received data: %x %x %x\n",
					 SHORT_MSG_STATUS(dwParam1), SHORT_MSG_BYTE1(dwParam1),
					 SHORT_MSG_BYTE2(dwParam1));
		break;

	case MIM_ERROR:
		printf("callback error\n");
		break;

	case MIM_LONGERROR:
		printf("callback long error");
		break;

	default:
		printf("received unknown event %x\n", uMsg);
		return;
	}
}

static int getInputDeviceName(int id, char *buf, int len) {
	MIDIINCAPS mic;

	if (!midiInGetDevCaps(id, &mic, sizeof(MIDIINCAPS))) {
		strncpy(buf, mic.szPname, len);
		return 0;
	} else {
		buf[0] = '\0';
		return -1;
	}
}

static int getOutputDeviceName(int id, char *buf, int len) {
	MIDIOUTCAPS moc;

	if (!midiOutGetDevCaps(id, &moc, sizeof(MIDIOUTCAPS))) {
		strncpy(buf, moc.szPname, len);
		return 0;
	} else {
		buf[0] = '\0';
		return -1;
	}
}

void MidiUartWinClass::init(int _inputDevice, int _outputDevice) {
	unsigned long result;
		
	inputDevice = _inputDevice;
	outputDevice = _outputDevice;

	char name[256];
	getOutputDeviceName(outputDevice, name, sizeof(name));
	result = midiOutOpen(&outHandle, outputDevice, 0, 0, CALLBACK_NULL);
	if (result) {
		throw "Error opening utput device";
	} else {
		printf("Opened output device %s\n", name);
	}

	getInputDeviceName(inputDevice, name, sizeof(name));
	result = midiInOpen(&inHandle, inputDevice, (DWORD)midiCallback, (DWORD)this, CALLBACK_FUNCTION);
	if (result) {
		throw "Error opening input devic";
	} else {
		printf("Opening input device %s\n", name);
	}

	midiHdr.lpData = inputBuffer;
	midiHdr.dwBufferLength = sizeof(inputBuffer);
	midiHdr.dwFlags = 0;
	result = midiInPrepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
	if (result) {
		throw "Could not prepare input buffer";
	}

	result = midiInAddBuffer(inHandle, &midiHdr, sizeof(MIDIHDR));
	if (result) {
		throw "Could not add input buffer";
	}

	result = midiInStart(inHandle);
	if (result) {
		throw "Could not start recording on input";
	}
}

void MidiUartWinClass::midiSendLong(unsigned char *buf, unsigned long len) {
	MIDIHDR midiHdr;

	midiHdr.lpData = (CHAR *)buf;
	midiHdr.dwBufferLength = len;
	midiHdr.dwFlags = 0;

	UINT err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
	if (err) {
		char errBuf[256];
		midiOutGetErrorText(err, errBuf, sizeof(errBuf));
		printf("error sending long message: %s\n", errBuf);
	}

	while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR)))
		;
}

void MidiUartWinClass::midiSendShort(unsigned char status,
																		 unsigned char byte1, unsigned char byte2) {
	midiOutShortMsg(outHandle, MAKE_SHORT_MSG(status, byte1, byte2));
}

void MidiUartWinClass::runLoop() {
}

//#endif /* WINDOWS */
