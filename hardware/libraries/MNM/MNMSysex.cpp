#include "WProgram.h"
#include "helpers.h"
#include "MNMParams.hh"
#include "MNMSysex.hh"
#include "MNMMessages.hh"

MNMSysexListenerClass MNMSysexListener;

void MNMSysexListenerClass::start() {
  isMNMEncodedMessage = false;
  isMNMMessage = false;
  msgLen = 0;
  msgCksum = 0;
}

void MNMSysexListenerClass::handleByte(uint8_t byte) {
  if (MidiSysex.len == 3) {
    isMNMEncodedMessage = false;
    if (byte == 0x03) {
      isMNMMessage = true;
    } else {
      isMNMMessage = false;
    }
    return;
  }
  
  if (isMNMMessage) {
    if (MidiSysex.len == sizeof(monomachine_sysex_hdr)) {
      msgType = byte;
      printf("msgType: %x\n", byte);
      switch (byte) {
      case MNM_STATUS_RESPONSE_ID:
	MidiSysex.startRecord();
	break;
	
      case MNM_GLOBAL_MESSAGE_ID:
	MidiSysex.resetRecord();
	isMNMEncodedMessage = true;
	break;
	
      case MNM_KIT_MESSAGE_ID:
	MidiSysex.resetRecord();
	isMNMEncodedMessage = true;
	break;
	
      case MNM_PATTERN_MESSAGE_ID:
	MidiSysex.resetRecord();
	isMNMEncodedMessage = true;
	break;
	
      case MNM_SONG_MESSAGE_ID:
	MidiSysex.resetRecord();
	isMNMEncodedMessage = true;
	break;
      }
    }

    if (isMNMEncodedMessage) {
      if (MidiSysex.len >= sizeof(monomachine_sysex_hdr)) {
	if (MidiSysex.len == 9) {
	  encoder.init(MidiSysex.recordBuf + MidiSysex.recordLen,
		       MidiSysex.maxRecordLen - MidiSysex.recordLen);
	}
	if (MidiSysex.len < 9) {
	  if (MidiSysex.len == 8) {
	    msgCksum = byte;
	    msgLen++;
	  }
	  MidiSysex.recordByte(byte);
	} else {
	  if (sysexCirc.size() == 4) {
	    uint8_t c = sysexCirc.get(0);
	    msgCksum += c;
	    msgLen++;
	    encoder.pack(c);
	  }
	  sysexCirc.put(byte);
	}
      }
    }
  }
}

void MNMSysexListenerClass::end() {
  if (!isMNMMessage)
    return;

  if (isMNMEncodedMessage) {
    uint16_t len = encoder.finish();
    if (len > 0) {
      MidiSysex.recordLen += len;
    }
    msgCksum &= 0x3FFF;
    uint16_t realCksum = ElektronHelper::to16Bit(sysexCirc.get(0), sysexCirc.get(1));
    uint16_t realLen = ElektronHelper::to16Bit(sysexCirc.get(2), sysexCirc.get(3));
    if ((msgLen + 4) != realLen) {
#ifdef HOST_MIDIDUINO
      fprintf(stderr, "wrong message len, %d should be %d\n", msgLen, realLen);
#endif
      return;
    }
    if (msgCksum != realCksum) {
#ifdef HOST_MIDIDUINO
      fprintf(stderr, "wrong message cksum, %x should be %x\n", msgCksum, realCksum);
#endif
      return;
    }
  }
  
  switch (msgType) {
  case MNM_STATUS_RESPONSE_ID:
    for (int i = 0 ; i < statusCallbacks.size; i++) {
      if (statusCallbacks.arr[i] != NULL)
	statusCallbacks.arr[i](MidiSysex.data[1], MidiSysex.data[2]);
    }
    break;
    
    
  case MNM_GLOBAL_MESSAGE_ID:
    if (onGlobalMessageCallback != NULL)
      onGlobalMessageCallback();
    break;
    
  case MNM_KIT_MESSAGE_ID:
    if (onKitMessageCallback != NULL)
      onKitMessageCallback();
    break;
    
  case MNM_PATTERN_MESSAGE_ID:
    if (onPatternMessageCallback != NULL)
      onPatternMessageCallback();
    break;
    
  case MNM_SONG_MESSAGE_ID:
    if (onSongMessageCallback != NULL)
      onSongMessageCallback();
    break;
  }
}

void MNMSysexListenerClass::setup() {
  MidiSysex.addSysexListener(this);
}
