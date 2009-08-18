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
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import name.cs.csutils.CSAction;
import name.cs.csutils.SwingActionData;

import org.apache.http.impl.client.DefaultHttpClient;
import org.jdesktop.layout.GroupLayout;

import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.client.protocol.ClientProtocol1;
import com.ruinwesen.patchmanager.client.protocol.DefaultPatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.PatchManagerClient;
import com.ruinwesen.patchmanager.client.protocol.Request;
import com.ruinwesen.patchmanager.client.protocol.RequestDeleteUser;
import com.ruinwesen.patchmanager.client.protocol.RequestRegisterNewUser;
import com.ruinwesen.patchmanager.client.protocol.Response;

public class NewLoginForm extends Form {

    public static final int OPTION_CREATE_USER_OR_CANCEL = 0;
    public static final int OPTION_DELETE_USER_OR_CANCEL = 1;
    public static final int OPTION_LOGIN = 2;
    
    private JPanel panel;
    private JTextField tfUsername;
    private JPasswordField tfPassword;
    private JPasswordField tfPasswordRepeated;
    private JTextField tfUsermail;
    private JTextField tfAdminUsername;
    private JPasswordField tfAdminPassword;
    private JButton btnOk;
    private JButton btnCancel;
    private JLabel statusLabel;
    private int messageType;
    private int option = JOptionPane.CANCEL_OPTION;
    private PatchManagerClient client;
    private JLabel lblUsername = new JLabel("Username");
    private JLabel lblPassword = new JLabel("Password");
    private JLabel lblPasswordRepeated = new JLabel("Repeat Password");
    private JLabel lblUsermail = new JLabel("Mail");
    private JLabel lblAdminUsername = new JLabel("Admin Username");
    private JLabel lblAdminPassword = new JLabel("Admin Password");

    
    public NewLoginForm(PatchManagerClient client, int messageType) {
        this.client = client;
        this.messageType = messageType;
        init();
    }
    
    public void setButtonsVisible(boolean value) {
        btnOk.setVisible(value);
        btnCancel.setVisible(value);
    }
    
    public int getSelectedOption() {
        return option;
    }
    
    @Override
    public JComponent getForm() {
        return panel;
    }
    
    private void init() {
        panel = new JPanel();
        GroupLayout ly = new GroupLayout(panel);
        panel.setLayout(ly);
        
        // create components
        tfUsername = new JTextField();
        tfPassword = new JPasswordField();
        tfPasswordRepeated = new JPasswordField();
        tfUsermail = new JTextField();
        CSAction okAction = new CSAction(this,"btnOkClick");
        switch (messageType) {
            case OPTION_CREATE_USER_OR_CANCEL: { okAction.useName("Create Login"); break; }
            case OPTION_DELETE_USER_OR_CANCEL: { okAction.useName("Delete User"); break; }
            case OPTION_LOGIN: { okAction.useName("Login"); break; }
            default: { break; }
        }
        tfAdminUsername = new JTextField();
        tfAdminPassword = new JPasswordField();
        btnOk = new JButton(okAction);
        btnCancel = new JButton(new CSAction(this,"btnCancelClick"));
        statusLabel = new JLabel("");
        statusLabel.setVisible(false);

        ly.setAutocreateGaps(true);
        ly.setAutocreateContainerGaps(true);
        
        ly.setHorizontalGroup(
                ly.createSequentialGroup()
                .add(ly.createParallelGroup()
                        .add(lblUsername)
                        .add(lblPassword)
                        .add(lblPasswordRepeated)
                        .add(lblUsermail)
                        .add(lblAdminUsername)
                        .add(lblAdminPassword)
                )
                .add(ly.createParallelGroup()
                        .add(tfUsername)
                        .add(tfPassword)
                        .add(tfPasswordRepeated)
                        .add(tfUsermail)
                        .add(tfAdminUsername)
                        .add(tfAdminPassword)
                        
                        .add(GroupLayout.LEADING, ly.createSequentialGroup()
                                .add(statusLabel)
                        )
                        .add(GroupLayout.TRAILING, ly.createSequentialGroup()
                                .add(btnCancel)
                                .add(btnOk)   
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
                        .add(lblPassword)
                        .add(tfPassword)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblPasswordRepeated)
                        .add(tfPasswordRepeated)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblUsermail)
                        .add(tfUsermail)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblAdminUsername)
                        .add(tfAdminUsername)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblAdminPassword)
                        .add(tfAdminPassword)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(statusLabel)
                )
                .add(ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(btnCancel)
                        .add(btnOk)
                )
        );        

        boolean requiresAdminFields = messageType == OPTION_DELETE_USER_OR_CANCEL;
        lblAdminUsername.setVisible(requiresAdminFields);
        tfAdminUsername.setVisible(requiresAdminFields);
        lblAdminPassword.setVisible(requiresAdminFields);
        tfAdminPassword.setVisible(requiresAdminFields);
        
        switch (messageType) {
        case OPTION_CREATE_USER_OR_CANCEL: {
            break;
        }
        case OPTION_DELETE_USER_OR_CANCEL: {
            lblUsermail.setVisible(false);
            tfUsermail.setVisible(false);
            lblPassword.setVisible(false);
            tfPassword.setVisible(false);
            lblPasswordRepeated.setVisible(false);
            tfPasswordRepeated.setVisible(false);
            break;
        }
        case OPTION_LOGIN: {
            lblUsermail.setVisible(false);
            tfUsermail.setVisible(false);
            lblPasswordRepeated.setVisible(false);
            tfPasswordRepeated.setVisible(false);
            break;
        }
        default: {
            break;
        }
        }

        ly.setHonorsVisibility(lblUsermail, false);
        ly.setHonorsVisibility(lblPassword, false);
        ly.setHonorsVisibility(lblPasswordRepeated, false);
        ly.setHonorsVisibility(lblUsername, false);
        ly.linkSize(new Component[]{btnCancel, btnOk}, SwingConstants.HORIZONTAL);
    }

    public void setExtraInputControlsVisible(boolean value) {
        lblPasswordRepeated.setVisible(value);
        tfPasswordRepeated.setVisible(value);
        lblUsermail.setVisible(value);
        tfUsermail.setVisible(value);
    }
    
    private final static String normalize(String str) {
        if (str == null) {
            str = "";
        }
        return str;
    }
    
    private void showStatus(String str) {
        statusLabel.setText(str);
        if (!statusLabel.isVisible()) {
            statusLabel.setVisible(true);
            panel.revalidate();
            panel.invalidate();
            panel.repaint();
        }
    }
    
    public Auth getAuth() {
        String username = normalize(tfUsername.getText());
        String password = normalize(new String(tfPassword.getPassword()));
        String usermail = normalize(tfUsermail.getText());
        if (username!=null&&username.length()==0) username = null;
        if (password!=null&&password.length()==0) password = null;
        if (usermail!=null&&usermail.length()==0) usermail = null;
        return new Auth(username,password,usermail);
    }
    
    @SwingActionData("Ok")
    public void btnOkClick() {

        String username = normalize(tfUsername.getText());
        String password = normalize(new String(tfPassword.getPassword()));
        String adminUsername = normalize(tfAdminUsername.getText());
        String adminPassword = normalize(new String(tfAdminPassword.getPassword()));
        String passwordRepeated = normalize(new String(tfPasswordRepeated.getPassword()));
        String usermail = normalize(tfUsermail.getText());

        // TODO validate fields
        
        
        final Request request;
        switch (messageType) {
        case OPTION_CREATE_USER_OR_CANCEL: {    
            if (!password.equals(passwordRepeated)) {
                showStatus("Passwords are not equal.");
                return;
            }
            request = new RequestRegisterNewUser(new Auth(username, password, usermail));
            break;
        }
        case OPTION_DELETE_USER_OR_CANCEL: {
            request = new RequestDeleteUser(new Auth(adminUsername, adminPassword), username);
            break;
        }
        case OPTION_LOGIN: {
            option = JOptionPane.OK_OPTION;
            closeDialog();
            return;
        }
        default: { request = null; break; }
        }

        if (request == null)  {
            return;
        }
        
        showStatus("Sending request...");
        new Thread() {
            public void run() {
        
                Response response;
                try {
                    response = client.execute(request);
                } catch (Exception ex) {
                    showStatus(ex.getMessage());
                    return;
                }
                if (!response.isOkStatus()) {
                    showStatus(response.getMessage());
                    return;
                }
                
                option = JOptionPane.OK_OPTION;
                closeDialog();
            }
        }.start();
    }

    @SwingActionData("Cancel")
    public void btnCancelClick() {
        option = JOptionPane.CANCEL_OPTION;
        closeDialog();
    }
    
    public NewLoginForm showDialog(Component parentComponent) {
        String title = "";

        switch (messageType) {
        case OPTION_CREATE_USER_OR_CANCEL: {
            title = "Create Login";
            break;
        }
        case OPTION_DELETE_USER_OR_CANCEL: {
            title = "Delete User";
            break;
        }
        case OPTION_LOGIN: {
            title = "Login";
            break;
        }
        default: {
            break;
        }
        }
        
        showDialog(parentComponent, title);
        return this;
    }
    
    public static void main(String[] args) {
        PatchManagerClient client = new DefaultPatchManagerClient(new DefaultHttpClient(), new ClientProtocol1());
        new NewLoginForm(client, NewLoginForm.OPTION_CREATE_USER_OR_CANCEL).showDialog(null);
        new NewLoginForm(client, NewLoginForm.OPTION_LOGIN).showDialog(null);
        new NewLoginForm(client, NewLoginForm.OPTION_DELETE_USER_OR_CANCEL).showDialog(null);
    }

    public String getUsername() {
        return tfUsername.getText();
    }

    public String getPassword() {
        return new String(tfPassword.getPassword());
    }

    public String getMail() {
        return tfUsermail.getText();
    }

    public void setUsername(String value) {
        tfUsername.setText(value);
    }

    public void setPassword(String value) {
        tfPassword.setText(value);
        tfPasswordRepeated.setText(value);
    }

    public void setUsermail(String value) {
        tfUsermail.setText(value);
    }

    public void setAuth(Auth auth) {
        setUsername(auth.getUsername());
        setPassword(auth.getPassword());
        setUsermail(auth.getUsermail());
    }

}
