#include "WProgram.h"

#include "Events.hh"
#include "GUI.h"
#include "ModalGui.hh"

class ModalGuiPage : public Page {
public:
  char line1[16];
  char line2[16];
  bool hasPressedKey;
  int pressedKey;
  int numButtons;
  
  ModalGuiPage() {
    hasPressedKey = false;
    pressedKey = 0;
    numButtons = 2;
    line1[0] = '\0';
    line2[0] = '\0';
  }

  int getModalKey(int _numButtons) {
    hasPressedKey = false;
    numButtons = _numButtons;

    Page *previousPage = GUI.page;
    
    GUI.setPage(this);
    
    while (! hasPressedKey) {
      __mainInnerLoop(false);
      GUI.updatePage();
      GUI.update();
    }
    GUI.setPage(previousPage);

    return pressedKey;
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill(line1);
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(line2);
    }
  }

  virtual bool handleGui() {
    if (BUTTON_PRESSED(Buttons.BUTTON2)) {
      hasPressedKey = true;
      pressedKey = 0;
    }
    if (BUTTON_PRESSED(Buttons.BUTTON3)) {
      hasPressedKey = true;
      pressedKey = 1;
    }
    return true;
  }
};

ModalGuiPage modalGuiPage;

int showModalGui(char *line1, char *line2, int numOptions) {
  m_strncpy(modalGuiPage.line1, line1, 16);
  m_strncpy(modalGuiPage.line2, line2, 16);
  return modalGuiPage.getModalKey(numOptions);
}

int showModalGui_p(PGM_P line1, PGM_P line2, int numOptions) {
  m_strncpy_p(modalGuiPage.line1, line1, 16);
  m_strncpy_p(modalGuiPage.line2, line2, 16);
  return modalGuiPage.getModalKey(numOptions);
}

class NameModalGuiPage : public EncoderPage {
public:
  char line1[16];
  char name[17];
  bool hasPressedKey;
  int pressedKey;
  int cursorPos;
  bool lineChanged;

  CharEncoder charEncoders[4];
  
  
  NameModalGuiPage() {
    hasPressedKey = false;
    pressedKey = 0;
    line1[0] = '\0';
    for (int i = 0; i < 4; i++) {
      encoders[i] = &charEncoders[i];
    }
  }

  char *getName(char *initName) {
    if (initName != NULL) {
      m_strncpy_fill(name, initName, 16);
    } else {
      m_strncpy_fill(name, (char *)"", 16);
    }
    
    cursorPos = 0;
    hasPressedKey = false;
    pressedKey = 0;
    lineChanged = false;

    Page *previousPage = GUI.page;
    
    GUI.setPage(this);
    moveCursor(0);
    LCD.blinkCursor(true);
    LCD.moveCursor(1, cursorPos);
    while (hasPressedKey == false) {
      __mainInnerLoop(false);
      GUI.updatePage();

      while (!EventRB.isEmpty()) {
	gui_event_t event_s;
	gui_event_t *event = &event_s;
	EventRB.getp(event);
	
	if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
	  moveCursor(cursorPos - 4);
	}
	if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
	  moveCursor(cursorPos + 4);
	}
	if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
	  hasPressedKey = true;
	  pressedKey = 0;
	}
	if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
	  hasPressedKey = true;
	  pressedKey = 1;
	}
      }

      for (int i = 0; i < 4; i++) {
	if (charEncoders[i].hasChanged()) {
	  name[cursorPos + i] = charEncoders[i].getChar();
	  lineChanged = true;
	}
      }
      GUI.update();
      LCD.moveCursor(1, cursorPos);
    }

    LCD.blinkCursor(false);
    delay(10);
    GUI.setPage(previousPage);

    if (pressedKey == 1) {
      return NULL;
    } else {
      return name;
    }
  }

  void moveCursor(int pos) {
    if (pos < 0)
      pos = 0;
    if (pos > 12)
      pos = 12;
    cursorPos = pos;
    for (int i = 0; i < 4; i++) {
      charEncoders[i].setChar(name[cursorPos + i]);
    }
    LCD.moveCursor(1, cursorPos);
  }
  
  virtual void display(bool redisplay = true) {
    if (redisplay || lineChanged) {
      GUI.setLine(GUI.LINE1);
      GUI.put_string_fill(line1);
      GUI.setLine(GUI.LINE2);
      GUI.put_string_fill(name);
      LCD.blinkCursor(true);
      LCD.moveCursor(1, cursorPos);
      lineChanged = false;
    }
  }

  virtual bool handleGui() {
    return true;
  }
};

NameModalGuiPage nameModalGuiPage;

char *getNameModalGui(char *line1, char *initName) {
  m_strncpy(nameModalGuiPage.line1, line1, 16);
  return nameModalGuiPage.getName(initName);
}
