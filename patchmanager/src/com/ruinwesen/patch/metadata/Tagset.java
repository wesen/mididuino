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
package com.ruinwesen.patch.metadata;

import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
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

    public Tagset(Tag ... tags) {
        this();
        for (Tag tag: tags) {
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

    @Override
    public boolean contains(Object o) {
        if (o instanceof String) {
            o = new Tag((String)o);
        }
        return set.contains(o);
    }
    
    @Override
    public boolean addAll(Collection<? extends Tag> tags) {
        return super.addAll(expose1(tags));
    }

    @Override
    public boolean removeAll(Collection<?> tags) {
        return super.removeAll(expose2(tags));
    }

    @Override
    public boolean add(Tag tag) {
        return set.add(tag);
    }

    @Override
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

    @Override
    public String toString() {
        return CSUtils.join(" ", set.iterator()); 
    }
    
    public Tag[] toTagArray() {
        return toArray(new Tag[size()]);
    }

    public String[] toStringArray() {
        String[] a = new String[set.size()];
        Iterator<Tag> iter = set.iterator();
        for (int i=0;i<a.length;i++) {
            a[i] = iter.next().value();
        }
        return a;
    }
    
    public Tagset filterPrefix(String prefix, boolean positive, boolean removePrefix) {
        if (prefix == null) {
            throw new IllegalArgumentException("prefix == null");
        }
        Tagset set = new Tagset();
        for (Tag tag: this) {
            if (tag.normalizedValue().startsWith(prefix) == positive) {
                if (positive && removePrefix) {
                    String str = tag.normalizedValue().substring(prefix.length());
                    if (!str.isEmpty())
                        set.add(str);
                } else {
                    set.add(tag);
                }
            }
        }
        return set;
    }
    
    public static Tagset parseTags(String tags) {
        Tagset set = new Tagset();
        for (String tag: tags.split("[\\n\\r\\s,]+")) {
            if (!tag.isEmpty()) {
                set.add(tag);
            }
        }
        return set;
    }

    public String toSortedString() {
        List<Tag> list = new ArrayList<Tag>(set);
        Collections.sort(list);
        return CSUtils.join(" ", list.iterator());
    }

    public void addAll(Collection<? extends String> c) {
        for (String str: c)
            add(str);
    }

    @Override
    public int hashCode() {
        return set.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) return false;
        if (o == this) return true;
        if (o instanceof Tagset) {
            Tagset set = (Tagset)o;
            if (set.size() != size()) {
                return false;
            }
            return set.set.equals(this.set);
        }
        return false;
        
    }

    public boolean containsSomeOf(Collection<Tag> c) {
        for (Tag t: c) {
            if (contains(t)) {
                return true;
            }
        }
        return false;
    }
    
}
