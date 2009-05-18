void setup() {
  GUI.setLine(GUI.LINE1);
  GUI.put_string_at(0, "HELLO");  
  GUI.setLine(GUI.LINE2);
  GUI.put_value(0, 10);
  GUI.put_value(1, 11);
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() { 
}
