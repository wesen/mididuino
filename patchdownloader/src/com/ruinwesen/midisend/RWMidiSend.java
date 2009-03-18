/**
 * Copyright (c) 2009, Christian Schneider
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the names of the authors nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package com.ruinwesen.midisend;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.LinkedList;
import java.util.List;

import name.cs.csutils.CSUtils;
import name.cs.csutils.Platform;
import name.cs.csutils.StringInputBuffer;
import name.cs.csutils.Platform.OS;

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
    private String command = null;
    
    /**
     * The input device.
     */
    private RWMidiDevice input;

    /**
     * The output device.
     */
    private RWMidiDevice output;
    
    private String getCommand(boolean locate) throws MidiSendException {
        if (command != null) {
            return command;
        }
        if (locate) {
            File file;
            try {
                file = locateMidiSend();
                command = file.getCanonicalPath();
            } catch (IOException ex) {
                throw new MidiSendException("could not locate midi-send", ex);
            }
            if (command != null && getLog().isDebugEnabled() ) {
                getLog().debug("midi-send located at: "+command);
            }
        }
        return command;
    }
    
    public File locateMidiSend() throws FileNotFoundException {
        final String PATH;
        if (Platform.isFlavor(OS.UnixFlavor)) {
            PATH = "/midi-send/unix/midi-send";
        } else if (Platform.isFlavor(OS.MacOSFlavor)) {
            PATH = "/midi-send/mac/midi-send";
        } else if (Platform.isFlavor(OS.WindowsFlavor)) {
            PATH = "/midi-send/windowsx/midi-send.exe";
        } else {
            throw new FileNotFoundException("midi-send is not available on your platform");
        }
        
        URL locationURL = getClass().getResource(PATH);
        if (locationURL == null) {
            throw new FileNotFoundException("could not locate midi-send at: "+PATH);
        }
        File file;
        try {
            file = new File(locationURL.toURI());
        } catch (URISyntaxException ex) {
            throw new FileNotFoundException("could not locate midi-send at:"+PATH+" ("+ex.getMessage()+")");
        }
        if (!file.exists()) {
            throw new FileNotFoundException("could not validate midi-send location at:"+file.getAbsolutePath());
        }
        return file;
    }
    
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

    @Override
    public List<MidiDevice> getOutputDeviceList()
        throws MidiSendException {
        return getDeviceList(false);
    }

    @Override
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
    @Override
    public void send(byte[] data, int fromIndex, int toIndex)
        throws MidiSendException {
        // reuse the argument testing of the parent class
        super.send(data, fromIndex, toIndex);
        final int length = toIndex-fromIndex;
        if (length == 0) {
            throw new MidiSendException("no data");
        }
        
        // create a temporary file
        File tempFile;
        try {
            tempFile = File.createTempFile("midi", ".bin");
        } catch (IOException ex) {
            throw new MidiSendException("could not create temporary file", ex);
        }
        
        OutputStream out = null;
        try {
            out = new BufferedOutputStream(new FileOutputStream(tempFile));
            out.write(data, fromIndex, length);
            out.flush();
        } catch (IOException ex) {
            throw new MidiSendException("could not write temporary file: "+tempFile.getAbsolutePath(), ex);
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    // no op
                }
            }
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

    private String parseAlsaName(String name) {
        int idx = name.indexOf(')');
        if (idx>0) return name.substring(0, idx);
        return name;
    }
    
    /**
     * {@inheritDoc}
     */
    @Override
    public void send(File file) throws MidiSendException, FileNotFoundException {
        ensureDevicesAreSet();
        if (!file.exists()) {
            throw new FileNotFoundException(file.getAbsolutePath());
        }
        String cmd = getCommand(true);

        String inputArg = Integer.toString(input.index);
        String outputArg = Integer.toString(output.index);
        
        if (getLog().isDebugEnabled()) {
            getLog().debug("midi-in:"+input+", midi-out:"+output);
        }
        
        if (Platform.isFlavor(OS.UnixFlavor)) {
            // midi-send fix for linx/alsa
            inputArg = parseAlsaName(input.getName());
            outputArg = parseAlsaName(output.getName());

            if (getLog().isDebugEnabled()) {
                getLog().debug("Decoding alsa device names. midi-in:"+inputArg+", midi-out:"+outputArg);
            }
        }

        boolean interrupted = false;    
        // create the process
        Process process;
        String[] commandArgs; 
        try {
            //File midisendFile = new File(command);
            //File midisendDir = midisendFile.getParentFile();
            commandArgs = new String[] {
                    cmd, "-b", "-I0x40", 
                    "-i",inputArg,
                    "-o",outputArg,
                    file.getCanonicalPath()};
            
            if (getLog().isDebugEnabled()) {
                getLog().debug("Invoking midi-send:"+CSUtils.join(" ", (String[])commandArgs));
            }
            
        process = new ProcessBuilder(commandArgs).start();
        //Runtime.getRuntime().exec( command );
        } catch (IOException ex) {
            throw new MidiSendException(
                    "midi-send:send(File) failed, could start process", ex);
        }
        StringInputBuffer stderrBuffer = new StringInputBuffer(process.getErrorStream());
        StringInputBuffer stdoutBuffer = new StringInputBuffer(process.getInputStream());
        try {
            stderrBuffer.start();
            stdoutBuffer.start();
            
            CSUtils.ProcessResult presult = CSUtils.waitFor(process, 10000, 100);
            if (getLog().isDebugEnabled()) {
                getLog().debug("midi-send:status("+presult.status+"),interrupted("+presult.interruptedException+")"
                        +",timeout("+presult.timeout+")");
            }
            interrupted = presult.interrupted;
            if (presult.timeout) {
                throw new MidiSendException(
                        "midi-send:send(File) timeout:"
                        +stderrBuffer.getOutput());
            }
            if (isErrorStatus(presult.status)) {
                throw new MidiSendException(
                        "midi-send:send(File) failed, return status:"+presult.status+":"
                        +stderrBuffer.getOutput());
            }
        } finally {
            try {
                stderrBuffer.close();
            } catch (IOException ex) {
                if (getLog().isDebugEnabled()) {
                    getLog().debug("error closing stderrBuffer", ex);
                }
            }
            try {
                stdoutBuffer.close();
            } catch (IOException ex) {
                if (getLog().isDebugEnabled()) {
                    getLog().debug("error closing stdoutBuffer", ex);
                }
            }
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

        String cmd = getCommand(true);

        Process process = null;
        boolean interrupted = false;
        List<MidiDevice> list = new LinkedList<MidiDevice>();
        StringInputBuffer processInput = null;
        try {
            try {
                process = new ProcessBuilder(new String[] {
                        cmd,"-l", inputs?"i":"o"}).start();
            } catch (IOException ex) {
                throw new MidiSendException(
                "midi-send:getDeviceList() failed, could start process", ex);
            } 
            processInput = new StringInputBuffer(process.getInputStream());
            

            CSUtils.ProcessResult presult = CSUtils.waitFor(process, 10000, 100);
            interrupted = presult.interrupted;
            if (presult.timeout) {
                throw new MidiSendException(
                        "midi-send:getDeviceList() timeout");
            }
            if (isErrorStatus(presult.status)) {
                throw new MidiSendException(
                        "midi-send:getDeviceList() failed, return status:"+presult.status+":"
                        +err(process));
            }
            
            processInput.run();
            String text = processInput.getOutput();
            for (String line: text.split("[\\n\\r]+")) {
                if (line.indexOf("optarg")>=0||line.trim().length()==0) {
                    // workaround for midi-send debug output
                    continue;
                }
                list.add(new RWMidiDevice(inputs, list.size(), line));
            }
            return list;
        } finally {
            if (processInput != null) {
                try {
                    processInput.close();
                } catch (IOException ex) {
                    if (getLog().isDebugEnabled()) {
                        getLog().debug("could not close process input buffer", ex);
                    }
                }
            }
            if (process != null)
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
        StringInputBuffer buf = new StringInputBuffer(process.getErrorStream());
        buf.run();
        return buf.getOutput();
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
        
        private String prettyName;
        
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
        @Override
        public String toString() {
            if (prettyName == null) {
                String name = getName().trim();
                StringBuilder sb = new StringBuilder();
                boolean previousCharWasWhitespace = false;
                for (int i=0;i<name.length();i++) {
                    char ch = name.charAt(i);
                    if (Character.isWhitespace(ch)) {
                        if (!previousCharWasWhitespace) {
                            sb.append(' ');
                            previousCharWasWhitespace = true;
                        }
                    } else {
                        sb.append(ch);
                        previousCharWasWhitespace = false;
                    }
                }
                prettyName = sb.toString();
            }
            
            return prettyName;
        }

        @Override
        public int hashCode() {
            return name.hashCode();
        }
        
        @Override
        public boolean equals(Object o) {
            if (o == null) return false;
            if (o == this) return true;
            if (getClass().equals(o.getClass())) {
                RWMidiDevice dev = (RWMidiDevice) o;
                return input == dev.input && index == dev.index && name.equals(dev.name); 
            }
            return false;
        }
        
    }
    
}
