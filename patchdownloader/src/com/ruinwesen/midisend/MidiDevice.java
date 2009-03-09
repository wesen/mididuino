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

/**
 * A midi device.
 * 
 * @author chresan
 */
public interface MidiDevice {

    /**
     * Returns the name of this midi device.
     * 
     * @return the name of this midi device
     */
    String getName();
    
    /**
     * Returns true if this midi device is an input device, otherwise false.
     * 
     * @return true if this midi device is an input device, otherwise false
     */
    boolean isInput();

    /**
     * Returns false if this midi device is an input device, otherwise false.
     * 
     * @return false if this midi device is an input device, otherwise false
     */
    boolean isOutput();

    /**
     * Returns the name of this midi device using {@link #getName()}.
     * 
     * @return the name of this midi device
     */
    String toString();
    
}
