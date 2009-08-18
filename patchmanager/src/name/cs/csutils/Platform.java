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

    private static final OS flavor;
    private static final String GNOME_ENV_VAR = "GNOME_DESKTOP_SESSION_ID";
    private static final String KDE_ENV_VAR = "KDE_FULL_SESSION";
    private static final boolean isGnomeEnvVarSet;
    private static final boolean isKDEEnvVarSet;
    
    static
    {
        String os = System.getProperty(SystemProperties.OS_NAME); 
        if (os != null) {
            os = os.toLowerCase();
            if (os.indexOf("windows")>=0 || os.indexOf("nt")>=0 || os.indexOf("os/2")>=0)
                flavor = OS.WindowsFlavor;
            else if (os.indexOf("beos")>=0)
                flavor = OS.BeOSFlavor;
            else if (os.indexOf("mac")>=0)
                flavor = OS.MacOSFlavor;
            else if (os.indexOf("linux")>=0 || 
                    os.indexOf("solaris")>=0 || 
                    os.indexOf("freebsd")>=0 ||
                    os.indexOf("unix")>=0)
                flavor = OS.UnixFlavor;
            else
                flavor = OS.UnknownFlavor;
        } else {
            flavor = OS.UnknownFlavor;
        }
        
        if (Platform.isFlavor(OS.UnixFlavor)) {
            // desktop
            String gnomeEnvValue = System.getenv(GNOME_ENV_VAR);
            String kdeEnvValue   = System.getenv(KDE_ENV_VAR);
            isGnomeEnvVarSet = gnomeEnvValue != null && gnomeEnvValue.trim().length()!=0;
            isKDEEnvVarSet = kdeEnvValue != null && kdeEnvValue.trim().length()!=0;
        } else {
            isGnomeEnvVarSet = false;
            isKDEEnvVarSet = false;
        }
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

    public static boolean maybeDeskopKDE() {
        return isKDEEnvVarSet;
    }

    public static boolean maybeDeskopGnome() {
        return isGnomeEnvVarSet;
    }
    
}
