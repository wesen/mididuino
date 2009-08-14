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

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Collection;

import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;

public class ComboBoxFormElement extends FormElement implements ItemListener,
    PropertyChangeListener {

    public static final String COMBOBOX_MODEL_PROPERTY = "model";
    
    private JComboBox field;
    private Object oldValue;

    public ComboBoxFormElement(Collection<?> collection) { 
        this(collection.toArray());
    }
    
    public ComboBoxFormElement(Object ... values) { 
        this(new JComboBox(values));
    }
    
    public ComboBoxFormElement(JComboBox field) {
        this.field = field;
        if (field == null) {
            throw new IllegalArgumentException();
        }
        oldValue = field.getSelectedItem();
        install();
    }
    
    protected void install() {
        field.addItemListener(this);
        field.addPropertyChangeListener(COMBOBOX_MODEL_PROPERTY, this);
    }
    
    public JComboBox getField() {
        return field;
    }
    
    @Override
    public Object getValue() {
        return field.getSelectedItem();
    }

    @Override
    public void setValue(Object value) {
        field.setSelectedItem(value);
    }

    @Override
    protected void setFormInputElementsEnabled(boolean enabled) {
        field.setEnabled(enabled);
    }

    @Override
    public void itemStateChanged(ItemEvent e) {
        Object newValue = getValue();
        if ((oldValue == null && oldValue != newValue) || !oldValue.equals(newValue)) {
            handleValueChange(oldValue, newValue);
            oldValue = newValue;
        }
    }

    @Override
    public void propertyChange(PropertyChangeEvent evt) {
        if (COMBOBOX_MODEL_PROPERTY.equals(evt.getPropertyName())) {
            Object newValue = getValue();
            if ((oldValue == null && oldValue != newValue) || !oldValue.equals(newValue)) {
                handleValueChange(oldValue, newValue);
                oldValue = newValue;
            }
        }
    }

    @Override
    public JComponent[] getComponents() {
        JLabel label = getLabel();
        if (label != null) {
            return new JComponent[]{label, field};
        }
        return new JComponent[]{field};
    }

}
