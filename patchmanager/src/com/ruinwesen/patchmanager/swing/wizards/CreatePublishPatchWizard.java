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
package com.ruinwesen.patchmanager.swing.wizards;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.FileFilterFactory;

import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarFileBuilder;
import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataUtils;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestStorePatch;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;
import com.ruinwesen.patchmanager.swing.form.CustomFormElement;
import com.ruinwesen.patchmanager.swing.form.Form;
import com.ruinwesen.patchmanager.swing.form.FormContainer;
import com.ruinwesen.patchmanager.swing.form.Wizard;
import com.ruinwesen.patchmanager.swing.forms2.LoginForm;
import com.ruinwesen.patchmanager.swing.forms2.PatchMetadataForm;

public class CreatePublishPatchWizard extends Wizard {

    private static Log log = LogFactory.getLog(CreatePublishPatchWizard.class);
    
    private Auth auth;
    private LoginForm loginForm;
    private PatchMetadataForm patchMetadataForm;
    private SwingPatchManager patchmanager;
    private SavePatchQuestion savePatchQuestion;
    private File fileSavedAs = null; 
    
    public CreatePublishPatchWizard(SwingPatchManager patchmanager) {
        super();
        this.patchmanager = patchmanager;
        init();
    }
    
    private void init() {
        patchMetadataForm = new PatchMetadataForm();
        auth = patchmanager == null ? null : patchmanager.getUserAuthentication(false);
        loginForm = new LoginForm(true);
        if (auth != null)
            loginForm.setFields(auth);

        patchMetadataForm.feAuthor.setText(auth != null ? auth.getUsername() : null);
        
        savePatchQuestion = new SavePatchQuestion();
        add(patchMetadataForm);
        add(savePatchQuestion);
        add(loginForm);
    }

    protected void currentFormSelected(FormContainer currentform) {
        if (currentform == savePatchQuestion) {
            savePatchQuestion.update();
        }
    }

    @Override
    protected boolean action_next(FormContainer currentform) {
        if (currentform == savePatchQuestion) {
            if (savePatchQuestion.cbAskSavePatchfile.isSelected()) {
                boolean ok = save_patch_local();
                if (!ok) {
                    return false;
                }
            }
        }
        
        return true;
    }

    @Override
    protected void action_finish() {
        if (getCurrentForm() == savePatchQuestion) {
            if (savePatchQuestion.cbAskSavePatchfile.isSelected()) {
                boolean ok = save_patch_local();
                if (!ok) { return ; }
            }
            
            if (savePatchQuestion.cbAskPublishPatchfile.isSelected()) {
                if (!tryUploadPatch()) {
                    return;
                }
            }
            
        }
        
        closeDialog();
    }
    
    protected boolean tryUploadPatch() {
        try {
            uploadPatch();
            return true;
        } catch (Exception ex) {
            if(log.isErrorEnabled()) {
                log.error("could not upload patch", ex);
            }
            SwingPatchManagerUtils.showErrorDialog(getContainer(), "Could not upload patch:\n"+ex.getMessage());
            return false;
        }
    }
    
    private void uploadPatch() throws Exception {
        PatchMetadata meta = new DefaultPatchMetadata();
        patchMetadataForm.copyMetadataTo(meta);
        if (fileSavedAs != null)
            meta.setName(fileSavedAs.getName());
        meta.setAuthor(auth.getUsername());
        

        File tmpPatchFile = File.createTempFile("patch-upload", ".tmp");
        tmpPatchFile.deleteOnExit();
        try {
            buildPatchFile(meta, tmpPatchFile);
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
        } finally {
            tmpPatchFile.delete();
        }
        
    }
    
    private boolean save_patch_local() {
        JFileChooser fc = new JFileChooser();
        
        SwingPatchManagerUtils.setSaveRWPFile(fc);
        
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        if (SwingPatchManagerUtils.showFileChooserLoop(getContainer(), fc) != JOptionPane.YES_OPTION) {
            return false;
        }
        CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
        File file = fc.getSelectedFile();
        this.fileSavedAs = file;
        
        PatchMetadata meta = new DefaultPatchMetadata();
        patchMetadataForm.copyMetadataTo(meta);
        meta.setName(file.getName());
        
        try {
            buildPatchFile(meta, file);
            return true;
        } catch (Exception ex) {
            if(log.isErrorEnabled()) {
                log.error("could not save patch locally", ex);
            }
            file.delete(); // delete corrupted file
            SwingPatchManagerUtils.showErrorDialog(getContainer(), "Could not save patch:\n"+ex.getMessage());
            return true;
        }
    }
    
    protected void buildPatchFile(PatchMetadata meta, File dst) throws Exception {
        
        File sourceCodeFile = patchMetadataForm.feSourceDir.getFile();
        File mididataFile = patchMetadataForm.feMidiFile.getFile();
        FileFilter sourceDirFilter = 
            FileFilterFactory.or(FileFilterFactory.DirectoriesOnly(), 
                    FileFilterFactory.parseFilterList(patchMetadataForm.feSourceFileFilter.getText()));
        
        
        List<Path> paths = new LinkedList<Path>();
        JarFileBuilder jarbuilder = new JarFileBuilder(dst);
        
        if (mididataFile != null) {
            jarbuilder.add("mididata.hex", new FileInputStream(mididataFile));
            paths.add(new Path(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME, "mididata.hex"));
        }
        if (sourceCodeFile != null) {
            jarbuilder.add("source", new FSDirectory(sourceCodeFile, sourceDirFilter));
            paths.add(new Path(PatchMetadata.DEFAULT_SOURCE_PATH_NAME, "source"));
        }
        meta.setPaths(paths);
        jarbuilder.add(PatchMetadata.FILENAME, PatchMetadataUtils.createXMLInputStream(meta));
        jarbuilder.write();
        jarbuilder.close();
    }

    public static void main(String[] args) {
        
        JFrame f = new JFrame();
        f.getContentPane().add(new CreatePublishPatchWizard(null).getContainer());
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setBounds(0,0,640,480);
        f.setVisible(true);
        
    }
    
    private final class SavePatchQuestion extends FormContainer implements ActionListener {
        JCheckBox cbAskSavePatchfile;
        JCheckBox cbAskPublishPatchfile;
        public SavePatchQuestion() {
            super();            
            Form form = new Form("savepatchquestion");
            form.add(new CustomFormElement(new JLabel("I wish to...")));
            cbAskSavePatchfile = new JCheckBox();
            cbAskSavePatchfile.setSelected(true);
            cbAskPublishPatchfile = new JCheckBox();
            cbAskPublishPatchfile.setSelected(false);
            cbAskSavePatchfile.setText("save the patch file locally.");
            cbAskPublishPatchfile.setText("publish the patch file.");
            form.add(new CustomFormElement(cbAskSavePatchfile));
            form.add(new CustomFormElement(cbAskPublishPatchfile));
            setForm(form);
            cbAskSavePatchfile.addActionListener(this);
            cbAskPublishPatchfile.addActionListener(this);
        }
        @Override
        public void actionPerformed(ActionEvent e) {
            update();
        }
        
        public void update() {

            boolean requiresNewUserAccount = 
                cbAskPublishPatchfile.isSelected()
                && auth == null;
            
            setNextCapable(requiresNewUserAccount);
            setFinishCapable((!requiresNewUserAccount)
                    && 
                    (cbAskPublishPatchfile.isSelected()
                            || cbAskSavePatchfile.isSelected())
            );
        }
    }
    
}
