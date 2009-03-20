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
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import javax.swing.AbstractAction;
import javax.swing.SwingUtilities;


import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;


public class RedirectAction extends AbstractAction  {

    public static final String UNASSIGNED = "[Unassigned]";
    
    /**
     * 
     */
    private static final long serialVersionUID = -682721581359931845L;

    @Retention(RetentionPolicy.RUNTIME)
    @Target(ElementType.METHOD)
    public static @interface RedirectActionMeta {
        String title() default UNASSIGNED;
        String resourcekey() default UNASSIGNED;
        boolean useInvokeLater() default true;
    }
    
    private static Log log = LogFactory.getLog(RedirectAction.class);
    private String methodName;
    private transient Method cachedMethod;
    private Object methodOwner;
    private Class<?> parameterType;
    private Object arg;
    private boolean useInvokeLater = true;

    public RedirectAction(Object ownerObject, String methodName) {
        this(ownerObject, methodName, null, null);
    }
    
    public RedirectAction(Class<?> ownerClass, String methodName) {
        this(ownerClass, methodName, null, null);
    }

    public RedirectAction(Object ownerObject, String methodName, Class<?> parameterType, Object arg) {
        this.methodOwner = ownerObject;
        this.methodName = methodName;
        this.parameterType = parameterType;
        this.arg = arg;
        init();
    }
    
    public RedirectAction(Class<?> ownerClass, String methodName, Class<?> parameterType, Object arg) {
        this.methodOwner = ownerClass;
        this.methodName = methodName;
        this.parameterType = parameterType;
        this.arg = arg;
        init();
    }

    private void init() {
        Method method = lookupMethod();
        if (method != null) {
            if (method.isAnnotationPresent(RedirectActionMeta.class)) {
                RedirectActionMeta annotation =
                    method.getAnnotation(RedirectActionMeta.class);
                this.useInvokeLater = annotation.useInvokeLater();
                if (annotation.resourcekey() != null) {
                    I18N.update(this, annotation.resourcekey());
                    IconLoader.update(this, annotation.resourcekey());
                }
                if (getValue(NAME) == null) {
                    if (annotation.title() == UNASSIGNED) {
                        putValue(NAME, methodName);
                    } else {
                        putValue(NAME, annotation.title());
                    }
                }
            } else {
                putValue(NAME, method.getName());
            }
        } else {
            putValue(NAME, methodName);
        }
    }

    protected Object getOwnerObject() {
        return methodOwner instanceof Class<?> ? null : methodOwner;
    }
    
    protected Class<?> getOwnerClass() {
        return methodOwner instanceof Class<?> ? (Class<?>) methodOwner :
            methodOwner.getClass();
    }
    
    private synchronized Method lookupMethod() {
        if (this.cachedMethod == null) {
            try {
                if (parameterType != null) {
                    this.cachedMethod = getOwnerClass().getMethod(methodName, parameterType);
                } else {
                    this.cachedMethod = getOwnerClass().getMethod(methodName);
                }
            } catch (Exception e) {
                log.error("tryLoadMethod() failed, "+info(), e);
                return null;
            }
        }
        return this.cachedMethod;
    }
    
    private String info() {
        return "owner: "+methodOwner+",methodname:"+methodName;
    }
    
    @Override
    public void actionPerformed(ActionEvent e) {
        if (useInvokeLater) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    performAction();
                }
            });
        } else {
            performAction();
        }
    }

    private void performAction() {
        Method method = lookupMethod();
        if (method == null) {
            return; // operation failed
        }
        
        try {
            if (parameterType != null) {
                method.invoke(getOwnerObject(), arg);
            } else {
                method.invoke(getOwnerObject());
            }
        } catch (IllegalArgumentException ex) {
            log.error("invoke failed, "+info(), ex);
        } catch (IllegalAccessException ex) {
            log.error("invoke failed, "+info(), ex);
        } catch (InvocationTargetException ex) {
            log.error("invoke failed, "+info(), ex.getTargetException());
        }
    }
    
    @Override
    public String toString() {
        return "RedirectAction["
        +"ownerobj="+getOwnerObject()
        +",ownerclass="+getOwnerClass()
        +",methodname="+methodName
        +",optional-parameter="+parameterType
        +"]";
    }
    
}
