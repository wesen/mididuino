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

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.URI;
import java.net.URL;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.Platform.OS;

public class Launcher {
    private static Log log = LogFactory.getLog(Launcher.class);

    private static Launcher INSTANCE = newInstance();
    private static Launcher PREFERRED_LAUNCHER;
    static {
        if (Platform.isFlavor(OS.UnixFlavor)) {
            PREFERRED_LAUNCHER = null;
        } else {
            PREFERRED_LAUNCHER = new Java6Launcher(); 
        }
    }
    
    private static Launcher newInstance() {
        if (Platform.isFlavor(OS.UnixFlavor)) {
            return new UnixBrowserLauncher();
        } else if (Platform.isFlavor(OS.MacOSFlavor)) {
            return new MacBrowserLauncher();
        } else if (Platform.isFlavor(OS.WindowsFlavor)) {
            return new WindowsBrowserLauncher();
        } else {
            // no support
            return new Launcher();
        }
    }
    
    // hide constructor
    private Launcher() {
        super(); 
    }

    public static Launcher getInstance() {
        return Launcher.INSTANCE;
    }
    
    /**
     * @throws NullPointerException the argument is null
     * @throws IllegalArgumentException the argument is null
     * @param url
     * @return
     */
    public boolean launchURL(URL url) {
        if (Launcher.PREFERRED_LAUNCHER == null) {
            return false;
        }
        return Launcher.PREFERRED_LAUNCHER.launchURL(url);
    }

    public boolean isLaunchURLSupported() {
        if (Launcher.PREFERRED_LAUNCHER == null) {
            return false;
        }
        return Launcher.PREFERRED_LAUNCHER.isLaunchURLSupported();
    }
    
    public boolean launchMailto(URI mailtoURI) {
        if (Launcher.PREFERRED_LAUNCHER == null) {
            return false;
        }
        return Launcher.PREFERRED_LAUNCHER.launchMailto(mailtoURI);
    }
    
    public boolean isLaunchMailtoSupported() {
        if (Launcher.PREFERRED_LAUNCHER == null) {
            return false;
        }
        return Launcher.PREFERRED_LAUNCHER.isLaunchMailtoSupported();
    }

    private static boolean exec(String ... command) {
        Process proc = null;
        CSUtils.ProcessResult res = null;
        try {
            proc = new ProcessBuilder(command).start();
            res = CSUtils.waitFor(proc, 6000, 10);
            return !res.timeout && res.status == 0;
        } catch (Exception ex) {
            return false;
        } finally {
            if (proc != null) {
                proc.destroy();
            }
            if (res != null && res.interrupted) {
                Thread.currentThread().interrupt();
            }
        }
    }
    
    private final static class WindowsBrowserLauncher extends Launcher {

        @Override
        public boolean launchURL(URL url) {
            if (super.launchURL(url)) {
                return true;
            }
        
            return Launcher.exec(
                    "rundll32.exe", "url.dll,FileProtocolHandler",
                    url.toExternalForm());
        }

        @Override
        public boolean isLaunchURLSupported() {
            return true;
        }
        
    }

    private final static class MacBrowserLauncher extends Launcher {

        private static Log log = LogFactory.getLog(MacBrowserLauncher.class);
        private static final String CLASS_FILEMANAGER = "com.apple.eio.FileManager";
        
        @Override
        public boolean launchURL(URL url) {
            if (super.launchURL(url)) {
                return true;
            }

            // try first launch-method
            try {
                Class<?> fileManager = Class.forName(CLASS_FILEMANAGER);
                Method openURL = fileManager.getDeclaredMethod("openURL", new Class[] {String.class});
                openURL.invoke(null, new Object[] {url.toExternalForm()});
                return true;
            } catch (ClassNotFoundException ex) {
                if (log.isDebugEnabled()) {
                    log.debug("Could not launch url with "+CLASS_FILEMANAGER+". (Class not found)");
                }
            } catch (NoSuchMethodException ex) {
                if (log.isDebugEnabled()) {
                    log.debug("Could not launch url with "+CLASS_FILEMANAGER
                            +". (openURL(java.lang.String) Method not found)");
                }
            } catch (IllegalAccessException ex) {
                if (log.isErrorEnabled()) {
                    log.error("Could not launch url with "+CLASS_FILEMANAGER,
                            ex);
                }
            } catch (InvocationTargetException ex) {
                if (log.isErrorEnabled()) {
                    log.error("Could not launch url with "+CLASS_FILEMANAGER,
                            ex.getTargetException());
                }
            } catch (RuntimeException ex) {
                // no op
            } 

            // try second launch-method
            return Launcher.exec("open", url.toExternalForm());
        }

        @Override
        public boolean isLaunchURLSupported() {
            return true;
        }
        
    }

    private final static class UnixBrowserLauncher extends Launcher {
        
        private Boolean launchURLSupported = null;
        
        // GNOME_DESKTOP_SESSION_ID
        // $KDE_FULL_SESSION == 'true'
        // $BROWSER 
        private boolean which(String executable) {
            return Launcher.exec("which", executable);
        }

        private boolean launchURL_BROWSER_ENV_VARIABLE(String url) {
            String BROWSER = System.getenv("BROWSER");
            return (BROWSER != null) && Launcher.exec(BROWSER, url);
        }

        @Override
        public synchronized boolean launchURL(URL url) {
            if (super.launchURL(url)) {
                return true;
            }

            if (!isUnixLaunchURLSupported()) {
                return false;
            }

            final String u = url.toExternalForm();
            if (which("xdg-open")
                    && Launcher.exec("xdg-open", u)) {
                return true;
            }  else if (launchURL_BROWSER_ENV_VARIABLE(u)) { // $BROWSER
                return true;
            // note: kfmclient openURL blocks and we don't get a return status == 0
            // although the browser window may be opened.
            // However kfmclient exec seems to work.
            } else if (Platform.maybeDeskopKDE() && which("kfmclient")
                && Launcher.exec("kfmclient", "exec", u)) { // KDE
                return true;
            } else if (Platform.maybeDeskopGnome() && which("gnome-open")
                && Launcher.exec("gnome-open", u)) { // Gnome
                return true;
            }
            // still not launched, find a browser
            // TODO we should try the other browsers in case exec failed
            // but this may cause multiple browser windows opening 
            if (which("firefox")) {
                return Launcher.exec("firefox", u);
            } else if (which("mozilla")) {
                return Launcher.exec("mozilla", u);
            } else if (which("konqueror")) {
                return Launcher.exec("konqueror", u);
            } else if (which("opera")) {
                return Launcher.exec("opera", u);
            } else if (which("epiphany")) {
                return Launcher.exec("epiphany", u);
            } else if (which("galeon")) {
                return Launcher.exec("galeon", u);
            } else if (which("netscape")) {
                return Launcher.exec("netscape", "-remote", "openURL("+u+",new-window)");
            }
            
            return false;
        }

        @Override
        public boolean isLaunchURLSupported() {
            if (super.isLaunchURLSupported()) {
                return true;
            }
            return isUnixLaunchURLSupported();
        }

        //kfmclient exec mailto:name@example.com
        private synchronized boolean isUnixLaunchURLSupported() {
            if (super.isLaunchURLSupported()) {
                return true;
            }
            
            if (launchURLSupported != null) {
                return launchURLSupported.booleanValue();
            }

            // we rely on the 'which' command
            if (!which("which")) {
                launchURLSupported = Boolean.FALSE;
                return false;
            }
            
            if (which("xdg-open")) {
                launchURLSupported = Boolean.TRUE;
                return true;                
            }
            
            if (System.getenv("BROWSER")!=null) { // $BROWSER
                launchURLSupported = Boolean.TRUE;
                return true;
            } else { // KDE
                if (Platform.maybeDeskopKDE() && which("kfmclient")) {
                    launchURLSupported = Boolean.TRUE;
                    return true;
                }
                if (Platform.maybeDeskopGnome() && which("gnome-open")) {
                    launchURLSupported = Boolean.TRUE;
                    return true;
                }
                String[] commands = {
                        "firefox", "mozilla",
                        "konqueror", "epiphany", "opera", "galeon",
                        "netscape"
                };
                for (String command: commands) {
                    if (which(command)) {
                        launchURLSupported = Boolean.TRUE;
                        return true;
                    }
                }
                if (launchURLSupported == null) {
                    launchURLSupported = Boolean.FALSE;
                }
            }
            
            return launchURLSupported.booleanValue();
        }

        @Override
        public boolean launchMailto(URI mailtoURI) {
            if (super.launchMailto(mailtoURI)) {
                return true;
            }
            
            if (Platform.maybeDeskopKDE() 
                && Launcher.exec("kfmclient", "exec", mailtoURI.toString())) {
                return true;
            }
            
            return false;
        }
        
    }
    
    private static class Java6Launcher extends Launcher {

        private boolean launchURL(URL url, boolean test) {
            System.out.println(url);
            try {
                // get the java.awt.Desktop class and test if this feature
                // is supported
                Class<?> classDesktop = Class.forName("java.awt.Desktop");
                final boolean isDesktopSupported = Boolean.TRUE.equals(
                        classDesktop.getMethod("isDesktopSupported").invoke(null)
                );
                if (!isDesktopSupported) {
                    return false;
                }
    
                // get the java.awt.Desktop instance
                Object objDesktop = classDesktop.getMethod("getDesktop").invoke(null);
                // get the java.awt.Desktop$Action.BROWSE enum constant 
                Class<?> enumAction = Class.forName("java.awt.Desktop$Action");
                Object[] enumConstants = enumAction.getEnumConstants();
                if (enumConstants == null) {
                    return false;
                }
                // test if the browse action is supported
                boolean browseSupported = false;
                for (Object enumConstant: enumConstants) {
                    if ("BROWSE".equals(enumConstant.toString())) {
                        browseSupported = Boolean.TRUE.equals(
                                classDesktop.getMethod("isSupported", enumAction)
                                .invoke(objDesktop, enumConstant));
                        
                        break;
                    }
                }
                if (!browseSupported) {
                    return false;
                }
                
                if (!test) {
                    // call browse
                    classDesktop.getMethod("browse", URI.class)
                    .invoke(objDesktop, url.toURI());
                }
                return true;
            } catch (Throwable th) {
                return false;
            }
        }

        private boolean launchMailto(URI mailtoURI, boolean test) {
            try {
                // get the java.awt.Desktop class and test if this feature
                // is supported
                Class<?> classDesktop = Class.forName("java.awt.Desktop");
                final boolean isDesktopSupported = Boolean.TRUE.equals(
                        classDesktop.getMethod("isDesktopSupported")
                        .invoke(null, new Object[0])
                );
                if (!isDesktopSupported) {
                    return false;
                }
    
                // get the java.awt.Desktop instance
                Object objDesktop = classDesktop.getMethod("getDesktop").invoke(null);
                // get the java.awt.Desktop$Action.BROWSE enum constant 
                Class<?> enumAction = Class.forName("java.awt.Desktop$Action");
                Object[] enumConstants = enumAction.getEnumConstants();
                if (enumConstants == null) {
                    return false;
                }
                // test if the browse action is supported
                boolean browseSupported = false;
                for (Object enumConstant: enumConstants) {
                    if ("MAIL".equals(enumConstant.toString())) {
                        browseSupported = Boolean.TRUE.equals(
                                classDesktop.getMethod("isSupported", enumAction)
                                .invoke(objDesktop, enumConstant));
                        
                        break;
                    }
                }
                if (!browseSupported) {
                    return false;
                }
                
                if (!test) {
                    // call browse
                    classDesktop.getMethod("mail", URI.class)
                    .invoke(objDesktop, mailtoURI);
                }
                return true;
            } catch (Throwable th) {
                if (log.isErrorEnabled()) {
                    log.error("launching URI:'"+mailtoURI+"' failed", th);
                }
                return false;
            }
        }

        @Override
        public boolean launchURL(URL url) {
            if (url == null) {
                throw new IllegalArgumentException("url==null");
            }
            return launchURL(url, false);
        }

        @Override
        public boolean isLaunchURLSupported() {
            return launchURL(null, true);
        }

        @Override
        public boolean launchMailto(URI mailtoURI) {
            return launchMailto(mailtoURI, false);
        }
        
        @Override
        public boolean isLaunchMailtoSupported() {
            return launchMailto(null, true);
        }

    }
 
}
