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
package com.ruinwesen.patchmanager.swing.tasks;

import java.util.Comparator;

import com.ruinwesen.patchmanager.client.index.IndexedPatch;
import com.ruinwesen.patchmanager.client.index.Query;
import com.ruinwesen.patchmanager.swing.SwingPatchManager;
import com.ruinwesen.patchmanager.swing.components.Highlighter;
import com.ruinwesen.patchmanager.swing.components.SearchOptionsControl.FilterUpdater;
import com.ruinwesen.patchmanager.swing.model.PatchQueryCollector;

import name.cs.csutils.collector.Collector;
import name.cs.csutils.collector.MultiCollector;
import name.cs.csutils.concurrent.SimpleSwingWorker;

public class PerformQueryTask extends SimpleSwingWorker {

    /**
     * 
     */
    private static final long serialVersionUID = 6615142642642992202L;
    private SwingPatchManager patchmanager;
    private String queryString;
    private FilterUpdater filterUpdater;
    private Comparator<IndexedPatch> order;

    public PerformQueryTask(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
    }

    @Override
    protected synchronized void setup() {
        this.order = patchmanager.getSearchOptionsControl().getOrder();
        this.queryString = patchmanager.getQueryString();
        patchmanager.getPatchListCellRenderer()
        .setHighlighter(new Highlighter(queryString));
        patchmanager.getPatchListModel().setOrder(order);
    }

    @Override
    protected void process() {
        String queryString;
        synchronized(this) {
            queryString = this.queryString;
        }
        
        Query query = new Query(null, queryString, 0);
        Query filteringQuery = new Query(null, queryString, 0.5f);
        patchmanager.getSearchOptionsControl().enableFiltersInQuery(filteringQuery);
        patchmanager.getPatchListModel().clear();

        FilterUpdater filterUpdater = patchmanager.getSearchOptionsControl().createFilterUpdater();
        
        Collector<IndexedPatch> collector 
            = new MultiCollector<IndexedPatch>(
                    new PatchQueryCollector(query, filterUpdater),
                    new PatchQueryCollector(filteringQuery, patchmanager.getPatchListModel())
              )
              ;        
        patchmanager.getIndex().collect(collector);
        patchmanager.getPatchListModel().update();
        patchmanager.getPatchListView().repaint(); // patchListModel bug => repaint should not be necessary
        synchronized (this) {
            this.filterUpdater = filterUpdater;
        }
    }
    
    @Override
    protected void cleanup() {
        FilterUpdater filterUpdater;
        synchronized (this) {
            filterUpdater = this.filterUpdater;
        }
        if (filterUpdater != null) {
            filterUpdater.updateFilterUI();
        }
    }

}
