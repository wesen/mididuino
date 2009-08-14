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
package com.ruinwesen.patchmanager.swing.forms2;

import java.util.regex.Pattern;

import javax.swing.JPasswordField;

import com.ruinwesen.patchmanager.client.protocol.Auth;
import com.ruinwesen.patchmanager.swing.form.AbstractFormElementValidator;
import com.ruinwesen.patchmanager.swing.form.Form;
import com.ruinwesen.patchmanager.swing.form.FormContainer;
import com.ruinwesen.patchmanager.swing.form.FormElement;
import com.ruinwesen.patchmanager.swing.form.RegexValidator;
import com.ruinwesen.patchmanager.swing.form.StringLengthValidator;
import com.ruinwesen.patchmanager.swing.form.TextFieldFormElement;

public class LoginForm extends FormContainer {

    private TextFieldFormElement tfUsername;
    private TextFieldFormElement tfPassword;
    private TextFieldFormElement tfPasswordRepeated;
    private TextFieldFormElement tfUsermail;
    private boolean createLoginMode;
    
    public LoginForm(boolean createLogin) {
        super();
        this.createLoginMode = createLogin;
        init();
    }

    public void setFields(Auth auth) {
        tfUsername.setText(auth.getUsername());
        tfPassword.setText(auth.getPassword());
        tfPasswordRepeated.setText(auth.getPassword());
        tfUsermail.setText(auth.getUsermail());
    }

    public Auth getFields() {
        return new Auth(tfUsername.getText(), tfPassword.getText(), tfUsermail.getText());
    }
    
    private void init() {
        tfUsername = new TextFieldFormElement();
        tfUsername.setLabel("Username");
        tfUsername.setValidator(new RegexValidator("Username must have 4-16 characters [a-z0-9_.-]", 
                "([a-z0-9_\\.\\-]){4,16}", Pattern.CASE_INSENSITIVE));
        tfPassword = new TextFieldFormElement(new JPasswordField());
        tfPassword.setLabel("Password");
        tfPassword.setValidator(new StringLengthValidator("Password must habe 4-16 characters",4,16));
        tfPasswordRepeated = new TextFieldFormElement(new JPasswordField());
        tfPasswordRepeated.setValidator(new PasswordRepeatedValidator("Passwords must be equal."));
        tfPasswordRepeated.setLabel("Repeat Password");
        tfUsermail = new TextFieldFormElement();
        tfUsermail.setLabel("Email");
        tfUsermail.setValidator(new StringLengthValidator("Email must not be empty.",0,400));
        
        Form form = new Form("login");
        form.add(tfUsername);
        form.add(tfPassword);
        if (createLoginMode) {
            form.add(tfPasswordRepeated);
            form.add(tfUsermail);
        }
        setForm(form);
    }
    
    private class PasswordRepeatedValidator extends AbstractFormElementValidator {
        public PasswordRepeatedValidator(String errorMessage) {
            super(errorMessage);
        }
        @Override
        public boolean isValid(FormElement elem) {
            return tfPassword.getText().equals(tfPasswordRepeated.getText());
        }
        
    }
    
}
