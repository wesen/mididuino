// Einbinden der Debounce Library
#include <Debounce.h>

// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

// Speichern der aktuellen Potentiometerwerte
int potValues[4]; 
// Die LEDs sind an den Pins 10 und 11 angeschlossen
int ledPins[2] = { 10, 11 };

// Konfigurieren der Buttons, die an Pins 12 und 9 angeshclossen sind
Debounce debounce1(20, 12);
Debounce debounce2(20, 9);
// Speichern der vorigen Tasterwerte
byte prevButtons[2] = { HIGH, HIGH };
// Speichern der aktuellen Tasterwert
byte newButtons[2];
// MIDI Tonhöhen, die beim Drücken eines Tasters gesendet werden
int buttonNotes[2] = { 60, 61 };

// Speichern der Kontrollernummern
int ccNumbers[4] = { 10, 11, 12, 13 };

void setup() {
  // Initialisieren der MIDI SChnittstelle
  MidiUart.init();
  // Konfigurieren der LED pins als Ausgang
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  // Konfigurieren der Buttonpins als Eingaenge, mit Pullup Widerstaenden
  pinMode(12, INPUT);
  pinMode(9, INPUT);
  digitalWrite(12, HIGH);
  digitalWrite(9, HIGH);
}

void loop() {
  // Einlesen der Tasterwerte
  debounce1.update();
  newButtons[0] = debounce1.read();
  debounce2.update();
  newButtons[1] = debounce2.read();
  
  int i;
  for (i = 0; i < 4; i++) {
    // Auslesen des Potentiometerwertes
    int value = analogRead(i);
    // Konvertieren des analogen Wertes auf den MIDI-Wertebereich
    value = map(value, 0, 1023, 0, 127);
    // Erkennen, ob der Potentiometer bewegt wurde
    if (abs(value - potValues[i]) >= 2) {
      // Speichern des neuen Potentiometerwertes
      potValues[i] = value;
      // Senden der MIDI CONTROL CHANGE Nachricht
      MidiUart.sendCC(ccNumbers[i], value);
    }
  }
  
  for (i = 0; i < 2; i++) {
    // Hat sich der TAsterwert gegenüber dem vorigen Wert geändert,
    // dann wurde der Taster gedrückt oder losgelassen
    if (newButtons[i] != prevButtons[i]) {
      if (newButtons[i] == LOW) {
        // Der Taster wurde gedrückt, Note senden und LED anschalten
        MidiUart.sendNoteOn(buttonNotes[i], 100);
        digitalWrite(ledPins[i], HIGH);
      } else {
        // Der Taster wurde losgelassen, Note stoppen und LED ausschalten
        MidiUart.sendNoteOff(buttonNotes[i]);
        digitalWrite(ledPins[i], LOW);
      }
      // Speichern der neuen Tasterwerte
      prevButtons[i] = newButtons[i];
    }
  }
}
