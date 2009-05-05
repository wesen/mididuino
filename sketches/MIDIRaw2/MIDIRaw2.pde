// Einbinden der Debounce-Library, um Tastenpreller zu vermeiden
#include <Debounce.h>

// Der Taster ist an Pin 12 angeschlossen
int buttonPin = 12;
// Die Debounce-Library wird auf 20 Millisekunden konfiguriert
Debounce button = Debounce(20, buttonPin);

// Der Potentiometer ist an Analogpin 0 angeschlossen
int potPin = 0;

// Eine Note Ÿber MIDI senden, indem die serielle Schnittstelle verwendet wird
void sendNoteOn(byte channel, byte noteNumber, byte velocity) {
  // Zuerst die Nachrichtennummer und den Kanal senden
  Serial.print(0x90 | channel, BYTE); 
  // Dann die Tonhšhe. Das obere Bit wird auf 0 gesetzt
  Serial.print(noteNumber & 0x7F, BYTE);
  // Letztendlich die Anschlagsgeschwindigkeit
  Serial.print(velocity & 0x7F, BYTE); 
}

// Einen Kontrollerwert Ÿber MIDI senden
void sendControllerChange(byte channel, byte ccNumber, byte ccValue) {
  // Zuerst die Nachrichtennummer und den Kanal senden
  Serial.print(0xB0 | channel, BYTE);
  // Dann die Kontrollernummer
  Serial.print(ccNumber & 0x7F, BYTE);
  // Letztendlich den Kontrollerwert
  Serial.print(ccValue & 0x7F, BYTE); 
}

void setup() {
  // Serielle Schnittstelle auf die Geschwindigkeit von MIDI konfigurieren
  Serial.begin(31250);
  // Tasterpin als Eingang mit Pullup-Widerstand konfigurieren
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
}

// Speichern des vorigen Tasterwerts, um TastendrŸcke zu erkennen
int prevButton = HIGH;
// Speicher des vorigen Potentiometerwertes, um Bewegungen zu erkennen
int prevPot = 0;

void loop() {
  // Debounce-Library benutzen, um Tastenpreller auszufilter
  button.update();
  // Aktuellen Tasterwert auslesen
  byte buttonValue = button.read();
  
  // Hat sich der TAsterstatus verŠndert, wird eine MIDI Note gesendet
  if (prevButton != buttonValue) {
    // Der Taster ist gedrŸckt worden, eine Note senden
    if (buttonValue == LOW) {
      sendNoteOn(0, 100, 100);
    } else {
      // Der Taster wurde losgelassen, die Note stoppen
      sendNoteOn(0, 100, 0);
    }
    prevButton = buttonValue; // Speichern des neuen Tasterzustande
  }
  
  // Einlesen des aktuellen Potentiometerwertes
  int potValue = analogRead(0); 
  // Konvertieren des Wertes auf den MIDI-Wertebereich
  potValue = map(potValue, 0, 1023, 0, 127); 
  // Bewegungen erkenne, die gršsser sind als 1, um Rauschen zu vermeiden
  if (abs(potValue - prevPot) >= 2) { 
     // Senden des Potentiometerwertes als Kontrollernachricht
    sendControllerChange(0, 1, potValue);
    // Speichern des neuen Kontrollerwertes
    prevPot = potValue;
  }
  
}
