/** 
 * Copyright (C) 2009 Christian Schneider
 *
 * This file is part of Patchdownloader.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
package name.cs.csutils.swing;

import java.awt.event.ActionEvent;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import javax.swing.AbstractAction;

import name.cs.csutils.i18n.I18N;

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
    }
    
    private static Log log = LogFactory.getLog(RedirectAction.class);
    private String methodName;
    private transient Method cachedMethod;
    private Object methodOwner;
    private Class<?> parameterType;
    private Object arg;

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
                if (annotation.resourcekey() != null) {
                    I18N.update(this, annotation.resourcekey());
                    Icons.update(this, annotation.resourcekey());
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

    public String toString() {
        return "RedirectAction["
        +"ownerobj="+getOwnerObject()
        +",ownerclass="+getOwnerClass()
        +",methodname="+methodName
        +",optional-parameter="+parameterType
        +"]";
    }
    
}
