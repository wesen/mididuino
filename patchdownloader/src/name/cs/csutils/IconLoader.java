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

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Properties;

import javax.swing.Action;
import javax.swing.ImageIcon;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class IconLoader {
    
    private static Log log;
    private static Log getLog() {
        if (log == null) {
            log = LogFactory.getLog(IconLoader.class);
        }
        return log;
    }

    private String iconsPropertiesPathPrefix = "/resources/icons/";
    private String iconsPropertiesFile = 
        iconsPropertiesPathPrefix+"icons.properties";
    
    private static IconLoader INSTANCE = new IconLoader();
    
    private IconLoader() {
        super();
        loadProperties();
    }
    
    public static IconLoader getSharedInstance() {
        return INSTANCE;
    }

    public static void update(Action action, String resourceKey) {
        IconLoader icons = INSTANCE;
        ImageIcon icon = icons.getIcon(resourceKey);
        if (icon != null) {
            action.putValue(Action.SMALL_ICON, icon);
        }
    }
    
    private Properties properties;
    
    private void loadProperties() {
        properties = new Properties();
        try {
            InputStream in = null;
            try {
                in = getClass().getResourceAsStream(iconsPropertiesFile);
                if (in == null) {
                    throw new FileNotFoundException("resource not found: "+iconsPropertiesFile);
                }
                properties.load(in);
            } finally {
                if (in != null) {
                    in.close();
                    in = null;
                }
            }
        } catch (IOException ex) {
            if (getLog().isDebugEnabled()) {
                getLog().debug("Could not read icon properties: "+iconsPropertiesFile, ex);
            }
            properties.clear();
        }
    }

    
    
    public ImageIcon getEnabledIcon(String name) {
        return getIcon(name);        
    }

    public ImageIcon getDisabledIcon(String name) {
        return getIcon(name+"$disabled");
    }
    
    public ImageIcon getIcon(String name) {
        String value = properties.getProperty(name);
        if (value == null) {
            return null;
        }
        URL url = getClass().getResource(iconsPropertiesPathPrefix+value);
        if (url == null) {
            return null;
        }
        return new ImageIcon(url);
    }
    
}
