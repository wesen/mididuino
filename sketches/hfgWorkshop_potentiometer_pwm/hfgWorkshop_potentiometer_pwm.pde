int ledPin = 11;
int potPin = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

int logHelligkeit[64] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 25,
28, 30, 33, 36, 39, 42, 46, 49, 53, 56, 60, 64, 68, 72, 77, 81, 86, 90,
95, 100, 105, 110, 116, 121, 127, 132, 138, 144, 150, 156, 163, 169,
176, 182, 189, 196, 203, 210, 218, 225, 233, 240, 248, 255
};

void loop() {
  int potWert = analogRead(potPin);
  int logWert = map(potWert, 0, 1023, 0, 63);
  int ledWert = logHelligkeit[logWert];
  analogWrite(ledPin, ledWert);
  Serial.println(potWert, DEC);
}
