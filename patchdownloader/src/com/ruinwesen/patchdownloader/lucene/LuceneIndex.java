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
package com.ruinwesen.patchdownloader.lucene;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.Field.Index;
import org.apache.lucene.document.Field.TermVector;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.Term;
import org.apache.lucene.index.IndexWriter.MaxFieldLength;

/**
 * 
 * @author chresan
 */
public class LuceneIndex {

    private static Log log = LogFactory.getLog(LuceneIndex.class);

    public static final String DOCUMENT_ID = "documentid";
    
    private IndexWriter indexWriter;
    protected File indexDir;
    private Analyzer indexAnalizer = new StandardAnalyzer();
    
    private boolean optimizeAfterCommit = true;
    
    public static class DebugIndex extends LuceneIndex {
        
        private DebugIndex() {
            super();
        }
        
        public static DebugIndex getInstance() {
            return new DebugIndex();
        }
        
        private File newTempDir() {
            File tempFile;
            try {
                tempFile = File.createTempFile("debug",".tmpindex");
                tempFile.delete(); // delete the file
                if (tempFile.mkdir()) { // recreate a directory
                    return tempFile;
                }
            } catch (IOException e) {
                // ignore
            }
            // it didn't work
            String tmpdir = System.getProperty("java.io.tmpdir");
            if (tmpdir == null) {
                throw new RuntimeException("could not locate the system temporary directory");
            }
            String chars = "abcdef0123456789";
            StringBuilder name = new StringBuilder();
            for (int i=0;i<10;i++) {
                int r = (int)(Math.random()*chars.length());
                name.append(chars.charAt(r));
            }
            
            tempFile = new File(tmpdir, "debug."+name+".tempindex");
            if (!tempFile.mkdir()) {
                throw new RuntimeException("could not create temporary directory: "+tempFile.getAbsolutePath());
            }
            return tempFile;
        }
        
        public void deleteIndex() {
            if (this.indexDir != null) {
                File f = this.indexDir;
                this.indexDir = null;
                deleteRecursive(f);
            }
            this.indexDir = null;
        }
        
        public File getIndexDir() {
            if (indexDir == null) {
                indexDir = newTempDir();
            }
            return this.indexDir;
        }

        public void setIndexDir(File file) {
            throw new UnsupportedOperationException("implementation uses a temporary directory");
        }
        
        private void deleteRecursive(File file) {
            if (file.isDirectory()) {
                for (File f: file.listFiles()) {
                    deleteRecursive(f);
                }
            }
            if (file.delete()) {
                System.out.println("[OK]     delete: "+file.getAbsolutePath());
            } else {
                System.out.println("[FAILED] delete: "+file.getAbsolutePath());
            }
        }

        
    }
    
    public LuceneIndex() {
        super();
    }
    
    public LuceneIndex(File indexDir) throws CorruptIndexException, IOException {
        super();
        setIndexDir(indexDir);
    }
    
    public void setOptimizeAfterCommitEnabled(boolean enable) {
        this.optimizeAfterCommit = enable;
    }
    
    public boolean isOptimizeAfterCommitEnabled() {
        return this.optimizeAfterCommit;
    }
    
    public File getIndexDir() {
        return this.indexDir;
    }
    
    public void setIndexDir(File file) {
        // only go in if the argument is different to the current indexDir
        if (this.indexDir != file || this.indexDir != null && !this.indexDir.equals(file)) {
            this.indexDir = file;
        }
    }
    
    protected void openIndexWriter(boolean forceNewIndex, boolean allowCreateIndex) throws CorruptIndexException, IOException {
        if (log.isDebugEnabled()) {
            log.debug("Index.openIndexWriter[forceNewIndex="+forceNewIndex+",allowCreateIndex="+allowCreateIndex+"]");
        }
        IndexWriter newIndexWriter;
        if (forceNewIndex) {
            newIndexWriter = new IndexWriter(getIndexDir(), indexAnalizer, true,
                    MaxFieldLength.LIMITED);
        } else {
            try {
                newIndexWriter = new IndexWriter(getIndexDir(), indexAnalizer, false,
                    MaxFieldLength.LIMITED);
            } catch (FileNotFoundException ex) {
                if (allowCreateIndex) {
                    newIndexWriter = new IndexWriter(getIndexDir(), indexAnalizer, true,
                            MaxFieldLength.LIMITED);
                } else {
                    throw ex; // rethrow exception because there is not index
                }       
            }
        }
        this.indexWriter = newIndexWriter;
    }

    protected void closeIndexWriter() throws CorruptIndexException, IOException {
        if (log.isDebugEnabled()) {
            log.debug("Index.closeIndexWriter");
        }
        if (indexWriter != null) {
            indexWriter.commit();
            indexWriter.close();
            indexWriter = null;
        }
    }
    
    public IndexReader newIndexReader() throws IOException {
        if (log.isDebugEnabled()) {
            log.debug("Index.newIndexReader");
        }
        ensureIndexWriterAvailable();
        IndexReader newIndexReader = IndexReader.open(indexWriter.getDirectory(), true);
        return newIndexReader;
    }

    protected void ensureIndexWriterAvailable() throws IOException {
        if (this.indexWriter  == null) {
            File dir = getIndexDir();
            throw new IOException("could not access index (IndexWriter unavailable): "+
                    (dir == null ? null : dir.getAbsolutePath()));
        }
    }

    /**
     * Updates the specified document.
     * 
     * @param type the document type
     * @param id the unique document id
     * @param metadata the new metadata
     * @throws IOException 
     * @throws CorruptIndexException 
     */
    public void update(String id, Document doc) throws CorruptIndexException, IOException {        
        if (log.isDebugEnabled()) {
            log.debug("Index.update index (document id: "+id+") ");
        }
        ensureIndexWriterAvailable();
        doc.add(documentIdField(id));
        indexWriter.updateDocument(documentIdTerm(id), doc);
    }

    /**
     * Deletes the document from the index.
     * @param id the document id
     * @throws CorruptIndexException
     * @throws IOException
     */
    public void delete(String id) throws CorruptIndexException, IOException {        
        if (log.isDebugEnabled()) {
            log.debug("Index.delete (document id: "+id+")");
        }
        ensureIndexWriterAvailable();
        indexWriter.deleteDocuments(documentIdTerm(id));
    }
    
    /**
     * Adds the specified document to the index.
     * @param id
     * @param doc
     * @throws CorruptIndexException
     * @throws IOException
     */
    public void index(String id, Document doc) throws CorruptIndexException, IOException { 
        if (log.isDebugEnabled()) {
            log.debug("Index.index (document id: "+id+")");
        }
        ensureIndexWriterAvailable();
        doc.add(documentIdField(id));
        indexWriter.addDocument(doc);
    }
    
    private Field documentIdField(String id) {
        return new Field(DOCUMENT_ID, id, Field.Store.YES, Index.NOT_ANALYZED, TermVector.NO);
    }

    private Term documentIdTerm(String id) {
        return new Term(DOCUMENT_ID, id);
    }
    
    public void open() throws CorruptIndexException, IOException {
        open(true);
    }

    public void open(boolean allowCreateIndex) throws CorruptIndexException, IOException {
        openIndexWriter(false, allowCreateIndex);
    }

    public void close() throws CorruptIndexException, IOException {
        if (log.isDebugEnabled()) {
            log.debug("Index.close");
        }
        closeIndexWriter();
    }
    
    public void commit() throws CorruptIndexException, IOException { 
        if (indexWriter != null) {
            if (log.isDebugEnabled()) {
                log.debug("Index.commit");
            }
            indexWriter.commit();
            if (optimizeAfterCommit) {
                indexWriter.optimize();
            }
        }
    }
    
    public void rollback() throws IOException {
        if (indexWriter != null) {
            // TODO check if this is ok
            indexWriter.rollback();
            close();
            open();
        }
    }
    
    protected void finalize() throws Throwable {
        try {
            close();
        } catch (CorruptIndexException ex) {
            if (log.isErrorEnabled()) {
                log.error("close() failed in finalize()", ex);
            }
        } catch (IOException ex) {
            if (log.isErrorEnabled()) {
                log.error("close() failed in finalize()", ex);
            }
        }
        super.finalize();
    }
    
    
}
