//
//  MidiUploader.java
//  Arduino
//
//  Created by Manuel Odendahl on 12/1/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

package processing.app;
import java.io.*;
import java.util.*;

public class MidiUploader extends AvrdudeUploader {
	
	public MidiUploader() {
	}
	
  public boolean uploadUsingPreferences(String buildPath, String className)
  throws RunnerException {
    String uploadUsing = Preferences.get("boards." + Preferences.get("board") + ".upload.using");
    if (uploadUsing == null) {
      // fall back on global preference
      uploadUsing = Preferences.get("upload.using");
    }
    if (uploadUsing.equals("bootloader")) {
			ArrayList params = new ArrayList();
			params.add(buildPath + File.separator + className + ".hex");
			return midisend(params);
    } else {
      Collection params = getProgrammerCommands(uploadUsing);
      params.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");
      return avrdude(params);
    }
  }
  
	protected String getMidiPortIndex(String midiPort) throws RunnerException {
		if (midiPort == null) {
			throw new RunnerException("No MIDI port selected!");
		}
		
		int parenIdx = midiPort.indexOf(")");
		if (parenIdx <= 0) {
			throw new RunnerException("Invalid MIDI port selected!");
		}
		
		String idx = midiPort.substring(0, parenIdx);
		return idx;
	}
	
  public boolean midisend(Collection params) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("midi-send");
		commandDownloader.add("-b");
		String midiPort = Preferences.get("midi.port");
		commandDownloader.add("-I" + Preferences.get("midi.deviceiid"));
		commandDownloader.add("-i" + getMidiPortIndex(Preferences.get("midi.inputport")));
		commandDownloader.add("-o" + getMidiPortIndex(Preferences.get("midi.outputport")));
		commandDownloader.addAll(params);
		
    if (Preferences.getBoolean("upload.verbose")) {
			// do something
    } else {
			// do something
    }

    return executeUploadCommand(commandDownloader);
  }
	
	public String[] getMidiPorts(String flag) {
	    List commandDownloader = new ArrayList();
		commandDownloader.add("midi-send");
		commandDownloader.add(flag);
		
		CommandRunner runner = new CommandRunner();
		int result = runner.runCommand(commandDownloader);
		if (result == 0) {
			ArrayList outputBuffer = runner.getOutputBuffer();
			String ports[] = new String[outputBuffer.size()];
			outputBuffer.toArray(ports);
			return ports;
		} else {
			return null;
		}
	}
	
	public String[] getMidiInputPorts() {
		return getMidiPorts("-li");
	}
	
	public String[] getMidiOutputPorts() {
		return getMidiPorts("-lo");
	}
	
	public void message(String s) {
    //System.err.println("MSG: " + s);
    System.err.print(s);
		
    // ignore cautions
    if (s.indexOf("Error") != -1) {
      //exception = new RunnerException(s+" Check the serial port selected or your Board is connected");
      //System.out.println(s);
      notFoundError = true;
      return;
    }
    if(notFoundError) {
      //System.out.println("throwing something");
      exception = new RunnerException("the selected serial port "+s+" does not exist or your board is not connected");
      return;
    }
    if (s.indexOf("Device is not responding") != -1 ) {
      exception =  new RunnerException("Device is not responding, check the right serial port is selected or RESET the board right before exporting");
      return;
    }
    if (s.indexOf("Programmer is not responding") != -1 ||
        s.indexOf("programmer is not responding") != -1 ||
        s.indexOf("protocol error") != -1) {
      exception = new RunnerException("Problem uploading to board.  See http://www.arduino.cc/en/Guide/Troubleshooting#upload for suggestions.");
      return;
    }
    if (s.indexOf("Expected signature") != -1) {
      exception = new RunnerException("Wrong microcontroller found.  Did you select the right board from the Tools > Board menu?");
      return;
    }
  }
}