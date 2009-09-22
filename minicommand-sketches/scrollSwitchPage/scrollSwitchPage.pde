class TitlePage : public Page {
  public:
  TitlePage(const char *name) : Page() {
    setName(name);
  }
  
  void display() {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill(name);
    }
  }
};

class MySketch : public Sketch {
public:
  ScrollSwitchPage switchPage;
  TitlePage page1, page2, page3;
  
  MySketch() : page1("BLABLABLABA PAGE"), page2("FOO"), page3("HALLLOOOO") {
  }
  
  void setup() {
    switchPage.addPage(&page1);
    switchPage.addPage(&page2);
    switchPage.addPage(&page3);
  }
  
  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
      return true;
    }
    
    return false;
  }
  
};

MySketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
