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

import javax.swing.JDialog;
import javax.swing.WindowConstants;

import name.cs.csutils.CSUtils;
import name.cs.csutils.FileFilterFactory;



/**
 * The patch builder application.
 */
public class PatchEdit {

    public static void main(String[] args) {
        if (args.length == 0) {        
            showDialog();
            return;
        } else {
            new PatchEdit(args).run();
        }
    }
    
    private String[] args;
    
    public PatchEdit(String[] args) {
        this.args = args;
    }
    
    public void run() {
        printHeader();

        if (args.length==2 && "--print-metadata".equals(args[0])) {
            String path = args[1];
            if (printMetadata(new File(path), path, false)) {
                return;
            }
        } else
        if (args.length==2 && "--print-metadatax".equals(args[0])) {
            String path = args[1];
            File dir = new File(path);
            if (!dir.isDirectory()) {
                new FileNotFoundException("no such directory: "+path).printStackTrace();
            }
            
            File[] files =
            dir.listFiles(
                    FileFilterFactory.or(
                            FileFilterFactory.DirectoriesOnly(),
                            FileFilterFactory.and(FileFilterFactory.FilesOnly(), FileFilterFactory.SuffixFilter(".rwp")
                                    )));
            
            for (File file: files) {
                printMetadata(file, null, true);
            }
            return;
        } else if (args.length>2 && "--patch".equals(args[0])) {
            
        }

        printUsage();
    }
    
    private boolean printMetadata(File file, String path, boolean ignoreMetadataError) {
        if (path == null)
            path = file.getAbsolutePath();
        
        if (!file.exists()) {
            System.err.println("file not found: "+path);
            return false;
        }
        
        StoredPatch patch = file.isFile() ?
                new StoredPatch.JarFilePatch(file) :
                    new StoredPatch.DirectoryPatch(file);

        PatchMetadata data;
        try {
            data = patch.getMetadata();
        } catch (IOException e) {
            if (ignoreMetadataError) 
                return false;
            e.printStackTrace();
            return false;
        }

        System.out.printf("patch:%s\n", path);
        System.out.printf("metadata-format-version:%s\n", n2(data.getMetadataFormatVersion(),""));
        System.out.printf("title:%s\n", n2(data.getTitle(), ""));
        System.out.printf("name:%s\n", n2(data.getName(), ""));
        System.out.printf("device-id:%s\n", n2(data.getDeviceId(),""));
        System.out.printf("author:%s\n", n2(data.getAuthor(),""));
        System.out.printf("tags:%s\n", data.getTags().toString());
        System.out.printf("last-modified:%s\n", CSUtils.dateToString(data.getLastModifiedDate()));
        System.out.printf("comment:%s\n", norm(n2(data.getComment(),"")));
        System.out.printf("source-available:%b\n", data.getPath(PatchMetadata.PATH_SOURCECODE) != null);
        if (data.getPath(PatchMetadata.PATH_SOURCECODE) != null) {
            System.out.printf("source:%s\n", data.getPath(PatchMetadata.PATH_SOURCECODE));
        }
        System.out.printf("midifile-available:%b\n", data.getPath(PatchMetadata.PATH_MIDIFILE) != null);
        if (data.getPath(PatchMetadata.PATH_MIDIFILE) != null) {
            System.out.printf("midifile:%s\n", data.getPath(PatchMetadata.PATH_MIDIFILE));
        }
        System.out.println();
        return true;
    }
    
    private static String n2(String s, String alternative) {
        return s == null ? alternative : s;
    }
    
    private static String norm(String s) {
        if (s == null) return "";
        StringBuilder sb = new StringBuilder();
        for (int i=0;i<s.length();i++) {
            char ch = s.charAt(i);
            switch (ch) {
            case '\t': sb.append("\\t"); break;
            case '\n': sb.append("\\n"); break;
            case '\r': sb.append("\\r"); break;
            default: sb.append(ch); break;
            }
        }
        return sb.toString();
    }

    public void printHeader() {
        System.out.println("Patch Edit Version 1.0");
        System.out.println("(C) 2009 - Ruin & Wesen");
    }
    
    public void printUsage() {
        System.out.println("Usage:");
        System.out.println("no options: shows patch builder dialog");
        System.out.println("--print-metadata <path>: print metadata of patch <path>");
        System.out.println("--print-metadatax <path>: print metadata of all patches in directory <path>");
        System.out.println();
        System.out.println("Metadata print format: (multiple records are separated by an empty line)");
        System.out.println("patch:<path>   (<path> points to the file or directory of the patch)");
        System.out.println("metadata-format-version:<string>");
        System.out.println("title:<string>");
        System.out.println("name:<string>");
        System.out.println("device-id:<string>");
        System.out.println("author:<string>");
        System.out.println("tags:<string>");
        System.out.println("last-modified:<string>");
        System.out.println("comment:<string> (\\n\\r\\t characters are escaped)");
        System.out.println("source-available:<true|false>");
        System.out.println("source:<path> (relative, only if source is available)");
        System.out.println("midifile-available:<true|false>");
        System.out.println("midifile:<path> (relative, only if midi file is available)");
    }
    
    public static void showDialog() {
        JDialog dlg = new CreatePatchFileDialog();
        dlg.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        dlg.pack();
        dlg.setSize(Math.max(640,dlg.getWidth()), Math.max(dlg.getHeight(),480));
        dlg.setLocationRelativeTo(null);
        dlg.setLocationByPlatform(true);
        dlg.setVisible(true);
    }

}
