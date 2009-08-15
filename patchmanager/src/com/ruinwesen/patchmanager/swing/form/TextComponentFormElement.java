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

import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.JTextComponent;

public class TextComponentFormElement extends FormElement implements DocumentListener {

    private JTextComponent field;
    private String oldValue;
    
    public TextComponentFormElement(JTextComponent field, int limit) {
        this(field);
        if (limit != -1) {
            field.setDocument(new TextDocumentWithLimit(limit));
        }
    }
    
    public TextComponentFormElement(JTextComponent field) {
        if (field == null) {
            throw new IllegalArgumentException("field == null");
        }
        this.field = field;
        this.oldValue = getText();
        setMainComponent(field);
        install();
    }

    private void install() {
        field.getDocument().addDocumentListener(this);
    }
    
    public JTextComponent getField() {
        return field;
    }
    
    @Override
    public Object getValue() {
        return getText();
    }

    @Override
    public void setValue(Object value) {
        setText((String)value);
    }

    public String getText() {
        return field.getText();        
    }
    
    public void setText(String value) {
        field.setText(value);
    }
    
    @Override
    public void changedUpdate(DocumentEvent evt) {
        documentChanged(evt);   
    }

    @Override
    public void insertUpdate(DocumentEvent evt) {
        documentChanged(evt);
    }

    @Override
    public void removeUpdate(DocumentEvent evt) {
        documentChanged(evt);
    }

    protected void documentChanged(DocumentEvent evt) {
        String newValue = getText();
        handleValueChange(oldValue, newValue);
        oldValue = newValue;
    }

}
