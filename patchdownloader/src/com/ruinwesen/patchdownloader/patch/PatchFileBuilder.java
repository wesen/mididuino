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
package com.ruinwesen.patchdownloader.patch;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.jar.JarEntry;
import java.util.zip.ZipOutputStream;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSUtils;

import com.ruinwesen.patchdownloader.repository.StoredPatch;

public class PatchFileBuilder {
    
    private static Log log = LogFactory.getLog(PatchFileBuilder.class);

    private PatchMetadata metadata;
    private File midifile;
    private File sourcecode;
    private File output;
    
    public PatchFileBuilder() {
        super();
        this.metadata = new DefaultPatchMetadata();
    }
    
    public void setMetadata(PatchMetadata metadata) {
        this.metadata.setMetadata(metadata);
    }
    
    public PatchMetadata getMetadata() {
        return metadata;
    }

    public void setMidifile(File file) {
        this.midifile = file;
    }
    
    public void setSourceCodeDir(File file) {
        this.sourcecode = file;
    }
    
    public void setOutput(File file) {
        this.output = file;
    }
    
    public void build() throws IOException, ParserConfigurationException, TransformerException {
        if (midifile==null && sourcecode == null) {
            throw new FileNotFoundException("neither midifile nor sourcecode specified");
        }
        if (midifile != null && (!midifile.exists() || !midifile.isFile())) {
            throw new FileNotFoundException("not a file: "+midifile.getAbsolutePath());
        }
        if (sourcecode != null && (!sourcecode.exists() || !sourcecode.isDirectory())) {
            throw new FileNotFoundException("not a directory: "+sourcecode.getAbsolutePath());
        }
        if (output == null) {
            throw new FileNotFoundException("output file not set");
        }
        
        String name = metadata.getName();
        if (name == null) {
            if (sourcecode != null) {
                metadata.setName(sourcecode.getName());
            } else if (midifile != null) {
                metadata.setName(CSUtils.removeFileExtension(midifile.getName()));
            }
        }
        if (metadata.getTitle() == null) {
            metadata.setTitle(name);
        }
        
        ZipOutputStream out = new ZipOutputStream(new BufferedOutputStream(new FileOutputStream(output)));
        out.setLevel(9);
        out.setMethod(ZipOutputStream.DEFLATED);
        try {
            try {
                String midifilePath = null;
                String sourcePath = null;
                if (midifile != null) {
                    midifilePath = "midifile/"+midifile.getName();
                    metadata.setPath(PatchMetadata.PATH_MIDIFILE, midifilePath);
                }
                if (sourcecode != null) {
                    sourcePath = "sourcecode";
                    metadata.setPath(PatchMetadata.PATH_SOURCECODE, sourcePath);
                }
                
                writeMetadata(out);
                if (midifile != null) {
                    writeFiles(out, midifilePath, midifile);
                }
                if (sourcecode != null) {
                    writeFiles(out, sourcePath, sourcecode);
                }
    
                out.flush();
            } finally {
                out.close();
            }
        } catch (TransformerException ex) {
            output.delete(); // don't leave corrupted files behind
            throw ex;
        } catch (ParserConfigurationException ex) {
            output.delete(); // don't leave corrupted files behind
            throw ex;
        } catch (IOException ex) {
            output.delete(); // don't leave corrupted files behind
            throw ex;
        }        
        
        
        
    }

    private void writeMetadata(ZipOutputStream out) throws IOException, ParserConfigurationException, TransformerException {
        out.putNextEntry(new JarEntry(StoredPatch.DEFAULT_METADATA));
        new XMLPatchMetadata(metadata).write(out);
        out.closeEntry();
    }

    private void writeFiles(ZipOutputStream out, String path,
            File file) throws IOException {
        if (log.isDebugEnabled()) {
            log.debug("adding zip entry path:"+path+" (source:"+file.getAbsolutePath()+")");
        }
        
        if (file.isFile()) {
            out.putNextEntry(new JarEntry(path));
            CSUtils.copy(file, out);
            out.closeEntry();
        } else if (file.isDirectory()) {
            // directory entry
            File[] files = file.listFiles();
            if (files == null || files.length == 0) {
                return;
            }
            
            out.putNextEntry(new JarEntry(path+'/'));
            if (files != null) {
                for (File child: files) {
                    writeFiles(out, path+"/"+child.getName(),child);
                }
            }
            out.closeEntry();
            
        }
    }

}
