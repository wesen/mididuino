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
package com.ruinwesen.patchmanager.swing.components;

import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;
import javax.swing.text.JTextComponent;

import name.cs.csutils.CSAction;
import name.cs.csutils.SwingActionData;

/**
 * A cut/copy/paste menue for text components.
 * @author chresan
 */
public class TextPopup implements PopupMenuListener {

    private JPopupMenu popup;
    private JMenuItem mnCut;
    private JMenuItem mnCopy;
    private JMenuItem mnPaste;
    private JMenuItem mnDelete;
    private JMenuItem mnSelectAll;
    private JTextComponent currentComponent;
    
    public TextPopup() {
        init();
    }
    
    public void installAt(JTextComponent c) {
        if (c == null) {
            throw new IllegalArgumentException("c == null");
        }
        c.setComponentPopupMenu(popup);
        this.currentComponent = c;
    }

    private void init() {
        this.popup = new JPopupMenu();
        popup.addPopupMenuListener(this);
        mnCut = new JMenuItem(new CSAction(this, "cut"));
        mnCopy = new JMenuItem(new CSAction(this, "copy"));
        mnPaste = new JMenuItem(new CSAction(this, "paste"));
        mnDelete = new JMenuItem(new CSAction(this, "delete"));
        mnSelectAll = new JMenuItem(new CSAction(this, "selectall")); 
        popup.add(mnCut);
        popup.add(mnCopy);
        popup.add(mnPaste);
        popup.add(mnDelete);
        popup.addSeparator();
        popup.add(mnSelectAll);
    }

    private boolean isTextComponentSelected() {
        return currentComponent != null;
    }
    
    @SwingActionData(value="Cut", resourcekey="textpopup.cut")
    public void cut() {
        if (isTextComponentSelected() && 
                currentComponent.isEnabled() && 
                currentComponent.isEditable()) {
            currentComponent.cut();
        }
    }

    @SwingActionData(value="Copy", resourcekey="textpopup.copy")
    public void copy() {
        if (isTextComponentSelected() &&
                currentComponent.isEnabled()) {
            currentComponent.copy();
        }
    }

    @SwingActionData(value="Paste", resourcekey="textpopup.paste")
    public void paste() {
        if (isTextComponentSelected() && 
                currentComponent.isEnabled() && 
                currentComponent.isEditable()) {
            currentComponent.paste();
        }
    }

    @SwingActionData(value="Delete", resourcekey="textpopup.delete")
    public void delete() {
        if (isTextComponentSelected() && 
                currentComponent.isEnabled() && 
                currentComponent.isEditable()) {
            currentComponent.replaceSelection("");
        }
    }

    @SwingActionData(value="Select all", resourcekey="textpopup.selectall")
    public void selectall() {
        if (isTextComponentSelected() && 
                currentComponent.isEnabled() && 
                currentComponent.getText().length()>0) {
            currentComponent.setSelectionStart(0);
            currentComponent.setSelectionEnd(currentComponent.getText().length());
        }
    }

    @Override
    public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
        if (isTextComponentSelected()) {
            // note: we test for the selected text because
            // there is no guarantee getSelectionEnd()<=getText().length()
            // meaning it can be outside the range of the text
            boolean hasSelectedText = currentComponent.getSelectionEnd()
                > currentComponent.getSelectionStart()
              && !currentComponent.getSelectedText().isEmpty();

            mnCut.setEnabled(
                    currentComponent.isEnabled() &&
                    currentComponent.isEditable() && hasSelectedText);
            mnCopy.setEnabled(
                    currentComponent.isEnabled() &&
                    hasSelectedText);
            mnPaste.setEnabled(
                    currentComponent.isEnabled() &&
                    currentComponent.isEditable());
            mnDelete.setEnabled(
                    currentComponent.isEnabled() &&
                    currentComponent.isEditable() && hasSelectedText);
            mnSelectAll.setEnabled(currentComponent.isEnabled()
                    && currentComponent.getText().length()>0);
        }
    }
    
    @Override
    public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {
        // no op
    }

    @Override
    public void popupMenuCanceled(PopupMenuEvent e) {
        // no op
    }

}
