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
    
    public String normalizedValue() {
        if (normalized == null) {
            normalized = value.trim().toLowerCase();
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
    
    public int hashCode() {
        if (hashCode == 0) {
            this.hashCode = normalizedValue().hashCode();
        }
        return hashCode;
    }
    
    public boolean equals(Object o) {
        return o == this 
            || o instanceof Tag
            && normalizedValue().equalsIgnoreCase(((Tag)o).normalizedValue());
    }
    
    public String toString() {
        return value;
    }
    
}
