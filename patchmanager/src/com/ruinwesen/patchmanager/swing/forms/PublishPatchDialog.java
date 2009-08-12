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
package com.ruinwesen.patchmanager.swing.forms;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSFileSelectionContext;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestStorePatch;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

public class PublishPatchDialog implements ActionListener {
    private static Log log = LogFactory.getLog(PublishPatchDialog.class);
    
    private static final String CMD_RB_USE_EXISTING_ACCOUNT = "CMD_RB_USE_EXISTING_ACCOUNT";
    private static final String CMD_RB_CREATE_ACCOUNT = "CMD_RB_CREATE_ACCOUNT";
    private static final String CMD_BTN_CONFIRM = "CMD_BTN_CONFIRM";
    private static final String CMD_BTN_CANCEL = "CMD_BTN_CANCEL";
    private static final String CMD_BTN_OPENPATCH = "CMD_BTN_OPENPATCH";

    private static final int DS_SELECT_PATCH_SOURCE = 0;
    private static final int DS_LOGIN = 1;
    private static final int DS_EDITPATCH = 2;
    private static final int DS_ASK_PUBLISH_PATCH = 3;
    
    private SwingPatchManager patchmanager;
    private JDialog dialog;
    private Component containerLogin;
    private NewLoginForm newLoginForm;
    private Component containerPatchEdit;
    private PatchEditForm patchEditForm;
    private JLabel lblMessageLabel;
    private JLabel lblStatusLabel;
    private CSAction acBtnConfirm;
    private CSAction acBtnCancel;
    private CSAction acBtnExtra;
    private JButton btnExtra;
    private int dialogState = DS_SELECT_PATCH_SOURCE;
    private Auth auth = null;
    private SelectPatchSourceDialog selectPatchSourceDialog;
    private File editSourcePatchFile = null;
    private Patch sourcePatch;
    private PublishSavePatchForm publishSavePatchForm;

    private Component currentDialogContainer = null;
    private File tmpPatchFile;
    
    public PublishPatchDialog(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
        init();
    }
    
    private void init() {
        dialog = new JDialog(patchmanager.getFrame(), "Publish Patch...");
        dialog.setResizable(true);
        dialog.setModal(true);
        dialog.setLocationByPlatform(true);
        dialog.setMinimumSize(new Dimension(320, 240));
        dialog.getContentPane().setLayout(new BorderLayout());
        
        lblMessageLabel = new JLabel("Message...");
        lblStatusLabel = new JLabel("Status.");

        JPanel topPanel = new JPanel(new BorderLayout());
        topPanel.add(lblMessageLabel, BorderLayout.CENTER);
        topPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));

        acBtnConfirm = new CSAction().useName("Next")
        .useActionListener(this)
        .useActionCommand(CMD_BTN_CONFIRM);
        acBtnCancel = new CSAction().useName("Cancel")
        .useActionListener(this)
        .useActionCommand(CMD_BTN_CANCEL);
        acBtnExtra = new CSAction().useName("Extra")
        .useActionListener(this);
        
        JPanel bottomPanel = new JPanel();
        bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.Y_AXIS));
        bottomPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
        bottomPanel.add(lblStatusLabel);
        Box bx = Box.createHorizontalBox();
        JButton btnConfirm = new JButton(acBtnConfirm);
        btnConfirm.setDefaultCapable(true);
        dialog.getRootPane().setDefaultButton(btnConfirm);
        btnExtra = new JButton(acBtnExtra);
        
        btnExtra.setVisible(false);
        
        bx.add(btnExtra);
        bx.add(Box.createHorizontalGlue());
        bx.add(new JButton(acBtnCancel));
        bx.add(btnConfirm);
        bottomPanel.add(bx);

        dialog.getContentPane().add(topPanel, BorderLayout.NORTH);
        dialog.getContentPane().add(bottomPanel, BorderLayout.SOUTH);

    }
    
    
    private void prepareSelectPatchSourceDialog() {
        if (selectPatchSourceDialog == null) {
            selectPatchSourceDialog = new SelectPatchSourceDialog();
        }
    }
    
    private void prepareLoginContainer() {
        
        if (containerLogin != null) {
            return;
        }
        JPanel panel = new JPanel() ;
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        
        newLoginForm = new NewLoginForm(patchmanager.getPatchManagerClient(), NewLoginForm.OPTION_CREATE_USER_OR_CANCEL);
        newLoginForm.setButtonsVisible(false);
        Auth auth = patchmanager.getUserAuthentication(false);
        if (auth != null) {
            newLoginForm.setAuth(auth);
        }
        

        JRadioButton rbCreateNewUser = 
            new JRadioButton(new CSAction().useName("Create New User")
                    .useActionListener(this)
                    .useActionCommand(CMD_RB_CREATE_ACCOUNT));

        JRadioButton rbLogin = 
            new JRadioButton(new CSAction().useName("Use existing Account")
                    .useActionListener(this)
                    .useActionCommand(CMD_RB_USE_EXISTING_ACCOUNT));
        
        ButtonGroup bg = new ButtonGroup();
        bg.add(rbCreateNewUser);
        bg.add(rbLogin);

        newLoginForm.setExtraInputControlsVisible(auth==null);
        if (auth!=null)
            rbLogin.setSelected(true);
        rbCreateNewUser.setSelected(!rbLogin.isSelected());

        rbCreateNewUser.setAlignmentX(Component.LEFT_ALIGNMENT);
        rbLogin.setAlignmentX(Component.LEFT_ALIGNMENT);
        newLoginForm.getForm().setAlignmentX(Component.LEFT_ALIGNMENT);
        
        Box rbBox = Box.createVerticalBox();
        rbBox.add(rbLogin);
        rbBox.add(rbCreateNewUser);
        panel.add(rbBox);
        
        panel.add(Box.createVerticalStrut(10));
        panel.add(newLoginForm.getForm());
        
        containerLogin = panel;
    }

    public void showDialog() {
        prepareSelectPatchSourceDialog();
        setDialogContainer(selectPatchSourceDialog.getForm());
        dialog.pack();
        
        dialog.setVisible(true);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getActionCommand() == CMD_RB_CREATE_ACCOUNT) {
            newLoginForm.setExtraInputControlsVisible(true);
        } else if (e.getActionCommand() == CMD_RB_USE_EXISTING_ACCOUNT) {
            newLoginForm.setExtraInputControlsVisible(false);
        } else if (e.getActionCommand() == CMD_BTN_CANCEL) {
            cancel();
        } else if (e.getActionCommand() == CMD_BTN_CONFIRM) {
            confirm();
        } else if (e.getActionCommand() == CMD_BTN_OPENPATCH) {
            JFileChooser fc = new JFileChooser();
            CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
            fc.setMultiSelectionEnabled(false);
            fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
            if (fc.showOpenDialog(dialog) == JFileChooser.APPROVE_OPTION) {
                CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
                try {
                    patchEditForm.setSourceFile(fc.getSelectedFile());
                } catch (Exception ex) {
                    if (log.isErrorEnabled()) {
                        log.error("set source file failed", ex);
                    }
                }
            }
        }
    }

    private void confirm() {
        switch (dialogState) {
        case DS_SELECT_PATCH_SOURCE: confirmOnSelectPatchSource(); break;
        case DS_EDITPATCH: confirmOnEditPatch(); break;
        case DS_ASK_PUBLISH_PATCH: confirmOnAskPublishPatch(); break;
        case DS_LOGIN: confirmOnLogin(); break;
        default:break;
        }
    }
    
    private void cancel() {
        dialog.setVisible(false);
        dialog.dispose();
    }
    
    private void preparePatchEditForm() {
        if (containerPatchEdit != null) {
            return;
        }
        patchEditForm = new PatchEditForm();
        patchEditForm.setButtonsVisible(false);
        containerPatchEdit = patchEditForm.getForm();
    }
    
    private void confirmOnSelectPatchSource() {
        File source = null;
        this.sourcePatch = null;
        if (!selectPatchSourceDialog.isCreateNewFileSelected()) {
            File file = selectPatchSourceDialog.getSelectedFile();
            if (file == null || !file.exists()) {
                SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(),
                        "File not found: "+file);
                return;
            }
            
            File selectedFile = file;
            if (file.getName().toLowerCase().equals(PatchMetadata.FILENAME)) {
                file = file.getParentFile(); // metadata file was selected
            }
            try {
                this.sourcePatch = new DefaultPatch(file);
                sourcePatch.getMetadata();
            } catch (Exception ex) {
                SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(),
                        "Patch metadata unavailable:"+selectedFile);
                return;
            }
            
            source = file;
        }
        
        preparePatchEditForm();

        if (source != null) {
            try {
                patchEditForm.setSourceFile(source);
            } catch (Exception ex) {
                SwingPatchManagerUtils.showErrorDialog(patchmanager.getFrame(),
                        "Could not set source file."+ex);
                return;
            }
        }
        
        patchEditForm.setAuthorFieldEditable(false);
        patchEditForm.setAuthorFieldEnabled(false);
        patchEditForm.setAuthor("- username -");
        
        if (sourcePatch != null) {
            patchEditForm.setPathFieldsEnabled(false);
            patchEditForm.setMidiFileSourceText("-");
            patchEditForm.setSourceDirText("-");
        }
        
        btnExtra.setVisible(false);
        acBtnExtra.setEnabled(false);
        acBtnExtra.useActionCommand(CMD_BTN_OPENPATCH);
        acBtnExtra.useName("Open Patch...");
        setDialogContainer(containerPatchEdit);
        dialog.pack();
        dialogState = DS_EDITPATCH;
    }
    
    private void preparePublishPatchQuestionForm() {
        if (publishSavePatchForm == null) {
            publishSavePatchForm = new PublishSavePatchForm();
        }
        setDialogContainer(publishSavePatchForm.getForm());
        dialog.pack();
        dialogState = DS_ASK_PUBLISH_PATCH;
    }
    
    private void setDialogContainer(Component c) {
        if (currentDialogContainer != null) {
            dialog.getContentPane().remove(currentDialogContainer);
            currentDialogContainer = null;
        }

        dialog.getContentPane().add(c, BorderLayout.CENTER);
        currentDialogContainer = c;
    }
    
    private void confirmOnEditPatch() {
        lblStatusLabel.setText("Generating patch...");
        File tmpFile;
        lblStatusLabel.setText("Creating patch...");
        
        if (sourcePatch != null) {
            
            PatchMetadata meta;
            try {
                meta = sourcePatch.getMetadata();
            } catch (Exception ex) {
                ex.printStackTrace();
                return ;
            }
            patchEditForm.extractFields(meta);
            
            try {
                tmpFile = SwingPatchManagerUtils.zipPatchTemporary(sourcePatch, meta);
            } catch (Exception ex) {
                ex.printStackTrace();
                return;
            }
        } else {
            try {
                tmpFile = File.createTempFile("patch-", ".tmp");
                tmpFile.deleteOnExit();
            } catch (IOException ex) {
                if (log.isErrorEnabled()) {
                    log.error("could not create temporary file", ex);
                }
                return;
            }

            try {
                patchEditForm.generatePatch(tmpFile);
            } catch (Exception ex) {
                tmpFile.delete();
                if (log.isErrorEnabled()) {
                    log.error("could not generate patch", ex);
                }
                return;
            }
        }

        this.tmpPatchFile = tmpFile;
        
        dialogState = DS_ASK_PUBLISH_PATCH;
        preparePublishPatchQuestionForm();
    }
    
    private void confirmOnAskPublishPatch() {
        if (!publishSavePatchForm.isPublishOptionSelected()) {
            cancel();
            return;
        }
        prepareLoginContainer();
        setDialogContainer(containerLogin);
        dialog.pack();
        dialog.repaint();
        dialogState = DS_LOGIN;
    }

    private void confirmOnLogin() {
        newLoginForm.btnOkClick(); // TODO create login
        
        this.auth = newLoginForm.getAuth();

        
        try {
            uploadPatch();
            tmpPatchFile.delete();
            cancel();
        } catch (Exception ex) {
            // TODO Auto-generated catch block
            ex.printStackTrace();
        }
        
    }
    
    private void uploadPatch() throws Exception {

        PatchMetadata meta = new DefaultPatch(tmpPatchFile).getMetadata();
        
        InputStream is = new BufferedInputStream(new FileInputStream(tmpPatchFile));
        try {
            Request request = new RequestStorePatch(auth, meta, is);
            Response response = patchmanager.getPatchManagerClient().execute(request);
            if (!response.isOkStatus()) {
                throw new Exception("error: "+response.getMessage());
            }
            
            patchmanager.syncRepository();
        } finally {
            is.close();
        }
    }
    
}
 