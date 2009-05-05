// Einbinden der LiquidCrystal-Library
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 8, 3, 4, 5, 6, 7);

// Einbinden der RuinWesen MIDI Library
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

void setup() {
   MidiUart.init(); // Initialisieren der MIDI Library
}

// Diese Variable zählt die Anzahl der eingegangenen Bytes, um diese
// auf dem kleinen 2x16 Display umzubrechen. Pro Zeile werden 5 Bytes
// angezeigt
int cnt = 0;

void loop() {
  if (MidiUart.avail()) { // Ueberpruefen, ob MIDI-Daten empfangen wurden
    byte c = MidiUart.getc(); // Einlesen des empfangenen MIDI Bytes

    lcd.print(c, HEX); // Anzeigen des Bytes in Hexadezimal auf dem Display
    lcd.print(" ");

    cnt++; // Die Anzahl der empfangenen Bytes wird hochgezählt
    if (cnt == 5) { // Nach 5 empfangenen Bytes müssen wir die Zeile wechseln
      lcd.setCursor(0, 1);
    }
    if (cnt == 10) { // Nach 10 empfangenen Bytes müssen wir wieder auf die erste
                     // Zeile wecheln
      lcd.setCursor(0, 0);
      cnt = 0;
    }
  }
}
