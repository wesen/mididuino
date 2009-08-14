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
package com.ruinwesen.patchmanager.swing.forms2;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;
import java.util.regex.Pattern;

import javax.swing.JFileChooser;

import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.PatchDataException;
import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.Entry;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.metadata.Tag;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;
import com.ruinwesen.patch.utils.Validator;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;
import com.ruinwesen.patchmanager.swing.form.AbstractFormElementValidator;
import com.ruinwesen.patchmanager.swing.form.ComboBoxFormElement;
import com.ruinwesen.patchmanager.swing.form.FileFormElement;
import com.ruinwesen.patchmanager.swing.form.Form;
import com.ruinwesen.patchmanager.swing.form.FormContainer;
import com.ruinwesen.patchmanager.swing.form.FormElement;
import com.ruinwesen.patchmanager.swing.form.RegexValidator;
import com.ruinwesen.patchmanager.swing.form.StringLengthValidator;
import com.ruinwesen.patchmanager.swing.form.TextAreaFormElement;
import com.ruinwesen.patchmanager.swing.form.TextFieldFormElement;
import com.ruinwesen.patchmanager.swing.form.ValidatorList;

public class PatchMetadataForm extends FormContainer {

    public TextFieldFormElement feAuthor;
    public TextFieldFormElement feTitle;
    public ComboBoxFormElement feDeviceId;
    public ComboBoxFormElement feEnvironmentId;
    public TextAreaFormElement feComment;
    public TextAreaFormElement feTags;
    public TextFieldFormElement feCategory;
    public FileFormElement feSourceDir;
    public TextFieldFormElement feSourceFileFilter;
    public FileFormElement feMidiFile;
    private SourceDirValidator sdv;
    
    public PatchMetadataForm() {
        init();
    }
    
    public void loadMetadata(Patch patch, PatchMetadata meta) {
        feAuthor.setText(meta.getAuthor());
        feTitle.setText(meta.getTitle());
        feDeviceId.setValue(meta.getDeviceId());
        feEnvironmentId.setValue(meta.getEnvironmentId());
        feComment.setText(meta.getComment());
        
        Tagset tags = meta.getTags();
        Tagset categories = tags.filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, true, false);
        Iterator<Tag> tagiterator = categories.iterator();
        boolean firstCategoryTag = true;
        while (tagiterator.hasNext()) {
            if (firstCategoryTag) {
                tags.remove(tagiterator.next());
                firstCategoryTag = false;
            } else {
                tagiterator.next();
                tagiterator.remove();
            }
        }

        feTags.setText(tags.filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, false, false).toSortedString());
        feCategory.setText(categories.filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, true, true).toSortedString());

        File midifile = null;
        File sourcefile = null;
        
        if (patch != null) {
            Path midifilePath = meta.getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME);
            Path sourcePath = meta.getPath(PatchMetadata.DEFAULT_SOURCE_PATH_NAME);

            try {
                Directory dir = patch.openDirectory();
                try {
                    if (midifilePath != null) {
                        Entry entry = dir.getEntry(midifilePath.getPath());
                        if (entry != null) {
                            midifile = entry.getLocalFile();
                        }
                    }
    
                    if (sourcePath != null) {
                        Entry entry = dir.getEntry(sourcePath.getPath());
                        if (entry != null) {
                            sourcefile = entry.getLocalFile();
                        }
                    }
                } finally {
                    try {
                        dir.close();
                    } catch (IOException ex) {
                    }
                }
            } catch (PatchDataException ex) {
                
            }
        }
        
        feMidiFile.setFile(midifile);
        feSourceDir.setFile(sourcefile);
        
    }
    
    public void copyMetadataTo(PatchMetadata dst) {
        dst.setAuthor(feAuthor.getText());
        dst.setComment(feComment.getText());
        dst.setDeviceId((DeviceId)feDeviceId.getValue());
        dst.setEnvironmentId((EnvironmentId)feEnvironmentId.getValue());
        dst.setTitle(feTitle.getText());
        Tagset tags = Tagset.parseTags(feTags.getText());
        tags.add(PatchMetadata.CATEGORY_TAG_PREFIX+feCategory.getText());
        dst.setTags(tags);
    }

    private void init() {
        Form form = new Form("patch-metadata");

        feAuthor = new TextFieldFormElement();
        feAuthor.setLabel("Author");
        feAuthor.getField().setColumns(10);
        feAuthor.setValidator(new RegexValidator(null, "[a-z0-9\\._\\-]{4,20}", Pattern.CASE_INSENSITIVE));
        feAuthor.getField().setToolTipText("4 to 20 characters: [a-z0-9_.-]");
        
        feTitle = new TextFieldFormElement();
        feTitle.setLabel("Title");
        feTitle.setValidator(new StringLengthValidator(null, 4,20));
        feTitle.getField().setToolTipText("4 to 20 characters");
        feTitle.getField().setColumns(16);
        
        feDeviceId = new ComboBoxFormElement(PatchMetadataIDInfo.DEVICE_ID_LIST);
        feDeviceId.setLabel("Device");
        
        feEnvironmentId = new ComboBoxFormElement(PatchMetadataIDInfo.ENVIRONMENT_ID_LIST);
        feEnvironmentId.setLabel("Environment");
        
        feComment = new TextAreaFormElement();
        feComment.getField().setWrapStyleWord(true);
        feComment.getField().setLineWrap(true);
        feComment.setLabel("Comment");
        feComment.setValidator(new StringLengthValidator(null, 0, 10000));
        feComment.getField().setColumns(26);
        feComment.getField().setRows(8);;
        
        feTags = new TextAreaFormElement();
        feTags.getField().setWrapStyleWord(true);
        feTags.getField().setLineWrap(true);
        feTags.setLabel("Tags");
        feTags.getField().setColumns(20);
        feTags.getField().setRows(4);

        feCategory = new TextFieldFormElement();
        feCategory.setLabel("Category");
        feCategory.setValidator(new RegexValidator(null, "[a-z0-9\\._\\-]{1,20}", Pattern.CASE_INSENSITIVE));
        feCategory.getField().setToolTipText("1 to 20 characters");
        feCategory.getField().setColumns(16);

        SourceDirOrFileValidator dof =
            new SourceDirOrFileValidator("Patch must contain either midi file or source code.");
        
        feMidiFile = new FileFormElement();
        feMidiFile.setFileFilter(SwingPatchManagerUtils.HEX_FILE_FILTER);
        feMidiFile.setLabel("Midi File");
        feMidiFile.setFileSelectionMode(JFileChooser.FILES_ONLY);
        feMidiFile.setValidator(
                new ValidatorList(
                        new HexFileValidator("Midi file does not exist or is not a valid Intel HEX File."),
                        dof));

        feSourceDir = new FileFormElement();
        feSourceDir.setLabel("Source Code");
        feSourceDir.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        sdv = new SourceDirValidator("Source directory does not exist.");
        feSourceDir.setValidator(new ValidatorList(sdv,dof));

        
        feSourceFileFilter = new TextFieldFormElement();
        feSourceFileFilter.getField().setText("*.h,*.hh,*.c,*.cpp,*.cxx");
        feSourceFileFilter.setLabel("Filter");
        
        form.add(feAuthor);
        form.add(feTitle);
        form.add(feDeviceId);
        form.add(feEnvironmentId);
        form.add(feComment);
        form.add(feTags);
        form.add(feCategory);
        form.add(feSourceDir);
        form.add(feSourceFileFilter);
        form.add(feMidiFile);
        
        setForm(form);
    }
    
    private static class HexFileValidator extends AbstractFormElementValidator {

        public HexFileValidator(String errorMessage) {
            super(errorMessage);
        }

        @Override
        public boolean isValid(FormElement elem) {
            File file = ((FileFormElement)elem).getFile();
            return file == null
                || (file.isFile() && isHexFile(file));
        }

        private boolean isHexFile(File file) {
            Validator v = new com.ruinwesen.patch.utils.HexFileValidator(false);
            try {
                InputStream is = new BufferedInputStream(new FileInputStream(file));
                v.update(is);
            } catch (IOException ex) {
                return false;
            }
            return v.isValid();
        }
        
    }

    private static class SourceDirValidator extends AbstractFormElementValidator {

        public SourceDirValidator(String errorMessage) {
            super(errorMessage);
        }

        @Override
        public boolean isValid(FormElement elem) {
            File file = ((FileFormElement)elem).getFile();
            return file == null
                || (file.isDirectory());
        }
    }

    private class SourceDirOrFileValidator extends AbstractFormElementValidator {

        public SourceDirOrFileValidator(String errorMessage) {
            super(errorMessage);
        }

        @Override
        public boolean isValid(FormElement elem) {
            if (feSourceDir == null || feMidiFile == null) {
                return false;
            }
            
            if (feSourceDir.getFile() == null && feMidiFile.getFile() == null) {
                return false;
            }
            return sdv.isValid(feSourceDir) || feMidiFile.isValid();
        }
    }
    
}
