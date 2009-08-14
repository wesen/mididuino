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

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.event.EventListenerList;

public abstract class FormElement {

    public static final String PROPERTY_VALUE = "formelement.property.value";
    public static final String PROPERTY_ENABLED = "formelement.property.enabled";
    public static final String PROPERTY_VALUE_VALIDITY = "formelement.property.value.validity";
    protected PropertyChangeSupport propertyChangeSupport;
    protected EventListenerList listenerList;
    private boolean validity_flag = true;
    private boolean enabled = true;
    private JLabel label;
    private FormElementValidator validator;

    public FormElement() {
        propertyChangeSupport = new PropertyChangeSupport(this);
        listenerList = new EventListenerList();
    }
    
    public String getLabelText() {
        return label == null ? null : label.getText();
    }
    
    public void validate() throws ValidationError {
        FormElementValidator validator = getValidator();
        try {
            if (validator != null) {
                validator.validate(this);
            } 
            setValid(true);
        } catch (ValidationError ex) {
            setValid(false);
            throw ex;
        }
    }
    
    public boolean checkIsValid() {
        FormElementValidator validator = getValidator();
        boolean valid = validator == null || validator.isValid(this);
        setValid(valid);
        return valid;
    }
    
    public void setValidator(FormElementValidator validator) {
        FormElementValidator oldValidator = this.validator;
        if (oldValidator != validator) {
            this.validator = validator;
            if (validator == null) {
                setValid(true);
            } else {
                checkValidity();
            }
        }
    }
    
    public FormElementValidator getValidator() {
        return validator;
    }
    
    public void setLabel(String labelValue) {
        if (labelValue == null) {
            throw new IllegalArgumentException("labelValue==null");
        }
        setLabel(new JLabel(labelValue));
    }
    
    public void setLabel(JLabel label) {
        this.label = label;
    }
    
    public JLabel getLabel() {
        return label;
    }
    
    public abstract JComponent[] getComponents();
    
    public void setValid(boolean value) {
        boolean oldValue = this.validity_flag;
        if (oldValue != value) {
            this.validity_flag = value;
            firePropertyChangeEvent(PROPERTY_VALUE_VALIDITY, oldValue, value);
            fireFormElementValueValidityChangedEvent();
        }
    }
    
    public boolean isEnabled() {
        return enabled;
    }
    
    public void setEnabled(boolean enabled) {
        boolean oldValue = this.enabled;
        if (oldValue != enabled) {
            this.enabled = enabled;
            setFormInputElementsEnabled(enabled);
            firePropertyChangeEvent(PROPERTY_ENABLED, oldValue, enabled);
        }
    }
    
    protected abstract void setFormInputElementsEnabled(boolean enabled);
    
    public boolean isValid() {
        return validity_flag;
    }
    
    public abstract Object getValue();
    public abstract void setValue(Object value);

    protected void checkValidity() {
        FormElementValidator validator = getValidator();
        if (validator != null) {
            setValid(validator.isValid(this));
        }
    }
    
    public void addFormElementListener(FormElementListener l) {
        listenerList.add(FormElementListener.class, l);
    }

    public void removeFormElementListener(FormElementListener l) {
        listenerList.add(FormElementListener.class, l);
    }

    protected void handleValueChange(Object oldValue, Object newValue) {
        propertyChangeSupport.firePropertyChange(PROPERTY_VALUE, oldValue, newValue);
        fireFormElementValueChangedEvent();
        checkValidity();
    }
    
    private void fireFormElementValueChangedEvent() {
        FormElementEvent evt = null;
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length-2; i>=0; i-=2) {
            if (listeners[i]==FormElementListener.class) {
                // Lazily create the event:
                if (evt == null)
                    evt = new FormElementEvent(this, FormElementEvent.FORM_ELEMENT_VALUE_CHANGED);
                ((FormElementListener)listeners[i+1]).formelementValueChanged(evt);
            }
        }
    }

    protected void fireFormElementValueValidityChangedEvent() {
        FormElementEvent evt = null;
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length-2; i>=0; i-=2) {
            if (listeners[i]==FormElementListener.class) {
                // Lazily create the event:
                if (evt == null)
                    evt = new FormElementEvent(this, FormElementEvent.FORM_ELEMENT_VALUE_VALIDITY_CHANGED);
                ((FormElementListener)listeners[i+1]).formelementValueValidityChanged(evt);
            }
        }
    }
    
    protected void firePropertyChangeEvent(String name, Object oldValue, Object newValue) {
        propertyChangeSupport.firePropertyChange(name, oldValue, newValue);
    }

    protected void firePropertyChangeEvent(String name, int oldValue, int newValue) {
        propertyChangeSupport.firePropertyChange(name, oldValue, newValue);
    }

    protected void firePropertyChangeEvent(String name, boolean oldValue, boolean newValue) {
        propertyChangeSupport.firePropertyChange(name, oldValue, newValue);
    }
    
    public void addPropertyChangeListener(PropertyChangeListener l) {
        propertyChangeSupport.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l) {
        propertyChangeSupport.removePropertyChangeListener(l);
    }

    public void addPropertyChangeListener(String propertyName, PropertyChangeListener l) {
        propertyChangeSupport.addPropertyChangeListener(propertyName, l);
    }

    public void removePropertyChangeListener(String propertyName, PropertyChangeListener l) {
        propertyChangeSupport.removePropertyChangeListener(propertyName, l);
    }
    
}
