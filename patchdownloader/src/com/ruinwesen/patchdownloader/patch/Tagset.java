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

import java.util.AbstractCollection;
import java.util.Collection;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

import name.cs.csutils.CSUtils;

public class Tagset extends AbstractCollection<Tag> implements Comparator<Tag> {

    private Set<Tag> set;
    
    public Tagset(Tagset tagset) {
        this();
        addAll(tagset);
    }
    
    public Tagset() {
        // ordered set
        this.set = new TreeSet<Tag>((Comparator<Tag>)this);    
    }
    
    public Tagset(String ... tags) {
        this();
        for (String tag: tags) {
            add(tag);
        }
    }
    
    public boolean add(String tagName) {
        if (tagName != null && tagName.trim().length()>0) {
            return add(new Tag(tagName));
        } else {
            return false;
        }
    }

    private Collection<? extends Tag> expose1(Collection<? extends Tag> tags) {
        if (tags instanceof Tagset) {
            return ((Tagset)tags).set;
        }
        return tags;
    }

    private Collection<?> expose2(Collection<?> tags) {
        if (tags instanceof Tagset) {
            return ((Tagset)tags).set;
        }
        return tags;
    }

    public boolean addAll(Collection<? extends Tag> tags) {
        return super.addAll(expose1(tags));
    }
    
    public boolean removeAll(Collection<?> tags) {
        return super.removeAll(expose2(tags));
    }

    public boolean add(Tag tag) {
        return set.add(tag);
    }
    
    public boolean remove(Object obj) {
        if (obj instanceof String) {
            set.remove(new Tag((String)obj));
        }
        return set.remove(obj);
    }

    @Override
    public Iterator<Tag> iterator() {
        return set.iterator();
    }

    @Override
    public int size() {
        return set.size();
    }

    @Override
    public int compare(Tag a, Tag b) {
        return a.normalizedValue().compareTo(b.normalizedValue());
    }
    
    public String toString() {
        return CSUtils.join(" ", set.iterator()); 
    }

    public static Tagset parseTags(String tags) {
        Tagset set = new Tagset();
        for (String tag: tags.split("[\\n\\r\\s]+")) {
            if (tag.length() == 0) {
                continue;
            }
            set.add(tag);
        }
        return set;
    }
    
}
