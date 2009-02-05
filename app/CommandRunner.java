//
//  CommandRunner.java
//  Arduino
//
//  Created by Manuel Odendahl on 12/1/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

package processing.app;

import java.io.*;
import java.util.*;

public class CommandRunner implements MessageConsumer {
	public CommandRunner() {
	}
	
	ArrayList outputBuffer = null;
	
	ArrayList getOutputBuffer() {
		return outputBuffer;
	}
	
	public int runCommand(List commandLine) {
		String[] commandArray = new String[commandLine.size()];
		commandLine.toArray(commandArray);
		String userdir = System.getProperty("user.dir") + File.separator;
		
		String avrBasePath;
		if(Base.isMacOS()) {
			avrBasePath = new String("hardware/tools/avr/bin/"); 
		}
		else if(Base.isLinux()) {
			avrBasePath = new String("hardware/tools/");     	
		}
		else {
			avrBasePath = new String(userdir + "hardware/tools/avr/bin/"); 
		}
		
		commandArray[0] = avrBasePath + commandArray[0];
		
		outputBuffer = new ArrayList();

		int result = 0;
		
		try {
			Process process = Runtime.getRuntime().exec(commandArray);
			new MessageSiphon(process.getInputStream(), this);
			new MessageSiphon(process.getErrorStream(), this);
			
			// wait for the process to finish.  if interrupted
			// before waitFor returns, continue waiting
			//
			boolean compiling = true;
			while (compiling) {
				try {
					result = process.waitFor();
					compiling = false;
				} catch (InterruptedException intExc) {
				}
			} 
		} catch (IOException e) {
			e.printStackTrace();
			return -1;
		}
		
		return result;
	}
	
	public void message(String s) {
		outputBuffer.add(s.trim());
	}
}
