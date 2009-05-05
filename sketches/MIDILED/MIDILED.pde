// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

// Die erste LED ist an Pin 10 angeschlossen
int ledPin = 10;
// Die zweite LED ist an Pin 11 angeschlossen
int ledPin2 = 11;

void setup() {
  // Initialisieren der MIDI Schnittstelle
  MidiUart.init();
  // Setzen der LED Pins auf Ausgang
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  // Beim Erkennen einer NOTE ON Nachricht wird onNoteOnCallback aufgerufen
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  // Beim Erkennen einer NOTE OFF Nachricht wird onNoteOffCallback aufgerufen
  Midi.setOnNoteOffCallback(onNoteOffCallback);
  // Beim Erkennen einer CONTROL CHANGE Nachricht wird onControlChangeCallback aufgerufen
  Midi.setOnControlChangeCallback(onControlChangeCallback);
}

void loop() {
  // Überpruefen, ob MIDI Daten empfangen wurden
  if (MidiUart.avail()) {
    // Einlesen des empfangenen MIDI Bytes
    byte c = MidiUart.getc();
    // Übergeben an die MIDI Library, damit diese die Daten erkennt
    Midi.handleByte(c);
  }
}

// Wenn eine eingehende NOTE ON Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOnCallback(byte msg[]) {
  // Die Helligkeit der LED wird auf die Tonhöhe gesetzt.
  analogWrite(ledPin, map(msg[1], 0, 127, 0, 255));  
}

// Wenn eine eingehende NOTE OFF Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOffCallback(byte msg[]) {
  // Ausschalten der LED
  digitalWrite(ledPin, LOW);
}

// Wenn eine eingehende CONTROL CHANGE Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onControlChangeCallback(byte msg[]) {
  // Prüfen, ob es sich um CC Nummer 1 handelt
  if (msg[1] == 1) {
    // Helligkeit der zweiten LED auf den CC Wert setzen
    analogWrite(ledPin2, msg[2]);
  }
}
