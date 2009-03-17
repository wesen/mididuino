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
package com.ruinwesen.patchdownloader.swing;

import java.util.List;

import javax.swing.SwingUtilities;

import com.ruinwesen.patchdownloader.indexer.Category;
import com.ruinwesen.patchdownloader.indexer.Doc;
import com.ruinwesen.patchdownloader.indexer.DocComparator;
import com.ruinwesen.patchdownloader.indexer.IndexReader;
import com.ruinwesen.patchdownloader.indexer.KeyMatcher;
import com.ruinwesen.patchdownloader.indexer.Query;
import com.ruinwesen.patchdownloader.patch.Tag;
import com.ruinwesen.patchdownloader.patch.Tagset;

public class SearchController {

    private PDFrame patchdownloader;
    private boolean changed;
    private Query query = new Query("");
    private Tagset displayedCategories = new Tagset();
    private final static DocComparator<Doc> BY_RELEVANCE = new DocComparator.ByRelevance();
    private final static DocComparator<Doc> BY_DATE = new DocComparator.ByDate();
    private boolean orderByRelevance = true;
    private ConcurrentSearch concurrentSearch = null;

    public SearchController(PDFrame patchdownloader) {
        this.patchdownloader = patchdownloader;
    }
    
    public void indexChanged() {
        changed = true;
        patchdownloader.getMetadataCache().clear();
    }
    
    public void setQuery(Query query) {
        if (query == null) {
            throw new IllegalArgumentException("query == null");
        }
        if (!this.query.equals(query)) {
            this.query = query;
            
            patchdownloader.getPatchListView().setQuery(query);
            setChangeFlag();
        }
    }
    
    public void setDisplayedCategories(Tagset tagset) {
        if (tagset == null) {
            tagset = new Tagset();
        }
        
        if (!this.displayedCategories.equals(tagset)) {
            this.displayedCategories = new Tagset(tagset);
            setChangeFlag();
        }
    }

    public void setOrderByRelevance() {
        setOrder(true);
    }

    public void setOrderByDate() {
        setOrder(false);
    }

    private void setOrder(boolean byRelevance) {
        if (this.orderByRelevance != byRelevance) {
            this.orderByRelevance = byRelevance;
            setChangeFlag();
        }
    }

    public boolean isOrderByRelevanceSet() {
        return this.orderByRelevance;
    }
    
    public void setChangeFlag() {
        this.changed = true;
    }
    
    public void update() {
        if (changed) {
            changed = false;
            performSearch();
        }
    }

    protected void performSearch() {
        if (concurrentSearch != null) {
            concurrentSearch.abortSearch();
            while (!concurrentSearch.isExited()) {
                Thread.yield();
            }
            concurrentSearch = null;
        }

        KeyMatcher keymatcher = displayedCategories.isEmpty() ? null 
                : new TagKeyMatcher(displayedCategories);
        
        
        concurrentSearch = new ConcurrentSearch(query, keymatcher,
                orderByRelevance ? BY_RELEVANCE : BY_DATE);
        new Thread(concurrentSearch).start();
        
    }

    private class ConcurrentSearch implements Runnable {
        
        private Query query;
        private volatile long stopped = 0;
        private volatile long exited = 0;
        private KeyMatcher keymatcher;
        private DocComparator<Doc> comparator;
        
        public ConcurrentSearch(Query query, KeyMatcher keymatcher,
                DocComparator<Doc> comparator) {
            this.query = query;
            this.keymatcher = keymatcher;
            this.comparator = comparator;
        }
        
        public void abortSearch() {
            stopped = 1;
        }
        
        public boolean isSearchAborted() {
            return stopped != 0; 
        }

        public boolean isExited() {
            return exited != 0;
        }
        
        public void run() {
            IndexReader patchIndex = patchdownloader.getPatchIndex();

            final List<Doc> docList = 
                patchIndex.search(query, keymatcher, 0.5f, 1.0f, comparator);

            //!isSearchAborted() && !PatchDownloader.getSharedInstance().isStopping()
            
            if (isSearchAborted()) {
                exited = 1;
                return;
            }
            final Category[] categories = 
                query == null || query.isEmpty() 
                ? patchIndex.getCategories()
                        : patchIndex.getCategories(docList);
            
            Runnable run = new Runnable() {
                public void run() {
                    try {
                        if (!isSearchAborted()) {
                            LargeListModel<Doc, List<Doc>> listmodel =
                                patchdownloader.getPatchListView().getListModel();
                            listmodel.setItems(docList);
                            listmodel.fireCachedEvents();
                            patchdownloader.getPatchListView().setPatchResultCounter(docList.size());   
                            patchdownloader.getFilterListView().setDisplayedCategories(categories);
                        }
                    } finally {
                        exited = 1;
                    }
                }
            };
            
            SwingUtilities.invokeLater(run);
        }
        
    }

    private static class TagKeyMatcher extends KeyMatcher {
        private Tagset set;

        public TagKeyMatcher(Tagset set) {
            this.set = set;
        }

        @Override
        public boolean matches(String key) {
            return set.contains(new Tag(key));
        }
    }
    
}
