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
package com.ruinwesen.patchdownloader.swing;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

import com.ruinwesen.midisend.MidiSendCallback;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.RedirectAction.RedirectActionMeta;

public class SendMidiDialog implements MidiSendCallback {

    private JFrame owner;
    private JDialog dialog;
    private JProgressBar progressBar;
    private JLabel labelStatus;
    private JButton btnAbortClose;
    private boolean isButtonModeAbort = true;
    
    public SendMidiDialog(JFrame owner) {
        this.owner = owner;
        init();
    }
    
    public void setIndeterminate(final boolean enable) {
        if (SwingUtilities.isEventDispatchThread()) {
            progressBar.setIndeterminate(enable);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    progressBar.setIndeterminate(enable);
                }
            });
        }
    }
    
    public void setProgress(final int value) {
        if (SwingUtilities.isEventDispatchThread()) {
            progressBar.setValue(value);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    progressBar.setValue(value);
                }
            });
        }
    }

    public void setMaximum(final int value) {
        if (SwingUtilities.isEventDispatchThread()) {
            progressBar.setMaximum(value);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    progressBar.setMaximum(value);
                }
            });
        }
    }

    public void setStatus(final String text) {
        if (SwingUtilities.isEventDispatchThread()) {
            labelStatus.setText(text);
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    labelStatus.setText(text);
                }
            });
        }
    }

    public void setError(final String message) {
        if (SwingUtilities.isEventDispatchThread()) {
            labelStatus.setText(message);
            progressBar.setIndeterminate(false);
            progressBar.setEnabled(false);
            setButtonModeToClose();
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    setError(message);
                }
            });
        }
    }
    
    private void init() {
        dialog = new JDialog(owner, I18N.translate("sendmididialog.title", "Send"));
        JPanel pane = new JPanel();
        pane.setBorder(BorderFactory.createEmptyBorder(2,2,2,2));
        
        progressBar = new JProgressBar();
        progressBar.setIndeterminate(true);
        progressBar.setValue(0);
        progressBar.setMinimum(0);
        progressBar.setMaximum(1);
        
        labelStatus = new JLabel("Sending...");
        labelStatus.setAlignmentX(Component.LEFT_ALIGNMENT);
        labelStatus.setAlignmentY(Component.TOP_ALIGNMENT);
        
        btnAbortClose = new JButton(new RedirectAction(this, "btnAbortCloseClick"));
        

        labelStatus.setAlignmentX(Component.LEFT_ALIGNMENT);
        labelStatus.setAlignmentY(Component.TOP_ALIGNMENT);
        progressBar.setAlignmentX(Component.LEFT_ALIGNMENT);
        progressBar.setAlignmentY(Component.TOP_ALIGNMENT);
        btnAbortClose.setAlignmentX(Component.RIGHT_ALIGNMENT);
        btnAbortClose.setAlignmentY(Component.TOP_ALIGNMENT);
        
        pane.setLayout(new BoxLayout(pane, BoxLayout.Y_AXIS));

        {
            Box box = Box.createHorizontalBox();
            box.add(labelStatus);
            box.add(Box.createHorizontalGlue());
            pane.add(box);
        }
        pane.add(Box.createVerticalStrut(2));
        {
            Box box = Box.createHorizontalBox();
            box.add(progressBar);
            pane.add(box);
        }
        pane.add(Box.createVerticalStrut(2));
        {
            Box box = Box.createHorizontalBox();
            box.add(Box.createHorizontalGlue());
            box.add(btnAbortClose);
            pane.add(box);
        }

        dialog.setContentPane(pane);
        dialog.pack();
        /*
        dialog.setMinimumSize(new Dimension(
                Math.max(240, dialog.getWidth()),
                Math.max(180, dialog.getHeight())));*/
        dialog.setResizable(false);
    }
    
    public void show() {
        
        if (owner != null) {
            Rectangle bounds = owner.getBounds();
            int x = bounds.x+(bounds.width-dialog.getWidth())/2;
            int y = bounds.y+(bounds.height-dialog.getHeight())/2;
            dialog.setLocation(x, y);
        }
        dialog.setVisible(true);
    }

    private void setButtonModeToClose() {
        isButtonModeAbort = false;
        btnAbortClose.setText("Close");
    }

    @RedirectActionMeta(title="Close")
    public void btnAbortCloseClick() {
        btnAbortClose.setEnabled(false);
        close();
    }

    @Override
    public void midisendProgress(float progress) {
        // TODO 
    }

    public void close() {
        dialog.setVisible(false);
        dialog.dispose();
    }

    @Override
    public void midisendCompleted() {
        if (SwingUtilities.isEventDispatchThread()) {
            animateShutdown();
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    midisendCompleted();
                }
            });
        }
    }

    private void animateShutdown() {
        progressBar.setIndeterminate(false);
        progressBar.setMaximum(100);
        progressBar.setValue(100);
        Timer timer = new Timer(500, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                close();
            }
        });
        timer.setRepeats(false);
        timer.setInitialDelay(500);
        timer.start();
    }
    
}
