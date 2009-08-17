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

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.LinkedList;
import java.util.List;

import name.cs.csutils.CSUtils;
import name.cs.csutils.InputStreamReaderThread;
import name.cs.csutils.Platform;
import name.cs.csutils.Platform.OS;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Implements the MidiSend class using the 'midi-send' commandline utility.
 * @author chresan
 */
public class RWMidiSend extends MidiSend {

    /** The logger. */
    private static Log log = LogFactory.getLog(RWMidiSend.class);
    
    /** The midi-send command. */
    private String command = null;
    
    /** The input device. */
    private RWMidiDevice input;

    /** The output device. */
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
            if (command != null && log.isDebugEnabled() ) {
                log.debug("midi-send located at: "+command);
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
            PATH = "/midi-send/windows/midi-send.exe";
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
            if (log.isDebugEnabled()) {
                log.debug(ex);
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
     * {@inheritDoc}
     */
    @Override
    public MidiSendProcess send(byte[] data, int fromIndex, int toIndex)
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
            tempFile = File.createTempFile("midi", ".hex");
            tempFile.deleteOnExit();
        } catch (IOException ex) {
            throw new MidiSendException("could not create temporary file", ex);
        }
        try {
            CSUtils.copy(new ByteArrayInputStream(data, fromIndex, toIndex-fromIndex), tempFile, true);
        } catch (IOException ex) {
            tempFile.delete();
            throw new MidiSendException("could not write temporary file: "+tempFile.getAbsolutePath(), ex);
        } 
        
        return send(tempFile, true);
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
    public MidiSendProcess send(File file) throws MidiSendException, FileNotFoundException {
        return send(file, false);
    }
    
    private MidiSendProcess send(File file, boolean deleteFileOnExit) throws MidiSendException {
        ensureDevicesAreSet();
        if (!file.exists()) {
            throw new MidiSendException(new FileNotFoundException(file.getAbsolutePath()));
        }
        String cmd = getCommand(true);

        String inputArg = Integer.toString(input.index);
        String outputArg = Integer.toString(output.index);
        
        if (log.isDebugEnabled()) {
            log.debug("midi-in:"+input+", midi-out:"+output);
        }
        
        if (Platform.isFlavor(OS.UnixFlavor)) {
            // midi-send fix for linx/alsa
            inputArg = parseAlsaName(input.getName());
            outputArg = parseAlsaName(output.getName());

            if (log.isDebugEnabled()) {
                log.debug("Decoding alsa device names. midi-in:"+inputArg+", midi-out:"+outputArg);
            }
        }
        
        String filepath;
        try {
            filepath = file.getCanonicalPath();
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("could not get canonical path", ex);
            }
            throw new MidiSendException("could not get canonical path", ex);
        }

        // create the process
        String[] command 
            //File midisendFile = new File(command);
            //File midisendDir = midisendFile.getParentFile();
            = new String[] {
                    cmd, "-b", "-I0x41", 
                    "-i",inputArg,
                    "-o",outputArg,
                    filepath};
            if (log.isDebugEnabled()) {
                log.debug("Invoking midi-send:"+CSUtils.join(" ", (String[])command));
            }

        RWMidiSendProcess send = new RWMidiSendProcess(file, deleteFileOnExit, command,
                callback);
        new Thread(send).start();
        return send;
    }

    private static class RWMidiSendProcess implements MidiSendProcess, Runnable {
        
        private File file;
        private boolean deleteFileOnExit;
        private String[] command;
        private Process process;
        private MidiSendCallback callback;
        private Throwable error;
        private boolean terminated = false;
        private final Object TERMINATION_LOCK = new Object();

        public RWMidiSendProcess(File file, boolean deleteFileOnExit,
                String[] command, MidiSendCallback callback) {
            this.file = file;
            this.deleteFileOnExit = deleteFileOnExit;
            this.command = command;
            this.callback = callback;
        }

        private synchronized void setProcess(Process  p) {
            this.process = p;
        }
        
        private synchronized Process getProcess() {
            return process;
        }

        private synchronized void setError(Throwable t) {
            this.error = t;
        }

        public synchronized boolean isTerminated() {
            return terminated;
        }
        
        private synchronized void setTerminated(boolean v) {
            this.terminated = v;
        }
        
        @Override
        public synchronized Throwable getError() {
            return error;
        }

        @Override
        public void cancel() {
            Process p = getProcess();
            if (p != null) {
                p.destroy();
            }
        }
        
        public void run() {
            try {
                runProcess();
            } catch (Exception ex) {
                setError(ex);
            }
        }
        
        private void runProcess() throws IOException, MidiSendException {
            boolean interrupted = false;
            InputStreamReaderThread stderrBuffer = null;
            InputStreamReaderThread stdoutBuffer = null;
            try {
                Process process = new ProcessBuilder(command).start();
                setProcess(process);
                stderrBuffer = new InputStreamReaderThread(process.getErrorStream(), false);
                stdoutBuffer = new InputStreamReaderThread(process.getInputStream(), false);
                stderrBuffer.start();
                stdoutBuffer.start();
    
                CSUtils.ProcessResult presult = CSUtils.waitFor(process,1000000, 100);
                if (log.isDebugEnabled()) {
                    log.debug("midi-send:status("+presult.status+"),interrupted("+presult.interruptedException+")"
                            +",timeout("+presult.timeout+")");
                }
                interrupted = presult.interrupted;
                if (presult.timeout) {
                    throw new MidiSendException(
                            "timeout "
                            +stderrBuffer.getBuffer());
                }
                if (isErrorStatus(presult.status)) {
                    String err = stderrBuffer.getBuffer();
                    if (!err.trim().isEmpty()) {
                        err = " "+err;
                    }
                    throw new MidiSendException(
                            "midi-send exited with status:"+presult.status+err);
                }
                
                if (callback != null) {
                    callback.midisendCompleted();
                }
            } finally {
                if (stderrBuffer!=null)
                    stderrBuffer.cancel();
                if (stdoutBuffer!=null)
                    stdoutBuffer.cancel();
                process.destroy();
                setTerminated(true);
                if (deleteFileOnExit) {
                    file.delete();
                }
                synchronized (TERMINATION_LOCK) {
                    TERMINATION_LOCK.notifyAll();
                }
                if (interrupted) {
                    Thread.currentThread().interrupt();
                }
            }
        }

        @Override
        public void waitFor(long timeout) throws InterruptedException {
            if (isTerminated()) {
                return;
            }
            
            synchronized (TERMINATION_LOCK) {
                TERMINATION_LOCK.wait(timeout);
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
        InputStreamReaderThread processInput = null;
        try {
            try {
                process = new ProcessBuilder(new String[] {
                        cmd,"-l", inputs?"i":"o"}).start();
            } catch (IOException ex) {
                throw new MidiSendException(
                "midi-send:getDeviceList() failed, could start process", ex);
            } 
            processInput = new InputStreamReaderThread(process.getInputStream(),
                    true);
            

            CSUtils.ProcessResult presult = CSUtils.waitFor(process,100000, 100);
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
            String text = processInput.getBuffer();
            for (String line: text.split("[\\n\\r]+")) {
                if (line.indexOf("optarg")>=0||line.trim().length()==0) {
                    // workaround for midi-send debug output
                    continue;
                }
                list.add(new RWMidiDevice(inputs, list.size(), line));
            }
            return list;
        } finally {
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
        InputStreamReaderThread buf = new InputStreamReaderThread(process.getErrorStream(),
                true);
        buf.run();
        return buf.getBuffer();
    }

    /**
     * Returns true if the status value indicates an error otherwise false.
     * @param status the status
     * @return true if the status value indicates an error otherwise false.
     */
    private static boolean isErrorStatus(int status) {
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
