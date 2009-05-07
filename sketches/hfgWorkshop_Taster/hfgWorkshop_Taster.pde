int ledPin = 11;
int tasterPin = 12;

void setup() {
  pinMode(ledPin, OUTPUT);   // als output definieren
  pinMode(tasterPin, INPUT); // als input definieren
  digitalWrite(tasterPin, HIGH); // pullup widerstand intern aktivieren
}

void loop() {
  if (digitalRead(tasterPin) == LOW) {
    digitalWrite(ledPin, HIGH);
  } else { 
    digitalWrite(ledPin, LOW);
  }
}
