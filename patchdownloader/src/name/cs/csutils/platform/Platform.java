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
package name.cs.csutils.platform;

import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import javax.swing.SwingUtilities;

public class Platform
{
    /* some values for System.getProperty("os.name")
      // unix flavor
      AIX 
      Digital Unix
      FreeBSD
      HP UX // also HP-UX
      Irix
      Linux
      Solaris
      UnixWare
      OpenUnix
      OpenServer
      Compaq's Digital UNIX
      OSF1
      
      // BeOS Flavor
      BeOS
      // ??? Haiku ???
      
      // mac os flavor
      Mac OS          
      
      // windows flavor
      Windows 2000    
      Windows 95
      Windows 98
      Windows NT
      Windows XP
      OS/2
      
      // unknown flavor
      MPE/iX
      Netware 4.11
     */
    public static enum OS
    {
        WindowsFlavor,
        UnixFlavor,
        MacOSFlavor,
        BeOSFlavor,
        UnknownFlavor
    }
    
    private static OS flavor;
    
    static
    {
        String os = System.getProperty("os.name").toLowerCase();
        if (os.indexOf("windows")>=0 || os.indexOf("nt")>=0 || os.indexOf("os/2")>=0)
            Platform.flavor = OS.WindowsFlavor;
        else if (os.indexOf("beos")>=0)
            Platform.flavor = OS.BeOSFlavor;
        else if (os.indexOf("mac")>=0)
            Platform.flavor = OS.MacOSFlavor;
        else if (os.indexOf("linux")>=0 || 
                os.indexOf("solaris")>=0 || 
                os.indexOf("freebsd")>=0 ||
                os.indexOf("unix")>=0)
            Platform.flavor = OS.UnixFlavor;
        else
            Platform.flavor = OS.UnknownFlavor;
    }
    
    public static OS flavor()
    {
        return flavor;
    }
    
    public static boolean isFlavor(OS flavor)
    {
        return Platform.flavor == flavor;
    }
    
    public static boolean canPerformKeyAction(KeyEvent e) {
        // TODO
        if (e.getID() == KeyEvent.KEY_PRESSED) {
            return true;
        } else if (e.getID() == KeyEvent.KEY_RELEASED) {
            return false;
        } else if (e.getID() == KeyEvent.KEY_TYPED) {
            // no op
        }
        return false;
    }
    
    public static boolean couldBePopupTrigger(MouseEvent e) {
    	if (Platform.isFlavor(Platform.OS.MacOSFlavor)) {
    		return (e.isControlDown() && (e.getButton() == MouseEvent.BUTTON1)) || e.isPopupTrigger();
    	} else {
    		return (e.getButton() == MouseEvent.BUTTON3) || isPopupTrigger(e);
    	}
    }
    public static boolean isPopupTrigger(MouseEvent e)
    {
    	return e.isPopupTrigger();
    	/*
    	if (Platform.isFlavor(OS.MacOSFlavor))
        {
            return (e.getID() == MouseEvent.MOUSE_RELEASED) && e.isPopupTrigger();
        }
        else
        {
            return (e.getID() == MouseEvent.MOUSE_PRESSED) && e.isPopupTrigger();
        }
        */
    	
    }

	public static boolean isToggleSelectionEvent(MouseEvent e) {
    	if (Platform.isFlavor(OS.MacOSFlavor)) {
    		return SwingUtilities.isLeftMouseButton(e) && e.isMetaDown();
    	} else {
    		return SwingUtilities.isLeftMouseButton(e) && e.isControlDown();
    	}
	}

	public static boolean isLeftMouseButtonOnly(MouseEvent e) {
    	if (Platform.isFlavor(OS.MacOSFlavor)) {
    		return SwingUtilities.isLeftMouseButton(e) && !e.isControlDown();
    	} else {
    		return SwingUtilities.isLeftMouseButton(e);
    	}
	}
  
}
