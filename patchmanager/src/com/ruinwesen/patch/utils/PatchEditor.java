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
package com.ruinwesen.patch.utils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;

import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.metadata.DefaultPatchMetadata;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.metadata.Tagset;

public class PatchEditor {

    public static final String VALIDATION_AUTHOR_UNDEFINED = "VALIDATION_AUTHOR_UNDEFINED";
    public static final String VALIDATION_NAME_UNDEFINED = "VALIDATION_NAME_UNDEFINED";
    public static final String VALIDATION_COMMENT_UNDEFINED = "VALIDATION_COMMENT_UNDEFINED";
    public static final String VALIDATION_DEVICEID_UNDEFINED = "VALIDATION_DEVICEID_UNDEFINED";
    public static final String VALIDATION_ENVIRONMENT_UNDEFINED = "VALIDATION_ENVIRONMENT_UNDEFINED";
    public static final String VALIDATION_NO_CATEGORY_DEFINED = "VALIDATION_NO_CATEGORY_ASSIGNED";
    public static final String VALIDATION_INVALID_PATH_KEY = "VALIDATION_INVALID_PATH_KEY";
    
    private Patch sourcePatch;
    private PatchMetadata metadata = new DefaultPatchMetadata();
    private boolean preserveMetadata = true;
    private boolean preserveFiles = true;
    private List<String> validationErrorList = new LinkedList<String>();
    
    public void setSource(File file) {
        if (file == null) {
            setSource((Patch)null);
        } else {
            setSource(new DefaultPatch(file));
        }
    }

    public boolean isPreserveMetadataEnabled() {
        return preserveMetadata;
    }

    public boolean isPreserveFilesEnabled() {
        return preserveFiles;
    }

    public void setPreserveMetadataEnabled(boolean value) {
        this.preserveMetadata = value;
    }

    public void setPreserveFilesEnabled(boolean value) {
        this.preserveFiles = value;
    }
    
    public void setSource(Patch patch) {
        this.sourcePatch = patch;
    }

    public void load() throws IOException, ParserConfigurationException, SAXException {
        if (sourcePatch != null) {
            PatchMetadata sourceMetadata = sourcePatch.getMetadata();
            if (preserveMetadata) {
                this.metadata = sourceMetadata.copy();
            } else {
                this.metadata = new DefaultPatchMetadata(sourceMetadata);
            }
            if (!preserveFiles) {
                metadata.setPaths(Collections.<Path>emptyList());
            }
        }
        if (this.metadata == null) {
            this.metadata = new DefaultPatchMetadata();
        }
    }
    
    public PatchMetadata getMetadata() {
        return metadata;
    }

    public static Path newExternalPath(File file, Path path) {
        return new SourcePath(file, path);
    }

    public static Path newExternalPath(File file, String name, String path) {
        return new SourcePath(file, new Path(name, path));
    }
    
    private static final class SourcePath extends Path {
        File file;
        public SourcePath(File file, Path path) {
            super(path.getName(), path.getPath());
            this.file = file;
        }        
    }
    
    public void generate(File dst) throws IOException {
        /*validationErrorList.clear();
        if (!validate(validationErrorList)) {
            throw new IOException("validation error");
        }
        */
        
        
        
        
    }
 
    private boolean validate(List<String> dst) {
        int errors = 0;
        if (isNull(metadata.getAuthor()) || isEmptyIfTrimmed(metadata.getAuthor())) {
            dst.add(VALIDATION_AUTHOR_UNDEFINED);
        }
        if (isNull(metadata.getComment()) || isEmptyIfTrimmed(metadata.getComment())) {
            dst.add(VALIDATION_COMMENT_UNDEFINED);
        }
        if (isNull(metadata.getDeviceId())) {
            dst.add(VALIDATION_DEVICEID_UNDEFINED);
        }
        if (isNull(metadata.getEnvironmentId())) {
            dst.add(VALIDATION_ENVIRONMENT_UNDEFINED);
        }
        if (isNull(metadata.getName()) || isEmptyIfTrimmed(metadata.getName())) {
            dst.add(VALIDATION_NAME_UNDEFINED);
        }
        Tagset categories = metadata.getTags().filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, true, true);
        boolean categoriesDefined = false;
        for (String t: categories.toStringArray()) {
            if (!isEmptyIfTrimmed(t)) {
                categoriesDefined = true;
                break;
            }
        }
        if (!categoriesDefined) {
            dst.add(VALIDATION_NO_CATEGORY_DEFINED);
        }

        // **** validate paths
        // validate path keys
        Map<String, Path> pathMap = metadata.getPaths();
        for (Map.Entry<String, Path> entry: pathMap.entrySet()) {
            if (entry.getKey() == null || !entry.getKey().equals(entry.getValue().getName())) {
                dst.add(VALIDATION_INVALID_PATH_KEY);
                errors ++;
                break;
            }
        }
        
        return errors == 0;
    }
    
    private boolean isNull(Object o) {
        return o == null;
    }

    private boolean isEmptyIfTrimmed(String s) {
        return s.trim().isEmpty();
    }
    
}
