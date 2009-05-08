#include <MidiUart.h>
#include <Midi.h>
#include <MidiClock.h>
#include <MD.h>
MidiClass Midi;

byte mididuino_sysex_data[128];

int dataPin = 2;
int oePin = 3;
int latchPin = 4;
int clockPin = 5;

int funcPin = 12;

int currentInstrument = 0;

// pins 8 - 11 knoepfe
int buttonPins[4] = { 
  8, 9, 10, 11 };
volatile byte matrix[4][4] = { 
  0 };
volatile byte redMatrix[4][4] = { 
  0 };
volatile byte blueMatrix[4][4] = { 
  0 };
volatile byte buttons[4][4] = { 
  0 };
volatile byte oldButtons[4][4] = { 
  0 };

volatile byte patterns[3][16] = { 
  0 };

void setup() {
  MidiUart.init();

  MidiClock.mode = MidiClock.INTERNAL_MIDI;
  MidiClock.setTempo(120);
  MidiClock.transmit = true;
  MidiClock.setOn16Callback(on16Callback);
  MidiClock.start();

  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    digitalWrite(buttonPins[i], HIGH); // pullup widerstand aktivieren
  }

  pinMode(funcPin, INPUT);
  digitalWrite(funcPin, HIGH);  
  digitalWrite(oePin, LOW);

  clearMatrix();
  sendMatrix();
  copyButtons();
}

void copyButtons() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      oldButtons[x][y] = buttons[x][y];
    }
  }
}

void checkButtons() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (buttons[x][y] != oldButtons[x][y]) {
        if (buttons[x][y] == LOW) {
          buttonGedrueckt(x, y);
        } 
        else {
          buttonLosgelassen(x, y);
        }
        oldButtons[x][y] = buttons[x][y];
      }
    }
  }  
}

void setPixel(int x, int y) {
  matrix[x][y] = 1;
}

void clearPixel(int x, int y) {
  matrix[x][y] = 0;
}

void setRedPixel(int x, int y) {
  redMatrix[x][y] = 1;
}

void clearRedPixel(int x, int y) {
  redMatrix[x][y] = 0;
}

void setBluePixel(int x, int y) {
  blueMatrix[x][y] = 1;
}

void clearBluePixel(int x, int y) {
  blueMatrix[x][y] = 0;
}

void clearMatrix() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      matrix[x][y] = 0;
    }
  }
}

void clearRedMatrix() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      redMatrix[x][y] = 0;
    }
  }
}

void clearBlueMatrix() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      blueMatrix[x][y] = 0;
    }
  }
}

void sendMatrix() {
  for (int y = 0; y < 4; y++) {
    byte rowNibble = ~(1 << y) & 0xf;
    short dataShort = (rowNibble | (matrix[y][0] << 4) | (matrix[y][1] << 5) | (matrix[y][2] << 6) | (matrix[y][3] << 7)) << 8 |
      ((blueMatrix[y][0] << 0) | (blueMatrix[y][1] << 1) | (blueMatrix[y][2] << 2) | (blueMatrix[y][3] << 3)) << 4 |
      ((redMatrix[y][0] << 0) | (redMatrix[y][1] << 1) | (redMatrix[y][2] << 2) | (redMatrix[y][3] << 3)) << 0;
    sendShortData(dataShort);
    for (int i = 0; i < 4; i++) {
      buttons[y][i] = digitalRead(buttonPins[i]);
    }
  }
  sendShortData(0xFFFF);
}


void setZeile(byte zeile, byte led1, byte led2, byte led3, byte led4) {
  byte rowNibble = ~(1 << zeile) & 0xf;
  byte dataByte = rowNibble | (led1 << 4) | (led2 << 5) | (led3 << 6) | (led4 << 7);
  sendData(dataByte);

}

void sendData(byte data) {
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, data);   
  //return the latch pin high to signal chip that it 
  //no longer needs to listen for information
  digitalWrite(latchPin, 1);
}

void sendShortData(unsigned short data) {
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, (data));   
  shiftOut(dataPin, clockPin, MSBFIRST, (data >> 8) & 0xFF);   
  //return the latch pin high to signal chip that it 
  //no longer needs to listen for information
  digitalWrite(latchPin, 1);
}

void loop() {
  //  setRedPixel(0, 0);
  switch (currentInstrument) {
  case 0:
    patternToMatrix(patterns[0], redMatrix);
    clearBlueMatrix();
    break;

  case 1:
    patternToMatrix(patterns[1], blueMatrix);
    clearMatrix();
    break;

  case 2:
    patternToMatrix(patterns[2], matrix);
    clearRedMatrix();
    break;
  }

  sendMatrix();    
  checkButtons();
  //sendShortData(0xFFFF);
  //delay(10);
  //sendShortData(0x0000);
  //delay(10);
}

void buttonLosgelassen(int x, int y) {
}

void patternToMatrix(volatile byte *pattern, volatile byte myMatrix[4][4]) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      int position = y * 4 + x;
      myMatrix[x][y] = pattern[position];
    }
  }

}

void buttonGedrueckt(int x, int y) {
  if (digitalRead(funcPin) == LOW) {
    if (y == 0) {
      if (x < 4) {
        currentInstrument = x;
      }
    }
  }
   else {
  patterns[currentInstrument][y * 4 + x]  = !patterns[currentInstrument][y * 4 + x]; 
   }

  /*
  Serial.print(x);
   Serial.print(" ");
   Serial.print(y);
   Serial.print(" ");
   Serial.print(matrix[x][y], DEC);
   Serial.println("");
   */
}

void on16Callback() {
  int position = MidiClock.div16th_counter % 16;
  //  Serial.println(position);
  int x = position % 4;
  int y = position / 4;
  switch (currentInstrument) {
  case 0:
    clearMatrix();
    setPixel(x, y);
    break;

  case 1:
    clearRedMatrix();
    setRedPixel(x, y);
    break;

  case 2:
    clearBlueMatrix();
    setBluePixel(x, y);
    break;
    
    case 3:
    clearBlueMatrix();
    clearRedMatrix();
    clearMatrix();
    patternToMatrix(patterns[0], redMatrix);
    patternToMatrix(patterns[1], blueMatrix);
    patternToMatrix(patterns[2], matrix);
    setBluePixel(x, y);
    setRedPixel(x, y);
    setPixel(x, y);
    break;
  }

  if (patterns[0][position]) {
//        MD.triggerTrack(0, 100);
    MidiUart.sendNoteOn(36, 100);
  }
  if (patterns[1][position]) {
    //    MD.triggerTrack(1, 100);
    MidiUart.sendNoteOn(50, 100);
  }
  if (patterns[2][position]) {
    //    MD.triggerTrack(2, 100);
    MidiUart.sendNoteOn(52, 100);
  }
  //  return;
  //  sendMatrix();
}
