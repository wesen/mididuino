#include <Debounce.h>
#include <EEPROM.h>

int buttonPin = 2;
Debounce debouncer = Debounce( 20 , buttonPin ); 
int potiPin = 5;
int ledPins[6] = { 11, 10, 9, 6, 5, 3 };

int currentLoop = 0;

int currentDelay = 20;

#define NUM_PROGRAMS 4

void setup() {
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  currentLoop =   EEPROM.read(0);
  if (currentLoop >= NUM_PROGRAMS) {
    currentLoop = 0;
  }
}

int loga[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 
18, 20, 22, 25, 28, 30, 33, 36, 39, 42, 46, 49, 53, 56, 60, 64, 68, 
72, 77, 81, 86, 90,
95, 100, 105, 110, 116, 121, 127, 132, 138, 144, 150, 156, 163, 169,
176, 182, 189, 196, 203, 210, 218, 225, 233, 240, 248, 255};

void loop1(int init) {
  static int pwmCounter = 0;
  static int pwmDirection = 1;
  for (int i = 0; i < 6; i++) {
    analogWrite(ledPins[i], loga[constrain(pwmCounter, 0, 63)]);
  }
  pwmCounter = (pwmCounter + pwmDirection);
  if (pwmCounter >= 63 || pwmCounter <= 0) {
     pwmDirection *= -1;
  }
}

void loop2(int init) {
  static int blinkCount = 0;
  static int blinkDirection = 1;
  for (int i = 0; i < 6; i++) {
    if (blinkDirection == 1) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
  blinkCount += blinkDirection;
  if (blinkCount >= 63 || blinkCount <= 0) {
     blinkDirection *= -1;
  }
}

void loop3(int init) {
  static int blinkCounters[6] = { 0 };
  static int blinkSpeeds[6] = { 0 };
  static int blinkDirections[6] = { 0 } ;
  if (init) {
    for (int i = 0; i < 6; i++) {
      blinkSpeeds[i] = random(1, 50);
      blinkDirections[i] = blinkSpeeds[i];
      blinkCounters[i] = 0;
    }
  }
  for (int i = 0; i < 6; i++) {
    if (blinkDirections[i] > 0) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
    blinkCounters[i] = (blinkCounters[i] + blinkDirections[i]);
    if (blinkCounters[i] >= (63 * 16) || blinkCounters[i] <= 0) {
       blinkDirections[i] *= -1;
    } 
  }
}

void loop4(int init) {
  static int pwmCounters[6] = { 0 };
  static int pwmSpeeds[6] = { 0 };
  static int pwmDirections[6] = { 0 } ;
  if (init) {
    for (int i = 0; i < 6; i++) {
      pwmSpeeds[i] = random(1, 50);
      pwmDirections[i] = pwmSpeeds[i];
      pwmCounters[i] = 0;
    }
  }
  for (int i = 0; i < 6; i++) {
    analogWrite(ledPins[i], constrain(pwmCounters[i] / 16, 0, 63));
    pwmCounters[i] = (pwmCounters[i] + pwmDirections[i]);
    if (pwmCounters[i] >= (63 * 16) || pwmCounters[i] <= 0) {
       pwmDirections[i] *= -1;
    } 
  }
}


typedef void (*loop_function_t)(int);
loop_function_t loops[NUM_PROGRAMS] = {loop1, loop4, loop4, loop4 };

int prevButton = HIGH;

void loop() {
  loops[currentLoop](0);
  for (int i = 0; i < currentDelay; i++) {
    currentDelay = map(analogRead(potiPin), 0, 1023, 5, 300);
    debouncer.update ( );
    int newButton = debouncer.read();
    if (newButton != prevButton) {
      if (newButton == LOW) {
        currentLoop = (currentLoop + 1) % NUM_PROGRAMS;
        EEPROM.write(0, currentLoop);
        loops[currentLoop](1);
      }
      prevButton = newButton;
    }
    delayMicroseconds(100);
  }
  
}
