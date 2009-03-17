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

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import com.ruinwesen.patchdownloader.patch.Tag;

import name.cs.csutils.CSUtils;

public class IndexReader {

    /**
     * This array stores all documents. 
     * 
     * For each (path,docid), path!=null:
     *  path == documentTable[docid]
     */
    private StaticDoc[] documentTable ;
    
    /**
     * The map stores for each tag name (key) a list of document IDs.
     */
    private Map<String,int[]> keymap ;
    
    private transient Set<Category>  categoryNameCache;  

    private File dir;
    
    private transient volatile int updateCounter;

    public IndexReader() {
        reset();
    }
    
    /**
     * Returns a number X for which all document IDs following is true: id < X. 
     * @return 
     */
    private final int getUpperDocumentId() {
        return documentTable.length;
    }
    
    public int getState() {
        return updateCounter;
    }

    public File getIndexDir() {
        return dir;
    }
    
    public void setIndexDir(File dir) {
        this.dir = dir;
    }

    private void reset() {
        updateCounter++;
        documentTable = new StaticDoc[0];
        keymap = new HashMap<String, int[]>();
        categoryNameCache = null;
    }
    
    public void update() throws IOException {
        if (dir == null) {
            throw new FileNotFoundException("index path found: "+dir);
        }
        update(dir);
    }
    
    public void update(File file) throws IOException {
        updateCounter ++;
        this.documentTable = new StaticDoc[0];
        this.keymap.clear();
        categoryNameCache = null;
        try {
        File fdocumentIndex = new File(file, "document.index");
        if (!fdocumentIndex.isFile() && !fdocumentIndex.exists()) {
            throw new FileNotFoundException("document index not found: "+fdocumentIndex.getAbsolutePath());
        }
        File fkeyIndex = new File(file, "key.index");
        if (!fkeyIndex.isFile() && !fkeyIndex.exists()) {
            throw new FileNotFoundException("key index not found: "+fkeyIndex.getAbsolutePath());
        }
        
        InputStream inDocumentIndex = null;
        InputStream inKeyIndex = null;
        try {
            inDocumentIndex = new FileInputStream(fdocumentIndex);
            inKeyIndex = new FileInputStream(fkeyIndex);
            update(inDocumentIndex, inKeyIndex);
        } finally {
            if (inDocumentIndex != null) {
                inDocumentIndex.close();
            }
            if (inKeyIndex != null) {
                inKeyIndex.close();
            }
        }
        } catch (IOException ex) {
            reset(); 
            throw ex;
        }
    }
    
    private Set<Category> __getCategoryNames() {
        Set<Category> names = categoryNameCache;
        if (names == null) {
            names = new HashSet<Category>();

            for (Entry<String, int[]> entry: keymap.entrySet()) {
                if (entry.getKey().startsWith("category:")) {
                    int[] docList = entry.getValue();
                    int count = 0;
                    for (int i=0;i<docList.length;i++) {
                        if (isDocumentValid(docList[i])) {
                            count++;
                        }
                    }
                    if (count>0) {
                        names.add(new Category(new Tag(entry.getKey()), count));
                    }
                }
            }
            
            
            this.categoryNameCache = names;
        }
        
        return names;
    }
    
    public Category[] getCategories() {
        Set<Category> names = __getCategoryNames();
        return names.toArray(new Category[names.size()]);
    }

    public Category[] getCategories(Collection<Doc> c) {
        
        BitSet idset = new BitSet(getUpperDocumentId());
        for (Iterator<Doc> iter=c.iterator(); iter.hasNext(); ) {
            idset.set(iter.next().id());
        }

        Set<Category> names = __getCategoryNames();
        List<Category> list = new LinkedList<Category>();
        for (Category cat: names) {
            int[] docList = keymap.get(cat.tag().value());
            if (docList == null) {
                continue;
            }
            int count = 0;
            for (int i=0;i<docList.length;i++) {
                if (isDocumentValid(docList[i]) && idset.get(docList[i])) {
                    count++;
                }
            }
            if (count>0) {
                list.add(new Category(cat.tag(), count));
            }
        }
        return list.toArray(new Category[list.size()]);
    }

    /*
    public void collectKeys(KeyCollector collector) {
        for (Map.Entry<String,int[]> entry: keymap.entrySet()) {
            String key = entry.getKey();
            int[] idList = entry.getValue();
            for (int i=0;i<idList.length;i++) {
                int docid = idList[i];
                if (isDocumentValid(docid)) {
                    if (!collector.takesMore()) {
                        return;
                    }
                    collector.collect(key, docid);
                }
            }
        }
    }*/

    public MetadataIndexWriter createAppendableIndexWriter() throws IOException {
        if (dir == null) {
            throw new FileNotFoundException("index directory undefined: "+dir);
        }
        MetadataIndexWriter writer = new MetadataIndexWriter(
                new BufferedOutputStream(new FileOutputStream(new File(dir, "document.index"))),
                new BufferedOutputStream(new FileOutputStream(new File(dir, "key.index")))
        );
        write(writer);
        return writer;
    }
    
    public void write(IndexWriter writer) throws IOException {
        Map<Integer, Integer> newDocumentIdMap = new HashMap<Integer, Integer>();
        for (int docid=0;docid<getUpperDocumentId();docid++) {
            int newdocid = writer.newDocument(documentTable[docid].path);
            newDocumentIdMap.put(docid, newdocid);
        }
        for (Map.Entry<String,int[]> entry: keymap.entrySet()) {
            for (int olddocid: entry.getValue()) {
                if (isDocumentValid(olddocid)) {
                    int newdocid = newDocumentIdMap.get(olddocid);
                    writer.putKey(newdocid, entry.getKey());
                }
            }
        }
    }
    
    public void update(InputStream inDocumentIndex,
            InputStream inKeyIndex) throws IOException {
        updateCounter ++;
        this.documentTable = new StaticDoc[0];
        this.keymap.clear();
        categoryNameCache = null;
        readDocumentIndex(inDocumentIndex);
        readKeyIndex(inKeyIndex);
        updateDocumentFields();
    }

    private void readDocumentIndex(InputStream in) throws IOException {
        updateCounter ++;
        BufferedReader r = new BufferedReader(new InputStreamReader(in));
        List<String> pathlist = new ArrayList<String>();
        String line;
        while ((line=r.readLine())!=null) {
            pathlist.add(line);
        }
        
        documentTable = new StaticDoc[pathlist.size()];
        for (int i=pathlist.size()-1;i>=0;i--) {
            String path = pathlist.get(i);
            documentTable[i] = new StaticDoc(i, 0, path, null);
        }
    }

    private void readKeyIndex(InputStream in) throws IOException {
        updateCounter ++;
        BufferedReader r = new BufferedReader(new InputStreamReader(in));
        String line;
        int[] tmp = new int[0];
        while ((line=r.readLine())!=null) {
            int tmplen = 0;
            int idx = line.indexOf(',');
            if (idx == -1 || idx==0) continue; // ignore this record
            String keyName = line.substring(0,idx);
            // read document id list
            int end = idx+1;
            for (int start=end;start<line.length();) {
                end = line.indexOf(',', start);
                if (end == -1) break;
                if (start == end) {
                    start++; continue;
                }
                try {
                    int docid = Integer.parseInt(line.substring(start,end));
                    if (isDocumentValid(docid)) {
                        if (tmplen>=tmp.length) {
                            tmp = Arrays.copyOf(tmp, tmplen+100);
                        }
                        tmp[tmplen++] = docid;
                    }
                } catch (NumberFormatException ex) {
                    break;
                }
                start = end+1;
            }
            if (tmplen>0) {
                keymap.put(keyName, Arrays.copyOf(tmp, tmplen));
            }
        }
    }

    /**
     * Updates the documents date and title fields.
     */
    private void updateDocumentFields() {
        final String PREFIX_DATE = "date:"; 
        final String PREFIX_TITLE = "title:";
        
        for (Map.Entry<String, int[]> entry: keymap.entrySet()) {
            String key = entry.getKey();
            int[] docList = entry.getValue();
            if (key.startsWith(PREFIX_DATE)) {
                // convert a date YYYY-MM-DD to a long value and
                // assign it to each document 
                String dateString = key.substring(PREFIX_DATE.length());
                Date date = CSUtils.parseDate(dateString);
                if (date != null) {
                    long dateValue = date.getTime();
                    for (int i=0;i<docList.length;i++) {
                        int docid = docList[i];
                        if (isDocumentValid(docid)) {
                            documentTable[docid].date = dateValue;
                        }
                    }
                }
            } else if (key.startsWith(PREFIX_TITLE)) {
                String title = key.substring(PREFIX_TITLE.length());
                // assign the title to each document
                if (title.length()>0) {
                    for (int i=0;i<docList.length;i++) {
                        int docid = docList[i];
                        if (isDocumentValid(docid)) {
                            documentTable[docid].title = title;
                        }
                    }
                }
            }
        }
    }
    
    private boolean isDocumentValid(int docid) {
        return docid>= 0 && docid<documentTable.length && documentTable[docid] != null;
    }
    
    public Set<String> searchKeys(List<? extends Doc> documents) {
        if (documents.isEmpty()) {
            return Collections.emptySet();
        }
        Set<String> keys = new HashSet<String>();
        outher:for (Entry<String, int[]> entry: keymap.entrySet()) {
            int[] docs = entry.getValue();
            int id;
            for (int i=0;i<docs.length;i++) {
                id = docs[i];
                if (isDocumentValid(id)) {
                    for (int j=0;j<documents.size();j++) {
                        if (id == documents.get(j).id()) {
                            keys.add(entry.getKey());
                            continue outher;
                        }
                    }
                }
            }
        }
        return keys;
    }
    
    public List<Doc> search(Query query, KeyMatcher keymatcher,
            float minscore, float maxscore, DocComparator<? extends Doc> comparator) {
        String[] keys = null;
        if (query != null && !query.isEmpty()) {
            List<String> keyList = query.tokenize();
            keys = keyList.toArray(new String[keyList.size()]);
        }
        return search(keys, keymatcher, minscore, maxscore, comparator);
    }
    
    /**
     * Performs a search on the current index
     * @param keys the keys used to compute the score value (matchedkeys/(keycount-1))
     * @param keymatcher only documents for which all keymatchers match are returned
     * @param comparator sorts the resulting document list
     */
    public List<Doc> search(String[] keys, KeyMatcher keymatcher,
            float minscore, float maxscore, DocComparator<? extends Doc> comparator) {

        if (keys == null && keymatcher == null) {
            List<Doc> docList = new ArrayList<Doc>(getUpperDocumentId());
            for (int docid=0;docid<getUpperDocumentId();docid++) {
                Doc doc = documentTable[docid];
                if (doc!=null) { // == isDocumentValid(docid)
                    docList.add(doc);
                }
            }
            
            if (comparator != null) {
                Collections.sort(docList, comparator);
            }
            
            return docList;
        }
        
        // if this set is null we can collect from all documents,
        // otherwise we can collecto only documents which are in this set.
        BitSet collectibleDocuments = null;
        if (keymatcher != null) {
            collectibleDocuments = new BitSet(getUpperDocumentId());
            findDocuments(keymatcher, collectibleDocuments);
            if (collectibleDocuments.isEmpty()) { 
                // emptyset intersected with anything else is still an 
                // empty set, nothing more to do
                return Collections.emptyList();
            }
        }
        
        if (keys == null) { // => keymatcher != null => collectibleDocuments != null
            List<Doc> docList = new ArrayList<Doc>(getUpperDocumentId());
            for (int docid=0;docid<getUpperDocumentId();docid++) {
                Doc doc = documentTable[docid];
                if (doc!=null && collectibleDocuments.get(doc.id())) { // == isDocumentValid(docid)
                    docList.add(doc);
                }
            }
            
            if (comparator != null) {
                Collections.sort(docList, comparator);
            }
            
            return docList;
        }
        
        // *********************************************************************
        // now we search for the specified keys and count the matching keys
        // for each document
        
        int[] matchingKeyCountForDocument = new int[getUpperDocumentId()];
        Arrays.fill(matchingKeyCountForDocument, 0);
        
        for (String key: keys) {
            int[] matchingDocumentsForKey = keymap.get(key);
            if (matchingDocumentsForKey != null) {
    
                // at this point it does not matter if the document is collectible or not
                // we spare us this check because a simple integer increment
                // should perform much better in comparison to a set.contains(docid)
                for (int i=0;i<matchingDocumentsForKey.length;i++) {
                    int docid = matchingDocumentsForKey[i];
                    if (isDocumentValid(docid)) {
                        matchingKeyCountForDocument[docid]++;
                    }
                }
            }
            
            // alsa search categories
            matchingDocumentsForKey = keymap.get("category:"+key);
            if (matchingDocumentsForKey != null) {
    
                // at this point it does not matter if the document is collectible or not
                // we spare us this check because a simple integer increment
                // should perform much better in comparison to a set.contains(docid)
                for (int i=0;i<matchingDocumentsForKey.length;i++) {
                    int docid = matchingDocumentsForKey[i];
                    if (isDocumentValid(docid)) {
                        matchingKeyCountForDocument[docid]++;
                    }
                }
            }
        }
        
        // get the scored documents
        List<Doc> scoredDocuments = 
            new ArrayList<Doc>(collectibleDocuments != null ? 
                    collectibleDocuments.size() : 200);
         
        float keycountf = keys.length;
        // now add the elements
        for (int i=0;i<getUpperDocumentId();i++) {
            int matchingKeyCount = matchingKeyCountForDocument[i];
            float score = matchingKeyCount/keycountf;
            Doc doc = documentTable[i];
            if (    (doc != null) &&
                    (score>=minscore) &&
                    (score<=maxscore) &&
                    (collectibleDocuments == null || collectibleDocuments.get(i))) {
                scoredDocuments.add(new ScoreDoc(doc, score));
            }
        }
        
        if (comparator != null) {
            Collections.sort(scoredDocuments, comparator);
        }
        
        return scoredDocuments;
    }
    
    private void findDocuments(KeyMatcher keymatcher, BitSet set) {
        if (keymatcher == null) {
            throw new IllegalArgumentException("keymatcher == null");
        }
        // remove all non-matching
        for (Map.Entry<String, int[]> entry: this.keymap.entrySet()) {
            final String key = entry.getKey();
            if (keymatcher.matches(key)) {
                int[] docs = entry.getValue();
                for (int i=0;i<docs.length;i++) {
                    if (isDocumentValid(docs[i])) {
                        set.set(docs[i]);
                    }
                }
            }
        }
    }
    
}
