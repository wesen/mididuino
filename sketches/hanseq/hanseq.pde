// -*- C++ -*-

#include <stdarg.h>

class PadKontrol {
  void sendSysex(uint8_t count ...);
public:
  void enterNativeMode();
  void exitNativeMode();

  void startLights();
  void makeControlsSendSysex();

  enum ledState { off = 0, on = 1, oneShot = 2, blink = 3 };

  void setLED(uint8_t ledNumber,
              ledState state,
              uint8_t time = 8);

  void set7seg(uint16_t number, bool blink = false);

  void handleSysex(uint8_t* data, uint8_t length);
};

void
PadKontrol::sendSysex(uint8_t count ...) {
  static uint8_t header[5] = { 0xf0, 0x42, 0x40, 0x6e, 0x08 };
  MidiUart.sendRaw(header, 5);
  va_list ap;
  va_start(ap, count);
  for (uint8_t i = 0; i < count; i++) {
    MidiUart.sendRaw(va_arg(ap, int));
  }
  va_end(ap);
  MidiUart.sendRaw(0xf7);
}

void
PadKontrol::enterNativeMode() {
  sendSysex(3, 0x00, 0x00, 0x01);
}

void
PadKontrol::exitNativeMode() {
  sendSysex(3, 0x00, 0x00, 0x00);
}

void
PadKontrol::startLights() {
  sendSysex(12,
            0x3f, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x03, 0x20, 0x20, 0x20);
}

void
PadKontrol::makeControlsSendSysex() {
  sendSysex(44,
            0x3f, 0x2a, 0x00, 0x00, 0x05, 0x05, 0x05, 0x7f,
            0x7e, 0x7f, 0x7f, 0x03, 0x0a, 0x0a, 0x0a, 0x0a,
            0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
            0x0a, 0x0a, 0x0a, 0x0a, 0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
            0x0d, 0x0e, 0x0f, 0x10);
}

void
PadKontrol::setLED(uint8_t ledNumber,
                   ledState state,
                   uint8_t time) {
  sendSysex(3, 0x01, ledNumber, (((uint8_t) state) << 5) | time);
}

void
PadKontrol::set7seg(uint16_t number, bool blink) {
  sendSysex(6, 0x22, 0x04, 0x00 + (blink ? 1 : 0),
            number / 100 + '0',
            (number % 100) / 10 + '0',
            (number % 10) + '0');
}

void
PadKontrol::handleSysex(uint8_t* data, uint8_t length)
{
}

PadKontrol PadKontrol;

class PadKontrolSysexReceiver
  : public MidiSysexClass
{
  uint8_t _buf[128];
public:
  PadKontrolSysexReceiver()
    : MidiSysexClass(_buf, sizeof _buf)
  {}

  virtual void end();    
};

void
PadKontrolSysexReceiver::end()
{
  if (aborted || (len < 5)) {
    return;
  }
  if ((_buf[0] == 0x42)
      && ((_buf[1] & 0xf0) == 0x40)
      && (_buf[2] == 0x6e)
      && (_buf[3] == 0x08)) {
    PadKontrol.handleSysex(_buf + 4, len - 4);
  }
}


PadKontrolSysexReceiver PadKontrolSysexReceiver;

class EuclidPage {
public:
  EuclidDrumTrack _track;
  EncoderPage _page;
  RangeEncoder _pulseEncoder;
  RangeEncoder _lengthEncoder;
  RangeEncoder _offsetEncoder;
  RangeEncoder _pitchEncoder;

  uint8_t _number;
  uint8_t _pitch;
  bool _muted;
  bool _solo;

  void toggleMute();
  void mute();
  void unMute();

  void toggleSolo();
  void solo();
  void unSolo();
  
  void updateDisplay();
  void checkEncoders();
  void on16Callback();

  void setup(uint8_t index);
  EuclidPage();
};

EuclidPage::EuclidPage()
  : _track(0, 8),
    _pulseEncoder(0, 32),
    _lengthEncoder(1, 32), 
    _offsetEncoder(0, 32),
    _pitchEncoder(0, 127),
    _pitch(0),
    _muted(false),
    _solo(false)
{
  _page.encoders[0] = &_pulseEncoder;
  _page.encoders[1] = &_lengthEncoder;
  _page.encoders[2] = &_offsetEncoder;
  _page.encoders[3] = &_pitchEncoder;
  _lengthEncoder.setValue(8);
}

EuclidPage euclids[4];
EuclidPage *currentEuclid = &euclids[0];

void
EuclidPage::toggleMute()
{
  if (_muted) {
    unMute();
  } else {
    mute();
  }
  updateDisplay();
}

void
EuclidPage::unMute()
{
  _muted = false;
}

void
EuclidPage::mute()
{
  if (_solo) {
    unSolo();
  }
  _muted = true;
}

void
EuclidPage::unSolo()
{
  for (uint8_t i = 0; i < 4; i++) {
    euclids[i]._muted = false;
  }
  _solo = false;
}

void
EuclidPage::solo()
{
  for (uint8_t i = 0; i < 4; i++) {
    if (this != &euclids[i]) {
      euclids[i].mute();
    }
  }
  _solo = true;
  _muted = false;
}

void
EuclidPage::toggleSolo()
{
  if (_solo) {
    unSolo();
  } else {
    solo();
  }
  updateDisplay();
}

void
EuclidPage::checkEncoders() {
  if (_pulseEncoder.hasChanged() || 
      _lengthEncoder.hasChanged() || 
      _offsetEncoder.hasChanged()) {
    _track.setEuclid(_pulseEncoder.getValue(), 
                     _lengthEncoder.getValue(), 
                     _offsetEncoder.getValue());
  }
}

void
EuclidPage::setup(uint8_t i)
{
  _number = i + 1;
}

void switchPage(uint8_t i) {
  currentEuclid = &euclids[i];
  currentEuclid->updateDisplay();
}

void handleGui() {
  if (BUTTON_PRESSED(4)) {
    MidiClock.pause();
  }

  //  tempoEncoder.update(&GUI.Encoders.encoders[4]);
  currentEuclid->_page.update();
  
  for (uint8_t i = 0; i < 4; i++) {
    if (BUTTON_DOWN(GUI.Buttons.SHIFT) && BUTTON_PRESSED(i)) {
      switchPage(i);
    }
  }

  if (BUTTON_PRESSED(6)) {
    currentEuclid->toggleMute();
  }
  
  if (BUTTON_PRESSED(7)) {
    currentEuclid->toggleSolo();
  }
}

void
EuclidPage::on16Callback() {
  if (!_muted && _track.isHit(MidiClock.div16th_counter)) {
    PadKontrol.setLED(MidiClock.div16th_counter % 16, PadKontrol.oneShot, 2);
    if (_pitch) {
      USBMidiUart.sendNoteOff(_pitch);
    }
    _pitch = _pitchEncoder.getValue(); //  + (random() % 16);
    USBMidiUart.sendNoteOn(_pitch, 100);
  }
}

void
EuclidPage::updateDisplay() {
  static char displayBuf[17];

  GUI.setLine(GUI.LINE1);
  GUI.put_value(0, (uint8_t) _number);
  GUI.put_string(12,
                 _muted ? "MUTE" : (_solo ? "SOLO" : "    "));
  GUI.setLine(GUI.LINE2);
  currentEuclid->_page.display(true);
}

void on16Callback() {
  static uint8_t padKontrolInitialized = 0;

  if (padKontrolInitialized < 3) {
    switch (padKontrolInitialized++) {
    case 0:
      PadKontrol.enterNativeMode();
      break;
    case 1:
      PadKontrol.startLights();
      break;
    case 2:
      PadKontrol.makeControlsSendSysex();
      break;
    }
  } else {
    if ((MidiClock.div16th_counter % 4) == 0) {
      if ((MidiClock.div16th_counter % 16) == 0) {
        static uint16_t bar = 0;
        PadKontrol.setLED(0x10, PadKontrol.oneShot, 4);
        PadKontrol.set7seg(bar++);
      } else {
        PadKontrol.setLED(0x10, PadKontrol.oneShot, 2);
      }
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    euclids[i].on16Callback();
  }
}

/* check encoder and real interrupt load */
void setup() {
  for (uint8_t i = 0; i < 4; i++) {
    euclids[i].setup(i);
  }

  MidiClock.mode = MidiClock.INTERNAL;
  // MidiClock.transmit = true;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.setTempo(150);
  MidiClock.start();

  Midi.setSysex(&PadKontrolSysexReceiver);

  switchPage(0);
}

void loop() {
  cli();
  currentEuclid->checkEncoders();

  GUI.setLine(GUI.LINE2);
  currentEuclid->_page.display();
  currentEuclid->_page.handle();
  sei();
}
