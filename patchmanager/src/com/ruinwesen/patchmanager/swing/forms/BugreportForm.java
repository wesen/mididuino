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

import java.awt.EventQueue;
import java.awt.Component;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

import name.cs.csutils.CSAction;
import name.cs.csutils.SwingActionData;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jdesktop.layout.GroupLayout;

import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.PatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestSendBugReport;
import com.ruinwesen.patchmanager.client.protocol.Response;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

public class BugreportForm extends Form {

    private static Log log = LogFactory.getLog(BugreportForm.class); 
    
    public static final int SEND_OPTION = 1;
    public static final int CANCEL_OPTION = 0;
    
    private JPanel panel;
    private JTextField tfUsername;
    private JTextField tfUsermail;
    private JTextArea taUsernote;
    private JTextArea taReport;
    private JCheckBox cbUserAllowsContact;
    private JButton btnCancel;
    private JButton btnSend;
    private PatchManagerClient client;
    
    public BugreportForm(
            PatchManagerClient client,
            Auth auth, String report) {
        this(client,
                auth != null ? auth.getUsername() : null,
                auth != null ? auth.getUsermail() : null,
                report);
    }
    
    public BugreportForm(
            PatchManagerClient client,
            String username, String usermail, String report) {
        init();
        this.client = client;
        tfUsername.setText(username == null ? "" : username);
        tfUsermail.setText(usermail == null ? "" : usermail);
        taReport.setText(report);
    }
    
    private void init() {
        panel = new JPanel();
        GroupLayout ly = new GroupLayout(panel);
        panel.setLayout(ly);
        
        // create components
        tfUsername = new JTextField();
        tfUsermail = new JTextField();
        taUsernote = new JTextArea();
        JScrollPane scrollUsernote = new JScrollPane(taUsernote);
        taReport = new JTextArea();
        JScrollPane scrollReport = new JScrollPane(taReport);
        btnCancel = new JButton(new CSAction(this,"btnCancelClick"));
        btnSend = new JButton(new CSAction(this,"btnSendClick"));
        cbUserAllowsContact = new JCheckBox("You may contact me.", true);

        tfUsername.setColumns(10);
        tfUsermail.setColumns(10);
        taUsernote.setColumns(40);
        taUsernote.setRows(8);
        taReport.setColumns(4);
        taReport.setRows(8);
        
        // labels
        JLabel lblUsername = new JLabel("Username");
        JLabel lblUsermail = new JLabel("Mail");
        JLabel lblUsernote = new JLabel("Description");
        JLabel lblReport = new JLabel("Report");
        JLabel lblUserAllowsContact = new JLabel("Allow Contact");
        
        ly.setAutocreateGaps(true);
        ly.setAutocreateContainerGaps(true);
        
        ly.setHorizontalGroup(
                ly.createSequentialGroup()
                .add(
                        ly.createParallelGroup()
                        .add(lblUsername)
                        .add(lblUsermail)
                        .add(lblUserAllowsContact)
                        .add(lblUsernote)
                        .add(lblReport)
                )
                .add(
                        ly.createParallelGroup()
                        .add(tfUsername)
                        .add(tfUsermail)
                        .add(cbUserAllowsContact)
                        .add(scrollUsernote)
                        .add(scrollReport)
                        .add(GroupLayout.TRAILING, ly.createSequentialGroup()
                                .add(btnCancel)
                                .add(btnSend)
                        )
                )
        );

        ly.setVerticalGroup(
                ly.createSequentialGroup()
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblUsername)
                        .add(tfUsername)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblUsermail)
                        .add(tfUsermail)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblUserAllowsContact)
                        .add(cbUserAllowsContact)
                )
                .add(ly.createParallelGroup()
                        .add(lblUsernote)
                        .add(scrollUsernote)
                )
                .add(ly.createParallelGroup()
                        .add(lblReport)
                        .add(scrollReport)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(btnCancel)
                        .add(btnSend)
                )
        );
        
        ly.linkSize(new Component[]{btnCancel, btnSend},SwingConstants.HORIZONTAL);
    }

    @SwingActionData("Send")
    public void btnSendClick() {
        new Thread(new Runnable() {
            public void run() {
                System.out.println("THREAD");
                try {
                    setLocked(true);
                    performSendReport();
                } finally {
                    setLocked(false);
                }
            }
        }).start();
    }

    @SwingActionData("Cancel")
    public void btnCancelClick() {
        closeDialog();
    }
    
    private void performSendReport() {
        String username;
        String usermail;
        if (cbUserAllowsContact.isSelected()) {
            username = tfUsername.getText();
            usermail = tfUsermail.getText();
        } else {
            username = null;
            usermail = null;
        }
        
        StringBuilder sb = new StringBuilder();
        sb.append("***********************************************************").append("\n");
        sb.append("* Bug Report                                              *").append("\n");
        sb.append("***********************************************************").append("\n");
        sb.append("Username: "+username).append("\n");
        sb.append("Usermail: "+usermail).append("\n");
        sb.append("Allow Contacting: "+cbUserAllowsContact.isSelected()).append("\n");
        sb.append("***********************************************************").append("\n");
        sb.append("* User Comment                                            *").append("\n");
        sb.append("***********************************************************").append("\n\n");
        sb.append(taUsernote.getText()).append("\n");
        sb.append("***********************************************************").append("\n");
        sb.append("* Bug Report                                              *").append("\n");
        sb.append("***********************************************************").append("\n\n");
        sb.append(taReport.getText()).append("\n");
        
        
        final Request request = new RequestSendBugReport(
                sb.toString(),
                new Auth(username, null, usermail)
        );
        
        
        boolean trouble = false;
        Response response;
        try {
            response = client.execute(request);

            if (!response.isOkStatus()) {
                log.error("Could not send bugreport. Response:"+response);
                trouble = true;
            }
        } catch (Exception ex) {
            trouble = true;
            log.error("Could not send bugreport", ex);
        }

        if (trouble) {
            SwingPatchManagerUtils.showErrorDialog(null, "Could not send the bug report");
        }        
    }
    
    private void setLocked(final boolean locked) {
        if (!EventQueue.isDispatchThread()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    setLocked(locked);
                }
            });
            return;
        }
        
        cbUserAllowsContact.setEnabled(!locked);
        tfUsermail.setEnabled(!locked);
        tfUsername.setEnabled(!locked);
        taReport.setEnabled(!locked);
        taUsernote.setEnabled(!locked);
        
        tfUsermail.setEditable(!locked);
        tfUsername.setEditable(!locked);
        taReport.setEditable(!locked);
        taUsernote.setEditable(!locked);
    }
    
    public void showDialog(Component parentComponent) {
        super.showDialog(parentComponent, "Bug Report");
    }
    
    @Override
    public Component getForm() {
        return panel;
    }
    
}
