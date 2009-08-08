#include <SDCard.h>
#include <string.h>

class DirPage : public EncoderPage {
  public:
  RangeEncoder dirEncoder;
  SDCardEntry entries[64];
  int numEntries;
  
  DirPage() {
    dirEncoder.initRangeEncoder(0, 0, "DIR");
    setEncoders(&dirEncoder);
    numEntries = 0;
  }
  
  bool handleEvent(gui_event_t *event) {
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      numEntries = SDCard.listDirectory("/", entries, countof(entries));

      if (numEntries <= 0) {
        GUI.flash_string_fill("ERROR DIR");
        numEntries = 0;
        dirEncoder.max = 0;
      } else {
        GUI.setLine(GUI.LINE1);
        GUI.flash_string_fill("READ");
        GUI.flash_put_value(2, numEntries);
        dirEncoder.max = numEntries - 1;
        GUI.setLine(GUI.LINE1);
        GUI.put_value(2, numEntries);
      }
    }
    
    if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
      testFat();
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
      testRead();
    }
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
      testWrite();
    }
  }
  
  void loop() {
    if (dirEncoder.hasChanged()) {
      GUI.setLine(GUI.LINE1);
      GUI.put_value(0, dirEncoder.getValue());
      GUI.setLine(GUI.LINE2);
      GUI.put_string_at_fill(0, entries[dirEncoder.getValue()].name);
    }
  }
  
  void display() {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_at_fill(8, "HELO");
    }
  }
};

DirPage dirPage;

void setup() {
  if (SDCard.init() != 0) {
    GUI.flash_strings_fill("SD CARD ERROR", "");
    for (;;) 
      ;
  }
  if (!SDCard.deleteFile("/foo.txt")) {
    GUI.flash_string_fill("ERROR DELETE");
    return;
  }
  GUI.setPage(&dirPage);
}

SDCardEntry entries[10];

void testFat() {
  int len;

#if 0
  SDCardEntry root;
  root.setPath("/");
  len = SDCard.listDirectory("/", entries, countof(entries));
  
  if (len < 0) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("DIRLIST ERROR");
    return;
  }
  SDCardEntry entry;
  if (!root.createSubDirectory("flortz", &entry)) {
    GUI.flash_string_fill("NO SUB 1");
    return;
  }
  if (!root.createSubDirectory("BratzelBanane", &entry)) {
    GUI.flash_string_fill("NO SUB 2");
    return;
  }

#endif  

#if 0
  if (!SDCard.deleteFile("/foo.txt")) {
    GUI.flash_string_fill("ERROR DELETE");
    return;
  }
  if (file.exists) {
    GUI.flash_string_fill("STILL EXISTS");
    return;
  }
#endif
  
  SDCardFile file("/foo.txt");
  if (!file.open(true)) {
    GUI.flash_string_fill("CREATE FILE");
    return;
  }
  len = file.write((const uint8_t *)"foobz", 5);
  if (len != 5) {
    GUI.flash_string_fill("ERROR MY WRITE");
    file.close();
    return;
  }
  file.close();
  
  #if 0
  if (file.writeFile((const uint8_t *)"blorg", 5) != 5) {
    GUI.flash_string_fill("ERROR WRITE2");
    return;
  }
  #endif
  
}

void testRead() {
  int len;
  char buf[10];
  if ((len = SDCard.readFile("/foo.txt", (uint8_t *)buf, sizeof(buf))) != 5) {
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("ERROR READING");
    GUI.setLine(GUI.LINE2);
    GUI.flash_put_value(0, len);
    return;
  } else {
    buf[len] = 0;
    GUI.setLine(GUI.LINE1);
    GUI.flash_string_fill("READ");
    GUI.setLine(GUI.LINE2);
    GUI.flash_string_fill(buf);
  }
}


void testWrite() {
  if (!SDCard.writeFile("/foo.txt", (const uint8_t *)"blorg", 5)) {
    GUI.flash_string_fill("ERROR WRITING");
    return;
  }
}

void loop() {
}
