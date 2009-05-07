int ledPin = 11;
int potPin = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int potWert = analogRead(potPin);
  int ledWert = map(potWert, 0, 1023, 0, 255);
  analogWrite(ledPin, ledWert);
  Serial.println(potWert, DEC);
}
