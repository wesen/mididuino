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

import java.util.LinkedList;
import java.util.List;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Sequencer;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.MidiDevice.Info;

import org.apache.log4j.Logger;

/**
 * Implements the MidiSend class using the {@link javax.sound.midi} classes.
 * @author chresan
 */
public class JavaMidiSend extends MidiSend {

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
     * The input device.
     */
    private JavaMidiDevice input;
    
    /**
     * The output device.
     */
    private JavaMidiDevice output;

    /**
     * {@inheritDoc}
     */
    public List<MidiDevice> getOutputDeviceList() throws MidiSendException {
        return getDeviceList(false);
    }

    /**
     * {@inheritDoc}
     */
    public List<MidiDevice> getInputDeviceList() throws MidiSendException {
        return getDeviceList(true);
    }

    /**
     * Returns a list containing either all input devices or all output devices.
     * 
     * @param inputs if true then the list returns only input devices,
     *               otherwise it contains only output devices
     * @return a list containing either all input devices or all output devices
     * @throws MidiSendException there was a problem while retrieving the information
     */
    private List<MidiDevice> getDeviceList(boolean inputs) 
        throws MidiSendException {
        // the result list
        List<MidiDevice> list = new LinkedList<MidiDevice>();
        
        // test all midi devices
        for (javax.sound.midi.MidiDevice.Info info:
            MidiSystem.getMidiDeviceInfo()) {
            
            // the device we want to test
            javax.sound.midi.MidiDevice device;
            try {
                device = MidiSystem.getMidiDevice(info);
            } catch (MidiUnavailableException ex) {
                // log the problem
                if (getLog().isDebugEnabled()) {
                    getLog().debug(ex);
                }
                // the device is unavailable for some reasen, thus can not be
                // used as input/output - continue with the next one
                continue;
            } 
            
            // we are only interested in hardware devices 
            if ( !(device instanceof Sequencer) 
                    && !(device instanceof Synthesizer)) {

                // test what kind of device we have and add it to the list
                // if we want it.
                if ((!inputs && device.getMaxReceivers() != 0) // output
                || (inputs && device.getMaxTransmitters() != 0) ) { // input
                    list.add(new JavaMidiDevice(inputs, info));
                }   
            }
        }
        return list;
    }

    /**
     * {@inheritDoc}
     */
    public void setOutputDevice(MidiDevice device) {
        if (device == null) {
            this.output = null;
            return;
        }
        if (!device.isOutput()) {
            throw new IllegalArgumentException("midi device not supported: "+device);
        }
        this.output = (JavaMidiDevice) device;
    }

    /**
     * {@inheritDoc}
     */
    public void setInputDevice(MidiDevice device) {
        if (device == null) {
            this.input = null;
            return;
        }
        if (!device.isInput()) {
            throw new IllegalArgumentException("midi device not supported: "+device);
        }
        this.input = (JavaMidiDevice) device;
    }

    /**
     * {@inheritDoc}
     */
    public MidiDevice getOutputDevice() {
        return output;
    }

    /**
     * {@inheritDoc}
     */
    public MidiDevice getInputDevice() {
        return input;
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
        // copy the range
        byte[] msg = new byte[length];
        System.arraycopy(data, fromIndex, msg, 0, msg.length);
        // create the message
        MidiMessage message = new RawMidiMessage(msg);
        try {
            // get the device and receiver and send the message
            javax.sound.midi.MidiDevice device;
            device = MidiSystem.getMidiDevice(output.info);
            Receiver receiver = device.getReceiver();
            receiver.send(message, 0);
            receiver.close();
        } catch (MidiUnavailableException ex) {
            throw new MidiSendException(ex.getMessage());
        }
    }
    
    /**
     * Raw midi message implementation.
     * @author chresan
     */
    private static class RawMidiMessage extends MidiMessage {
        /**
         * @param data the midi message
         */
        public RawMidiMessage(byte[] data) {
            super(data);
        }
        
        @Override
        public Object clone() {
            return new RawMidiMessage(data.clone());
        }
    }

    /**
     * A java midi device.
     * {@inheritDoc}
     */
    private static class JavaMidiDevice implements MidiDevice {
        
        /**
         * true if this device is an input device
         * false if this device is an output device
         */
        private boolean input;
        
        /**
         * The device info object.
         */
        private Info info;

        /**
         * Creates a new java midi device wrapper.
         * @param input true if the device is an input device,
         *              false if the device is an output device
         * @param info    the wrapped java midi info object
         */
        public JavaMidiDevice(boolean input,
                javax.sound.midi.MidiDevice.Info info) {            
            this.input = input;
            this.info = info;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public String getName() {
            return info.getName();
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
