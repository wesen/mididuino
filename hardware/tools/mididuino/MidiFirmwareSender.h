/*
 * MidiFirmwareSender.h
 *
 *  Created on: Jun 14, 2012
 *      Author: manuel
 */

#ifndef MIDIFIRMWARESENDER_H_
#define MIDIFIRMWARESENDER_H_

#include "MidiSend.h"
#include "HexFile.h"
#include "midi-common.hh"
#include "Midi.h"
#include <string.h>

class MidiFirmwareSender : public MidiSysexListenerClass {
protected:
  bool canSendSysex;
  bool firmwareChecksumSent;

  bool waitingForBootloader;

  int deviceId;
  int verbose;
  bool statusMessage;
  HexFile *file;

  bool transferIsComplete;

public:
  MidiFirmwareSender(const int deviceId, bool statusMessage, int verbose = 1) {
    this->deviceId = deviceId;
    this->verbose = verbose;
    this->statusMessage = statusMessage;
    ids[0] = MIDIDUINO_SYSEX_VENDOR_1;
    ids[1] = MIDIDUINO_SYSEX_VENDOR_2;
    ids[2] = deviceId;
    this->waitingForBootloader = false;
    this->canSendSysex = true;
    this->file = NULL;
  }

  virtual ~MidiFirmwareSender() {
  }

  bool upload(HexFile *file);
  void sendSysexBootloadCommand();
  void sendSysexRebootCommand();
  bool sendNextSysexPart();

  bool isTransferComplete() { return transferIsComplete; }

  virtual void handleByte(uint8_t byte);
};

#endif /* MIDIFIRMWARESENDER_H_ */
