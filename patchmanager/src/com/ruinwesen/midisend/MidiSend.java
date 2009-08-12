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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Collections;
import java.util.List;

import name.cs.csutils.CSUtils;

/**
 * The MidiSend class sends midi messages to the connected device.
 * This class is a dummy class which provides no inputs/outputs.
 * 
 * Subclasses should implement the following methods
 *  getInputDeviceList() 
 *  getOutputDeviceList()
 *  setOutputDevice(MidiDevice out)
 *  setInputDevice(MidiDevice out)
 *  getOutputDevice()
 *  getInputDevice()
 *  send(byte[], int, int)
 *  
 * @author chresan
 */
public class MidiSend {

    protected MidiSendCallback callback;
    
    /**
     * Creates a dummy MidiSend instance which provides no inputs/outputs.
     */
    public MidiSend() {
        super();
    }

    public void setCallback(MidiSendCallback callback) {
        this.callback = callback;
    }
    
    public MidiSendCallback getCallback() {
        return this.callback;
    }
    
    /**
     * Returns a list containing all output devices.
     * 
     * @return a list containing all output devices
     * @throws MidiSendException there was a problem while retrieving the information
     */
    public List<MidiDevice> getOutputDeviceList()
        throws MidiSendException {
        return Collections.emptyList();
    }

    /**
     * Returns a list containing all input devices.
     * 
     * @return a list containing all input devices
     * @throws MidiSendException there was a problem while retrieving the information
     */
    public List<MidiDevice> getInputDeviceList()
        throws MidiSendException {
        return Collections.emptyList();
    }

    /**
     * Uses the specified device as output device. Only <code>null</code>
     * or instances of class {@link MidiDevice} obtained using 
     * {@link #getOutputDeviceList()} should be passed as argument.
     * 
     * @throws IllegalArgumentException if the device is not an output device 
     * @throws ClassCastException the device class is not supported
     * @param device the new output device or <code>null</code> to clear the property
     */
    public void setOutputDevice(MidiDevice device) {
        if (device != null) {
            throw new ClassCastException("unsupported device class: "+device.getClass().getName());
        }
    }

    /**
     * Uses the specified device as input device. Only <code>null</code>
     * or instances of class {@link MidiDevice} obtained using 
     * {@link #getInputDeviceList()} should be passed as argument.
     * 
     * @throws IllegalArgumentException if the device is not an input device 
     * @throws ClassCastException the device class is not supported
     * @param device the new input device or <code>null</code> to clear the property
     */
    public void setInputDevice(MidiDevice device) {
        if (device != null) {
            throw new ClassCastException("unsupported device class: "+device.getClass().getName());
        }
    }
    
    /**
     * Sets the input device property if {@link MidiDevice#isInput()} returns
     * true for the specified object, otherwise sets the output device property.
     * Only instances of class {@link MidiDevice} obtained using 
     * {@link #getInputDeviceList()} or {@link #getOutputDeviceList()}
     * should be passed as argument, <code>null</code>-arguments are not
     * permitted.
     * 
     * @throws IllegalArgumentException if the argument is <code>null</code>
     * @throws ClassCastException the device class is not supported
     * @param device use this device as input or output device
     */
    public void setDevice(MidiDevice device) {
        if (device == null) {
            throw new IllegalArgumentException("device == null");
        }
        if (device.isInput()) {
            setInputDevice(device);
        } else {
            setOutputDevice(device);
        }
    }
    
    /**
     * Returns the output device or <code>null</code> if the property is not set.
     * 
     * @return the output device
     */
    public MidiDevice getOutputDevice() {
        return null;
    }

    /**
     * Returns the input device or <code>null</code> if the property is not set.
     * 
     * @return the input device
     */
    public MidiDevice getInputDevice() {
        return null;
    }

    /**
     * Returns the output device with the specified name.
     * @param name name of the device
     * @return the output device with the specified name
     * @throws MidiSendException there was a problem while retrieving the information
     */
    public MidiDevice getOutputDevice(String name) throws MidiSendException {
        if (name == null) {
            throw new IllegalArgumentException("name == null");
        }
        for (MidiDevice device: getOutputDeviceList()) {
            if (name.equals(device.getName())) {
                return device;
            }
        }
        return null;
    }
    
    /**
     * Returns the input device with the specified name.
     * @param name name of the device
     * @return the input device with the specified name
     * @throws MidiSendException there was a problem while retrieving the information
     */
    public MidiDevice getInputDevice(String name) throws MidiSendException {
        if (name == null) {
            throw new IllegalArgumentException("name == null");
        }
        for (MidiDevice device: getInputDeviceList()) {
            if (name.equals(device.getName())) {
                return device;
            }
        }
        return null;
    }
    
    /**
     * Ensures that {@link #getOutputDevice()} and {@link #getInputDevice()}
     * return both non-<code>null</code> values.
     * 
     * @throws MidiSendException if the input device or the output device or
     *  both are <code>null</code>
     */
    protected void ensureDevicesAreSet() throws MidiSendException {
        if (getOutputDevice() == null) {
            throw new MidiSendException("output not set");
        }
        if (getInputDevice() == null) {
            throw new MidiSendException("input not set");
        }
    }
    
    /**
     * Sends the data to the output device.
     * 
     * @param data the midi data
     * @throws MidiSendException sending the data failed
     */
    public void send(byte[] data) throws MidiSendException {
        send(data, 0, data.length);
    }

    /**
     * Sends a region of the data array to the output device.
     * 
     * @param data the midi data
     * @param fromIndex send data from this index, inclusive
     * @param toIndex send data up to this index, exclusive
     * @throws IllegalArgumentException if the range fromIndex to toIndex is invalid
     * @throws MidiSendException sending the data failed or the data range is empty
     */
    public void send(byte[] data, int fromIndex, int toIndex)
        throws MidiSendException {
        ensureDevicesAreSet();
        if (fromIndex<0 || toIndex>data.length || fromIndex>toIndex) {
            throw new IllegalArgumentException(
              "fromIndex:"+fromIndex+" toIndex:"+toIndex+" size:"+data.length);
        }
    }
    
    /**
     * Sends the contents of the specified file to the output device.
     * 
     * @param file the file containing the midi message
     * @throws FileNotFoundException the file does not exist
     * @throws MidiSendException sending the data failed
     */
    public void send(File file) throws MidiSendException, FileNotFoundException {
        send(new FileInputStream(file), true);
    }

    /**
     * Sends the contents of the specified input stream to the output device.
     * 
     * @param stream the input stream containing the midi message
     * @throws MidiSendException sending the data failed
     */
    public void send(InputStream stream) throws MidiSendException {
        send(stream, false);
    }

    /**
     * Sends the contents of the specified input stream to the output device.
     * 
     * @param stream the input stream containing the midi message
     * @param close  if true, then the stream is closed 
     *               at the end of the operation
     * @throws MidiSendException sending the data failed
     */
    public void send(InputStream is, boolean close) throws MidiSendException {
        ByteArrayOutputStream buffer = new ByteArrayOutputStream(2048);
        try {
            CSUtils.copy(is, buffer);
        } catch (IOException ex) {
            throw new MidiSendException("could not read stream",ex);
        } finally {
            if (close) {
                try {
                    is.close();
                } catch (IOException ex) {
                    // no op
                }
            }
        }
        send(buffer.toByteArray());
    }

}
