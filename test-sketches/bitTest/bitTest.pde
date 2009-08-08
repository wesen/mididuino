class BitTestSketch : public Sketch {
  RangeEncoder foo;
  EncoderPage page;
  uint32_t value;
  
  public:
  void setup() {
    foo.initRangeEncoder(0, 31, "BIT");
    page.setEncoders(&foo);
    value = 0;
    setPage(&page);
  }
  
  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      SET_BIT32(value, foo.getValue());
      GUI.setLine(GUI.LINE2);
      GUI.put_value16(2, (uint16_t)(value >> 16));
      GUI.put_value16(3, value & 0xFFFF);
    }
  }
};

BitTestSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
