#include "USBMidiUart.h"
#include "usb_midi.h"
#include "d12.h"
#include "Midi.h"

USBMidiUartClass USBMidiUart;

void usb_midi_uart_putc(uint8_t c) {
  USBMidiUart.rxRb.put(c);
}

void USBMidiUartClass::putc(uint8_t c) {
  if (d12_device_is_configured()) {
    if (configured >= 100) {
      usb_midi_handle_rx_byte(c);
    }
  }
}

bool USBMidiUartClass::avail() {
  return !rxRb.isEmpty();
}

uint8_t USBMidiUartClass::getc() {
  return rxRb.get();
}

MidiClass USBMidi;
