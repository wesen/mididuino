package com.ruinwesen.patchmanager.swing;

import java.awt.Component;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.NoSuchElementException;

import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.CSUtils;
import name.cs.csutils.FileFilterFactory;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.Entry;
import com.ruinwesen.patch.directory.JarFileBuilder;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataUtils;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.utils.HexFileValidator;
import com.ruinwesen.patch.utils.ValidatorInputStream;

public final class SwingPatchManagerUtils {

    public static final javax.swing.filechooser.FileFilter RWP_FILE_FILTER =
        FileFilterFactory.createNamedFileFilter(
                "Ruin & Wesen Patch (*.rwp)",
                FileFilterFactory.and(
                FileFilterFactory.FilesOnly(),
                FileFilterFactory.SuffixFilter(".rwp")));

    public static final javax.swing.filechooser.FileFilter HEX_FILE_FILTER =
        FileFilterFactory.createNamedFileFilter(
                "Intel Hex File (*.hex)",
                FileFilterFactory.and(
                FileFilterFactory.FilesOnly(),
                FileFilterFactory.SuffixFilter(".hex")));
    
    private static Log log = LogFactory.getLog(SwingPatchManagerUtils.class);

    private SwingPatchManagerUtils() {
        super();
    }
    
    public static void updateMetadata(Patch patch, PatchMetadata meta) throws Exception {
        try {
            patch.getMetadata();
        } catch (Exception ex) {
            throw new IOException("not a valid patch file: "+patch);
        }
        
        File file = patch.getLocalFile();
        if (file != null && file.isDirectory()) {
            File dstfile = new File(file, PatchMetadata.FILENAME);

            PatchMetadataUtils.writeXML(meta, dstfile);
            return;
        }
        
        zipPatch(patch, meta, file);
    }

    public static File zipPatchTemporary(Patch patch, PatchMetadata meta) throws Exception {
        // we have to update the jar file 

        File tmp = File.createTempFile("patch", ".rwp.tmp");
        tmp.deleteOnExit();
        
        JarFileBuilder builder = new JarFileBuilder(tmp);
        try {            
            Directory srcdir = patch.openDirectory();
            try {
                // add everything in the directory, except the old metadata file
                builder.add(srcdir, new FilterEnum(srcdir.entryEnumeration()));
                // add the updated metadata file
                builder.add(PatchMetadata.FILENAME, PatchMetadataUtils.createXMLInputStream(meta));
                // write
                builder.write();
            } finally {
                srcdir.close();
            }
        } catch (Exception ex) {
            builder.close();
            builder = null;
            tmp.delete();
            throw ex;
        } finally {
            if (builder != null)
                builder.close();
        }
        
        return tmp;
    }
    
    public static void zipPatch(Patch patch, PatchMetadata meta, File dst) throws Exception {
        // we have to update the jar file 

        File tmp = zipPatchTemporary(patch, meta);
        // now rewrite old file
        try {
            CSUtils.copy(tmp, dst, true);
        } finally {
            tmp.delete();
        }
        
    }
    
    private static final class FilterEnum implements Enumeration<Entry> {
        private Enumeration<Entry> enumeration;
        private Entry next;
        private String LONG_NAME = "/"+PatchMetadata.FILENAME;
        public FilterEnum(Enumeration<Entry> enumeration) {
            this.enumeration = enumeration;
            align();
        }
        
        private void align() {
            next = null;
            while (enumeration.hasMoreElements()) {
                Entry e = enumeration.nextElement();
                if (!(PatchMetadata.FILENAME.equals(e.getName())
                      || LONG_NAME.equals(e.getName()))) {
                    next = e;
                    break;
                }
            }
        }

        @Override
        public boolean hasMoreElements() {
            return next != null;
        }

        @Override
        public Entry nextElement() {
            if (!hasMoreElements()) {
                throw new NoSuchElementException();
            }
            Entry e = next;
            align();
            return e;
        }
    }

    public static void showErrorDialog(Component parentComponent, String message) {
        JOptionPane.showMessageDialog(parentComponent, 
                message,
                "Error",
                JOptionPane.ERROR_MESSAGE);
    }
    

    public static void showStoreSourceDialog(Component parentComponent,
            Patch patch) {
        
    }
    
    public static void showStoreMidiFileDialog(Component parentComponent,
            Patch patch) {
        if (patch == null) {
            throw new IllegalArgumentException();
        }
        
        PatchMetadata meta;
        try {
            meta = patch.getMetadata();
        } catch (Exception ex) {
            showErrorDialog(parentComponent, "Could not read patch metadata.\n"
                    +ex.getMessage());
            return;
        }

        Path path = meta.getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME);
        if (path == null) {
            showErrorDialog(parentComponent, "Patch does not contain a midi file.");
            return;
        }
        
        String name = CSUtils.getLastPathComponent(path.getPath());
        
        JFileChooser fc = new JFileChooser();
        fc.setMultiSelectionEnabled(false);
        fc.setAcceptAllFileFilterUsed(true);
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setSelectedFile(new File(name));

        for(boolean done=false;!done;) {
            if (fc.showSaveDialog(parentComponent) != JFileChooser.APPROVE_OPTION) {
                done = true; // cancel was pressed
                break;
            }

            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            File file = fc.getSelectedFile();
            
            if (file.exists()) {
                int option = JOptionPane.showConfirmDialog(parentComponent, 
                        "Overwrite existing file: "+file.getName()+"?",
                        "File exists",
                        JOptionPane.YES_NO_CANCEL_OPTION,
                        JOptionPane.QUESTION_MESSAGE
                );
                if (option == JOptionPane.CANCEL_OPTION) {
                    return;
                }
                if (option == JOptionPane.NO_OPTION) {
                    continue;
                }   
                if (option != JOptionPane.YES_OPTION) {
                    // illegal state
                    return;
                }
            }
            
            
            try {
                Directory dir = patch.openDirectory();
                try {
                    HexFileValidator validator = new HexFileValidator(true);
                    InputStream in = 
                        new ValidatorInputStream(
                                dir.getInputStream(path.getPath()),
                                validator);
                    try {
                        CSUtils.copy(in, file, true);
                        validator.validate();
                    } finally {
                        in.close();
                    }
                } finally {
                    dir.close();
                }
            } catch (Exception ex) {
                if (log.isErrorEnabled()) {
                    log.error("Could not store midifile", ex);
                }
                showErrorDialog(parentComponent, "Could not store midifile.\n"
                        +ex.getMessage());
            }
            finally {
                done = true; // ensure we exit the infinite-loop 
            }
        }
    }

    public static int showFileChooserLoop(Component parentComponent, JFileChooser chooser) {
        for (;;) {
            int option = chooser.showOpenDialog(parentComponent);
            if (option != JFileChooser.APPROVE_OPTION) {
                return JOptionPane.CANCEL_OPTION;
            }
            
            File file = chooser.getSelectedFile();
            if (file.exists()) {
                int op2 = showReplaceExistingFileConfirmDialog(parentComponent, file);
                if (op2 == JOptionPane.YES_OPTION) {
                    return JOptionPane.YES_OPTION;
                } else if (op2 == JOptionPane.CANCEL_OPTION) {
                    return JOptionPane.CANCEL_OPTION;
                }
            } else {
                return JOptionPane.YES_OPTION;
            }
        }
    }
    
    public static int showReplaceExistingFileConfirmDialog(Component parentComponent,
            File file) {
        return JOptionPane.showConfirmDialog(parentComponent,
                "Do you want to replace the existing file "+file+" ?",
                "Warning",
                JOptionPane.YES_NO_CANCEL_OPTION,
                JOptionPane.QUESTION_MESSAGE);
    }

    public static void setSaveRWPFile(JFileChooser fc) {
        fc.setFileFilter(RWP_FILE_FILTER);
    }
    
}
