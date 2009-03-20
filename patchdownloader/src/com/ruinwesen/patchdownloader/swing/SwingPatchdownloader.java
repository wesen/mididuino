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
package com.ruinwesen.patchdownloader.swing;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.util.Properties;

import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.WindowConstants;
import javax.swing.UIManager.LookAndFeelInfo;

import name.cs.csutils.I18N;
import name.cs.csutils.Platform;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.LockFile.MultipleApplicationInstancesException;
import name.cs.csutils.Platform.OS;
import name.cs.csutils.RedirectAction.RedirectActionMeta;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendException;
import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.swing.dialogs.AboutDialog;
import com.ruinwesen.patchdownloader.swing.dialogs.CreatePatchFileDialog;
import com.ruinwesen.patchdownloader.swing.misc.MetadataCache;
import com.ruinwesen.patchdownloader.tasks.DeletePatchIndexTask;
import com.ruinwesen.patchdownloader.tasks.RandomPatchGeneratorTask;
import com.ruinwesen.patchdownloader.tasks.RepositoryUpdateTask;

public class SwingPatchdownloader extends PatchDownloader {

    private static Log log = LogFactory.getLog(SwingPatchdownloader.class);
    
    private static final String KEY_FRAME_X = "frame.bounds.x";
    private static final String KEY_FRAME_Y = "frame.bounds.y";
    private static final String KEY_FRAME_W = "frame.bounds.width";
    private static final String KEY_FRAME_H = "frame.bounds.height";
    private static final String KEY_PLAF = "plaf";
    private static final String KEY_MIDI_IN = "midi.in";
    private static final String KEY_MIDI_OUT = "midi.out";
    
    public SwingPatchdownloader() {
        super();
    }
    
    public static SwingPatchdownloader getSharedInstance() {
        return (SwingPatchdownloader) PatchDownloader.getSharedInstance();
    }
    
    private JFrame frame;
    private PatchListView patchListView;
    private FilterListView filterListView;
    private PatchDetailsView patchDetailsView;
    private File lastFileChooserLocation = null;
    private SearchController searchController = new SearchController(this);
    private MetadataCache metadataCache = new MetadataCache();
    
    public MetadataCache getMetadataCache() {
        return metadataCache;
    }

    public SearchController getSearchController() {
        return searchController;
    }
    
    public File getLastFileChooserLocation() {
        return lastFileChooserLocation;
    }
    
    public void setLastFileChooserLocation(JFileChooser chooser) {
        File file = chooser.getSelectedFile();
        if (file != null) {
            lastFileChooserLocation = file;
        }
    }
    
    public void setLastFileChooserLocation(File file) {
        this.lastFileChooserLocation = file;
    }
    
    protected void createFrame() {
        String title = "Patchdownloader "+PatchDownloader.getVersion();
        if (PatchDownloader.isDebugModeOn()) {
            title += " - Build id: "+PatchDownloader.getBuildNumber();
            title += " - Build time: "+PatchDownloader.getBuildTimestamp();
        }
        
        this.frame = new JFrame(title);
        this.frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        frame.addWindowListener(new PDWindowListener());
        frame.setLayout(new BorderLayout());
        /*
        TopBar topBar;
        topBar = new TopBar(this);
        frame.add(topBar.getContainer(), BorderLayout.NORTH);*/
        
        patchListView = new PatchListView(this);
        JComponent patchListViewComponent = patchListView.getComponent();
        
        filterListView = new FilterListView(this);
        JComponent filterListViewComponent = filterListView.getComponent();
        filterListViewComponent.setPreferredSize(new Dimension(150,Short.MAX_VALUE));
        
        patchDetailsView = new PatchDetailsView(this);
        JComponent patchDetailsViewComponent = patchDetailsView.getComponent();
        patchDetailsViewComponent.setPreferredSize(patchDetailsViewComponent.getPreferredSize());
        
        frame.add(patchListViewComponent, BorderLayout.CENTER);
        frame.add(patchDetailsViewComponent, BorderLayout.EAST);
        frame.add(filterListViewComponent,BorderLayout.WEST);
        
      ///  BottomBar bottomBar = new BottomBar(this);
       // frame.add(bottomBar.getContainer(), BorderLayout.SOUTH);
        
        // create menu
        JMenuBar jmenubar = new JMenuBar();
        JMenu menuFile = new JMenu(I18N.action("File","menu.file"));
        menuFile.add(new RedirectAction(this, "updateLocalRepository"));
        menuFile.addSeparator();
        menuFile.add(new RedirectAction(this, "exit"));
        jmenubar.add(menuFile);
        
        JMenu menuHelp = new JMenu(I18N.action("Help","menu.help"));
        menuHelp.add(new RedirectAction(this, "showAboutDialog"));
        
        jmenubar.add(menuHelp);

        if (PatchDownloader.isDebugModeOn()) {
            JMenu menuDebug = new JMenu("Debug");
            menuDebug.add(new RedirectAction(this, "showCreatePatchFileDialog"));
            menuDebug.add(new RedirectAction(this, "debugGenerateRandomPatches"));
            menuDebug.add(new RedirectAction(this, "debugDeletePatchIndex"));
            jmenubar.add(menuDebug);
        }
        
        
        frame.setJMenuBar(jmenubar);
        // create menu : end
        
        frame.pack();
    }

    @RedirectActionMeta(title="Generate random patches...")
    public void debugGenerateRandomPatches() {
        if (!PatchDownloader.isDebugModeOn()) {
            return;
        }
        RandomPatchGeneratorTask gen = new RandomPatchGeneratorTask();
        gen.showDialog(frame);
    }

    @RedirectActionMeta(title="Reset Patch Repository (Deletes all content!)")
    public void debugDeletePatchIndex() {
        if (!PatchDownloader.isDebugModeOn()) {
            return;
        }
        new DeletePatchIndexTask().start();
    }

    public PatchListView getPatchListView() {
        return patchListView;
    }

    private int intValue(Properties props, String key,
            int minValue, int maxValue, int defaultValue) {
        String value = props.getProperty(key);
        if (value == null) {
            return defaultValue;
        }
        int intValue;
        try {
            intValue = Integer.parseInt(value);
        } catch (NumberFormatException ex) {
            return Math.max(minValue, Math.min(maxValue, defaultValue));
        }
        
        return Math.max(minValue, Math.min(maxValue, intValue));
    }
    
    @Override
    protected void start(Properties config) throws Exception {
        super.start(config);
        try {
            fixLookAndFeel(config);
        } catch (Exception ex) {
            if (log.isDebugEnabled()) {
                log.debug("fixLookAndFeel() failed", ex);
            }
        }
        
        createFrame();
        // pack the components
        frame.pack();

        Toolkit AWTToolkit = Toolkit.getDefaultToolkit();
        Dimension screen = AWTToolkit.getScreenSize();
        int w = frame.getWidth();
        int h = frame.getHeight();
        if (w<640) { w = 640; } else if (w>screen.width) { w = screen.width/2; } 
        if (h<480) { h = 480; } else if (h>screen.height) { h = screen.height/2; }
        int x = (screen.width-w)/2;
        int y = (screen.height-h)/2;

        x = intValue(config, KEY_FRAME_X, 0, screen.width-100, x);
        y = intValue(config, KEY_FRAME_Y, 0, screen.height-100, y);
        w = intValue(config, KEY_FRAME_W, 100, screen.width, w);
        h = intValue(config, KEY_FRAME_H, 100, screen.height, h);
        
        frame.setMinimumSize(new Dimension(520,300));
        frame.setBounds(x,y,w,h);
        frame.setLocationByPlatform(true);
        
        // load midi 

        MidiSend midisend = getMidiSend();
        String inputName = config.getProperty(KEY_MIDI_IN);
        if (inputName != null) {
            try {
                MidiDevice device = midisend.getInputDevice(inputName);
                if (device != null) {
                    midisend.setInputDevice(device);
                }
            } catch (MidiSendException ex) {
                // ignorable
            }
        }
        String outputName = config.getProperty(KEY_MIDI_OUT);
        if (outputName != null) {
            try {
                MidiDevice device = midisend.getOutputDevice(outputName);
                if (device != null) {
                    midisend.setOutputDevice(device);
                }
            } catch (MidiSendException ex) {
                // ignorable
            }
        }

        patchDetailsView.getMidiBar().refreshMidideviceLists();
        
        frame.setVisible(true);
    }
    
    @Override
    protected void stop(Properties config) throws IOException {
        try {
            Rectangle b = frame.getBounds();
            config.setProperty(KEY_FRAME_X, Integer.toString(b.x));
            config.setProperty(KEY_FRAME_Y, Integer.toString(b.y));
            config.setProperty(KEY_FRAME_W, Integer.toString(b.width));
            config.setProperty(KEY_FRAME_H, Integer.toString(b.height ));
            
            MidiSend midisend = getMidiSend();
            MidiDevice input = midisend.getInputDevice();
            MidiDevice output = midisend.getOutputDevice();
            if (input != null) {
                config.setProperty(KEY_MIDI_IN, input.getName());
            }
            if (output != null) {
                config.setProperty(KEY_MIDI_OUT, output.getName());
            }
            
            String plaf = UIManager.getLookAndFeel().getClass().getName();
            if (!UIManager.getSystemLookAndFeelClassName().equals(plaf)) {
                config.setProperty(KEY_PLAF, plaf);
            }
            
            frame.setVisible(false);
            frame.dispose();
        } finally {
            super.stop(config);
        }
    }

    @RedirectActionMeta(title="About Patchdownloader",
            resourcekey="aboutdialog")
    public void showAboutDialog() {
        new AboutDialog(frame).show();
    }

    @RedirectActionMeta(title="Update Repository...",
            resourcekey="menu.db.update-repository")
    public void updateLocalRepository() {
        SwingUtilities.invokeLater(new RepositoryUpdateTask());
    }

    private CreatePatchFileDialog createPatchFileDialog;

    @RedirectActionMeta(title="Create Patch...",
            resourcekey="menu.file.createpatch")
    public void showCreatePatchFileDialog() {
        if (!PatchDownloader.isDebugModeOn()) {
            return;
        }
        if (createPatchFileDialog != null) {
            createPatchFileDialog.toFront();
        } else {
            createPatchFileDialog = new CreatePatchFileDialog();
            createPatchFileDialog.setModal(false);
            createPatchFileDialog.pack();
            createPatchFileDialog.addWindowListener(new WindowAdapter() {
                @Override
                public void windowClosed(WindowEvent e) {
                    createPatchFileDialog = null;
                }
            });
            createPatchFileDialog.setVisible(true);
        }
    }
    
    @RedirectActionMeta(resourcekey="menu.file.exit")
    public void exit() {
        stop();
    }

    @Override
    protected void startExceptionHandler(Exception ex) {
        if (ex instanceof RuntimeException) {
            ex.printStackTrace();
            return; // not important
        }
        if (ex instanceof MultipleApplicationInstancesException) {
            JOptionPane.showMessageDialog(null,
                    "An instance of this application is already running.",
                    "Warning", JOptionPane.WARNING_MESSAGE);
            System.exit(1);
        }

        String message = ex.getMessage();
                
        JOptionPane.showMessageDialog(null, 
                "Error while starting the application:"+message,
                "Error",JOptionPane.ERROR_MESSAGE);
    }

    @Override
    protected void stopExceptionHandler(Throwable ex) {
        JOptionPane.showMessageDialog(null, 
                "Error while closing the application:"+ex.getMessage(),
                "Error",JOptionPane.ERROR_MESSAGE);
        
    }

    @Override
    protected void handleFatalExceptionOnStart(String message, Exception ex) {
        
        int code = 
        JOptionPane.showConfirmDialog(null,
                message,
                "Fatal",
                JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);

        if (code == JOptionPane.YES_OPTION) {
            try {
                log.fatal(message, ex);
            } finally {
                System.exit(1);
            }
        }
        
    }

    private boolean runInitTasks = false;
    private void runInitTasks() {
        if (!runInitTasks) {
            runInitTasks = true;
        }
        // TODO: ensureIndexNotCorrupted();

        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                searchController.indexChanged();
                searchController.update();
            }
        });
    }

    
    private void fixLookAndFeel(Properties config) {
        if (log.isDebugEnabled()) {
            log.debug("system look and feel: "+UIManager.getSystemLookAndFeelClassName());
            for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                log.debug("available look and feel: "+info.getClassName());
            }   
        }
        
        final String NIMBUS = "com.sun.java.swing.plaf.nimbus.NimbusLookAndFeel";

        String selectedPLAF = config.getProperty(KEY_PLAF);
        // selectedPLAF = null;
        if (selectedPLAF != null) {
            for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                if (info.getClassName().equals(selectedPLAF)) {
                    try {
                        UIManager.setLookAndFeel(selectedPLAF);
                        /*if (NIMBUS.equals(selectedPLAF)) {
                            UIManager.getLookAndFeelDefaults().put("defaultFont", 
                                    new Font("Bitstream Vera Sans", Font.PLAIN, 9));
                        }*/
                        if (log.isDebugEnabled()) {
                            log.debug("Using look and feel from configuration:"+selectedPLAF);
                        }
                        return;
                    } catch (Exception ex) {
                        if (log.isDebugEnabled()) {
                            log.debug("Could not load look and feel from configuration:"+selectedPLAF, ex);
                        }
                        return;
                    }
                }
            }
            if (log.isDebugEnabled()) {
                log.debug("Could not find look and feel from configuration:"+selectedPLAF);
            }
        }
        
        // fix look and feel depending on the platform
        
        
        if (Platform.isFlavor(OS.WindowsFlavor)) {
            if (log.isDebugEnabled()) {
                log.debug("Switching to look and feel:"+UIManager.getSystemLookAndFeelClassName());
            }
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception ex) {
                if (log.isDebugEnabled()) {
                    log.debug("Could not set system look and feel:"+UIManager.getSystemLookAndFeelClassName());
                }
            }
        } else if (Platform.isFlavor(OS.MacOSFlavor)) { 
            //System.setProperty("apple.laf.useScreenMenuBar", "true");
            //System.setProperty("com.apple.mrj.application.apple.menu.about.name", "Patchdownloader");
            if (log.isDebugEnabled()) {
                log.debug("Switching to look and feel:"+UIManager.getSystemLookAndFeelClassName());
            }
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception ex) {
                if (log.isDebugEnabled()) {
                    log.debug("Could not set system look and feel:"+UIManager.getSystemLookAndFeelClassName());
                }
            }
        } else if (Platform.isFlavor(OS.UnixFlavor)) {
            
            // TODO do not force a look and feel, but write the automatically selected laf to the config file
            // then only rely on this setting so users can change their preferences
            
            String laf = UIManager.getSystemLookAndFeelClassName();
            String nimbus = null;
            String gtk = null;
            if (laf.toLowerCase().equalsIgnoreCase("javax.swing.plaf.metal.MetalLookAndFeel")) {
                for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                    if (info.getClassName().equalsIgnoreCase(NIMBUS)) {
                        nimbus = info.getClassName();
                    }
                    else if (info.getClassName().equalsIgnoreCase("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")) {
                        gtk = info.getClassName();
                    }
                }
                //nimbus=null;
                gtk = null; // no - this looks too bad
                if (nimbus != null) {
                    laf = nimbus;
                } else if (gtk != null) {
                    laf = gtk;
                } else {
                    laf = null;
                }

                if (laf != null) {
                    if (log.isDebugEnabled()) {
                        log.debug("Switching to look and feel:"+laf);
                    }
                    try {
                        UIManager.setLookAndFeel(laf);
                    } catch (Exception ex) {
                        if (log.isDebugEnabled()) {
                            log.debug("Could not set look and feel:"+laf);
                        }
                    }
                }
            }
            
        }
    }

    public PatchDetailsView getPatchDetailsView() {
        return patchDetailsView;
    }

    public FilterListView getFilterListView() {
        return filterListView;
    }

    public JFrame getFrame() {
        return frame;
    }

    private class PDWindowListener extends WindowAdapter {

        @Override
        public void windowOpened(WindowEvent e) {
            // no op
            SwingPatchdownloader.this.runInitTasks();
        }

        @Override
        public void windowClosing(WindowEvent e) {
            // make sure everything is shut down correctly
            SwingPatchdownloader.this.exit();
        }

    }
    
}
