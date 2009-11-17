void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_fill("BUTTON TEST");
}

void loop() {
  GUI.setLine(GUI.LINE2);
  if (BUTTON_DOWN(Buttons.BUTTON1)) {
    GUI.put_string_fill("DOWN");
  } 
  else {
    GUI.put_string_fill("UP");
  }
}


