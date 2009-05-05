void setup() {
  // Serielle Schnittstelle auf die Geschwindigkeit von MIDI konfigurieren
  Serial.begin(31250);
}

void loop() {
  // Zuerst senden einer NOTE ON Nachricht
  Serial.print(0x90, BYTE);
  Serial.print(100, BYTE);
  Serial.print(100, BYTE);
  delay(200);
  // Dann senden einer NOTE OFF Nachricht
  Serial.print(0x90, BYTE);
  Serial.print(100, BYTE);
  Serial.print(0, BYTE);
  delay(2000);
}
