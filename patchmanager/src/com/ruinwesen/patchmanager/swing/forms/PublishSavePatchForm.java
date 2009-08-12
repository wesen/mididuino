package com.ruinwesen.patchmanager.swing.forms;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

import name.cs.csutils.CSAction;

public class PublishSavePatchForm extends Form implements ActionListener {

    public static final String AC_PUBLISH = "publish";
    public static final String AC_SAVEAS = "saveas";
    private JPanel container;
    
    private File file;
    private CSAction acPublish;
    private CSAction acSaveAs;
    private JCheckBox cbPublish;
    
    public PublishSavePatchForm() {
        super();
        init();
    }
    
    public void setFile(File file) {
        this.file = file;
    }
   
    private void init() {
        acSaveAs = new CSAction("Save patch as...").useActionCommand(AC_SAVEAS)
        .useActionListener(this);
        acPublish = new CSAction("Publish patch").useActionCommand(AC_PUBLISH)
        .useActionListener(this);
   
        cbPublish = new JCheckBox(acPublish);
        
        container = new JPanel(new GridLayout(0,1,4,4));
        container.add(new JButton(acSaveAs));
        container.add(cbPublish);
        
    }
    
    @Override
    public Component getForm() {
        return container;
    }

    public boolean isPublishOptionSelected() {
        return cbPublish.isSelected();
    }
    
    @Override
    public void actionPerformed(ActionEvent e) {
        if (AC_PUBLISH.equals(e.getActionCommand())) {
            // no op
        } else if (AC_SAVEAS.equals(e.getActionCommand())) {
            doSaveAs();
        }
    }
    
    private void setActionsEnabled(boolean value) {
        acPublish.setEnabled(value);
        acSaveAs.setEnabled(value);
    }

    private void doSaveAs() {
        
    }

}
