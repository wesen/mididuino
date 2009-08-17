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
import java.awt.Color;
import java.awt.event.MouseEvent;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;

import name.cs.csutils.CSEventAdapter;
import name.cs.csutils.IconLoader;

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
        
        JPanel iconPanel = new JPanel();
        iconPanel.setBackground(new Color(0x171717));
        iconPanel.setLayout(new BorderLayout());
        iconPanel.add(newLabel(IconLoader.getInstance().getIcon("rw-logo")),
                BorderLayout.NORTH);
        iconPanel.setBorder(BorderFactory.createEmptyBorder(2,2,2,2));
        
        
        
        JPanel panel = new JPanel();
        panel.setBackground(new Color(0x171717));
        panel.setBorder(BorderFactory.createEmptyBorder(2,2,2,2));
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        panel.add(newLabel("PatchManager"));
        panel.add(newLabel("(C) 2009 Ruin & Wesen"));
        panel.add(Box.createVerticalStrut(5));
        panel.add(newLabel("Version: "+patchmanager.getVersion()));
        panel.add(newLabel("Build: "+patchmanager.getBuildNumber()+
                " Time: "+patchmanager.getBuildTimestamp()));
        panel.add(Box.createVerticalStrut(5));
        panel.add(newLabel("Credits"));
        panel.add(newLabel("- Manuel Odendahl"));
        panel.add(newLabel("- Christian Schneider"));
        
        final JDialog dialog = new JDialog(patchmanager.getFrame(),"About Patchmanager");
        
        dialog.getContentPane().addMouseListener(new CSEventAdapter(){
            public void mouseClicked(MouseEvent e) {
                close(dialog);
            }
        });

        dialog.getContentPane().setLayout(new BorderLayout());
        dialog.getContentPane().add(iconPanel,BorderLayout.WEST);
        dialog.getContentPane().add(panel,BorderLayout.CENTER);
        dialog.setResizable(false);
        dialog.setModal(true);
        dialog.pack();
        dialog.setLocationByPlatform(true);
        dialog.setVisible(true);
    }


    private void close(JDialog dialog) {
        dialog.setVisible(false);
        dialog.dispose();
    }
    
    private JLabel newLabel(String text) {
        JLabel label = newLabel();
        label.setText(text);
        return label;
    }

    private JLabel newLabel(ImageIcon icon) {
        JLabel label = newLabel();
        label.setIcon(icon);
        return label;
    }

    private JLabel newLabel() {
        JLabel label = new JLabel();
        label.setForeground(Color.WHITE);
        return label;
    }
    
}
