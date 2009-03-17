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

public final class Tag implements Comparable<Tag> {

    private String value;
    private transient String normalized;
    private transient int hashCode = 0;
   
    public Tag(String value) {
        if (value == null || value.trim().length() == 0) {
            throw new IllegalArgumentException("invalid tag: "+value);
        }
        this.value = value;
    }
    
    public Tag normalize() {
        String n = normalizedValue();
        Tag tag = new Tag(n);
        tag.normalized = n;
        tag.hashCode = hashCode;
        return tag;
    }
    
    public String value() {
        return value;
    }
    
    static String normalizedValue(String str) {
        return str.trim().toLowerCase();
    }
    
    public String normalizedValue() {
        if (normalized == null) {
            normalized = normalizedValue(value);
            if (normalized.equals(value)) {
                normalized = value;
            }   
        }
        return normalized;
    }

    @Override
    public int compareTo(Tag o) {
        return o == this ? 0 : normalizedValue().compareTo(o.normalizedValue());
    }
    
    @Override
    public int hashCode() {
        if (hashCode == 0) {
            this.hashCode = normalizedValue().hashCode();
        }
        return hashCode;
    }
    
    @Override
    public boolean equals(Object o) {
        return o == this 
            || o instanceof Tag
            && normalizedValue().equalsIgnoreCase(((Tag)o).normalizedValue());
    }
    
    @Override
    public String toString() {
        return value;
    }
    
}
