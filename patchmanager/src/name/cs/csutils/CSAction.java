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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.lang.reflect.Method;

import javax.swing.AbstractAction;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

import name.cs.csutils.i18n.I18N;

public class CSAction extends AbstractAction {

    /**
     * 
     */
    private static final long serialVersionUID = -4203736212526885282L;
    public static final String RESOURCE_KEY = 
        CSAction.class.getName()+".resourcekey";
    public static final String INVOKE_LATER_KEY = 
        CSAction.class.getName()+".invokelater";
    
    private ActionListener actionListener;

    public CSAction(String name) {
        super();
        useName(name);
    }

    public CSAction(String name, String resourceKey) {
        super();
        useName(name);
        useResourceKey(resourceKey);
    }
    
    public CSAction() {
        super();
    }
    
    public CSAction(InvokationTarget t) {
        this();
        useInvokationTarget(t);
    }

    public CSAction(Object obj, String methodName, Class<?>...parameterTypes) {
        this(new InvokationTarget(obj, methodName, parameterTypes));
    }
    
    public CSAction useName(String name) {
        putValue(NAME, name);
        return this;
    }
    
    public CSAction useActionListener(ActionListener l) {
        this.actionListener = l;
        return this;
    }

    public CSAction useInvokationTarget(Object obj, String methodName, Class<?>...parameterTypes) {
        return useInvokationTarget(new InvokationTarget(obj, methodName, parameterTypes));
    }
    
    public CSAction useInvokationTarget(InvokationTarget target) {
        Method m = target.getMethod();
        
        if (m != null) {
            String name = null;
            if (m.isAnnotationPresent(SwingActionData.class)) {
                SwingActionData info = m.getAnnotation(SwingActionData.class);
                if (info.value() != CSUtils.UNASSIGNED_STRING) {
                    name = info.value();
                }
                if (info.resourcekey() != CSUtils.UNASSIGNED_STRING) {
                    useResourceKey(info.resourcekey());
                }
                if (info.useInvokeLater()) {
                    useInvokeLater();
                }
            }
            if (name == null) {
                name = m.getName();
            }
            if (getValue(NAME) == null) {
                putValue(NAME, name);
            }
        }
        
        useActionListener(target);
        return this;
    }

    public CSAction useInvokeLater() {
        return useInvokeLater(true);
    }
    
    public CSAction useInvokeLater(boolean enable) {
        if (enable)
            putValue(INVOKE_LATER_KEY, Boolean.TRUE);
        else 
            putValue(INVOKE_LATER_KEY, null);
        return this;
    }
    
    public CSAction useAcceleratorKey(KeyStroke k) {
        putValue(ACCELERATOR_KEY, k);
        return this;
    }

    public CSAction useActionCommand(String actionCommand) {
        putValue(ACTION_COMMAND_KEY, actionCommand);
        return this;
    }

    public CSAction useResourceKey(String key) {
        putValue(RESOURCE_KEY, key);
        if (key != null) {
            IconLoader.update(this, key);
            I18N.update(this, key);
        }
        return this;
    }
    
    public CSAction setSelected(boolean selected) {
        putValue(SELECTED_KEY, selected?Boolean.TRUE:Boolean.FALSE);
        return this;
    }
    
    public boolean isSelected() {
        return Boolean.TRUE.equals(getValue(SELECTED_KEY));
    }

    public boolean isNotSelected() {
        return Boolean.FALSE.equals(getValue(SELECTED_KEY));
    }
    
    @Override
    public void actionPerformed(final ActionEvent e) {
        if (Boolean.TRUE.equals(getValue(INVOKE_LATER_KEY))) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    performAction(e);
                }
            });
        } else {
            performAction(e);
        }
    }
    
    private void performAction(ActionEvent e) {
        if (actionListener != null) {
            actionListener.actionPerformed(e);
        }
    }

    public CSAction setAsEnabled(boolean enabled) {
        setEnabled(enabled);
        return this;
    }

    public Object getActionCommand() {
        return getValue(ACTION_COMMAND_KEY);
    }
    
}
