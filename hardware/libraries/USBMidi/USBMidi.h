/**# USB midi specific data structures and definitions.
 **
 ** Includes descriptors for a simple MIDI link USB device (2 jacks
 ** and end connectors). The midi receiving state machine is
 ** implemented in `usb_midi_handle_rx_byte' which should be called
 ** each time a character is received on the midi interface. MIDI
 ** messages from USB are handled by the endpoint callback.
 **/

#ifndef USB_MIDI_CPP_H__
#define USB_MIDI_CPP_H__

#include <USBMidiCommon.h>

class MidiUartClass;

#define MIDI_BUF_SIZE 32

class USBMidi {
  midi_state_t in_state;
  uint8_t last_status;
  uint8_t running_status;
  uint8_t in_msg_len;
  usb_midi_message_t in_msg;

  uint8_t midi_buf[MIDI_BUF_SIZE];
  uint8_t midi_buf_len;

  MidiUartClass *uart;
  uint8_t inEP, outEP;

  /**
   ** Conversion table from "real" midi to USB MIDI.
   **
   ** First parameter is the "real" midi message type, second is the
   ** state-machine state used to parse the incoming "real" midi message.
   **/
  static const midi_to_usb_t midi_to_usb[];

 public:
 USBMidi(MidiUartClass *_uart, uint8_t _inEP, uint8_t _outEP) :
  uart(_uart), inEP(_inEP), outEP(_outEP) {
    init();
  }
  
  void init();
  void handleByte(uint8_t byte);
  void sendMessage(usb_midi_message_t *msg);
  uint8_t handleUsbMessage(usb_midi_message_t *out_msg);

  void poll();
  void sendBuf();
};

#endif /* USB_MIDI_CPP_H__ */
