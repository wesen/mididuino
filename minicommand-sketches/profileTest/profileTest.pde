extern volatile uint16_t profileIpBuf[256];
void enableProfiling();
void disableProfiling();

extern volatile uint16_t ipBuf[256];
void showIrqCaller() {
  cli();
  GUI.setLine(GUI.LINE1);
  for (int i = 0; i < 4; i++) {
    GUI.put_value16(i, profileIpBuf[i]);
  }
  GUI.setLine(GUI.LINE2);
  for (int i = 0; i < 4; i++) {
    GUI.put_value16(i, profileIpBuf[i + 4]);
  }
  sei();
}

void sendProfileStuff() {
  disableProfiling();

  uint16_t ipBuf[256];
  m_memcpy(ipBuf, (void *)profileIpBuf, sizeof(ipBuf));
  m_memclr((void *)profileIpBuf, sizeof(profileIpBuf));
  
  MidiUart.putc(0xF0);
  MidiUart.putc(0x01);
  for (int i = 0; i < 256; i++) {
    MidiUart.putc((ipBuf[i] >> 14) & 0x7F);
    MidiUart.putc((ipBuf[i] >> 7) & 0x7F);
    MidiUart.putc((ipBuf[i] >> 0) & 0x7F);
  }
  MidiUart.putc(0xF7);
  enableProfiling();
}

void foo2() {
  GUI.put_value16(0, (uint16_t)__builtin_return_address(0));
  GUI.put_value16(1, (uint16_t)__builtin_return_address(0));
}

void foo3() {
  foo2();
}

void foo4() {
  foo3();
}

bool handleEvent(gui_event_t *event) {
  if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
    showIrqCaller();
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
    foo2();
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    foo3();
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
    foo4();
  }
  return true;
}

Task fooTask(100, sendProfileStuff);

void setup() {
  enableProfiling();
  GUI.addEventHandler(handleEvent);
  GUI.addTask(&fooTask);
}

void loop() {
}
