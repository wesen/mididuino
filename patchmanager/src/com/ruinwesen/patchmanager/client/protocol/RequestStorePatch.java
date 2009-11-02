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
package com.ruinwesen.patchmanager.client.protocol;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.NoSuchAlgorithmException;
import java.util.LinkedList;
import java.util.List;

import name.cs.csutils.FileFilterFactory;

import com.ruinwesen.patch.directory.FSDirectory;
import com.ruinwesen.patch.directory.JarFileBuilder;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadataUtils;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patchmanager.swing.form.TextFieldFormElement;

public class RequestStorePatch extends MandatoryAuthenticatingRequest {

    private PatchMetadata metadata;
    private InputStream is;
    private DataInfo dataInfo;

    public RequestStorePatch(Auth auth,
            PatchMetadata metadata, InputStream is) {
        this(null, auth, metadata, is);
    }
    
    public static void buildPatchFile(File sourceCodeFile, File mididataFile, PatchMetadata meta, String docu, File dst) throws Exception {
    	String filterText = "*.h,*.hh,*.c,*.cpp,*.cxx";
            	
        FileFilter sourceDirFilter = 
            FileFilterFactory.or(FileFilterFactory.DirectoriesOnly(), 
                    FileFilterFactory.parseFilterList(filterText));
        
        
        List<Path> paths = new LinkedList<Path>();
        JarFileBuilder jarbuilder = new JarFileBuilder(dst);
        
        if (mididataFile != null) {
            jarbuilder.add("mididata.hex", new FileInputStream(mididataFile));
            paths.add(new Path(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME, "mididata.hex"));
        }
        if (sourceCodeFile != null) {
            jarbuilder.add("source", new FSDirectory(sourceCodeFile, sourceDirFilter));
            paths.add(new Path(PatchMetadata.DEFAULT_SOURCE_PATH_NAME, "source"));
        }
        if (docu != null && docu.trim().length()!=0) {
            String path = "documentation.txt";
            jarbuilder.add(path, new ByteArrayInputStream(docu.getBytes()));
            paths.add(new Path(PatchMetadata.TEXT_DOCUMENTATION_PATH_NAME, path));
        }
        meta.setPaths(paths);
        jarbuilder.add(PatchMetadata.FILENAME, PatchMetadataUtils.createXMLInputStream(meta));
        jarbuilder.write();
        jarbuilder.close();
    }

    public RequestStorePatch(String protocolId,Auth auth, 
            PatchMetadata metadata, InputStream is) {
        super(protocolId, ACTION_STORE_NEW_PATCH, auth);
        this.metadata = metadata;
        this.is = is;
    }

    public PatchMetadata getMetadata() {
        return metadata;
    }
    
    public synchronized DataInfo getDataInfo() throws NoSuchAlgorithmException, IOException {
        if (this.dataInfo == null) {
            this.dataInfo = new DataInfo(is);
        }
        return dataInfo;
    }
    
}
