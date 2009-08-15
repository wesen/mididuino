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
import java.util.ArrayList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.GroupLayout.Group;
import javax.swing.GroupLayout.ParallelGroup;
import javax.swing.GroupLayout.SequentialGroup;

import name.cs.csutils.IconLoader;

public class FormContainer implements FormElementListener {

    public static final String FORMELEMENT_INDEX = "FORMELEMENT_INDEX";
    
    private Form form;
    private JPanel panel;
    private List<JLabel> warningButtons = new ArrayList<JLabel>(20);
    
    protected FormContainer() {
        super();
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

    private GroupLayout.Alignment horizontalAlignment(Component c) {
        float a = c.getAlignmentX();
        if (a == Component.LEFT_ALIGNMENT) {
            return GroupLayout.Alignment.LEADING;
        } else if (a ==  Component.RIGHT_ALIGNMENT) {
            return GroupLayout.Alignment.TRAILING;
        } else if (a == Component.CENTER_ALIGNMENT) {
            return null;//GroupLayout.Alignment.CENTER;
        } else {
            return null;
        }
    }
    
    protected void buildContainer() {
        
        ImageIcon warnIcon = IconLoader.getInstance().getIcon("dialog.warning");
        
        form.addFormElementListener(this);
        
        panel = new JPanel();
        GroupLayout ly = new GroupLayout(panel);
        ly.setAutoCreateGaps(true);
        panel.setLayout(ly);
        
        {   // horizontal
            Group hgroup = ly.createSequentialGroup();

            // label column
            {
                int rowcount = 0;
                ParallelGroup column = ly.createParallelGroup();
                
                for (FormElement elem: form) {
                    JComponent[] components = elem.getComponents();
                    if (0<components.length && components[0] == elem.getLabel()) {
                        column.addComponent(components[0]);
                        rowcount++;
                    }
                }
                if (rowcount > 0) {
                    hgroup.addGroup(column);
                }
            }
            
            // remaining
            {
                int rowcount = 0;
                ParallelGroup column = ly.createParallelGroup();
                
                for (FormElement elem: form) {
                    JComponent[] components = elem.getComponents();
                    if (components.length == 1  && components[0] == elem.getMainComponent()) {
                        GroupLayout.Alignment al = horizontalAlignment(components[0]);
                        if (al != null)
                            column.addComponent(components[0], al);
                        else
                            column.addComponent(components[0]);
                    } else {
                        SequentialGroup compact = ly.createSequentialGroup();
                        
                        for (int i=0;i<components.length;i++) {
                            if (i == 0 && components[i] == elem.getLabel()) {
                               // System.out.println("ignore: "+((JLabel)components[i]).getText());
                                continue; // ignore label
                            } else {
                                compact.addComponent(components[i]);
                            } 
                        }
                        
                        column.addGroup(compact);
                    }
                    rowcount++;
                }
                
                if (rowcount > 0) {
                    hgroup.addGroup(column);
                } 
            }
            
            // warnings

            {
                int rowcount = 0;
                Group column = ly.createParallelGroup();
                
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
                        column.addComponent(lbl, 16, 16, 16);
                        rowcount++;
                    }
                }
                
                index++;
                if (rowcount > 0) {
                    hgroup.addGroup(column);
                } 
            }
            
            
            
            ly.setHorizontalGroup(hgroup);
        }
        
        {   // vertical
            Group vgroup = ly.createSequentialGroup();
            
            int index = 0;
            for (FormElement elem: form) {
                ParallelGroup line = ly.createParallelGroup(GroupLayout.Alignment.BASELINE);
                
                JComponent[] comp = elem.getComponents();
                for (int i=0;i<comp.length;i++) {
                    /*if (i == 0 && comp[i] instanceof JLabel) {
                        line.addComponent(comp[i], GroupLayout.Alignment.LEADING);
                    } else if (i==1) {
                        line.addComponent(comp[i], GroupLayout.Alignment.CENTER);
                    } else if (i==comp.length-1 && comp[i] instanceof JButton) {
                        line.addComponent(comp[i], GroupLayout.Alignment.TRAILING);
                    } else {
                        line.addComponent(comp[i]);
                    }*/
                    line.addComponent(comp[i]);
                }
                
                if (index<warningButtons.size() && warningButtons.get(index)!=null) {
                    line.addComponent(warningButtons.get(index), GroupLayout.Alignment.CENTER);
                }
                
                vgroup.addGroup(line);
                index++;
            }
            
            ly.setVerticalGroup(vgroup);
        }
        
        updateValidityIcons();
    }

    @Override
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

    @Override
    public void formelementValueValidityChanged(FormElementEvent evt) {
        int index = form.indexOf(evt.getFormElement());
        updateValidity(index, evt.getFormElement());
    }
    
    private void updateValidity(int index, FormElement elem) {
        if (index>=0 && index<warningButtons.size()) {
            JLabel btn = warningButtons.get(index);
            if (btn != null) {
                btn.setVisible(!elem.checkIsValid());
            }
        }
    }
    
}
