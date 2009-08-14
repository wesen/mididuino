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
package com.ruinwesen.patchmanager.swing.form;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.swing.event.EventListenerList;

public class Form implements FormElementListener, Iterable<FormElement> {

    private List<FormElement> formelementList;
    private EventListenerList listenerList;
    private String name;
    
    public Form(String name) {
        super();
        if (name == null) {
            throw new IllegalArgumentException("name==null");
        }
        this.name = name;
        this.listenerList = new EventListenerList();
        this.formelementList = new ArrayList<FormElement>(20);
    }
    
    public String getName() {
        return name;
    }
    
    public boolean isValid() {
        for (FormElement elem: this) {
            if (!elem.checkIsValid())
                return false;
        }
        return true;
    }
    
    public void validate() throws ValidationError {
        for (FormElement elem: this) {
            elem.validate();
        }
    }
    
    public void add(FormElement e) {
        formelementList.add(e);
        e.addFormElementListener(this);
    }

    public void addFormElementListener(FormElementListener l) {
        if (l == this) {
            throw new IllegalArgumentException("form can not listen to itself");
        }
        listenerList.add(FormElementListener.class, l);
    }

    public void removeFormElementListener(FormElementListener l) {
        listenerList.remove(FormElementListener.class, l);
    }
    
    @Override
    public void formelementValueChanged(FormElementEvent evt) {
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length-2; i>=0; i-=2) {
            if (listeners[i]==FormElementListener.class) {
                // Lazily create the event:
                ((FormElementListener)listeners[i+1]).formelementValueChanged(evt);
            } 
        }
    }

    @Override
    public void formelementValueValidityChanged(FormElementEvent evt) {
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length-2; i>=0; i-=2) {
            if (listeners[i]==FormElementListener.class) {
                // Lazily create the event:
                ((FormElementListener)listeners[i+1]).formelementValueValidityChanged(evt);
            }
        }
    }

    @Override
    public Iterator<FormElement> iterator() {
        return formelementList.iterator();
    }

    public int indexOf(FormElement formElement) {
        return formelementList.indexOf(formElement);
    }

    public FormElement item(int index) {
        return formelementList.get(index);
    }
    
    
    
}
