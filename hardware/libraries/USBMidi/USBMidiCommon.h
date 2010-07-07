#ifndef USB_MIDI_COMMON_H__
#define USB_MIDI_COMMON_H__

#include <midi-common.hh>

/**
 ** USB MIDI message structure definition
 **
 **/
typedef struct {
  unsigned int cin:4;
  unsigned int cable:4;
  uint8_t      msg[3];
} usb_midi_message_t;

typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
  uint8_t      cin;
} midi_to_usb_t;


/**
 ** USB MIDI message types
 **/
#define CIN_MISC             0x00
#define CIN_CABLE            0x01
#define CIN_SYSCOMMON_2BYTES 0x02
#define CIN_SYSCOMMON_3BYTES 0x03
#define CIN_SYSEX_START      0x04
#define CIN_SYSCOMMON_1BYTE  0x05
#define CIN_SYSEX_END_2BYTES 0x06
#define CIN_SYSEX_END_3BYTES 0x07
#define CIN_NOTEOFF          0x08
#define CIN_NOTEON           0x09
#define CIN_POLYKEYPRESS     0x0A
#define CIN_CONTROL_CHANGE   0x0B
#define CIN_PROGRAM_CHANGE   0x0C
#define CIN_CHANNEL_PRESSURE 0x0D
#define CIN_PITCHBEND_CHANGE 0x0E
#define CIN_SINGLE_BYTE      0x0F

#endif /* USB_MIDI_COMMON_H__ */
