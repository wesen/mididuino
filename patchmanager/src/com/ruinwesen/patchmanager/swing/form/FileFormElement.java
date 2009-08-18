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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;

import com.ruinwesen.patchmanager.swing.components.TextPopup;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSFileSelectionContext;

public class FileFormElement extends TextFieldFormElement 
    implements ActionListener {

    private static final String AC_SELECT_FILE = "form.select.file";
    
    private JButton btnSelectFile;
    private int fileSelectionMode = JFileChooser.FILES_AND_DIRECTORIES;

    private List<FileFilter> fileFilterList = Collections.emptyList();
    
    public FileFormElement() {
        new TextPopup().installAt(getField());
        btnSelectFile = new JButton(
                new CSAction("Browse...")
                .useResourceKey("form.element.file.select-action")
                .useActionCommand(AC_SELECT_FILE)
                .useActionListener(this));
        getField().setColumns(20);
    }

    public void setFileSelectionMode(int mode) {
        this.fileSelectionMode = mode;
    }
    
    public int getFileSelectionMode() {
        return fileSelectionMode;
    }
    
    @Override
    protected void setFormInputElementsEnabled(boolean enabled) {
        super.setFormInputElementsEnabled(enabled);
        btnSelectFile.setEnabled(enabled);
    }

    public void actionPerformed(ActionEvent e) {
        if (AC_SELECT_FILE.equals(e.getActionCommand())) {
            selectFile();
        }
    }
/*
    @Override
    public Object getValue() {
        return getFile();
    }

    @Override
    public void setValue(Object value) {
        if (value instanceof String) {
            setText((String)value);
        } else {
            setFile((File)value);
        }
    }

    */
    public File getFile() {
        String text = super.getText();
        return text == null || text.trim().length()==0 ? null : new File(text);
    }
    
    public void setFile(File file) {
        setText(file == null ? "" : file.getAbsolutePath());
    }
    
    protected void selectFile() {
        JFileChooser fc = new JFileChooser();
        fc.setFileSelectionMode(fileSelectionMode);
        fc.setMultiSelectionEnabled(false);
        FileFilter selectedFilter = null;
        for (FileFilter filter: fileFilterList) {
            if (selectedFilter == null) {
                selectedFilter = filter;
            }
            fc.addChoosableFileFilter(filter);
        }
        fc.setFileFilter(selectedFilter);
        
        
        /*if (fileSelectionMode == JFileChooser.DIRECTORIES_ONLY) {
            fc.setAcceptAllFileFilterUsed(false);
        }*/
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setSelectedFile(getFile());
        if (fc.showOpenDialog(getField()) == JFileChooser.APPROVE_OPTION) {
            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            setFile(fc.getSelectedFile());
        }
    }

    @Override
    public JComponent[] getComponents() {
        return componentArray(getLabel(), getField(), btnSelectFile);
    }

    public void setFileFilter(FileFilter fileFilter) {
        fileFilterList = 
            fileFilter == null 
            ? Collections.<FileFilter>emptyList()
            : Arrays.<FileFilter>asList(fileFilter);
    }

    public void setFileFilterList(List<FileFilter> list) {
        if (list == null) {
            throw new IllegalArgumentException("list==null");
        }
        fileFilterList = new ArrayList<FileFilter>(list);
    }

}
