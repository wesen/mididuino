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
import java.awt.GridLayout;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

import name.cs.csutils.i18n.I18N;
import name.cs.csutils.i18n.Translation;

public class AuthenticationDialog {

    private JPanel panel;
    private JTextField txtUsername;
    private JTextField txtPassword;
    private JTextField txtConfirmedPassword;
    private boolean confirmPasswordEnabled;
    private int minUsernameLength = 3;
    private int maxUsernameLength = 32;
    private int minPasswordLength = 5;
    private int maxPasswordLength = 32;

    private static final Translation CREATE_USER_STRING = I18N.getTranslation("Create User");
    private static final Translation LOGIN_STRING = I18N.getTranslation("Login");
    private static final Translation USERNAME_STRING = I18N.getTranslation("Username");
    private static final Translation PASSWORD_STRING = I18N.getTranslation("Password");
    private static final Translation REPEAT_PASSWORD_STRING = I18N.getTranslation("Repeat Password");
    private static final Translation USERNAME_LENGTH_WARNING_STRING = I18N.getTranslation("username must have %d to %d characters");
    private static final Translation PASSWORD_LENGTH_WARNING_STRING = I18N.getTranslation("password must have %d to %d characters");
    
    public AuthenticationDialog(boolean confirmPasswordEnabled,
            String initialUsername,
            String initialPassword) {
        this.confirmPasswordEnabled = confirmPasswordEnabled;
        init();
        txtUsername.setText(initialUsername);
        txtPassword.setText(initialPassword);
        txtConfirmedPassword.setText(initialPassword);
    }
    
    public int showCreateUserDialog(Component parentComponent) {
        return showDialog(parentComponent, CREATE_USER_STRING.value());
    }
    
    public int showLoginDialog(Component parentComponent) {
        return showDialog(parentComponent, LOGIN_STRING.value());
    }

    public int showDialog(Component parentComponent, String title) {
        int result = JOptionPane.CANCEL_OPTION;
        for(;;) {
            result = JOptionPane.showConfirmDialog(parentComponent,
                panel, 
                title,
                JOptionPane.OK_CANCEL_OPTION,
                JOptionPane.QUESTION_MESSAGE);

            if (result == JOptionPane.CANCEL_OPTION) {
                break;
            }
            
            String warning = null;
            if (txtUsername.getText().length()<minUsernameLength
                    || txtUsername.getText().length()>maxUsernameLength) {
                warning =  USERNAME_LENGTH_WARNING_STRING.format(minUsernameLength, maxUsernameLength);
            } else if (txtPassword.getText().length()<minPasswordLength
                    || txtPassword.getText().length()>maxPasswordLength) {
                warning =  PASSWORD_LENGTH_WARNING_STRING.format(minPasswordLength, maxPasswordLength);
            } else if (confirmPasswordEnabled && !txtConfirmedPassword.getText().equals(txtPassword.getText())) {
                warning = "password and password confirmation are not equal";
            }
            if (warning == null) {
                break; // input is valid
            }
            JOptionPane.showMessageDialog(parentComponent, 
                    warning, 
                    "input error",
                    JOptionPane.WARNING_MESSAGE);
        }
        return result;
    }
    
    public JComponent getComponent() {
        return panel;
    }
    
    private void init() {
        txtUsername = new JTextField();
        txtPassword = new JPasswordField();
        txtConfirmedPassword = new JPasswordField();
        
        panel = new JPanel();
        panel.setLayout(new GridLayout(0,2,2,2));
        panel.add(new JLabel(USERNAME_STRING.value()));
        panel.add(txtUsername);
        panel.add(new JLabel(PASSWORD_STRING.value()));
        panel.add(txtPassword);
        if (confirmPasswordEnabled) {
            panel.add(new JLabel(REPEAT_PASSWORD_STRING.value()));
            panel.add(txtConfirmedPassword);
        }
    }

    public void setUsername(String value) {
        txtUsername.setText(value);
    }

    public void setPassword(String value) {
        txtPassword.setText(value);
    }

    public void setConfirmedPassword(String value) {
        txtConfirmedPassword.setText(value);
    }

    public String getUsername() {
        return txtUsername.getText();
    }

    public String getPassword() {
        return txtPassword.getText();
    }

    public String getConfirmedPassword() {
        return txtConfirmedPassword.getText();
    }

    public static void main(String[] args) {
        AuthenticationDialog af = new AuthenticationDialog(false, null, null);
        af.setUsername("hugo");
        af.setPassword("bimmelbert");
        af.showLoginDialog(null);
        /*
        JFrame fr = new JFrame();
        fr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        fr.getContentPane().add(af.getComponent());
        fr.pack();
        fr.setLocationByPlatform(true);
        fr.setVisible(true);
        */
    }
    
}
