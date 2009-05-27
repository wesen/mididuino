void setup() {
}

void loop() {
	GUI.updatePage();
	if (BUTTON_DOWN(Buttons.BUTTON1)) {
		GUI.put_string_fill("DOWN");
	} else {
		GUI.put_string_fill("UP");
	}
	GUI.update();
	delay(10);
}

void handleGui() {
}
