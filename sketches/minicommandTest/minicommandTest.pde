#include <GUI.h>

EncoderPage page;
Encoder encoders[4];

void onUartNoteOnCallback(uint8_t *msg) {
  LCD.line2("uart1");
}

void onUart2NoteOnCallback(uint8_t *msg) {
  LCD.line2("uart2");
}

void setup() {
  INIT_PAGE(page, encoders, 4);
  Midi.setOnNoteOnCallback(onUartNoteOnCallback);
  Midi2.setOnNoteOnCallback(onUart2NoteOnCallback);
}

uint16_t cnt = 0;
void loop() {
  cli();
  page.display();
  page.handle();
  GUI.update();
  sei();

  delay(10);
}

void handleGui() {
  page.update();
  for (int i = 0; i < 8; i++) {
    if (BUTTON_PRESSED(i)) {
      LCD.line2();
      LCD.putnumber(i);
    }
  }
  if (BUTTON_PRESSED(Buttons.BUTTON1)) {
    setLed();
  } else if (BUTTON_RELEASED(Buttons.BUTTON1)) {
    clearLed();
  }
  if (BUTTON_PRESSED(Buttons.BUTTON2)) {
    setLed2();
  } else if (BUTTON_RELEASED(Buttons.BUTTON2)) {
    clearLed2();
  }
  
  if (BUTTON_PRESSED(Buttons.BUTTON3)) {
    MidiUart.sendNoteOn(0, 100, 100);
  }
}
