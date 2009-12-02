RangeEncoder enc1(0, 127, "A1");
RangeEncoder enc2(0, 127, "A2");
RangeEncoder enc3(0, 127, "A3");
RangeEncoder enc4(0, 127, "A4");

EncoderPage page(&enc1, &enc2, &enc3, &enc4);
int j = 0;
int i = 0;

void loop() {
  
//  LCD.initLCD();
    GUI.setLine(GUI.LINE1);
    GUI.put_value(0, i);
    GUI.put_value(1, j);
    LCD.putnumber(j);
    i++;
    if (i > 200) {
      i = 0;
  j++;  
    }
    MidiUart.sendCC(0, i % 128);
  delay(5);
}

void setup() {
  GUI.setPage(&page);
}
