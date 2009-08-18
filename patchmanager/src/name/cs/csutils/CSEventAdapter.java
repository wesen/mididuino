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
package name.cs.csutils;

import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.MouseInputAdapter;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

public class CSEventAdapter extends MouseInputAdapter
    implements ComponentListener, ContainerListener, PopupMenuListener,
        WindowListener, DocumentListener {


    public void changedUpdate(DocumentEvent e) {
        documentChanged(e);
    }

    public void insertUpdate(DocumentEvent e) {
        documentChanged(e);
    }

    public void removeUpdate(DocumentEvent e) {
        documentChanged(e);
    }
    
    protected void documentChanged(DocumentEvent e) {
        // no op
    }
    

    public void componentHidden(ComponentEvent e) {
        // no op
    }


    public void componentMoved(ComponentEvent e) {
        // no op
    }


    public void componentResized(ComponentEvent e) {
        // no op
    }


    public void componentShown(ComponentEvent e) {
        // no op
    }


    public void componentAdded(ContainerEvent e) {
        // no op
    }


    public void componentRemoved(ContainerEvent e) {
        // no op
    }


    public void popupMenuCanceled(PopupMenuEvent e) {
        // no op
    }


    public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {
        // no op
    }


    public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
        // no op
    }


    public void windowActivated(WindowEvent e) {
        // no op
    }


    public void windowClosed(WindowEvent e) {
        // no op
    }


    public void windowClosing(WindowEvent e) {
        // no op
    }


    public void windowDeactivated(WindowEvent e) {
        // no op   
    }


    public void windowDeiconified(WindowEvent e) {
        // no op
    }


    public void windowIconified(WindowEvent e) {
        // no op        
    }


    public void windowOpened(WindowEvent e) {
        // no op
    }

}
