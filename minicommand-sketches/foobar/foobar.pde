int startAddress = 0xF000;

uint16_t checksum = 0x00;

void setup() {
  for (int i = 0; i < 0x1000; i++) {
    checksum ^= pgm_read_byte(startAddress + i);
  }
  
  LCD.putnumber16(checksum);
}

void loop() {
}

void handleGui() {
}
