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

public class MidiSendException extends Exception {

    /**
     * 
     */
    private static final long serialVersionUID = -6649183596767880267L;

    /**
     * {@inheritDoc}
     */
    public MidiSendException() {
        super();
    }

    /**
     * {@inheritDoc}
     */
    public MidiSendException(String message) {
        super(message);
    }

    /**
     * {@inheritDoc}
     */
    public MidiSendException(Throwable cause) {
        super(cause);
    }

    /**
     * {@inheritDoc}
     */
    public MidiSendException(String message, Throwable cause) {
        super(message, cause);
    }

}
