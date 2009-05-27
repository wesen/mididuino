void setup() {
}

void loop() {
	GUI.updatePage();
	GUI.update();
}

void handleGui() {
	if (BUTTON_PRESSED(Buttons.BUTTON1)) {
		GUI.put_string_fill("PRESSED");
	}
	if (BUTTON_RELEASED(Buttons.BUTTON1)) {
		GUI.put_string_fill("RELEASED");
	}
}
