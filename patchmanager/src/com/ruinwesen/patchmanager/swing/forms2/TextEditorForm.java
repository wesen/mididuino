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

import java.awt.Component;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;

import javax.swing.JButton;
import javax.swing.JFileChooser;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.CSUtils;

import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;
import com.ruinwesen.patchmanager.swing.form.CustomFormElement;
import com.ruinwesen.patchmanager.swing.form.Form;
import com.ruinwesen.patchmanager.swing.form.FormContainer;
import com.ruinwesen.patchmanager.swing.form.TextAreaFormElement;

public class TextEditorForm extends FormContainer {

    TextAreaFormElement tfTextArea;
    
    public TextEditorForm(boolean editable) {
        Form form = new Form("text-editor");
        if (editable) {
            CSAction acLoadFile = new CSAction("Load File...")
            .useInvokationTarget(this, "loadFile")
            .useInvokeLater();
            
            JButton btnLoadFile = new JButton(acLoadFile);
            btnLoadFile.setAlignmentX(Component.RIGHT_ALIGNMENT);
            form.add(new CustomFormElement(btnLoadFile));
        }
        tfTextArea = new TextAreaFormElement();
        tfTextArea.getField().setEditable(editable);
        tfTextArea.getField().setColumns(40);
        tfTextArea.getField().setRows(20);
        tfTextArea.getField().setWrapStyleWord(true);
        tfTextArea.getField().setLineWrap(false);
        form.add(tfTextArea);
        setForm(form);
    }

    public String getText() {
        return tfTextArea.getText();
    }

    public void setText(String text) {
        tfTextArea.setText(text);
        tfTextArea.getField().setCaretPosition(0);
    }

    public void loadFile() {
        JFileChooser jc = new JFileChooser();
        jc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(jc);
        if (jc.showOpenDialog(getContainer()) == JFileChooser.APPROVE_OPTION) {
            File file = jc.getSelectedFile();
            try {
                setText(loadText(file));
            } catch (IOException ex) {
                SwingPatchManagerUtils.showErrorDialog(getContainer(), "Could not load file:"+ex);
            }
        }
    }

    public static String loadText(File file) throws IOException {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        CSUtils.copy(file, os);
        return new String(os.toByteArray());
    }
    
}
