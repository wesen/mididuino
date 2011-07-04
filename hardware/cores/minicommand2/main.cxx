/*
 * MidiCtrl - Main routines for minicommand2 (startup + main loop)
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include <Midi.h>

#include "Platform.h"
extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
}

/**
 * Initialize the AVR timers. Timer 1 is used for the midi
 * clock. Timer 2 is called 1000 times per second and used for the GUI
 * polling.
 **/
void timer_init(void) {
  TCCR0 = _BV(CS01);
  //  TIMSK |= _BV(TOIE0);

  /* fast timer for the MIDI clock, called 7812 times per second
   * (every 128 uS)
   */
  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B |= _BV(CS10) | _BV(WGM12); // every cycle
#ifdef MIDIDUINO_MIDI_CLOCK
  TIMSK |= _BV(TOIE1);
#endif

  /* slow timer, called 977 times per second (every 1.024 ms). */
  TCCR2 = _BV(WGM20) | _BV(WGM21) | _BV(CS20) | _BV(CS21); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK |= _BV(TOIE2);
}

/**
 * Initialize the minicommand.
 **/
void init(void) {
  /** Disable watchdog. **/
  wdt_disable();

  /* move interrupts to bootloader section */
  DDRC = 0xFF;
  PORTC = 0x00;
  MCUCR = _BV(IVCE);
  MCUCR = _BV(SRE);

  /* activate level converter for SD card */
  SET_BIT(DDRD, PD4);
  SET_BIT(PORTD, PD4);

  /* activate background LED pwm using timer 3 */
  TCCR3B = _BV(WGM32) | _BV(CS30);
  TCCR3A = _BV(WGM30) | _BV(COM3A1);
  OCR3A = 160;

  /* active the pins for the LEDs */
  DDRE |= _BV(PE4) | _BV(PE5);
  //  DDRB |= _BV(PB0);
  //  DDRC |= _BV(PC3);

  /* initialize the timer. */
  timer_init();
}

/** Helper function to go to the bootloader. **/
void (*jump_to_boot)(void) = (void(*)(void))0xFF11;

/**
 * Start the bootloader by letting the watchdog run out. Safer than
 * jumping directly into the bootloader.
 **/
void start_bootloader(void) {
  cli();
  eeprom_write_word(START_MAIN_APP_ADDR, 0);
  wdt_enable(WDTO_30MS); 
  while(1) {};
}

void setup();
void loop();
void handleGui();

/**
 * Fast timer, just update the clock counter, and call the MidiClock
 * timer if the MidiClock is enabled.
 **/
ISR(TIMER1_OVF_vect) {
  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED) {
    MidiClock.handleTimerInt();
  }
#endif
}

static uint16_t oldsr = 0;

/**
 * Poll the GUI by reading the SR165, and passing it over to the
 * encoders and the buttons classes.
 **/
void gui_poll() {
  /* avoid reentering the interrupt. */
  static bool inGui = false;
  if (inGui) { 
    return;
  } else {
    inGui = true;
  }
  sei();

  uint16_t sr = SR165.read16();
  if (sr != oldsr) {
    Buttons.clear();
    Buttons.poll(sr >> 8);
    Encoders.poll(sr);
    oldsr = sr;
    pollEventGUI();
  }
  inGui = false;
}

uint16_t lastRunningStatusReset = 0;

#define OUTPUTPORT PORTD
#define OUTPUTDDR  DDRD
#define OUTPUTPIN PD0

/**
 * Slow timer, update the slow clock, handle running status and active
 * sense, and poll the GUI.
 **/
ISR(TIMER2_OVF_vect) {
  slowclock++;

  /* check if we have to reset the last running status (after 3 seconds) */
  if (abs(slowclock - lastRunningStatusReset) > 3000) {
    MidiUart.resetRunningStatus();
    lastRunningStatusReset = slowclock;
  }

  MidiUart.tickActiveSense();
  MidiUart2.tickActiveSense();
  
#ifdef MIDIDUINO_POLL_GUI_IRQ
  gui_poll();
#endif
}

/**
 * Defines the two MIDI receiving state machines and their sysex buffers.
 **/
uint8_t sysexBuf[8192];
MidiClass Midi(&MidiUart, sysexBuf, sizeof(sysexBuf));
uint8_t sysexBuf2[512];
MidiClass Midi2(&MidiUart2, sysexBuf2, sizeof(sysexBuf2));

/**
 * Check if a MIDI byte has been received and pass it to the MIDI state machines.
 **/
void handleIncomingMidi() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
  
  while (MidiUart2.avail()) {
    Midi2.handleByte(MidiUart2.getc());
  }
}

/**
 * Separate the main inner loop so that it can be called out of modal GUIs.
 **/
void __mainInnerLoop(bool callLoop) {
  if ((MidiClock.mode == MidiClock.EXTERNAL ||
       MidiClock.mode == MidiClock.EXTERNAL_UART2)) {
    MidiClock.updateClockInterval();
  }

  handleIncomingMidi();
  
  if (callLoop) {
    GUI.loop();
  }
}

void setupEventHandlers();
void setupMidiCallbacks();
void setupClockCallbacks();

/**
 * Main routine for the minicommand2.
 **/
int main(void) {
  /** Wait a bit (can never hurt). **/
  delay(100);

  /** Initialize the minicommand2 hardware. **/
  init();
  clearLed();
  clearLed2();

  /** Read in the first shift register value (to discard the initial status). **/
  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.poll(sr);
  oldsr = sr;

  MidiSysex.addSysexListener(&MididuinoSysexListener);

  OUTPUTDDR |= _BV(OUTPUTPIN);
  setup();
  setupEventHandlers();
  setupMidiCallbacks();
  setupClockCallbacks();
  sei();

  for (;;) {
    __mainInnerLoop();
  }
  return 0;
}

/**
 * Legacy method to poll the GUI by hand.
 **/
void handleGui() {
  pollEventGUI();
}

