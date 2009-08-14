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

import java.io.IOException;
import java.io.Serializable;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import javax.swing.Action;
import javax.swing.ImageIcon;

public final class IconLoader implements Serializable {
    /**
     * 
     */
    private static final long serialVersionUID = -2622648692957477946L;
    private static final String iconsPropertiesPathPrefix = "/resources/icons/";
    private static final String iconsPropertiesFile = iconsPropertiesPathPrefix+"icons.properties";
    
    private static IconLoader INSTANCE = new IconLoader();

    private transient Map<String, ImageIcon> iconmap;
    
    private IconLoader() {
        super();
        init();
    }
    
    private void init() {
        iconmap = new HashMap<String, ImageIcon>();
        CSProperties properties = CSUtils.loadProperties(getClass().getResource(iconsPropertiesFile));
        for (Map.Entry<Object,Object> entry: properties.entrySet()) {
            if (entry.getKey() instanceof String 
                    && entry.getValue() instanceof String) {
                String key = (String) entry.getKey();
                URL url = getClass().getResource(iconsPropertiesPathPrefix+
                        (String)entry.getValue());
                if (url != null) {
                    iconmap.put(key, new ImageIcon(url));
                }
            }
        }
        
    }
    
    public static IconLoader getInstance() {
        return INSTANCE;
    }

    public static void update(Action action, String resourceKey) {
        IconLoader icons = INSTANCE;
        ImageIcon icon = icons.getIcon(resourceKey);
        if (icon != null) {
            action.putValue(Action.SMALL_ICON, icon);
        }
    }
    
    public ImageIcon getEnabledIcon(String name) {
        return getIcon(name);        
    }

    public ImageIcon getDisabledIcon(String name) {
        return getIcon(name+"$disabled");
    }
    
    public ImageIcon getIcon(String name) {
        return iconmap.get(name);
    }

    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    
    private void readObject(java.io.ObjectInputStream in)
        throws IOException, ClassNotFoundException {
        in.defaultReadObject();
        init();
    }
    
}
