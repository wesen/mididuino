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

import java.awt.CardLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.util.ArrayList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.SwingConstants;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSEventAdapter;

public class Wizard implements ActionListener, FormElementListener {

    public static final String AC_NEXT = "wizard.action.next";
    public static final String AC_FINISH = "wizard.action.finish";
    public static final String AC_CANCEL = "wizard.action.cancel";
    
    private List<FormContainer> forms = new ArrayList<FormContainer>();
    private CardLayout cardlayout;
    private JPanel container;
    private JPanel formContainer;

    private CSAction acNext;
    private CSAction acFinish;
    private CSAction acCancel;
    private JLabel labelErrorHint;
    
    private FormContainer currentForm;
    private JDialog dialog;
    private boolean finishCapable = false;
    private boolean nextCapable = true;
    
    public Wizard() {
        super();
        init();
    }

    public void setNextCapable(boolean value) {
        nextCapable = value;
        updateOptions();
    }

    public void setFinishCapable(boolean value) {
        finishCapable = value;
        updateOptions();
    }
    
    public JDialog getDialog() {
        return dialog;
    }
    
    public void showDialog(JFrame parentComponent) {
        JDialog dialog = new JDialog(parentComponent);
        this.dialog = dialog;
        
        dialog.addWindowListener(new CSEventAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                action_cancel();
            }
        });
        dialog.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        dialog.getContentPane().add(getContainer());
        dialog.pack();
        dialog.setLocationByPlatform(true);
        dialog.setModal(true);
        dialog.setVisible(true);
        dialog.dispose();
        this.dialog = null;
    }
    
    private void setCurrentForm(FormContainer form) {
        FormContainer oldForm = this.currentForm;
        if (oldForm != form) {
            this.currentForm = form;
            currentFormSelected(form);
            show(form.getName());
            updateOptions();
        }
    }

    protected void currentFormSelected(FormContainer form) {
        // no op
    }

    public FormContainer getCurrentForm() {
        return currentForm;
    }
    
    public JComponent getContainer() {
        return container;
    }
    
    public void add(FormContainer form) {
        forms.add(form);
        form.getForm().addFormElementListener(this);
        formContainer.add(form.getContainer(), form.getName());
        if (forms.size()==1) { // first form
            setCurrentForm(form);
        }
    }
    
    private void init() {
        cardlayout = new CardLayout();
        formContainer = new JPanel(cardlayout);

        acNext = new CSAction("Next >")
        .useActionCommand(AC_NEXT)
        .useResourceKey(AC_NEXT)
        .useActionListener(this);

        acFinish = new CSAction("Finish")
        .useActionCommand(AC_FINISH)
        .useResourceKey(AC_FINISH)
        .useActionListener(this);

        acCancel = new CSAction("Cancel")
        .useActionCommand(AC_CANCEL)
        .useResourceKey(AC_CANCEL)
        .useActionListener(this);

        JButton btnNext = new JButton(acNext);
        JButton btnFinish = new JButton(acFinish);
        JButton btnCancel = new JButton(acCancel);
        
        labelErrorHint = new JLabel("aaaaaaaaa ");

        JSeparator ehintSeparator = new JSeparator(SwingConstants.HORIZONTAL);
        
        JSeparator buttonSeparator = new JSeparator(SwingConstants.HORIZONTAL);
        
        container = new JPanel();
        GroupLayout ly = new GroupLayout(container);        
        container.setLayout(ly);
        
        ly.setAutoCreateContainerGaps(true);
        ly.setAutoCreateGaps(true);
        
        ly.setHorizontalGroup(ly.createParallelGroup()
                        .addComponent(formContainer)       
                        .addComponent(ehintSeparator)   
                        .addComponent(labelErrorHint)   
                        .addComponent(buttonSeparator)   
                        .addGroup(GroupLayout.Alignment.TRAILING, ly.createSequentialGroup()
                                .addComponent(btnNext)
                                .addComponent(btnFinish)
                                .addComponent(btnCancel)
                        )
        );
        
        ly.setVerticalGroup(ly.createSequentialGroup()
                        .addComponent(formContainer)   
                        .addComponent(ehintSeparator, GroupLayout.PREFERRED_SIZE
                                ,GroupLayout.PREFERRED_SIZE,GroupLayout.PREFERRED_SIZE)
                        .addComponent(labelErrorHint)   
                        .addComponent(buttonSeparator, GroupLayout.PREFERRED_SIZE
                                        ,GroupLayout.PREFERRED_SIZE,GroupLayout.PREFERRED_SIZE)
                        .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                                .addComponent(btnNext)
                                .addComponent(btnFinish)
                                .addComponent(btnCancel)
                        )  
        );
        
        ly.linkSize(btnNext, btnFinish, btnCancel);
    }
    
    public void show(String name) {
        cardlayout.show(formContainer, name);
        for (FormContainer form: forms)
            if (form.getName().equals(name)) {
                setCurrentForm(form);
                return;
            }
        
        setCurrentForm(null);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (AC_NEXT.equals(e.getActionCommand())) {
            action_next();
        } else if (AC_FINISH.equals(e.getActionCommand())) {
            action_finish();
        } else if (AC_CANCEL.equals(e.getActionCommand())) {
            action_cancel();
        } 
    }

    protected void action_next() {
        if (!acNext.isEnabled()) {
            return;
        }   
        
        FormContainer next = getNextForm();
        if (action_next(getCurrentForm())) {
            setCurrentForm(next);
        }
    }
    
    protected boolean action_next(FormContainer currentform) {
        return true;
    }

    protected void action_finish() {
        if (!acFinish.isEnabled()) {
            return;
        }
    }

    protected void action_cancel() {
        if (!acCancel.isEnabled()) {
            return;
        }

        if (JOptionPane.showConfirmDialog(getContainer(), 
                "Are you sure you want to cancel ?",
                "Cancel",
                JOptionPane.YES_NO_OPTION
        ) == JOptionPane.YES_OPTION) {
            closeDialog();
        }
    }
    
    protected void closeDialog() {
        JDialog dlg = getDialog();
        if (dlg != null) {
            dlg.setVisible(false);
        }
    }

    @Override
    public void formelementValueChanged(FormElementEvent evt) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void formelementValueValidityChanged(FormElementEvent evt) {
        updateOptions();
        labelErrorHint.revalidate();
        labelErrorHint.repaint();
    }
    
    private FormContainer getNextForm() {
        FormContainer current = getCurrentForm();
        boolean returnNext = false;
        for (FormContainer pos: forms) {
            if (pos == current) {
                returnNext = true;
            } else if (returnNext) {
                return pos;
            }
        }
        return null;
    }
    
    protected void updateOptions() {
        FormContainer form = getCurrentForm();
        if (form == null) {
            acNext.setEnabled(false);
            acFinish.setEnabled(false);
            labelErrorHint.setText(" ");
        } else {
            boolean valid = form.getForm().isValid();
            acNext.setEnabled(nextCapable && valid && getNextForm()!=null);
            acFinish.setEnabled(finishCapable && valid);
            
            if (valid) {
                labelErrorHint.setText(" ");
                
            } else {
                String msg = null;
                try {
                    form.getForm().validate();
                } catch (ValidationError ex) {
                    msg = ex.getMessage();
                }
                if (msg == null) {
                    msg = "Please check input";
                }
                labelErrorHint.setText(msg);
            }
        }
    }
    
}
