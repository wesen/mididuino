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

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Date;
import java.util.Enumeration;

import javax.xml.parsers.ParserConfigurationException;

import name.cs.csutils.directoryreader.DirectoryReader;
import name.cs.csutils.directoryreader.Entry;


public class PatchMetadataValidator {


    public void validate(StoredPatch patch) 
        throws IOException, PatchMetadataValidationException, ParserConfigurationException {
        DirectoryReader reader = patch.getDirectoryReader();
        if (reader == null) {
            throw new PatchMetadataValidationException("can not validate patch "+
                    "reader unavailable");
        }

        PatchMetadata metadata;
        try {
            InputStream in = reader.open(StoredPatch.DEFAULT_METADATA);
            try {
                metadata = new XMLPatchMetadata(in);
            } finally {
                in.close();
            }
        } catch (FileNotFoundException ex) {
            throw new PatchMetadataValidationException("could not find the metadata",
                    ex);
        } finally {
            reader.close();
        }

        validate(patch, metadata);
    }

    public void validate(PatchMetadata metadata) 
        throws IOException, PatchMetadataValidationException {
        validate(null, metadata);
    }

    public void validate(StoredPatch patch, PatchMetadata metadata) 
        throws IOException, PatchMetadataValidationException {
        // note we don't use the getters of the metadate object more than
        // once so the operation performs well even with XMLPatchMetadata
        // where the getter methods are very expensive
        String title = metadata.getTitle();
        notNull("title", title);
        notEmpty("title", title);
        
        String author = metadata.getAuthor();
        notNull("author", author);
        notEmpty("author", author);

        String deviceId = metadata.getDeviceId();
        notNull("device-id", deviceId);
        notEmpty("device-id", deviceId);
        
        Date date = metadata.getLastModifiedDate();
        notNull("date", date);
        
        Tagset tagset = metadata.getTags();
        if (tagset.filterPrefix("category:", true, false).isEmpty()) {
            throw new PatchMetadataValidationException("the patch must have " +
                    "at least one category:<name> tag");
        }
        
        String midifilePath = metadata.getPath(PatchMetadata.PATH_MIDIFILE); 
        String sourcecodePath = metadata.getPath(PatchMetadata.PATH_SOURCECODE);  
        
        if (midifilePath==null && sourcecodePath==null) {
            throw new PatchMetadataValidationException("the patch does contain" 
                    +" neither midi file nor source code");
        }

        if (midifilePath != null && patch != null) {
            validateMidiFile(midifilePath, patch);
        }

        if (sourcecodePath != null && patch != null) {
            validateSourceCode(sourcecodePath, patch);
        }
    }

    private void validateSourceCode(String path, StoredPatch patch)
    throws IOException, PatchMetadataValidationException {
        DirectoryReader reader = patch.getDirectoryReader();
        if (reader == null) {
            throw new PatchMetadataValidationException("no directory reader");
        }
        try {
            if (reader.isFile(path)) {
                return; // a single source code file is valid
            }
            
            if (!reader.isDirectory(path)) {
                throw new PatchMetadataValidationException(
                        "the source code directory could not be found at: "+path
                );
            }
            
            Enumeration<? extends Entry> enumeration = 
                reader.entryEnumeration(path);
            
            boolean hasFiles = false;
            while (enumeration.hasMoreElements()) {
                if (enumeration.nextElement().isFile()) {
                    hasFiles = true;
                    break;
                }
            }
            
            if (!hasFiles) {
                throw new PatchMetadataValidationException(
                        "The source code directory does not "+
                        "contain any files at:"+path
                );
            }
            
        } finally {
            reader.close();
        }
    }

    private void validateMidiFile(String path, StoredPatch patch)
    throws IOException, PatchMetadataValidationException {
        DirectoryReader reader = patch.getDirectoryReader();
        if (reader == null) {
            throw new PatchMetadataValidationException("no directory reader");
        }
        try {
            if (!reader.isFile(path)) {
                throw new PatchMetadataValidationException(
                        "the midi file could not be found at: "+path
                );
            }
        } finally {
            reader.close();
        }
    }

    private void notNull(String field, Object obj) 
        throws PatchMetadataValidationException {
        if (obj == null) {
            throw new PatchMetadataValidationException(field+" must not be null");
        }
    }

    private void notEmpty(String field, String str) 
        throws PatchMetadataValidationException {
        if (str.trim().isEmpty()) {
            throw new PatchMetadataValidationException(field+" must not be empty");
        }
    }
    
}
