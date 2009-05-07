int ledPin = 11;
int potPin = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

}

int ledStatus = HIGH;
int delayWert = 500;

void loop() {
  digitalWrite(ledPin, ledStatus);
  unsigned long schaltTime = millis();
  ledStatus = !ledStatus;

  while ((millis() - schaltTime) < delayWert) {
    int potWert = analogRead(potPin);
    delayWert = map(potWert, 0, 1023, 50, 500);
  }

}
