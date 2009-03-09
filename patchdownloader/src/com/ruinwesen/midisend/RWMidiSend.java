/** 
 * Copyright (C) 2009 Christian Schneider
 *
 * This file is part of Patchdownloader.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
package com.ruinwesen.midisend;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

import org.apache.log4j.Logger;

/**
 * Implements the MidiSend class using the 'midi-send' commandline utility.
 * @author chresan
 */
public class RWMidiSend extends MidiSend {

    /**
     * The cached logger.
     */
    private static Logger log;
    
    /**
     * Returns the logger for this class.
     * @return the logger for this class
     */
    private static Logger getLog() {
        if (log == null) {
            log = Logger.getLogger(JavaMidiSend.class);
        }
        return log;
    }
    
    /**
     * The midi-send command.
     */
    private String command =
        "/home/christian/dev/mididuino/mididuino-read-only/hardware/tools/mididuino/"+
        "midi-send";
    
    /**
     * The input device.
     */
    private RWMidiDevice input;

    /**
     * The output device.
     */
    private RWMidiDevice output;
    
    @Override
    public void setOutputDevice(MidiDevice device) {
        if (device == null) {
            this.output = null;
            return;
        }
        if (!device.isOutput()) {
            throw new IllegalArgumentException("midi device not supported: "+device);
        }
        this.output = (RWMidiDevice) device;
    }

    @Override
    public RWMidiDevice getOutputDevice() {
        return this.output;
    }

    @Override
    public void setInputDevice(MidiDevice device) {
        if (device == null) {
            this.input = null;
            return;
        }
        if (!device.isInput()) {
            throw new IllegalArgumentException("midi device not supported: "+device);
        }
        this.input = (RWMidiDevice) device;
    }

    @Override
    public RWMidiDevice getInputDevice() {
        return this.input;
    }

    public List<MidiDevice> getOutputDeviceList()
        throws MidiSendException {
        return getDeviceList(false);
    }

    public List<MidiDevice> getInputDeviceList()
        throws MidiSendException {
        return getDeviceList(true);
    }

    /**
     * Returns the command used to invoke the midi-send program.
     * @return the command used to invoke the midi-send program.
     */
    public String getCommand() {
        return command;
    }

    /**
     * Sets the command used to invoke the midi-send program.
     * @return the command used to invoke the midi-send program.
     */
    public void setCommand(File command) {
        try {
            setCommand(command.getCanonicalPath());
        } catch (IOException ex) {
            if (getLog().isDebugEnabled()) {
                getLog().debug(ex);
            }
            setCommand(command.getAbsolutePath());
        }
    }

    /**
     * Sets the command used to invoke the midi-send program.
     * @return the command used to invoke the midi-send program.
     */
    public void setCommand(String command) {
        this.command = command; 
    }
    
    /**
     * Ensures the command to invoke the midi-send program is set. 
     * @throws MidiSendException the command was not set
     */
    private void ensureCommandIsSet() throws MidiSendException {
        if (command == null) {
            throw new MidiSendException("the midi-send command is not set");
        }
    }

    /**
     * {@inheritDoc}
     */
    public void send(byte[] data, int fromIndex, int toIndex)
        throws MidiSendException {
        // reuse the argument testing of the parent class
        super.send(data, fromIndex, toIndex);
        int length = toIndex-fromIndex;
        if (length == 0) {
            throw new MidiSendException("no data");
        }
        
        // create a temporary file
        File tempFile;
        try {
            tempFile = File.createTempFile("midi", ".bin");
            tempFile.deleteOnExit();
        } catch (IOException ex) {
            throw new MidiSendException("could not create temporary file", ex);
        }
        // then send the file
        try {
            send(tempFile);
        } catch (FileNotFoundException ex) {
            throw new MidiSendException("temporary file vanished magically", ex);
        } finally {
            tempFile.delete();
        }
    }

    /**
     * {@inheritDoc}
     */
    public void send(File file) throws MidiSendException, FileNotFoundException {
        ensureDevicesAreSet();
        ensureCommandIsSet();
        if (!file.exists()) {
            throw new FileNotFoundException(file.getAbsolutePath());
        }

        boolean interrupted = false;    
        // create the process
        Process process;
        try {
        process = Runtime.getRuntime().exec( new String[] {
                command, "-i",Integer.toString(input.index),
                "-o",Integer.toString(output.index), 
                file.getCanonicalPath()} );
        } catch (IOException ex) {
            throw new MidiSendException(
                    "midi-send:send(File) failed, could start process", ex);
        }
        try {  
            int status;
            while (true) {
                try {
                    status = process.waitFor();
                    break;
                } catch (InterruptedException ex) {
                    interrupted = true;                
                    // fall through and retry
                }
            }
            if (isErrorStatus(status)) {
                throw new MidiSendException(
                        "midi-send:send(File) failed, return status:"+status
                        +err(process));
            }
        } finally {
            process.destroy();
            if (interrupted) {
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * Returns a list containing either all input devices or all output devices.
     * 
     * @param inputs if true then the list returns only input devices,
     *               otherwise it contains only output devices
     * @return a list containing either all input devices or all output devices
     * @throws MidiSendException there was a problem while retrieving the information
     */
    public List<MidiDevice> getDeviceList(boolean inputs)
        throws MidiSendException {

        ensureCommandIsSet();

        Process process;
        try {
            process = Runtime.getRuntime().exec(
                new String[] {command,"-l", inputs?"i":"o"});
        } catch (IOException ex) {
            throw new MidiSendException(
            "midi-send:getDeviceList() failed, could start process", ex);
        }
        boolean interrupted = false;
        List<MidiDevice> list = new LinkedList<MidiDevice>();
        try {
            int status;
            while (true) {
                try {
                    status = process.waitFor();
                    break;
                } catch (InterruptedException ex) {
                    interrupted = true;                
                    // fall through and retry
                }
            }
            if (isErrorStatus(status)) {
                throw new MidiSendException(
                        "midi-send:getDeviceList() failed, status:"+status
                        +err(process));
            }
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            String line;
            while (true) {
                try {
                    if ((line=reader.readLine())==null) break;
                } catch (IOException ex) {
                    getLog()
                    .error("reading from the process input stream failed", ex);
                    break;
                }
                if (line.indexOf("optarg")>=0) {
                    // workaround for midi-send debug output
                    continue;
                }
                list.add(new RWMidiDevice(inputs, list.size(), line));
            }
            return list;
        } finally {
            process.destroy();
            if (interrupted) {
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * Returns the error output of the process.
     * 
     * @param process the process
     * @return the error output of the process
     */
    private String err(Process process) {
        BufferedReader reader = 
            new BufferedReader(new InputStreamReader(process.getErrorStream()));
        StringBuilder sb = null;
        try {
            String line;
            int lineno = 0;
            while ((line=reader.readLine())!=null) {
                if (sb == null) sb = new StringBuilder();
                if (lineno++ == 0) sb.append('\n');
                sb.append(line);
            }
        } catch (IOException ex) {
            getLog().error("error while reading stderr of the process "+
                    process, ex);
        }
        return sb == null ? "" : sb.toString();
    }

    /**
     * Returns true if the status value indicates an error otherwise false.
     * @param status the status
     * @return true if the status value indicates an error otherwise false.
     */
    private boolean isErrorStatus(int status) {
        return status != 0;
    }

    /**
     * A java midi device.
     * {@inheritDoc}
     */
    private static class RWMidiDevice implements MidiDevice {

        /**
         * true if this device is an input device
         * false if this device is an output device
         */
        boolean input;

        /**
         * The device index.
         */
        int index;

        /**
         * The device name.
         */
        String name;
        
        /**
         * Creates the midi device info.
         * @param input true if the device is an input device,
         *              false if the device is an output device
         * @param index the device index
         * @param plaintext the device name
         */
        public RWMidiDevice(boolean input, int index, String name) {
            this.input = input;
            this.index = index;
            this.name = name;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public String getName() {
            return name;
        }
        
        /**
         * {@inheritDoc}
         */
        @Override
        public boolean isInput() {
            return input;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean isOutput() {
            return !input;
        }
        
        /**
         * {@inheritDoc}
         */
        public String toString() {
            return getName();
        }

    }
    
}
