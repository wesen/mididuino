/*
 * MidiCtrl - Sketch to demonstrate how to receive and parse Sysex message
 *
 * (c) 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include <AutoMidiController.h>
#include <GUI.h>

AutoMidiControllerSketch sketch;

#define RW_CMD_SET_ENCODER_NAME  0x01
#define RW_CMD_SET_PAGE_NAME     0x02
#define RW_CMD_SET_ENCODER_CC    0x03
#define RW_CMD_SET_ENCODER_VALUE 0x04
#define RW_CMD_FLASH_STRING      0x05

class SysexReceiveListenerClass : public MidiSysexListenerClass {
public:
  SysexReceiveListenerClass() : MidiSysexListenerClass() {
    ids[0] = MIDIDUINO_SYSEX_VENDOR_1;
    ids[1] = MIDIDUINO_SYSEX_VENDOR_2;
    ids[2] = 0x79;
  }

  RecordingEncoder<64> *getRecEncoder(uint8_t num) {
    return &sketch.autoPages[num / 4].recEncoders[num & 3];
  }

  Encoder *getRealEncoder(uint8_t num) {
    return &sketch.autoPages[num / 4].realEncoders[num & 3];
  }

  void handleByte(uint8_t byte) {
  }
  
  void start() {
  }

  void end() {
    if (MidiSysex.len > 3) {
      uint8_t *ptr = MidiSysex.data + 3;
      while ((ptr - MidiSysex.data) < MidiSysex.len) {
        switch (*(ptr++)) {
        case RW_CMD_SET_ENCODER_NAME:
          {
            uint8_t idx = *(ptr++);
            Encoder *encoder = getRealEncoder(idx);
            encoder->setName((char *)ptr);
            sketch.autoPages[idx / 4].displayNames();
            /* advance to end of string */
            while (*ptr) {
              ptr++;
            }
            ptr++;
          }
          break;
          
        case RW_CMD_SET_PAGE_NAME:
          {
            uint8_t idx = *(ptr++);
            sketch.autoPages[idx].setShortName((char *)ptr);
            while (*ptr) {
              ptr++;
            }
            ptr++;
          }
          break;
          
        case RW_CMD_SET_ENCODER_CC:
          {
            uint8_t idx = *(ptr++);
            CCEncoder *encoder = (CCEncoder *)getRealEncoder(idx);
            encoder->cc = *(ptr++);
          }
          break;
          
        case RW_CMD_SET_ENCODER_VALUE:
          {
            uint8_t idx = *(ptr++);
            RecordingEncoder<64> *encoder = getRecEncoder(idx);
            encoder->setValue(*(ptr++));
          }
          break;
          
        case RW_CMD_FLASH_STRING:
          {
            uint8_t num = *(ptr++);
            if (num == 1) {
              GUI.setLine(GUI.LINE1);
            } else {
              GUI.setLine(GUI.LINE2);
            }
            GUI.flash_string_fill((char *)ptr);
            while (*ptr) {
              ptr++;
            }
            ptr++;
          }
          break;
        }
      }
    }
  }
};

SysexReceiveListenerClass sysexListener;

void setup() {
  sketch.setup();
  MidiSysex.addSysexListener(&sysexListener);
  
  GUI.setSketch(&sketch);
}


