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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import name.cs.csutils.CSAction;

import com.ruinwesen.patchmanager.swing.SwingPatchManager;

public class AboutDialog {

    private SwingPatchManager patchmanager;
    
    public AboutDialog(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
    }
    
    public static void showDialog(SwingPatchManager patchmanager) {
        new AboutDialog(patchmanager).showDialog();
    }

    private void showDialog() {
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        panel.add(new JLabel("PatchManager"));
        panel.add(new JLabel("Version: "+patchmanager.getVersion()));
        panel.add(new JLabel("Build: "+patchmanager.getBuildNumber()+
                " Time: "+patchmanager.getBuildTimestamp()));
        
        final JDialog dialog = new JDialog(patchmanager.getFrame(),"About Patchmanager");
        
        JButton btn = 
        new JButton(new CSAction("Close")
        .useActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    dialog.setVisible(false);
                }
        }
        ));
        
        btn.setHorizontalAlignment(SwingConstants.CENTER);
        panel.add(btn);

        dialog.getContentPane().setLayout(new BorderLayout());
        dialog.getContentPane().add(panel,BorderLayout.CENTER);
        dialog.setModal(true);
        dialog.pack();
        dialog.setLocationByPlatform(true);
        dialog.setVisible(true);
    }
    
}
