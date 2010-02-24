package com.ruinwesen.patchmanager.swing.wizards;

import javax.swing.JFrame;

import com.ruinwesen.patchmanager.swing.form.Wizard;
import com.ruinwesen.patchmanager.swing.forms2.MidiSendForm;

public class FaderfoxMidiSendWizard extends MidiSendWizard {

	public FaderfoxMidiSendWizard() {
		super();
		setTitle("Faderfox Firmware Update");
		// midisendForm.getMidiSend().setDeviceID(0x50); // faderfox adapter
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
        Wizard w = new FaderfoxMidiSendWizard();
        w.showDialog((JFrame)null);

	}

}
