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
package name.cs.csutils;

import java.awt.EventQueue;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.File;
import java.io.IOException;
import java.io.Serializable;

import javax.swing.JFileChooser;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.EventListenerList;

public class CSFileSelectionContext implements Serializable {

    private static final long serialVersionUID = -8956092433113221663L;

    public static final String PROPERTY_FILE = "file";
    
    private static final CSFileSelectionContext DEFAULT_CONTEXT =
        new CSFileSelectionContext();
    
    private File file;
    private boolean updated = false;
    private EventListenerList listenerList;
    private transient ChangeEvent changeEvent;
    protected PropertyChangeSupport propertyChangeSupport;
    
    public CSFileSelectionContext() {
        super();
        listenerList = new EventListenerList();
        propertyChangeSupport = new PropertyChangeSupport(this);
    }
    
    public static CSFileSelectionContext getDefaultContext() {
        return DEFAULT_CONTEXT;
    }

    public void beforeFileSelection(JFileChooser chooser) {
        chooser.setCurrentDirectory(file);
    }

    public void afterFileSelection(JFileChooser chooser) {
        setFile(chooser.getSelectedFile());
    }
    
    public synchronized void setFile(File value) {
        updated = true;
        File oldValue = this.file;
        if (value != oldValue && (value==null||!value.equals(oldValue))) {
            this.file = value;
            if (EventQueue.isDispatchThread()) {
                fireChangeEvent();
                firePropertyChangeEvent(oldValue, value);
            } else {
                SwingUtilities.invokeLater(new FireChangeEvent(oldValue, value));
            }
        }
    }
    
    public synchronized File getFile() {
        if ((!updated) && this != DEFAULT_CONTEXT) {
            setFile(DEFAULT_CONTEXT.getFile());
        }
        return file;
    }

    public void addPropertyChangeListener(String propertyName, 
            PropertyChangeListener l) {
        propertyChangeSupport.addPropertyChangeListener(propertyName, l);
    }
    
    public void addPropertyChangeListener(PropertyChangeListener l) {
        propertyChangeSupport.addPropertyChangeListener(l);
    }

    public void addChangeListener(ChangeListener l) {
        listenerList.add(ChangeListener.class, l); 
    }

    public void removeChangeListener(ChangeListener l) {
        listenerList.remove(ChangeListener.class, l); 
    }
    
    private final void fireChangeEvent() {
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length-2; i>=0; i-=2) {
            if (listeners[i]==ChangeListener.class) {
                // Lazily create the event:
                if (changeEvent == null)
                    changeEvent = new ChangeEvent(this);
                ((ChangeListener)listeners[i+1]).stateChanged(changeEvent);
            }
        }
    }

    private final void firePropertyChangeEvent(File oldValue, File newValue) {
        propertyChangeSupport.firePropertyChange(PROPERTY_FILE, oldValue, newValue);
    }
    
    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    
    private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException {
        in.defaultReadObject();   
    }
    
    private final class FireChangeEvent implements Runnable {
        private File oldValue;
        private File newValue;
        public FireChangeEvent(File oldValue, File newValue) {
            this.oldValue = oldValue;
            this.newValue = newValue;
        }
        public void run() {
            fireChangeEvent();
            firePropertyChangeEvent(oldValue, newValue);
        }
    }
    
}
