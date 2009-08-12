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

import java.awt.EventQueue;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JLabel;
import javax.swing.SwingUtilities;

public class CSCall  {

    private static final Method JLABEL_SETTEXT = getMethod(JLabel.class, "getText");
    
    private static final Method getMethod(Class<?> cls, String name, Class<?>...parameterTypes) {
        try {
            return cls.getMethod(name, parameterTypes);
        } catch (Exception ex) {
            return null;
        }
    }
    
    private List<Call> callList = new LinkedList<Call>();
    
    public CSCall() {
        super();
    }

    public CSCall setLabelText(JLabel label, String text) {
        add(new MethodCall(label, JLABEL_SETTEXT, text));
        return this;
    }
    
    public CSCall add(Call call) {
        return this;
    }
    
    public void invokeAndWait() throws InterruptedException, InvocationTargetException {
        if (EventQueue.isDispatchThread()) {
            invokeNow();
        } else {
            SwingUtilities.invokeAndWait(createRunnable());
        }
    }

    public void invokeLater() {
        SwingUtilities.invokeLater(createRunnable());
    }

    private void invokeNow() {
        createRunnable().run();
    }
    
    private Runnable createRunnable() {
        return new Runner(this);
    }
    
    private static final class Runner implements Runnable {
        private List<Call> callList;
        public Runner(CSCall parent) {
            this.callList = new ArrayList<Call>(parent.callList);
        }
        public void run() {
            if (!EventQueue.isDispatchThread()) {
                throw new IllegalThreadStateException("not in event dispatch thread");
            }
            
            try {
                for (Call call: callList) {
                    call.call();
                }
            } catch (RuntimeException rex) {
                throw rex;
            } catch (Exception ex) {
                throw new RuntimeException(ex);
            }
        }

    }
    
    public static interface Call {
        void call() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException;
    }

    private static abstract class SetPropertyCall<C,V> implements Call {
        C obj;
        V arg;
        public SetPropertyCall(C obj, V arg) {
            this.obj = obj;
            this.arg = arg;
        }
    }

    private static final class MethodCall extends SetPropertyCall<Object,Object> {
        private Method method;
        public MethodCall(Object obj, Method method, Object ... arg) {
            super(obj, arg);
            this.method = method;
        }
        public void call() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
            method.invoke(obj, arg);
        }
    }
    
}
