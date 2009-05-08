int dataPin = 2;
int oePin = 3;
int latchPin = 4;
int clockPin = 5;

// pins 8 - 11 knoepfe
int buttonPins[4] = { 8, 9, 10, 11 };
volatile byte matrix[4][4] = { 0 };
volatile byte buttons[4][4] = { 0 };
volatile byte oldButtons[4][4] = { 0 };


void setup() {
  Serial.begin(115200);
  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    digitalWrite(buttonPins[i], HIGH); // pullup widerstand aktivieren
  }
  
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
        } else {
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

void clearMatrix() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      matrix[x][y] = 0;
    }
  }
}

void sendMatrix() {
  for (int y = 0; y < 4; y++) {
      byte rowNibble = ~(1 << y) & 0xf;
      byte dataByte = rowNibble | (matrix[y][0] << 4) | (matrix[y][1] << 5) | (matrix[y][2] << 6) | (matrix[y][3] << 7);
      sendData(dataByte);
      for (int i = 0; i < 4; i++) {
        buttons[y][i] = digitalRead(buttonPins[i]);
      }
  }
  sendData(0xFF);
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

void loop() {
    sendMatrix();
    checkButtons();
}

void buttonLosgelassen(int x, int y) {
}

void buttonGedrueckt(int x, int y) {
  matrix[x][y] = !matrix[x][y];

  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(matrix[x][y], DEC);
  Serial.println("");
}

