#include <MD.h>
#include <MidiClockPage.h>

#include "WProgram.h"
void setup();
void loop();
class PatternRandomizationPage : public EncoderPage, ClockCallback, MDCallback {
  public:
  RangeEncoder lengthEncoder;
  
  uint8_t randomPatterns[16];
  uint8_t len;
  uint8_t idx;
  
  PatternRandomizationPage() : lengthEncoder(1, 16, "LEN") {
    setEncoders(&lengthEncoder);
    lengthEncoder.pressmode = true;
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      randomPatterns[i] = 0;
    }
    len = 1;
    idx = 0;
  }
  
  void setRandomLength(uint8_t _len) {
    len = _len;
  }
  
  void setup() {
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&PatternRandomizationPage::on16Callback);
    MDTask.addOnPatternChangeCallback(this, (md_callback_ptr_t)&PatternRandomizationPage::onPatternChange);
  }
  
  virtual void loop() {
    if (lengthEncoder.hasChanged()) {
      setRandomLength(lengthEncoder.getValue());
    }
  }

  void advancePattern() {
    #if 0
    GUI.setLine(GUI.LINE1);
    GUI.flash_put_value(1, idx);
    GUI.flash_put_value(2, randomPatterns[idx]);
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(1, len);
    GUI.flash_put_value(2, MD.currentPattern);
    #endif
    
    if (MD.currentPattern == randomPatterns[idx]) {
      idx++;
      if (idx >= len) {
        idx = 0;
      }
      MD.loadPattern(randomPatterns[idx]);
    } else {
      MD.loadPattern(randomPatterns[idx]);
    }
  }

  void onPatternChange() {
  }
  
  void on16Callback(uint32_t counter) {
    if (counter == 0) {
      randomizePatterns();
    }
    if ((counter % 16) == 0) {
      MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_PATTERN_REQUEST);
    }
    if ((counter % 16) == 8) {
      advancePattern();
    }
  }
  
  void flashPatternNames() {
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      char name[5];
      MD.getPatternName(randomPatterns[i], name);
      if (i < 4) {
        GUI.setLine(GUI.LINE1);
        GUI.flash_string_at(i << 2, name);
      } else if (i < 8) {
        GUI.setLine(GUI.LINE2);
        GUI.flash_string_at((i - 4) << 2, name);
      }
    }
  }
  
  void randomizePatterns() {
    for (uint8_t i = 0; i < countof(randomPatterns); i++) {
      randomPatterns[i] = random(127);
    }
    
    flashPatternNames();
    
    idx = 0;
    MD.loadPattern(randomPatterns[0]);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.ENCODER1)) {
      randomizePatterns();
      return true;
    }
    return false;
  }
};

class LeoSketch : public Sketch {
  public:
  PatternRandomizationPage randomPage;

  void setup() {
    randomPage.setup();
    setPage(&randomPage);
  }
};

LeoSketch sketch;

void setup() {
  initMDTask();
  MDTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);
  
  initClockPage();
}
  
  void loop() {
  }

#include <Midi.h>

#include "WProgram.h"
extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
}

void timer_init(void) {
  TCCR0 = _BV(CS01);
  //  TIMSK |= _BV(TOIE0);
  
  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B |= _BV(CS10) | _BV(WGM12); // every cycle
#ifdef MIDIDUINO_MIDI_CLOCK
  TIMSK |= _BV(TOIE1);
#endif

  TCCR2 = _BV(WGM20) | _BV(WGM21) | _BV(CS20) | _BV(CS21); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK |= _BV(TOIE2);
}

void init(void) {
  /** Disable watchdog. **/
  wdt_disable();
  //  wdt_enable(WDTO_15MS);

  /* move interrupts to bootloader section */
  DDRC = 0xFF;
  PORTC = 0x00;
  MCUCR = _BV(IVCE);
  MCUCR = _BV(SRE);

  // activate lever converter
  SET_BIT(DDRD, PD4);
  SET_BIT(PORTD, PD4);

  // activate background pwm
  TCCR3B = _BV(WGM32) | _BV(CS30);
  TCCR3A = _BV(WGM30) | _BV(COM3A1);
  OCR3A = 160;

  DDRE |= _BV(PE4) | _BV(PE5);
  //  DDRB |= _BV(PB0);
  //  DDRC |= _BV(PC3);

  timer_init();
}


void (*jump_to_boot)(void) = (void(*)(void))0xF000;

void start_bootloader(void) {
  cli();
  eeprom_write_word(START_MAIN_APP_ADDR, 0);
  jump_to_boot();
}

void setup();
void loop();
void handleGui();

#define PHASE_FACTOR 16
static inline uint32_t phase_mult(uint32_t val) {
  return (val * PHASE_FACTOR) >> 8;
}

ISR(TIMER1_OVF_vect) {

  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED) {
    MidiClock.handleTimerInt();
  }
#endif

  //  clearLed2();
}

// XXX CMP to have better time

static uint16_t oldsr = 0;

void gui_poll() {
  static bool inGui = false;
  if (inGui) { 
    return;
  } else {
    inGui = true;
  }
  sei(); // reentrant interrupt

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

ISR(TIMER2_OVF_vect) {
  slowclock++;
  if (abs(slowclock - lastRunningStatusReset) > 3000) {
    MidiUart.resetRunningStatus();
    lastRunningStatusReset = slowclock;
  }
  
  //  SET_BIT(OUTPUTPORT, OUTPUTPIN);

#ifdef MIDIDUINO_POLL_GUI_IRQ
  gui_poll();
#endif
  //  CLEAR_BIT(OUTPUTPORT, OUTPUTPIN);
}

uint8_t sysexBuf[8192];
MidiClass Midi(&MidiUart, sysexBuf, sizeof(sysexBuf));
uint8_t sysexBuf2[512];
MidiClass Midi2(&MidiUart2, sysexBuf2, sizeof(sysexBuf2));

void handleIncomingMidi() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
  
  while (MidiUart2.avail()) {
    Midi2.handleByte(MidiUart2.getc());
  }
}

void __mainInnerLoop(bool callLoop) {
  //  SET_BIT(OUTPUTPORT, OUTPUTPIN);
  //  setLed2();
  if ((MidiClock.mode == MidiClock.EXTERNAL ||
       MidiClock.mode == MidiClock.EXTERNAL_UART2)) {
    MidiClock.updateClockInterval();
  }

  //  CLEAR_BIT(OUTPUTPORT, OUTPUTPIN);
  handleIncomingMidi();
  
  if (callLoop) {
    GUI.loop();
  }
}

int main(void) {
  delay(100);
  init();
  clearLed();
  clearLed2();

  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.poll(sr);
  oldsr = sr;

  MidiSysex.addSysexListener(&MididuinoSysexListener);

  OUTPUTDDR |= _BV(OUTPUTPIN);
  setup();
  sei();

  for (;;) {
    __mainInnerLoop();
  }
  return 0;
}

void handleGui() {
  pollEventGUI();
}

