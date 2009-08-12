package com.ruinwesen.patchmanager.swing.forms;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.ButtonGroup;
import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.event.DocumentListener;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSEventAdapter;
import name.cs.csutils.CSFileSelectionContext;

public class SelectPatchSourceDialog extends Form {

    public static final String AC_NEW_PATCH = "new patch";
    public static final String AC_EDIT_PATCH = "edit patch";
    public static final String AC_SELECT_PATCH = "select patch file";
    
    private JPanel container;
    private CSAction acEditPatch;
    private CSAction acNewPatch;
    private CSAction acSelectPatchFile;
    private JRadioButton rbNewPatch;
    private JRadioButton rbEditPatch;
    private JTextField tfPatchFile;
    
    public SelectPatchSourceDialog() {
        super();
        init();
    }
    
    @Override
    public JComponent getForm() {
        return container;
    }
    
    private void init() {
        container = new JPanel();
        acNewPatch = new CSAction("Create New Patch")
        .useActionCommand(AC_NEW_PATCH);
        acEditPatch = new CSAction("Modify existing Patch")
        .useActionCommand(AC_EDIT_PATCH);
        acSelectPatchFile = new CSAction("File...")
        .useActionCommand(AC_SELECT_PATCH);
        acNewPatch.setSelected(true);
        rbNewPatch = new JRadioButton(acNewPatch);
        rbEditPatch = new JRadioButton(acEditPatch);
        ButtonGroup bg = new ButtonGroup();
        bg.add(rbNewPatch);
        bg.add(rbEditPatch);

        JLabel lblPatchFile = new JLabel("Patch");
        
        tfPatchFile = new JTextField();
        tfPatchFile.setColumns(20);
        JButton btnSelectPatchFile = new JButton(acSelectPatchFile);
        
        GroupLayout ly = new GroupLayout(container);
        container.setLayout(ly);
        
        ly.setAutoCreateGaps(true);
        ly.linkSize(SwingConstants.HORIZONTAL,
                rbNewPatch, rbEditPatch);
        
        ly.setHorizontalGroup(ly.createParallelGroup()
                .addComponent(rbNewPatch)
                .addComponent(rbEditPatch)
                .addGroup( ly.createSequentialGroup()
                        .addComponent( lblPatchFile)
                        .addComponent(tfPatchFile)
                        .addComponent(btnSelectPatchFile)
                )
        );
        
        ly.setVerticalGroup(ly.createSequentialGroup()
                .addComponent(rbNewPatch)
                .addComponent(rbEditPatch)
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblPatchFile)
                        .addComponent(tfPatchFile)
                        .addComponent(btnSelectPatchFile)          
                )
        );
        patchCreateModeChanged();
        new EventHandler().install();
    }
    
    private void patchCreateModeChanged() {
       boolean newpatch = acNewPatch.isSelected();
       tfPatchFile.setEditable(!newpatch);
       tfPatchFile.setEnabled(!newpatch);
       acSelectPatchFile.setEnabled(!newpatch);
    }
    
    public File getSelectedFile() {
        String str = tfPatchFile.getText().trim();
        return str == null || str.trim().isEmpty() ? null : new File(str);
    }
    
    public boolean isCreateNewFileSelected() {
        return acNewPatch.isSelected();
    }
    
    public static void main(String[] args) {
        new SelectPatchSourceDialog().showDialog(null, "");
    }
    
    private class EventHandler extends CSEventAdapter
    implements ActionListener, DocumentListener {
        public void install() {
            acSelectPatchFile.useActionListener(this);
            acEditPatch.useActionListener(this);
            acNewPatch.useActionListener(this);
        }
        @Override
        public void actionPerformed(ActionEvent e) {
            if (AC_EDIT_PATCH.equals(e.getActionCommand())) {
                patchCreateModeChanged();
            } else if (AC_NEW_PATCH.equals(e.getActionCommand())) {
                patchCreateModeChanged();
            } else if (AC_SELECT_PATCH.equals(e.getActionCommand())) {

                JFileChooser fc = new JFileChooser();
                fc.setMultiSelectionEnabled(false);
                fc.setAcceptAllFileFilterUsed(true);
                fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
                CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
                fc.setSelectedFile(getSelectedFile());
                
                if (fc.showOpenDialog(container) == JFileChooser.APPROVE_OPTION) {
                    CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
                    File file = fc.getSelectedFile();
                    tfPatchFile.setText(file.getAbsolutePath());
                }
            }
        }
    }
    
}
