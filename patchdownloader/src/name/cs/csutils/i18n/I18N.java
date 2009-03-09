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
package name.cs.csutils.i18n;

import java.awt.event.KeyEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.Action;
import javax.swing.KeyStroke;
import javax.swing.event.EventListenerList;

import name.cs.csutils.platform.Platform;
import name.cs.csutils.swing.NoAction;

public class I18N {

    private static I18N INSTANCE = new I18N();
    private static final String LOCALE_PROPERTY = "LOCALE";
    private EventListenerList eventListeners = new EventListenerList();
    private static Locale defaultLocale = Locale.getDefault();
    
    private ResourceBundle resourceBundle;
    private String resourceBundleName = 
        "resources/lang/MessageBundle";
    
    // no subclasses allowed
    private I18N () {
        super();
    }
    
    public void setResourceBundleName(String name) {
        if (name == null) {
            throw new IllegalArgumentException("name == null");
        }
        this.resourceBundleName = name;
        loadResourceBundle();
    }

    public ClassLoader getResourceClassLoader() {
        return getClass().getClassLoader();
    }
    
    private void loadResourceBundle() {
        resourceBundle = ResourceBundle.getBundle(resourceBundleName, 
                getLocale(), getResourceClassLoader());
    }
    
    public static ResourceBundle getResourceBundle() {
        return INSTANCE.resourceBundle;
    }
    
    public static I18N getSharedInstance() {
        return INSTANCE;
    }
    
    public static Locale getDefaultLocale() {
        return defaultLocale;
    }

    public static Locale getLocale() {
        return Locale.getDefault();
    }
    
    public static void setLocale(Locale locale) {
        INSTANCE.__setLocale(locale);
    }
    
    private void __setLocale(Locale locale) {
        Locale oldValue = getLocale();
        if (oldValue != locale || oldValue != null && !oldValue.equals(locale)) {
            Locale.setDefault(locale);  
            loadResourceBundle();
            fireLocaleChangeEvent(oldValue, locale);
        }
    }
    
    private void fireLocaleChangeEvent(Locale prevLocale, Locale newLocale) {
        if (eventListeners != null)
        {
            PropertyChangeEvent pcEvent = null;
            Object[] listeners = eventListeners.getListenerList();
            for (int i=listeners.length-2;i>=0;i-=2)
            {
                if (listeners[i]==PropertyChangeListener.class) {
                    // Lazily create the event:
                    if (pcEvent == null)
                        pcEvent = new PropertyChangeEvent(this, LOCALE_PROPERTY, prevLocale, newLocale);
                    ((PropertyChangeListener)listeners[i+1]).propertyChange(pcEvent);
                }
            }
        }
    }
    
    public void addLocaleChangeListener(PropertyChangeListener l) {
        eventListeners.add(PropertyChangeListener.class, l);
    }
    
    public void removeLocaleChangeListener(PropertyChangeListener l) {
        eventListeners.remove(PropertyChangeListener.class, l);
    }

    public final static String SUFFIX_KEYBINDING = "$keybinding";
    public final static String SUFFIX_KEYBINDING_OSX = "$keybinding.osx";
    public final static String SUFFIX_SHORT_DESC = "$description";
    public final static String SUFFIX_LONG_DESC = "$long-description";

    private static Map<String, Integer> modifierMap = new HashMap<String, Integer>();
    
    static 
    {
        modifierMap.put("SHIFT", KeyEvent.SHIFT_MASK);
        modifierMap.put("ALT", KeyEvent.ALT_MASK);
        modifierMap.put("ALTGR", KeyEvent.ALT_GRAPH_MASK);
        modifierMap.put("META", KeyEvent.META_MASK);
        modifierMap.put("CTRL", KeyEvent.CTRL_MASK);
    }

    private static Pattern kbPattern = 
        Pattern.compile("(((ALT)|(ALTGR)|(SHIFT)|(CTRL)|(META))\\+)*(\\w|\\d|F\\d\\d?)", Pattern.CASE_INSENSITIVE);

    public static String getStringOrNull(ResourceBundle bundle, String key) {
        try {
            return bundle.getString(key);
        } catch (MissingResourceException e) {
            return null;
        }
    }

    public static Action action(String title, String resourceKey) {
        return new NoAction(title, resourceKey);
    }

    public static <A extends Action> A update(A action, String resourceKey) {
        if (INSTANCE.resourceBundle == null) {
            return action;
        }
        return update(action, resourceKey, INSTANCE.resourceBundle);
    }

    public static <A extends Action> A update(A action,String key,ResourceBundle bundle) {
        
        String eName = null;
        KeyStroke eAcceleratorKey = null;
        Integer eMnemonicKey = 0;
        String eShortDesc = null;
        String eLongDesc = null;

        if (key != null)
        {
            // get the name
            eName = getStringOrNull(bundle,key);

            if (eName != null)
            {
              // get the mnemonic key,
              // get index of the ampersand  (&) identifying the mnemonic key
                int amp = eName.indexOf('&');
                
                // check if index is valid
                if (amp>=0 && amp<eName.length()-1)
                {
                    eMnemonicKey = new Integer(eName.codePointAt(amp+1));

                    // remove ampersand
                    eName = eName.substring(0, amp) + eName.substring(amp+1, eName.length());
                }
            }
            
            // get descriptions
            eShortDesc = getStringOrNull(bundle,key+SUFFIX_SHORT_DESC);
            eLongDesc = getStringOrNull(bundle,key+SUFFIX_LONG_DESC);
            String keybinding = null;
            if (Platform.isFlavor(Platform.OS.MacOSFlavor)) {
                keybinding = getStringOrNull(bundle,key+SUFFIX_KEYBINDING_OSX);
            } 
            if (keybinding == null) {
                keybinding = getStringOrNull(bundle,key+SUFFIX_KEYBINDING);
            }
            if (keybinding != null)
                eAcceleratorKey = extractKeyStroke(keybinding);
        }
        
        if (eName != null)
            action.putValue(Action.NAME, eName);
        if (eMnemonicKey != null)
            action.putValue(Action.MNEMONIC_KEY, eMnemonicKey);
        if (eAcceleratorKey != null)
            action.putValue(Action.ACCELERATOR_KEY, eAcceleratorKey);
        if (eShortDesc != null)
            action.putValue(Action.SHORT_DESCRIPTION, eShortDesc);
        if (eLongDesc != null)
            action.putValue(Action.LONG_DESCRIPTION, eLongDesc);
        return action;
    }
    
    public static KeyStroke extractKeyStroke(String keybinding)
    {
        Matcher m = kbPattern.matcher(keybinding);

        if (m.matches())
        {
            int modifiers = 0;
            
            for (int i=1;i<m.groupCount();i++)
            {
                String s = m.group(i);

                if (s!=null)
                {
                    Integer mod = modifierMap.get(s.toUpperCase());
                    if (mod!=null)
                        modifiers|=mod.intValue();
                }
            }

            String key = m.group(m.groupCount());

            if (key.length()>0)
            {
                int cp = key.codePointAt(0);
                
                if (Character.charCount(cp) == key.length())
                {
                    return KeyStroke.getKeyStroke(cp, modifiers); 
                }
            
                if (key.length()>=2 && key.toUpperCase().startsWith("F"))
                {
                    try
                    {
                        int FX = Integer.parseInt(key.substring(1));
                        
                        if (1<=FX && FX<=12)
                        {
                            cp = KeyEvent.VK_F1+FX-1;
                            
                            return KeyStroke.getKeyStroke(cp, modifiers);
                        }
                    }
                    catch (NumberFormatException e)
                    {
                        
                    }
                }    
            }
        }
        
        return null;
    }

    
}
