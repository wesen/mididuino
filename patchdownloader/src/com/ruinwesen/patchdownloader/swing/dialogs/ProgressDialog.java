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
package com.ruinwesen.patchdownloader.swing.dialogs;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.IllegalComponentStateException;
import java.awt.Rectangle;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JRootPane;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import javax.swing.WindowConstants;

import name.cs.csutils.I18N;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.RedirectAction.RedirectActionMeta;

/**
 * A progress dialog. 
 * 
 * Note: The dialog is modal thus before calling 
 * {@link #showProgressDialog()} or {@link #showProgressDialogAnimated()}
 * the controlling {@link Thread} must be started first otherwise the
 * Thread will not execute until the dialog is closed.
 * 
 * @author chresan
 * 
 * TODO add ability to abort/close the dialog  
 */
public class ProgressDialog extends JDialog implements ActionListener {

    private static final long serialVersionUID = -1579290983823999669L;
    private static final int ANIMATE_NOTHING = -1;
    private static final int ANIMATE_START_DIALOG = 1;
    private static final int ANIMATE_CLOSE_DIALOG = 2;
    
    private static final int MAXIMUM = 1000000;
    
    private JProgressBar progressBar;
    private JLabel labelStatus;
    private JButton btnCloseCancel;
    private Timer timer;
    private int timerAnimateType = ANIMATE_NOTHING;
    private ActionListener closeAction;
    private ActionListener cancelAction;
    
    public ProgressDialog(Component owner, String title) {
        this(windowFor(owner), title);
    }

    public void setCloseAction(ActionListener a) {
        this.closeAction = a;
        actionUpdated();
    }

    public void setCancelAction(ActionListener a) {
        this.cancelAction = a;
        actionUpdated();
    }

    private void actionUpdated() {
        boolean enabled = closeAction != null || cancelAction != null;
        if (cancelAction != null) {
            I18N.update(btnCloseCancel.getAction(), "dialog.button.cancel");
        } else {
            I18N.update(btnCloseCancel.getAction(), "dialog.button.close");
        }
        btnCloseCancel.setEnabled(enabled);
    }

    private static Window windowFor(Component owner) {
        if (owner instanceof Window) {
            return (Window) owner;
        }
        return (Window)SwingUtilities.getWindowAncestor(owner);
    }

    public ProgressDialog(Window owner, String title) {
        super(owner, title);
        init();
        setStatusText(title);
    }
    
    public void setProgressBarProgress(final float progress) {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    setProgressBarProgress(progress);
                }
            });
        } else {
            progressBar.setValue((int)Math.round(progress*(float)MAXIMUM));
        }
    }
    
    public void setProgressBarIndeterminate(final boolean enable) {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    setProgressBarIndeterminate(enable);
                }
            });
        } else {
            progressBar.setIndeterminate(enable);
        }
    }
    
    public void setStatusText(final String text) {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    setStatusText(text);
                }
            });
        } else {
            labelStatus.setText(text);
        }
    }

    public void setProgressBarTaskCompleted() {
        setStatusText("Finished.");
        setProgressBarIndeterminate(false);
        setProgressBarProgress(1.0f);
    }
    
    private void init() {
        JDialog dialog = this;
        // setup the content pane
        JPanel pane = new JPanel();
        dialog.setContentPane(pane);
        pane.setBorder(BorderFactory.createEmptyBorder(2,2,2,2));
        
        // create components
        // the progress bar
        progressBar = new JProgressBar();
        progressBar.setMinimum(0);
        progressBar.setMaximum(MAXIMUM);
        progressBar.setIndeterminate(false);
        // the label
        labelStatus = new JLabel();
        labelStatus.setAlignmentX(Component.LEFT_ALIGNMENT);
        // the close/abort button
        btnCloseCancel = new JButton(new RedirectAction(this, "btnCloseCancelClick"));
        btnCloseCancel.setAlignmentX(Component.RIGHT_ALIGNMENT);
        btnCloseCancel.setEnabled(false);
        btnCloseCancel.setDefaultCapable(true);
        dialog.getRootPane().setDefaultButton(btnCloseCancel);
        
        // create the layout
        pane.setLayout(new BoxLayout(pane, BoxLayout.Y_AXIS));
        // first line
        {
            Box box = Box.createHorizontalBox();
            box.add(new JLabel("Status:"));
            box.add(Box.createHorizontalStrut(2));
            box.add(labelStatus);
            box.add(Box.createHorizontalGlue());
            pane.add(box);
        }
        pane.add(Box.createVerticalStrut(2));
        // second line
        {
            Box box = Box.createHorizontalBox();
            box.add(progressBar);
            pane.add(box);
        }
        pane.add(Box.createVerticalStrut(2));
        // third line
        {
            Box box = Box.createHorizontalBox();
            box.add(Box.createHorizontalGlue());
            box.add(btnCloseCancel);
            pane.add(box);
        }
        
        try {
            // before call to dialog.pack()
            //dialog.setUndecorated(true);
            dialog.getRootPane().setWindowDecorationStyle(JRootPane.PLAIN_DIALOG); 
        } catch (IllegalComponentStateException ex) {
            // no op
            ex.printStackTrace();
        }
        dialog.pack();
        Dimension size = new Dimension(
                Math.max(dialog.getWidth(), 280),
                dialog.getHeight()
        );
        dialog.setMinimumSize(size);
        dialog.setSize(size);
        dialog.setResizable(false);
        dialog.setModal(true);
        dialog.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        
        timer = new Timer(0, this);
        timer.setRepeats(false);
        timer.setInitialDelay(1000);
    }
    
    @RedirectActionMeta(title="Close",resourcekey="dialog.button.close")
    public void btnCloseCancelClick() {
        if (cancelAction != null) {
            cancelAction.actionPerformed(new ActionEvent(this, 0, "cancel"));
        } else if (closeAction != null) {
            closeAction.actionPerformed(new ActionEvent(this, 0, "close"));
        }
        btnCloseCancel.setEnabled(false);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (timerAnimateType == ANIMATE_CLOSE_DIALOG) {
            timerAnimateType = ANIMATE_NOTHING;
            timer.stop();
            closeProgressDialog();
        } else if (timerAnimateType == ANIMATE_START_DIALOG) {
            timerAnimateType = ANIMATE_NOTHING;
            timer.stop();
            showProgressDialog();
        } 
    }
    
    public void showProgressDialog() {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    showProgressDialog();
                }
            });
        } else {
            final JDialog dialog = this;
            if (!dialog.isVisible()) {
                
                Container parent = dialog.getParent();
                if (parent != null) {
                    Rectangle b = parent.getBounds();
                    dialog.setLocation(
                            b.x + (b.width-dialog.getWidth())/2,
                            b.y + (b.height-dialog.getHeight())/2
                    );
                }
                
                dialog.setVisible(true);   
            }
        }
    }

    public void showProgressDialogAnimated() {
        final JDialog dialog = this;
        if (timerAnimateType == ANIMATE_NOTHING && !dialog.isVisible()) {
            timerAnimateType = ANIMATE_START_DIALOG; 
            timer.start();
        }
    }
    
    public void closeProgressDialogAnimated() {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    closeProgressDialogAnimated();
                }
            });
        } else {
            timerAnimateType = ANIMATE_CLOSE_DIALOG;
            if (!timer.isRunning()) {
                timer.start();
            }
        }
    }
    
    public void closeProgressDialog() {
        if (!SwingUtilities.isEventDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    closeProgressDialog();
                }
            });
        } else {
            final JDialog dialog = this;
            if (dialog.isVisible()) {
                dialog.setVisible(false);
                dialog.dispose();
            }
        }
    }

}

