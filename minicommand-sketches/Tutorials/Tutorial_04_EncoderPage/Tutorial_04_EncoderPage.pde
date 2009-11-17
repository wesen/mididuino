RangeEncoder param1(0, 127, "P1");
RangeEncoder param2(0, 127, "P2");
EncoderPage page(&param1, &param2);

void setup() {
  GUI.setPage(&page);
}

