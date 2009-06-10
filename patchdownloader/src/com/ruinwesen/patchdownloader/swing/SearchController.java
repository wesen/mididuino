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

import java.util.Comparator;
import java.util.List;

import javax.swing.SwingUtilities;

import com.ruinwesen.patch.PatchMetadata;
import com.ruinwesen.patch.Tagset;
import com.ruinwesen.patchdownloader.indexer.Index;
import com.ruinwesen.patchdownloader.indexer.MultiCollector;
import com.ruinwesen.patchdownloader.indexer.Order;
import com.ruinwesen.patchdownloader.indexer.Query;
import com.ruinwesen.patchdownloader.indexer.ResultCollector;
import com.ruinwesen.patchdownloader.indexer.TagStats;
import com.ruinwesen.patchdownloader.swing.extra.LargeListModel;

public class SearchController {

    private SwingPatchdownloader patchdownloader;
    private boolean changed;
    private Query query = new Query();
    private Tagset displayedCategories = new Tagset();
    private Comparator<PatchMetadata> order = Order.BY_RELEVANCE;
    private ConcurrentSearch concurrentSearch = null;

    public SearchController(SwingPatchdownloader patchdownloader) {
        this.patchdownloader = patchdownloader;
    }
    
    public void indexChanged() {
        changed = true;
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
        setOrder(Order.BY_RELEVANCE);
    }

    public void setOrderByDate() {
        setOrder(Order.BY_DATE);
    }

    public void setOrder(Comparator<PatchMetadata> order) {
        if (order == null) {
            throw new IllegalArgumentException("order == null");
        }
        
        if (this.order != order) {
            this.order = order;
            setChangeFlag();
        }
    }

    public Comparator<PatchMetadata> getOrder() {
        return order;
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

       // KeyMatcher keymatcher = displayedCategories.isEmpty() ? null 
       //         : new TagKeyMatcher(displayedCategories);
        
        if (!displayedCategories.isEmpty()) {
            query.expectedTags().addAll(displayedCategories);
        } else {
            query.expectedTags().clear();
        }
        
        concurrentSearch = new ConcurrentSearch(query, order);
        new Thread(concurrentSearch).start();
        
    }
    
    private class ConcurrentSearch implements Runnable {
        
        private Query query;
        private volatile long stopped = 0;
        private volatile long exited = 0;
        private Comparator<PatchMetadata>  comparator;
        
        public ConcurrentSearch(Query query, Comparator<PatchMetadata> comparator) {
            this.query = query;
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
            Index patchIndex = patchdownloader.getPatchIndex();
   
            final TagStats statsCollector = new TagStats();
            ResultCollector resultCollector = new ResultCollector();
            MultiCollector collector = new MultiCollector(statsCollector, resultCollector);
            patchIndex.collect(collector, query.createScorer());
            
            final List<PatchMetadata> docList = resultCollector.result(order);
/*            final List<Doc> docList = 
                patchIndex.search(query, keymatcher, 0.5f, 1.0f, comparator);*/

            //!isSearchAborted() && !PatchDownloader.getSharedInstance().isStopping()
            
            if (isSearchAborted()) {
                exited = 1;
                return;
            }
            /*
            final Category[] categories = 
                query == null || query.isEmpty() 
                ? patchIndex.getCategories()
                        : patchIndex.getCategories(docList);
            */
            Runnable run = new Runnable() {
                public void run() {
                    try {
                        if (!isSearchAborted()) {
                            LargeListModel<PatchMetadata, List<PatchMetadata>> listmodel =
                                patchdownloader.getPatchListView().getListModel();
                            listmodel.setItems(docList);
                            listmodel.fireCachedEvents();
                            patchdownloader.getPatchListView().setPatchResultCounter(docList.size());   
                            //patchdownloader.getFilterListView().setDisplayedCategories(categories);
                            patchdownloader.getFilterListView().update(statsCollector);
                        }
                    } finally {
                        exited = 1;
                    }
                }
            };
            
            SwingUtilities.invokeLater(run);
        }
        
    }
    
}
