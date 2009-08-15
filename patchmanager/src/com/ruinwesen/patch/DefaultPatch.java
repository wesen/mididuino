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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipException;

import name.cs.csutils.CSUtils;

import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarDirectory;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.utils.HexFileValidator;
import com.ruinwesen.patch.utils.Validator;
import com.ruinwesen.patch.utils.ValidatorInputStream;

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
    public Directory openDirectory() throws PatchDataException {
        try {
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
        } catch (IOException ex) {
            throw new PatchDataException(ex);
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
    
    public static InputStream getMidiFileInputStream(Patch patch) throws PatchDataException, IOException {
        PatchMetadata meta = patch.getMetadata();
        Path path = meta.getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME);
        if (path == null) {
            return null;
        }
        Directory dir = patch.openDirectory();
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        try {
            InputStream is = dir.getInputStream(path.getPath());
            if (is == null) {
                throw new IOException("path does not exist: "+path.getPath());
            }
            Validator validator = new HexFileValidator(true);
            is = new ValidatorInputStream(is, validator);
            try {
                CSUtils.copy(is, os);
            } finally {
                is.close();
            }
            if (!validator.isValid()) {
                throw new IOException("not a valid intel hex file");
            }
        } finally {
            dir.close();
        }
        byte [] data = os.toByteArray();
        if (data.length==0) {
            return null;
        }
        return new ByteArrayInputStream(data);
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
 