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

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import org.jdesktop.layout.GroupLayout;
import org.jdesktop.layout.GroupLayout.ParallelGroup;
import org.jdesktop.layout.GroupLayout.SequentialGroup;

import name.cs.csutils.IconLoader;

public class FormContainer implements FormElementListener {

    public static final String FORMELEMENT_INDEX = "FORMELEMENT_INDEX";
    
    private Form form;
    private JPanel panel;
    private List<JLabel> warningButtons = new ArrayList<JLabel>(20);
    private boolean validationHintsEnabled = true;
    
    protected FormContainer() {
        super();
    }

    public void setValidationHintsEnabled(boolean value) {
        if (value != validationHintsEnabled) {
            this.validationHintsEnabled = value;
            updateValidityIcons();
        }
    }
    
    public FormContainer(Form form) {
        setForm(form);
    }
    
    public String getName() {
        return form.getName();
    }
    
    protected void setForm(Form form) {
        if (form == null) {
            throw new IllegalArgumentException("form==null");
        }
        this.form = form;
        buildContainer();
    }
    
    public Form getForm() {
        return form;
    }
    
    public JComponent getContainer() {
        return panel;
    }

    private int horizontalAlignment(Component c) {
        float a = c.getAlignmentX();
        if (a == Component.LEFT_ALIGNMENT) {
            return GroupLayout.LEADING;
        } else if (a ==  Component.RIGHT_ALIGNMENT) {
            return GroupLayout.TRAILING;
        } else if (a == Component.CENTER_ALIGNMENT) {
            return GroupLayout.CENTER;
        } else {
            return GroupLayout.CENTER;
        }
    }
    
    protected void buildContainer() {
        
        ImageIcon warnIcon = IconLoader.getInstance().getIcon("dialog.warning");
        
        form.addFormElementListener(this);
        
        panel = new JPanel();
        GroupLayout ly = new GroupLayout(panel);
        ly.setAutocreateGaps(true);
        panel.setLayout(ly);
        
        {   // horizontal
            SequentialGroup hgroup = ly.createSequentialGroup();

            // label column
            {
                int rowcount = 0;
                ParallelGroup column = ly.createParallelGroup();
                
                for (FormElement elem: form) {
                    JComponent[] components = elem.getComponents();
                    if (0<components.length && components[0] == elem.getLabel()) {
                        column.add(components[0]);
                        rowcount++;
                    }
                }
                if (rowcount > 0) {
                    hgroup.add(column);
                }
            }
            
            // remaining
            {
                int rowcount = 0;
                ParallelGroup column = ly.createParallelGroup();
                
                for (FormElement elem: form) {
                    JComponent[] components = elem.getComponents();
                    if (components.length == 1  && components[0] == elem.getMainComponent()) {
                        int al = horizontalAlignment(components[0]);
                        if (al != GroupLayout.CENTER)
                            column.add(al, components[0]);
                        else
                            column.add(components[0]);
                    } else {
                        SequentialGroup compact = ly.createSequentialGroup();
                        
                        for (int i=0;i<components.length;i++) {
                            if (i == 0 && components[i] == elem.getLabel()) {
                               // System.out.println("ignore: "+((JLabel)components[i]).getText());
                                continue; // ignore label
                            } else {
                                compact.add(components[i]);
                            } 
                        }
                        
                        column.add(compact);
                    }
                    rowcount++;
                }
                
                if (rowcount > 0) {
                    hgroup.add(column);
                } 
            }
            
            // warnings

            {
                int rowcount = 0;
                ParallelGroup column = ly.createParallelGroup();
                
                int index = 0;
                for (FormElement elem: form) {
                    if (elem.getValidator() == null) { // no validator
                        warningButtons.add(null);
                    } else {
                        JLabel lbl = new JLabel(warnIcon);
                        lbl.setAlignmentY(0.5f);
                        lbl.setVerticalTextPosition(SwingConstants.CENTER);
                        lbl.putClientProperty(FORMELEMENT_INDEX, index++);
                        warningButtons.add(lbl);
                        column.add(lbl, 16, 16, 16);
                        rowcount++;
                    }
                }
                
                index++;
                if (rowcount > 0) {
                    hgroup.add(column);
                } 
            }
            
            
            
            ly.setHorizontalGroup(hgroup);
        }
        
        {   // vertical
            SequentialGroup vgroup = ly.createSequentialGroup();
            
            int index = 0;
            for (FormElement elem: form) {
                ParallelGroup line = ly.createParallelGroup(GroupLayout.BASELINE);
                
                JComponent[] comp = elem.getComponents();
                for (int i=0;i<comp.length;i++) {
                    /*if (i == 0 && comp[i] instanceof JLabel) {
                        line.add(comp[i], GroupLayout.Alignment.LEADING);
                    } else if (i==1) {
                        line.add(comp[i], GroupLayout.Alignment.CENTER);
                    } else if (i==comp.length-1 && comp[i] instanceof JButton) {
                        line.add(comp[i], GroupLayout.Alignment.TRAILING);
                    } else {
                        line.add(comp[i]);
                    }*/
                    line.add(comp[i]);
                }
                
                if (index<warningButtons.size() && warningButtons.get(index)!=null) {
                    line.add(GroupLayout.CENTER, warningButtons.get(index));
                }
                
                vgroup.add(line);
                index++;
            }
            
            ly.setVerticalGroup(vgroup);
        }
        
        updateValidityIcons();
    }

    public void formelementValueChanged(FormElementEvent evt) {
        // no op
    }
    
    public void updateValidityIcons() {
        int index = 0;
        for (FormElement elem: form) {
            updateValidity(index, elem);
            index ++;
        }
    }

    public void formelementValueValidityChanged(FormElementEvent evt) {
        int index = form.indexOf(evt.getFormElement());
        updateValidity(index, evt.getFormElement());
    }
    
    private void updateValidity(int index, FormElement elem) {
        if (index>=0 && index<warningButtons.size()) {
            JLabel btn = warningButtons.get(index);
            if (btn != null) {
                btn.setVisible(validationHintsEnabled && !elem.checkIsValid());
            }
        }
    }

    public void actionPerformed(ActionEvent e) {
        // TODO Auto-generated method stub
        return;
    }
    
}
