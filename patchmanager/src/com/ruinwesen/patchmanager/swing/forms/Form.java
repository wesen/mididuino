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
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.HeadlessException;
import java.awt.Window;

import javax.swing.JDialog;

public abstract class Form {

    private JDialog dialog;
    
    protected JDialog getDialog() {
        return dialog;
    }
    
    protected void closeDialog() {
        JDialog dlg = getDialog();
        if (dlg != null) {
            dlg.setVisible(false);
        }
    }
    
    public abstract Component getForm();
    
    protected void showDialog(Component parentComponent, String title) {

        JDialog dialog;

        Window window = getWindowForComponent(parentComponent);
        if (window == null) {
            dialog = new JDialog((Frame)null, title, true);
        } else if (window instanceof Frame) {
            dialog = new JDialog((Frame)window, title, true);   
        } else {
            dialog = new JDialog((Dialog)window, title, true);
        }
        dialog.setResizable(false);
        dialog.setLocationRelativeTo(parentComponent);
        Component form = getForm();
        dialog.getContentPane().add(form);
        dialog.pack();
        
        Dimension minSize = form.getMinimumSize();
        if (minSize != null) {
            Dimension curSize = form.getSize();
            if (minSize.width>curSize.width || minSize.height>curSize.height) {
                Dimension newSize = new Dimension(Math.max(curSize.width, minSize.width),
                Math.max(curSize.height, minSize.height));

                int dw = newSize.width-curSize.width;
                int dh = newSize.height-curSize.height;
                
                dialog.setSize(dialog.getWidth()+dw, dialog.getHeight()+dh);
            }
        }

        this.dialog = dialog;
        dialog.setVisible(true);
        dialog.setVisible(false);
        
        dialog.getContentPane().remove(form);
        dialog.dispose();
    }
    
    static Window getWindowForComponent(Component parentComponent) 
    throws HeadlessException {
        if (parentComponent == null)
            return null;
        if (parentComponent instanceof Frame || parentComponent instanceof Dialog)
            return (Window)parentComponent;
        return getWindowForComponent(parentComponent.getParent());
    }
        
}
