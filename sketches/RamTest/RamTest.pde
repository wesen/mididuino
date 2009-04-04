#include <sd_raw.h>

uint8_t *ram = (uint8_t *)0x4000;

RangeEncoder encoder(0, 128, "NUM");
EncoderPage page(&encoder);

void setup() {
  SET_BIT(DDRD, PD4);
  SET_BIT(PORTD, PD4);

  
  uint8_t *ptr = ram;
  for (int i = 0; i < 1024; i++) {
    *(ptr++) = i & 0xFF;
  }
  GUI.setPage(&page);

  
}

void loop() {
  GUI.updatePage();
  
  if (encoder.hasChanged()) {
    uint8_t val = ram[encoder.getValue()];
    GUI.setLine(GUI.LINE2);
    GUI.put_value(1, val);
  }
  
  GUI.update();
// sd_raw_init();
  
  
/*  if (sd_raw_init()) {
  MidiUart.sendNoteOn(60, 60);
  }
  */

/*
      struct sd_raw_info disk_info;
    if(!sd_raw_get_info(&disk_info))
    {
        return;
    }
    */


  /*
  volatile uint8_t bla;
    uint8_t *ptr = ram;

  for (int i = 0; i < 1024; i++) {
    bla = *ptr;
    *(ptr++) = i & 0xFF;
  }
  */
  
}

void handleGui() {
}
