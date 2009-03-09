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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.channels.Channels;
import java.nio.channels.WritableByteChannel;
import java.util.Collections;
import java.util.List;

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

    /**
     * Creates a dummy MidiSend instance which provides no inputs/outputs.
     */
    public MidiSend() {
        super();
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
    public void send(InputStream stream, boolean close) throws MidiSendException {
        ByteArrayOutputStream buffer = new ByteArrayOutputStream(2048);
        try {
            if (stream instanceof FileInputStream) {
                WritableByteChannel chout = Channels.newChannel(buffer);
                ((FileInputStream)stream).getChannel().transferTo(0,0,chout);
                chout.close();
            } else {
                byte[] tmp = new byte[2048];
                int r;
                while ((r=stream.read(tmp))!=-1) {
                    buffer.write(tmp, 0, r);
                }
            }
        } catch (IOException ex) {
            throw new MidiSendException("could not read stream",ex);
        } finally {
            if (close) {
                try {
                    stream.close();
                } catch (IOException ex) {
                    throw new MidiSendException("could not close stream",ex);
                }
            }
        }
        send(buffer.toByteArray());
    }

}
