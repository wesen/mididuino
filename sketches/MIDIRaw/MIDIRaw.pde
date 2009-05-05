// Einbinden der Debounce-Library, um Tastenpreller zu vermeiden
#include <Debounce.h>

// Der Taster ist an Pin 12 angeschlossen
int buttonPin = 12;
// Die Debounce-Library wird auf 20 Millisekunden konfiguriert
Debounce button = Debounce(20, buttonPin);

// Eine Note über MIDI senden, indem die serielle Schnittstelle verwendet wird
void sendNoteOn(byte channel, byte noteNumber, byte velocity) {
  // Zuerst die Nachrichtennummer und den Kanal senden
  Serial.print(0x90 | channel, BYTE); 
  // Dann die Tonhöhe. Das obere Bit wird auf 0 gesetzt
  Serial.print(noteNumber & 0x7F, BYTE);
  // Letztendlich die Anschlagsgeschwindigkeit
  Serial.print(velocity & 0x7F, BYTE); 
}

void setup() {
  // Serielle Schnittstelle auf die Geschwindigkeit von MIDI konfigurieren
  Serial.begin(31250);
  // Tasterpin als Eingang mit Pullup-Widerstand konfigurieren
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
}

// Speichern des vorigen Tasterwerts, um Tastendrücke zu erkennen
int prevButton = HIGH;

void loop() {
  // Debounce-Library benutzen, um Tastenpreller auszufilter
  button.update();
  // Aktuellen Tasterwert auslesen
  byte buttonValue = button.read();
  
  // Hat sich der TAsterstatus verändert, wird eine MIDI Note gesendet
  if (prevButton != buttonValue) {
    // Der Taster ist gedrückt worden, eine Note senden
    if (buttonValue == LOW) {
      sendNoteOn(0, 100, 100);
    } else {
      // Der Taster wurde losgelassen, die Note stoppen
      sendNoteOn(0, 100, 0);
    }
    prevButton = buttonValue; // Speichern des neuen Tasterzustande
  }
}
