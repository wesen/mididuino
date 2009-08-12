package com.ruinwesen.patchmanager.swing.forms;

import java.awt.Component;

import javax.swing.JPanel;
import javax.swing.JRadioButton;

import com.ruinwesen.patchmanager.swing.SwingPatchManager;

public class CreatePatchDialog extends Form {

    private SwingPatchManager patchmanager;
    
    private JPanel containerSelectSource;

    public CreatePatchDialog(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
    }
    
    @Override
    public Component getForm() {
        // TODO Auto-generated method stub
        return null;
    }

    private void initSelectSourceDialog() {
        containerSelectSource = new JPanel();
        JRadioButton rbCreateNewPatch = new JRadioButton("Create New Patch");
        JRadioButton rbFromExistingPatch = new JRadioButton("Existing Patch");
    }
    
}
