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
package com.ruinwesen.patchdownloader.indexer;

import java.io.IOException;
import java.io.OutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

public class IndexWriter {

    private int documentIdCounter = 0;
    private OutputStream outDocumentIndex;
    private OutputStream outKeyIndex;
    private List<Key> keyList = new ArrayList<Key>();
    

    public IndexWriter(OutputStream outDocumentIndex,
            OutputStream outKeyIndex) {
        this.outDocumentIndex = outDocumentIndex;
        this.outKeyIndex = outKeyIndex;
    }
    
    /**
     * Adds a new document entry and returns the document id.
     * @param path path of the document
     * @return the document id
     * @throws IOException 
     */
    public int newDocument(String path) throws IOException {
        String str = path+"\n";
        writeString(outDocumentIndex, str);
        return documentIdCounter++;
    }
    
    private void writeString(OutputStream out, String str) throws IOException {
        byte[] b = str.getBytes();
        out.write(b, 0, b.length);
    }

    public void putKey(int docid, String key) throws IOException {
        keyList.add(new Key(docid,key));
    }
    
    public void flush() throws IOException {
        Collections.sort(keyList, new KeyNameComparator()); // sort by key name
        Iterator<Key> iter = keyList.iterator();
        
        
        String lastkey = null;
        final String EOL = "\n"; // end of line
        while (iter.hasNext()) {
            Key key = iter.next();
            if (key.key.equals(lastkey)) {
                // there is already a record - continue it
                writeString(outKeyIndex, Integer.toString(key.docid)+',');
            } else {
                if (lastkey != null) { // if there is one, then ...
                    writeString(outKeyIndex, EOL); // end the previous record
                }
                lastkey = key.key;
                // begin new record
                writeString(outKeyIndex,
                        key.key+","+Integer.toString(key.docid)+',');
            }
            if (lastkey == null) {
                writeString(outKeyIndex, key+",");
            }
        }
        if (lastkey != null) {
            writeString(outKeyIndex, EOL);
        }
    }
    
    public void close() throws IOException {
        outDocumentIndex.flush();
        outKeyIndex.flush();
        outDocumentIndex.close();
        outKeyIndex.close();
    }

    private static class Key {
        int docid;
        String key;
        public Key(int docid, String key) {
            this.docid = docid;
            this.key=key;
        }
    }

    private static final class KeyNameComparator 
        implements Comparator<Key>, Serializable {

        private static final long serialVersionUID = 4625170710796375432L;

        @Override
        public int compare(Key a, Key b) {
            return a.key.compareTo(b.key);
        }
        
    }
    
}
