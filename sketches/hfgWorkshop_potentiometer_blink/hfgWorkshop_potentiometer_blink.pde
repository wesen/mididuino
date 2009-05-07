int ledPin = 11;
int potPin = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int potWert = analogRead(potPin);
  int delayWert = map(potWert, 0, 1023, 50, 500);

  // LED blinken
  digitalWrite(ledPin, HIGH);
  delay(delayWert);
  digitalWrite(ledPin, LOW);
  delay(delayWert);
}
