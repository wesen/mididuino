// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

int ledPin = 13;

void setup() {
  // Initialisieren der MIDI Schnittstelle
  MidiUart.init();
  // Setzen der LED Pins auf Ausgang
  pinMode(ledPin, OUTPUT);
  
  // Beim Erkennen einer NOTE ON Nachricht wird onNoteOnCallback aufgerufen
  Midi.setOnNoteOnCallback(onNoteOnCallback);
  // Beim Erkennen einer NOTE OFF Nachricht wird onNoteOffCallback aufgerufen
  Midi.setOnNoteOffCallback(onNoteOffCallback);
}

void loop() {
  // †berpruefen, ob MIDI Daten empfangen wurden
  if (MidiUart.avail()) {
    // Einlesen des empfangenen MIDI Bytes
    byte c = MidiUart.getc();
    // †bergeben an die MIDI Library, damit diese die Daten erkennt
    Midi.handleByte(c);
  }
}

// Wenn eine eingehende NOTE ON Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOnCallback(byte msg[]) {
  digitalWrite(ledPin, HIGH);
}

// Wenn eine eingehende NOTE OFF Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOffCallback(byte msg[]) {
  // Ausschalten der LED
  digitalWrite(ledPin, LOW);
}

