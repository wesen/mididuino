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
package com.ruinwesen.patchdownloader.patch;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * A tag.
 * 
 * @author Christian Schneider
 */
public final class Tag implements Comparable<Tag>, Serializable {

    private static final long serialVersionUID = 2469660950046706553L;

    /** the not-normalized tag value */
    private String value;
    
    /** the normalized tag value */
    private transient String normalized;
   
    /** 
     * A new tag.
     * @param value the tag value
     * @throws IllegalArgumentException if the specified argument is <code>null</code>
     * of the {@link String#length() string-length} of the value is <code>0</code> 
     */
    public Tag(String value) {
        if (value == null || value.trim().length() == 0) {
            throw new IllegalArgumentException("invalid tag: "+value);
        }
        this.value = value;
    }
    
    /**
     * Creates a new tag instance with a normalized tag value.
     * @return a new tag instance with a normalized tag value
     */
    public Tag normalize() {
        String n = normalizedValue();
        Tag tag = new Tag(n);
        tag.normalized = n;
        return tag;
    }
    
    /**
     * Returns the not-normalized tag value.
     * @return the not-normalized tag value
     */
    public String value() {
        return value;
    }
    
    /**
     * Normalizes the specified tag value.
     * @param str a tag
     * @return the normalized tag
     * @see #normalizedValue()
     */
    static String normalizedValue(String str) {
        return str.trim().toLowerCase();
    }
    
    /**
     * Returns the normalized tag value.
     * The normalized tag value is in lowercase letters only and 
     * does not start / end with whitespace characters.
     * 
     * @return the normalized tag value
     */
    public String normalizedValue() {
        if (normalized == null) {
            normalized = normalizedValue(value);
            if (normalized.equals(value)) {
                normalized = value;
            }   
        }
        return normalized;
    }

    /**
     * Compares this tag's normalized value to the normalized value of the
     * specified tag lexicographically. 
     * @see String#compareTo(String)
     */
    @Override
    public int compareTo(Tag o) {
        return o == this ? 0 : normalizedValue().compareTo(o.normalizedValue());
    }

    /**
     * Returns a hash code value for the object. 
     */
    @Override
    public int hashCode() {
        return normalizedValue().hashCode();
    }
    
    /**
     * Indicates whether some other object is "equal to" this one.
     */
    @Override
    public boolean equals(Object o) {
        return o == this 
            || o instanceof Tag
            && normalizedValue().equalsIgnoreCase(((Tag)o).normalizedValue());
    }
    
    /**
     * Returns the tag value.
     * @return the tag value
     */
    @Override
    public String toString() {
        return value;
    }

    private void writeObject(ObjectOutputStream oos) throws IOException {
        oos.defaultWriteObject();
    }

    private void readObject(ObjectInputStream ois) 
        throws ClassNotFoundException, IOException {
        ois.defaultReadObject();
        this.normalized = null;
    }
}
