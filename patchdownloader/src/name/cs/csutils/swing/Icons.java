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

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Properties;

import javax.swing.Action;
import javax.swing.ImageIcon;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class Icons {
    
    private static Log log;
    private static Log getLog() {
        if (log == null) {
            log = LogFactory.getLog(Icons.class);
        }
        return log;
    }

    private String iconsPropertiesPathPrefix = "/resources/icons/";
    private String iconsPropertiesFile = 
        iconsPropertiesPathPrefix+"icons.properties";
    
    private static Icons INSTANCE = new Icons();
    
    private Icons() {
        super();
        loadProperties();
    }
    
    public static Icons getSharedInstance() {
        return INSTANCE;
    }

    public static void update(Action action, String resourceKey) {
        Icons icons = INSTANCE;
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
