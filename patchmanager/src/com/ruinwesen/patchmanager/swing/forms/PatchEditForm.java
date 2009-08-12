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

import java.awt.Component;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactoryConfigurationError;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.FileFilterFactory;
import name.cs.csutils.SwingActionData;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarFileBuilder;
import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo;
import com.ruinwesen.patch.metadata.PatchMetadataUtils;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;
import com.ruinwesen.patch.utils.HexFileValidator;

public class PatchEditForm extends Form {
    private static Log log = LogFactory.getLog(PatchEditForm.class);
    public static final int CANCEL_OPTION = 0;
    public static final int SAVE_AS_OPTION = 1;
    public static final int PUBLISH_OPTION = 2;

    private JPanel pane;
    private JTextField tfAuthor;
    private JTextField tfTitle;
    private JComboBox cbDeviceId;
    private JComboBox cbEnvironmentId;
    private JTextArea taComment;
    private JScrollPane scrollComment;
    private JTextArea taTags;
    private JScrollPane scrollTags;
    private JTextField tfMidiFile;
    private JTextField tfSourceDir;
    private JTextField tfSourceDirFileFilter;

    private JButton btnPublish;
    private JButton btnSaveAs;
    private JButton btnCancel;

    private JButton btnSelectMidiFile;
    private JButton btnSelectSourceDir;
    private int option = CANCEL_OPTION;
    
    public PatchEditForm() {
        init();
    }
    
    @Override
    public Component getForm() {
        return pane;
    }
    
    public int getSelectedOption() {
        return option;
    }
    
    public void showDialog(Component parentComponent) {
        showDialog(parentComponent, "Create Patch");
    }

    public void setButtonsVisible(boolean value) {
        btnPublish.setVisible(value);
        btnSaveAs.setVisible(value);
        btnCancel.setVisible(value);
    }
    
    private void init() {
        pane = new JPanel();
        GroupLayout ly = new GroupLayout(pane);
        pane.setLayout(ly);
        
        // create components
        tfAuthor = new JTextField();
        tfTitle = new JTextField();
        cbDeviceId = new JComboBox(PatchMetadataIDInfo.DEVICE_ID_LIST.toArray());
        cbEnvironmentId = new JComboBox(PatchMetadataIDInfo.ENVIRONMENT_ID_LIST.toArray());
        taComment = new JTextArea();
        scrollComment = new JScrollPane(taComment);
        taTags = new JTextArea();
        taTags.setWrapStyleWord(true);
        taComment.setWrapStyleWord(true);
        scrollTags = new JScrollPane(taTags);
        tfMidiFile = new JTextField();
        tfSourceDir = new JTextField();
        tfSourceDirFileFilter = new JTextField("*.c,*.h");

        tfTitle.setColumns(20);
        tfAuthor.setColumns(20);
        tfMidiFile.setColumns(20);
        tfSourceDir.setColumns(20);
        
        taComment.setLineWrap(true);
        taTags.setLineWrap(true);
        
        btnPublish = new JButton(new CSAction(this, "btnPublishClick"));
        btnSaveAs = new JButton(new CSAction(this, "btnSaveAsClick"));
        btnCancel = new JButton(new CSAction(this, "btnCancelClick"));
        btnSelectMidiFile = new JButton(new CSAction(this, "btnSelectMidiFileClick"));
        btnSelectSourceDir = new JButton(new CSAction(this, "btnSelectSourceDirClick"));
        
        JLabel lblAuthor = new JLabel("Author");
        JLabel lblTitle = new JLabel("Title");
        JLabel lblDeviceId = new JLabel("Device");
        JLabel lblEnvironmentId = new JLabel("Environment");
        JLabel lblComment = new JLabel("Comment");
        JLabel lblTags = new JLabel("Tags");
        JLabel lblMidiFile = new JLabel("Midi File");
        JLabel lblSourceDir = new JLabel("Source Code");
        JLabel lblSourceDirFilter = new JLabel("Source File Filter");
        
        
        
        ly.setAutoCreateGaps(true);
        ly.setAutoCreateContainerGaps(true);
        
        ly.setHorizontalGroup(
                ly.createSequentialGroup()
                .addGroup(ly.createParallelGroup()
                        .addComponent(lblAuthor)
                        .addComponent(lblTitle)
                        .addComponent(lblDeviceId)
                        .addComponent(lblEnvironmentId)
                        .addComponent(lblComment)
                        .addComponent(lblTags)
                        .addComponent(lblMidiFile)
                        .addComponent(lblSourceDir)
                )
                .addGroup(ly.createParallelGroup()
                        .addComponent(tfAuthor)
                        .addComponent(tfTitle)
                        .addComponent(cbDeviceId)
                        .addComponent(cbEnvironmentId)
                        .addComponent(scrollComment)
                        .addComponent(scrollTags)
                        .addGroup(ly.createSequentialGroup()
                                .addComponent(tfMidiFile)
                                .addComponent(btnSelectMidiFile)
                        )
                        .addGroup(ly.createSequentialGroup()
                                .addComponent(tfSourceDir)
                                .addComponent(btnSelectSourceDir)
                        )
                        .addGroup(ly.createSequentialGroup()
                                .addComponent(lblSourceDirFilter)
                                .addComponent(tfSourceDirFileFilter)
                         )
                        
                        .addGroup(GroupLayout.Alignment.TRAILING, ly.createSequentialGroup()
                                .addComponent(btnPublish)
                                .addComponent(btnSaveAs)
                                .addComponent(btnCancel)   
                        )
                )
        );
        
        ly.setVerticalGroup(
                ly.createSequentialGroup()
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblAuthor)
                        .addComponent(tfAuthor)
                )
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblTitle)
                        .addComponent(tfTitle)
                )  
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblDeviceId)
                        .addComponent(cbDeviceId)
                )  
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblEnvironmentId)
                        .addComponent(cbEnvironmentId)
                )  
                .addGroup(ly.createParallelGroup()
                        .addComponent(lblComment)
                        .addComponent(scrollComment, GroupLayout.PREFERRED_SIZE, 80, Short.MAX_VALUE)
                )  
                .addGroup(ly.createParallelGroup()
                        .addComponent(lblTags)
                        .addComponent(scrollTags, GroupLayout.PREFERRED_SIZE, 80, Short.MAX_VALUE)
                )   
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblMidiFile)
                        .addComponent(tfMidiFile)
                        .addComponent(btnSelectMidiFile)
                )   
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblSourceDir)
                        .addComponent(tfSourceDir)
                        .addComponent(btnSelectSourceDir)
                )   
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblSourceDirFilter)
                        .addComponent(tfSourceDirFileFilter)
                )   
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(btnPublish)
                        .addComponent(btnSaveAs)
                        .addComponent(btnCancel)
                )
        );
        ly.linkSize(SwingConstants.HORIZONTAL, btnSelectMidiFile, btnSelectSourceDir);
        ly.linkSize(SwingConstants.HORIZONTAL, btnPublish, btnSaveAs, btnCancel);
        
    }

    @SwingActionData("File")
    public void btnSelectMidiFileClick() {
        JFileChooser fc = new JFileChooser();
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        fc.setMultiSelectionEnabled(false);
        if (fc.showOpenDialog(pane) == JFileChooser.APPROVE_OPTION) {
            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            tfMidiFile.setText(fc.getSelectedFile().getAbsolutePath());
        }
    }

    @SwingActionData("Dir")
    public void btnSelectSourceDirClick() {
        JFileChooser fc = new JFileChooser();
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        fc.setMultiSelectionEnabled(false);
        if (fc.showOpenDialog(pane) == JFileChooser.APPROVE_OPTION) {
            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            tfSourceDir.setText(fc.getSelectedFile().getAbsolutePath());
        }
    }
    
    @SwingActionData("Publish...")
    public void btnPublishClick() {
        option = PUBLISH_OPTION;
        closeDialog();
    }

    @SwingActionData("Cancel")
    public void btnCancelClick() {
        option = CANCEL_OPTION;
        closeDialog();
    }

    @SwingActionData("Save as...")
    public void btnSaveAsClick() {
        option = SAVE_AS_OPTION;
        closeDialog();
    }
    
    public void setFields(PatchMetadata meta, File sourceFile) {
        if (tfAuthor.isEditable())
            tfAuthor.setText(meta.getAuthor());
        
        tfTitle.setText(meta.getTitle());
        cbDeviceId.setSelectedItem(meta.getDeviceId());
        cbEnvironmentId.setSelectedItem(meta.getEnvironmentId());
        taComment.setText(meta.getComment());
        taTags.setText(meta.getTags().toString());

        tfMidiFile.setText("");
        tfSourceDir.setText("");
        {
            Path path = meta.getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME);
            if (path != null) {
                String name = path.getName();
                if (sourceFile != null) {
                    name = sourceFile.getAbsolutePath()+"/"+name;
                }
                tfMidiFile.setText(name);
            }
        }
        {
            Path path = meta.getPath(PatchMetadata.DEFAULT_SOURCE_PATH_NAME);
            if (path != null) {
                String name = path.getName();
                if (sourceFile != null) {
                    name = sourceFile.getAbsolutePath()+"/"+name;
                }
                tfSourceDir.setText(name);
            }
        }
        
    }
    
    public void extractFields(PatchMetadata dst) {
        dst.setAuthor(tfAuthor.getText());
        dst.setTitle(tfTitle.getText());
        dst.setDeviceId((DeviceId)cbDeviceId.getSelectedItem());
        dst.setEnvironmentId((EnvironmentId)cbEnvironmentId.getSelectedItem());
        dst.setComment(taComment.getText());
        dst.setTags(Tagset.parseTags(taTags.getText()));
    }
    
    public void savePatchAs() {
        JFileChooser fc = new JFileChooser();
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        fc.setMultiSelectionEnabled(false);
        if (fc.showOpenDialog(pane) == JFileChooser.APPROVE_OPTION) {
            File fdst = fc.getSelectedFile();
            if (fdst.exists() && JOptionPane.showConfirmDialog(pane, "Replace existing file: "+fdst.getAbsolutePath())
                    != JOptionPane.YES_OPTION) {
                return;
            }
            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            try {
            generatePatch(fdst);
            } catch (Exception ex) {
                if (log.isErrorEnabled()){
                    log.error("could not gnerate patch", ex);
                }
            }
        }
    }
    
    public PatchMetadata generatePatch(File file) throws IOException, ParserConfigurationException, TransformerFactoryConfigurationError, TransformerException {
        OutputStream os = new FileOutputStream(file);
        PatchMetadata meta = generatePatch(os);
        os.flush();
        os.close();
        return meta;
    }
    
    public PatchMetadata generatePatch(OutputStream out) throws IOException, ParserConfigurationException, TransformerFactoryConfigurationError, TransformerException {
        String fname;
        File midiFile = null;
        fname = tfMidiFile.getText();
        if (!fname.isEmpty()) {
            midiFile = new File(fname);
            if (!midiFile.isFile()) {
                throw new FileNotFoundException("not a file: "+midiFile.getAbsolutePath());
            }
        }
        FileFilter dirFilter = null;
        File srcDir = null;
        fname = tfSourceDir.getText();
        if (!fname.isEmpty()) {
            srcDir = new File(fname);
            if (!srcDir.isDirectory()) {
                throw new FileNotFoundException("not a directory: "+srcDir.getAbsolutePath());
            }
            String fstr = tfSourceDirFileFilter.getText();
            if (fstr.isEmpty()) {
                throw new IOException("invalid source dir file filter");
            }
            dirFilter = 
                FileFilterFactory.or(FileFilterFactory.DirectoriesOnly(), 
                FileFilterFactory.parseFilterList(fstr));
        }
        
        
        PatchMetadata meta = new DefaultPatchMetadata();
        extractFields(meta);
        List<Path> paths = new LinkedList<Path>();
        JarFileBuilder jarbuilder = new JarFileBuilder(out);
        
        if (midiFile != null) {
            if (!isValidIntelHexFile(midiFile)) {
                throw new IOException("not a valid intel hex file");
            }
            jarbuilder.add("mididata.hex", new FileInputStream(midiFile));
            paths.add(new Path(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME, "mididata.hex"));
        }
        if (srcDir != null) {
            jarbuilder.add("source", new FSDirectory(srcDir, dirFilter));
            paths.add(new Path(PatchMetadata.DEFAULT_SOURCE_PATH_NAME, "source"));
        }
        meta.setPaths(paths);
        jarbuilder.add(PatchMetadata.FILENAME, PatchMetadataUtils.createXMLInputStream(meta));
        jarbuilder.write();
        jarbuilder.close();
        return meta;
    }
    
    private boolean isValidIntelHexFile(File file)throws IOException {
        try {
            new HexFileValidator().update(file);
            return true;
        } catch (IOException ex) {
            throw new IOException("could not validate midi file", ex);
        }
    }

    public void setAuthor(String value) {
        tfAuthor.setText(value);
    }

    public void setAuthorFieldEditable(boolean e) {
        tfAuthor.setEditable(e);
    }

    public void setAuthorFieldEnabled(boolean e) {
        tfAuthor.setEnabled(e);
    }

    public void setSourceFile(File file) throws Exception {
        Patch patch = new DefaultPatch(file);
        PatchMetadata meta = patch.getMetadata();
        
        setFields(meta, file);
        
    }

    public void setPathFieldsEnabled(boolean value) {
        tfMidiFile.setEditable(value);
        tfMidiFile.setEnabled(value);
        tfSourceDir.setEditable(value);
        tfSourceDir.setEnabled(value);
        tfSourceDirFileFilter.setEditable(value);
        tfSourceDirFileFilter.setEnabled(value);
        btnSelectMidiFile.setEnabled(false);
        btnSelectSourceDir.setEnabled(false);
    }
    
    public void setMidiFileSourceText(String text) {
        tfMidiFile.setText(text);
    }
    
    public void setSourceDirText(String text) {
        tfSourceDir.setText(text);
    }

}
