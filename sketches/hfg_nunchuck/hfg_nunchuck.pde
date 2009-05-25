#include <Wire.h>
#include <string.h>
#include <MidiUart.h>
#include <Midi.h>
MidiClass Midi;

#define pwrpin PORTC3
#define gndpin PORTC2

void nunchuckInit() {
  DDRC |= _BV(pwrpin) | _BV(gndpin);
  PORTC &=~ _BV(gndpin);
  PORTC |= _BV(pwrpin);
  delay(100); 

  Wire.begin();
  Wire.beginTransmission(0x52);
  Wire.send(0x40);
  Wire.send(0x00);
  Wire.endTransmission();
}

int nunchuckReadData(byte outbuf[]) {
  nunchuckSendZero();
  delay (30);

  Wire.requestFrom (0x52, 6);	// request data from nunchuck
  int cnt = 0;
  while (Wire.available ()) {
    outbuf[cnt] = nunchukDecodeByte(Wire.receive ());
    cnt++;
  }
  
  return cnt;
} 

byte nunchukDecodeByte(byte x) {
  x = (x ^ 0x17) + 0x17;
  return x;
}

void nunchuckSendZero() {
  Wire.beginTransmission(0x52);	// transmit to device 0x52
  Wire.send(0x00);		// sends one byte
  Wire.endTransmission();	// stop transmitting
}

int joystick[2] = { 0 };
int oldJoystick[2] = { 0 };
byte joystickCC[2] = { 2, 3 };

int accel[3] = { 0 };
int oldAccel[3] = { 0 };
byte accelCC[3] = { 4, 5, 6 };

int button[2] = { 0 };
int oldButton[2] = { 0 };
byte buttonNote[2] = { 60, 62 };

void nunchuckParseData(byte outbuf[]) {
  joystick[0] = outbuf[0];
  joystick[1] = outbuf[1];
  accel[0] = outbuf[2] << 2; 
  accel[1] = outbuf[3] << 2;
  accel[2] = outbuf[4] << 2;

  button[0] = bitRead(outbuf[5], 0);
  button[1] = bitRead(outbuf[5], 1);

  for (int i = 0; i < 3; i++) {  
    accel[i] += bitRead(outbuf[5], i + 2) * 2 + bitRead(outbuf[5], i + 3);
  }
}

void nunchuckSendMidi() {
  for (int i = 0; i < 2; i++) {
    if (abs(oldJoystick[i] - joystick[i]) >= 2) {
      MidiUart.sendCC(joystickCC[i], map(joystick[i], 0, 255, 0, 127));
      oldJoystick[i] = joystick[i];
    }
  }
  
  for (int i = 0; i < 3; i++) {
    if (abs(oldAccel[i] - accel[i]) >= 7) {
      int value = map(accel[i], 300, 800, 0, 127);
      MidiUart.sendCC(accelCC[i], constrain(value, 0, 127));
      oldAccel[i] = accel[i];
    }
  }
  
  for (int i = 0; i < 2; i++) {
    if (oldButton[i] != button[i]) {
      if (button[i] == 0) {
        MidiUart.sendNoteOn(buttonNote[i], 100);
      } else {
        MidiUart.sendNoteOff(buttonNote[i]);
      }
      oldButton[i] = button[i];
    }
  }
}

void setup () {
  MidiUart.init();
  nunchuckInit();
}

void loop () {
  byte outbuf[6];

  if (nunchuckReadData(outbuf) >= 5) {
    nunchuckParseData(outbuf);
    nunchuckSendMidi();
  }
}

