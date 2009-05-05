// Einbinden der LiquidCrystal-LCD-Library
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 8, 3, 4, 5, 6, 7);

// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

// Funktion zum Anzeigen des Nachrichtentyps
void showType(char msg[]) {
  // LCD loeschen, Type anzeige, und auf die naechste Zeile springen
 lcd.clear(); lcd.print(msg); lcd.setCursor(0, 1);
}

// Funktion zum Anzeigen eines Wert in Dezimaldarstellung, gefolgt von einem Leerzeichen
void printValue(byte value) {
  lcd.print(value, DEC); lcd.print(" ");
}

// Wenn eine eingehende NOTE ON Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOnCallback(byte msg[]) {
  showType("NOTE ON"); // Typ der Nachricht anzeigen
  // Kanal, Tonhšhe und Anschlagsgeschwindigkeit anzeigen
  printValue(msg[0] & 0xF); printValue(msg[1]); printValue(msg[2]);
}

// Wenn eine eingehende NOTE OFF Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onNoteOffCallback(byte msg[]) {
  showType("NOTE OFF"); // Typ der Nachricht anzeigen
  // Kanal und Tonhšhe anzeigen
  printValue(msg[0] & 0xF); printValue(msg[1]); 
}

// Wenn eine eingehende CONTROL CHANGE Nachricht von der MIDI Library erkannt wurde
// wird diese Funktion aufgerufen
void onControlChangeCallback(byte msg[]) {
  showType("CONTROL CHANGE"); // Typ der Nachricht anzeigen
  // Kanal, Kontrollernummer und Kontrollerwert anzeigen
  printValue(msg[0] & 0xF); printValue(msg[1]); printValue(msg[2]);
}

void setup() {
   MidiUart.init(); // Initialisieren der MIDI-Schnittstelle
   lcd.print("MIDI SNIFFER"); // Begruessungsnachricht auf dem Display anzeigen
   // Rueckruffunktionen fuer Noten und Kontrollernachrichten einrichten
   Midi.setOnNoteOnCallback(onNoteOnCallback);
   Midi.setOnNoteOffCallback(onNoteOffCallback);
   Midi.setOnControlChangeCallback(onControlChangeCallback);
}

void loop() {
  // In der Hauptschleife werden MIDI Nachrichten empfangen, und an den MIDI-Stack 
  // weiter gereicht
  if (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
}
