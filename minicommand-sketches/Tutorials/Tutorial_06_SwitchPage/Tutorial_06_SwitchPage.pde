class MySketch : 
public Sketch {
public:
  RangeEncoder param1, param2, param3, param4;

  EncoderPage page, page2;
  SwitchPage switchPage;

  MySketch() : 
  param1(0, 127, "P1"),
  param2(0, 127, "P2"),
  param3(0, 127, "P3"),
  param4(0, 127, "P4"),
  page(&param1, &param2),
  page2(&param3, &param4, &param2) {
    page.setShortName("PG1");
    page2.setShortName("PG2");
    switchPage.initPages(&page, &page2);
  }
  
  void show() {
    if (currentPage() == NULL)
      setPage(&page);
  }

  bool handleEvent(gui_event_t *evt) {
    if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
      pushPage(&switchPage);
      return true;
    } 
    else if (EVENT_RELEASED(evt, Buttons.BUTTON1)) {
      popPage(&switchPage);
      return true;
    }

    return false;
  }

};

MySketch mySketch;

void setup() {
  mySketch.setup();
  GUI.setSketch(&mySketch);
}



