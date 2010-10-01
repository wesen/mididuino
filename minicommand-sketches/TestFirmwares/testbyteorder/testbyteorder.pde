uint16_t foo PROGMEM = 0xFFEE;
uint16_t foo2 PROGMEM = 0xDDCC;

void setup() {
  uint8_t i = pgm_read_byte((uint8_t *)&foo);
  uint8_t i2 = pgm_read_byte((uint8_t *)&foo + 1);
  GUI.setLine(GUI.LINE1);
  GUI.printf("%x %x", i, i2);
  GUI.setLine(GUI.LINE2);
  i = pgm_read_byte((uint8_t *)&foo2);
  i2 = pgm_read_byte((uint8_t *)&foo2 + 1);
  GUI.printf("%x %x", i, i2);
}
