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
package com.ruinwesen.patchdownloader.swing;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.IOException;
import java.util.Properties;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.UIManager.LookAndFeelInfo;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.search.MatchAllDocsQuery;
import org.apache.lucene.store.LockObtainFailedException;

import name.cs.csutils.CSUtils;
import name.cs.csutils.i18n.I18N;
import name.cs.csutils.platform.Platform;
import name.cs.csutils.platform.Platform.OS;
import name.cs.csutils.swing.RedirectAction;
import name.cs.csutils.swing.RedirectAction.RedirectActionMeta;

import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.patch.DefaultPatchMetadata;
import com.ruinwesen.patchdownloader.patch.PatchDocument;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.Tagset;

public class PDFrame extends PatchDownloader {

    private static Log log;
    
    private Log getLog() {
        if (log == null) {
            log = LogFactory.getLog(PDFrame.class);
        }
        return log;
    }
    
    private static final String KEY_FRAME_X = "frame.bounds.x";
    private static final String KEY_FRAME_Y = "frame.bounds.y";
    private static final String KEY_FRAME_W = "frame.bounds.width";
    private static final String KEY_FRAME_H = "frame.bounds.height";
    
    private JFrame frame;
    private SearchFilters searchFilters;
    private SimpleSearchBar searchBar;
    private MidiMenu midiMenu;
    private PatchListView patchListView;
    
    protected void createFrame() {
        this.frame = new JFrame("Patchdownloader "+PatchdownloaderVersion());
        this.frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        frame.addWindowListener(new PDWindowListener());
        frame.setLayout(new BorderLayout());
        
        searchFilters = new SearchFilters();
        searchBar = new SimpleSearchBar(this);
        
        frame.add(searchBar.getContainer(), BorderLayout.NORTH);
        
        patchListView = new PatchListView();
        JScrollPane scPLV = new JScrollPane(patchListView.getComponent());
        frame.add(scPLV, BorderLayout.CENTER);
        
        // create menu
        JMenuBar jmenubar = new JMenuBar();
        JMenu menuFile = new JMenu(I18N.action("File","menu.file"));
        menuFile.add(new RedirectAction(this, "showCreatePatchFileDialog"));
        menuFile.add(new RedirectAction(this, "exit"));
        jmenubar.add(menuFile);
        
        JMenu menuFilter = new JMenu(I18N.action("Filter","menu.filter"));
        menuFilter.add(new RedirectAction(this, "showAllPatches"));
        menuFilter.add(new RedirectAction(this, "showUserPatches"));
        menuFilter.addSeparator();
        menuFilter.add(new RedirectAction(this, "saveSearchAs"));
        
        jmenubar.add(menuFilter);

        midiMenu = new MidiMenu(); 
        jmenubar.add(midiMenu.getMenu());

        JMenu menuDatabase = new JMenu("Database");
        menuDatabase.add(new RedirectAction(this, "updateLocalRepository"));
        
        jmenubar.add(menuDatabase);

        JMenu menuHelp = new JMenu("Help");
        menuHelp.add(new RedirectAction(this, "showHelpContents"));
        menuHelp.add(new RedirectAction(this, "showAboutDialog"));
        
        jmenubar.add(menuHelp);
        frame.setJMenuBar(jmenubar);
        // create menu : end
        
        frame.pack();
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
            return defaultValue;
        }
        
        return Math.max(minValue, Math.min(maxValue, intValue));
    }
    
    @Override
    protected void start(Properties config) throws IOException {
        super.start(config);
        try {
            fixLookAndFeel();
        } catch (Throwable ex) {
            if (getLog().isDebugEnabled()) {
                getLog().debug("fixLookAndFeel() failed", ex);
            }
        }
        
        
        createFrame();
        // pack the components
        frame.pack();

        Toolkit AWTToolkit = Toolkit.getDefaultToolkit();
        Dimension screen = AWTToolkit.getScreenSize();
        int w = frame.getWidth();
        int h = frame.getHeight();
        if (w<640) w = 640;
        if (h<480) h = 480;
        int x = (screen.width-w)/2;
        int y = (screen.height-h)/2;

        x = intValue(config, KEY_FRAME_X, 0, screen.width-100, x);
        y = intValue(config, KEY_FRAME_Y, 0, screen.height-100, y);
        w = intValue(config, KEY_FRAME_W, 100, screen.width, w);
        h = intValue(config, KEY_FRAME_H, 100, screen.height, h);
        frame.setBounds(x,y,w,h);
        
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
            
            frame.setVisible(false);
            frame.dispose();
        } finally {
            super.stop(config);
        }
    }

    @RedirectActionMeta(title="About Patchdownloader",
            resourcekey="menu.help.about-patchdownloader")
    public void showAboutDialog() {
        throw CSUtils.NotImplementedYet();
    }

    @RedirectActionMeta(title="Help Contents",
            resourcekey="menu.help.help-contents")
    public void showHelpContents() {
        throw CSUtils.NotImplementedYet();
    }

    @RedirectActionMeta(title="Update Repository...",
            resourcekey="menu.db.update-repository")
    public void updateLocalRepository() {
        throw CSUtils.NotImplementedYet();
    }

    public void saveSearchAs() {
        throw CSUtils.NotImplementedYet();
    }
    
    private CreatePatchFileEditor createPatchFileDialog;

    @RedirectActionMeta(title="Create Patch...",
            resourcekey="menu.file.createpatch")
    public void showCreatePatchFileDialog() {
        if (createPatchFileDialog != null) {
            createPatchFileDialog.toFront();
        } else {
            createPatchFileDialog = new CreatePatchFileEditor();
            createPatchFileDialog.setModal(false);
            createPatchFileDialog.pack();
            createPatchFileDialog.addWindowListener(new WindowAdapter() {
                public void windowClosed(WindowEvent e) {
                    createPatchFileDialog = null;
                }
            });
            createPatchFileDialog.setVisible(true);
        }
    }
    
    @RedirectActionMeta(title="Show own Patches",
            resourcekey="menu.filter.showUserPatches")
    public void showUserPatches() {
        throw CSUtils.NotImplementedYet();
    }

    public void showAllPatches() {
        throw CSUtils.NotImplementedYet();
    }

    @RedirectActionMeta(resourcekey="menu.file.exit")
    public void exit() {
        stop();
    }

    protected void startExceptionHandler(Throwable ex) {
        JOptionPane.showMessageDialog(null, 
                "Error while starting the application:"+ex.getMessage(),
                "Error",JOptionPane.ERROR_MESSAGE);
        
    }

    protected void stopExceptionHandler(Throwable ex) {
        JOptionPane.showMessageDialog(null, 
                "Error while closing the application:"+ex.getMessage(),
                "Error",JOptionPane.ERROR_MESSAGE);
        
    }

    private boolean runInitTasks = false;
    private void runInitTasks() {
        if (!runInitTasks) {
            runInitTasks = true;
        }
        // TODO: ensureIndexNotCorrupted();
        
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {

                boolean createExamples = false;
                if (createExamples) {
                    try {
                    // DEBUG: example data
                    for (int i=0;i<10;i++) {
                        getLuceneIndex().index("id"+i, PatchDocument.Document(exampleMetadata(i)));
                    
                    }
                    getLuceneIndex().commit();
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
                
                // TODO load the default query
                patchListView.setQuery(new MatchAllDocsQuery());
                     
            }
        });
    }

    protected void handleLockObtainFailedOnStartException(
            LockObtainFailedException error) {
        
        String messageA = "Please ensure no other application instance is running.\n"
        +"If this problem still exists try to delete the lock-file.\n";
        
        String messageB = messageA
        +"It is recommended to close the application now.\n\n"
        +"Close the application now?";
        
        
        int code = 
        JOptionPane.showConfirmDialog(null,
                error.getMessage()+"\n\n"+messageB,
                "Error",
                JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);

        if (code == JOptionPane.YES_OPTION) {
            getLog().error(messageA);
            try {
                getLuceneIndex().close();
            } catch (CorruptIndexException ex) {
                // no op
            } catch (IOException ex) {
                // no op
            } finally {
                System.exit(1);
            }
        }
        
    }

    private static PatchMetadata exampleMetadata(int id) {
        PatchMetadata data = new DefaultPatchMetadata();
        data.setTitle("title"+id);
        data.setAuthor("author"+id);
        data.setComment("comment"+id);
        data.setTags(new Tagset("tag"+id, "synth", "waldorf"));
        return data;
    }
    
    private class PDWindowListener implements WindowListener {

        @Override
        public void windowClosing(WindowEvent e) {
            // make sure everything is shut down correctly
            PDFrame.this.exit();
        }

        @Override
        public void windowActivated(WindowEvent e) {
            // no op
        }

        @Override
        public void windowClosed(WindowEvent e) {
            // no op
        }

        @Override
        public void windowDeactivated(WindowEvent e) {
            // no op
        }

        @Override
        public void windowDeiconified(WindowEvent e) {
            // no op
        }

        @Override
        public void windowIconified(WindowEvent e) {
            // no op
        }

        @Override
        public void windowOpened(WindowEvent e) {
            // no op
            PDFrame.this.runInitTasks();
        }
        
    }

    
    private void fixLookAndFeel() {
        if (!Platform.isFlavor(OS.UnixFlavor)) {
            return;
        }
        
        Log log = getLog();
        
        String laf = UIManager.getSystemLookAndFeelClassName();
        if (laf.toLowerCase().equalsIgnoreCase("javax.swing.plaf.metal.MetalLookAndFeel")) {
            for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                
                if (log.isDebugEnabled()) {
                    log.debug("installed look and feel: "+info.getClassName());
                }

                if (info.getClassName().equalsIgnoreCase("com.sun.java.swing.plaf.nimbus.NimbusLookAndFeel")) {
                    laf = info.getClassName();
                }/*
                if (info.getClassName().equalsIgnoreCase("com.sun.java.swing.plaf.gtk.GTKLookAndFeel")) {
                    laf = info.getClassName();
                }*/
            }   
        }
        
        try {
            UIManager.setLookAndFeel(laf);
        } catch (ClassNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (InstantiationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (UnsupportedLookAndFeelException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
    }
}
