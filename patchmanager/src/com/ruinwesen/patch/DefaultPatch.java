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
package com.ruinwesen.patch;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.zip.ZipException;

import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarDirectory;
import com.ruinwesen.patch.metadata.PatchMetadata;

public class DefaultPatch extends AbstractPatch {

    private File file;
    
    public DefaultPatch(File file) {
        if (file == null) {
            throw new NullPointerException();
        }
        this.file = file;
    }

    public boolean isFile() {
        return file.isFile();
    }

    public boolean isDirectory() {
        return file.isDirectory();
    }
    
    @Override
    public Directory openDirectory() throws IOException {
        if (file.isFile()) {
            try {
            return new JarDirectory(file);
            } catch (ZipException ex) {
                throw new IOException("could not open file:"+file.getAbsolutePath(), ex);
            }
        } else if (file.isDirectory()) {
            return new FSDirectory(file);
        } else {
            throw new FileNotFoundException(file.getAbsolutePath());
        }
    }

    @Override
    public String toString() {
        return getClass().getSimpleName()+"[file='"+file.getAbsolutePath()+"']";
    }

    @Override
    public File getLocalFile() {
        return file;
    }

    public static boolean containsSource(Patch patch) {
        try {
            return patch
            .getMetadata()
            .getPath(PatchMetadata.DEFAULT_SOURCE_PATH_NAME) != null;
        } catch (Exception ex) {
        return false;
        }
    }
    
    public static boolean containsMidiFile(Patch patch) {
        try {
            return patch
            .getMetadata()
            .getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME) != null;
        } catch (Exception ex) {
        return false;
        }
    }
    
}
