// Einbinden der LiquidCrystal-LCD-Library
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 8, 3, 4, 5, 6, 7);

// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
// Einbinden der MIDI Clock Funktionalitaet
#include <MidiClock.h>
MidiClass Midi;

void on16Callback() {
  lcd.setCursor(0, 1);
  lcd.print(MidiClock.div16th_counter % 32, DEC);
}

void setup() {
   MidiUart.init(); // Initialisieren der MIDI-Schnittstelle
   MidiClock.mode = MidiClock.INTERNAL_MIDI;
   MidiClock.transmit = false;
   MidiClock.setOn16Callback(on16Callback);
   MidiClock.setTempo(120);
   MidiClock.start();
   lcd.print("MIDI SEQUENCER"); // Begruessungsnachricht auf dem Display anzeigen
}

void loop() {
  // In der Hauptschleife werden MIDI Nachrichten empfangen, und an den MIDI-Stack 
  // weiter gereicht
  if (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
//  lcd.setCursor(0, 1);
//  lcd.print(clock, DEC);
}
