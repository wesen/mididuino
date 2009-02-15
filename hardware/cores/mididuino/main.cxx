#include "WProgram.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>

#ifdef MIDIDUINO_ENABLE_USB
#include "d12.h"
#include "usb_midi.h"
#include "midi_link_desc.h"
#endif

void setup();
void loop();
void handleGui();

MidiClass Midi;

#define CHECK_RESET() (BUTTON_DOWN(BUTTON_1) && BUTTON_DOWN(ENCODER_1) && BUTTON_DOWN(ENCODER_2))

#ifdef MIDIDUINO_HANDLE_SYSEX
uint8_t mididuino_sysex_data[128];
MididuinoSysexClass mididuinoSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));

#ifdef MIDIDUINO_ENABLE_USB
MididuinoSysexClass USBmididuinoSysex(NULL, 0);
#endif

#endif

ISR(TIMER1_OVF_vect) {
  setLed();
  clock++;

#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED)
    MidiClock.handleTimerInt();
#endif
  
  clearLed();
}

void gui_poll() {
  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.poll(sr);
}

ISR(TIMER2_OVF_vect) {
#ifdef MIDIDUINO_ENABLE_LFOS
  uint8_t i;
  for (i = 0; i < 4; i++) {
    LFO[i].acc += LFO[i].inc;
  }
#endif /* MIDIDUINO_ENABLE_LFOS */
  
#if defined(MIDIDUINO_POLL_GUI) && defined(MIDIDUINO_POLL_GUI_IRQ)
  gui_poll();

#ifdef MIDIDUINO_RESET_COMBO
  if (CHECK_RESET()) {
    start_bootloader();
  }
#endif

  handleGui();
#endif
  slowclock++;
}

#ifdef MIDIDUINO_ENABLE_USB
/** Callback for outgoing MIDI messages. Send midi buffer if not empty. **/
void midi_ep_in_callback(uint8_t ep) {
  uint8_t buf[1];
  d12_read_cmd(D12_CMD_READ_LAST_TX_STATUS + ep, buf, 1);

  if (!d12_device_is_configured())
    return;

  usb_midi_send_buf();
}

/** Buffer for receiving midi messages. **/
static uint8_t recv_bytes = 0;
static uint8_t recv_buf[64];

void handle_midi_ep_out(void) {
  /** Read data from D12. **/
  recv_bytes = d12_read_ep_pkt(D12_MIDI_EP_OUT, recv_buf, sizeof(recv_buf));
  if (!(recv_bytes % 4)) {
    uint8_t count =0;
    uint8_t i;
    /** Dispatch to MIDI stack (same as on AT90USB1286 **/
    for (i = 0; i < recv_bytes; i += 4, count++) {
      
      if (!usb_midi_handle_out_msg(0, (usb_midi_message_t *)(recv_buf + i))) {
      	break;
      }
    }
  }
  
}

/** Process MIDI data from the host, called when data is received on
 ** the OUT endpoint.
 **/
void midi_ep_out_callback(uint8_t  ep) {
  uint8_t buf[1];
  d12_read_cmd(D12_CMD_READ_LAST_TX_STATUS + ep, buf, 1);
  if (!d12_device_is_configured())
    return;

  handle_midi_ep_out();
}
#endif

int main(void) {
  init();

#ifdef MIDIDUINO_ENABLE_USB
  /** Initialize D12 pins and stack. **/
  d12_pins_init();
  d12_init();
  usb_midi_init();
#endif  
  
#ifdef MIDIDUINO_HANDLE_SYSEX
  //  Midi.setSysex(&mididuinoSysex);

#ifdef MIDIDUINO_ENABLE_USB
  USBMidi.setSysex(&USBmididuinoSysex);
#endif
#endif

  setup();
  sei();

  for (;;) {
    if (MidiUart.avail()) {
      Midi.handleByte(MidiUart.getc());
    }

#ifdef MIDIDUINO_ENABLE_USB
    if (USBMidiUart.avail()) {
      uint8_t byte = USBMidiUart.getc();
      USBMidi.handleByte(byte);
    }
    
    if (d12_device_is_configured()) {
      if (USBMidiUart.configured < 100) {
	USBMidiUart.configured++;
	delay(10);
      } else {
	/* try to send outgoing data as soon as possible */
	usb_midi_send_buf();
      }
    } else {
      USBMidiUart.configured = 0;
    }
    
    /** Handle usb status. **/
    d12_main();

    /* check ep2 again because of d12 bug */
    uint8_t status;
    do {
      d12_read_cmd(D12_CMD_SELECT_EP + D12_MIDI_EP_OUT, &status, 1);
      if ((status & 1) && d12_device_is_configured()) {
	handle_midi_ep_out();
      }
    } while (status & 1);

#endif

#if defined(MIDIDUINO_POLL_GUI) && !defined(MIDIDUINO_POLL_GUI_IRQ)
    gui_poll();

#ifdef MIDIDUINO_RESET_COMBO
  if (CHECK_RESET()) {
    start_bootloader();
  }
#endif

    handleGui();
#endif

    loop();

  }
  return 0;
}
