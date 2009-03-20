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
    
    private Log log = LogFactory.getLog(PatchFileBuilder.class);
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
    
    public void setSourceCode(File file) {
        this.sourcecode = file;
    }
    
    public void setOutput(File file) {
        this.output = file;
    }

    public File getOutput() {
        return output;
    }
    
    public void build() throws IOException, ParserConfigurationException, TransformerException {
        if (midifile==null && sourcecode == null) {
            throw new FileNotFoundException("neither midifile nor sourcecode specified");
        }
        if (midifile != null && (!midifile.exists() || !midifile.isFile())) {
            throw new FileNotFoundException("not a file: "+midifile.getAbsolutePath());
        }
        if (sourcecode != null && !sourcecode.exists()) {
            throw new FileNotFoundException("file not found: "+sourcecode.getAbsolutePath());
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
            if (log.isErrorEnabled()) {
                log.error("Could not write metadata.", ex);
            }
            if (!output.delete()) { // don't leave corrupted files behind
                if (log.isDebugEnabled()) {
                    log.debug("Could not delete output file: "+output.getAbsolutePath());
                }
            }
            throw ex;
        } catch (ParserConfigurationException ex) {
            if (log.isErrorEnabled()) {
                log.error("Could not write metadata.", ex);
            }
            if (!output.delete()) { // don't leave corrupted files behind
                if (log.isDebugEnabled()) {
                    log.debug("Could not delete output file: "+output.getAbsolutePath());
                }
            }
            throw ex;
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("Could not write metadata.", ex);
            }
            if (!output.delete()) { // don't leave corrupted files behind
                if (log.isDebugEnabled()) {
                    log.debug("Could not delete output file: "+output.getAbsolutePath());
                }
            }
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
        /* 
          if (log.isDebugEnabled()) {
            log.debug("adding zip entry path:"+path+" (source:"+file.getAbsolutePath()+")");
        }*/
        
        if (file.isFile()) {
            out.putNextEntry(new JarEntry(path));
            CSUtils.copy(file, out);
            out.closeEntry();
        } else if (file.isDirectory()) {
            // directory entry
            File[] files = file.listFiles();
            if (files != null && files.length >= 0) {
                out.putNextEntry(new JarEntry(path+'/'));
                for (File child: files) {
                    writeFiles(out, path+"/"+child.getName(),child);
                }
                out.closeEntry();
            }
            
        }
    }
    /*
    public static void main(String[] args) throws IOException, ParserConfigurationException, TransformerException {
    
        PatchFileBuilder builder = new PatchFileBuilder();
        builder.sourcecode = new File("/home/christian/dev/mididuino/mididuino/patchdownloader/playground/example-patch1/source");
        builder.midifile = new File("/home/christian/dev/mididuino/mididuino/patchdownloader/playground/example-patch1/chresan.hex");
        builder.output = new File("/home/christian/dev/mididuino/mididuino/patchdownloader/playground/test.jar");
        builder.build();
        
        PatchMetadata meta = new StoredPatch.JarFilePatch(builder.output).getMetadata();
        System.out.println(meta.getPath(PatchMetadata.PATH_MIDIFILE));
        System.out.println(meta.getPath(PatchMetadata.PATH_SOURCECODE));
    }*/
    
}
