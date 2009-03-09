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
package com.ruinwesen.patchdownloader.swing;

import java.awt.Component;
import java.io.IOException;

import javax.swing.DefaultListCellRenderer;
import javax.swing.DefaultListModel;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.SwingUtilities;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.HitCollector;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.Searcher;

import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.lucene.LuceneIndex;
import com.ruinwesen.patchdownloader.patch.PatchDocument;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;

public class PatchListView {

    private JList listView;
    private LuceneIndex luceneIndex;
    private IndexSearcher searcher = null;
    private Query query;
    private SearchHitsListModel listmodel;
    
    public PatchListView() {
        super();
        init();
        this.luceneIndex = PatchDownloader.getSharedInstance().getLuceneIndex();
    }
    
    private void init() {
        listmodel = new SearchHitsListModel();
        listView = new JList(listmodel);
        //Font font = CSUtils.changeFontSize(UIManager.getFont("TextPane.font"), 0.8f);
        // listView.setFont(font); 
    }
    
    public JComponent getComponent() {
        return listView;
    }
    
    public void setQuery(Query query) {
        this.query = query;
        if (query != null) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    performSearch();
                }
            });
        }
    }
    
    protected void performSearch() {
        if (query == null) {
            return;
        }
        // clean up
        if (searcher != null) {
            try {
                searcher.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            searcher = null;
        }
        
        IndexReader reader;
        try {
            reader = luceneIndex.newIndexReader();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return;
        }
       
        
        this.searcher = new IndexSearcher(reader);
        
        // now create new listmodel and renderer and ...
        listmodel = new SearchHitsListModel();
        listView.setModel(listmodel);
        listView.setCellRenderer(new IndexedPatchListCellRenderer(searcher));
        
        try {
            searcher.search(query, listmodel.createHitCollector());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    private static class IndexedPatch {
        int docid;
        float score;

        public IndexedPatch(int docid) {
            this.docid = docid;
            this.score = -1;
        }
        
        public IndexedPatch(int docid, float score) {
            this.docid = docid;
            this.score = score;
        }
    }
    
    private static class PutHitsIntoListModel extends HitCollector {
        private SearchHitsListModel model;

        public PutHitsIntoListModel(SearchHitsListModel model) {
            this.model = model;
        }

        @Override
        public void collect(int doc, float score) {
            model.put(this, new IndexedPatch(doc, score));
        }
    }
    
    private static class SearchHitsListModel extends DefaultListModel {
        
        public HitCollector createHitCollector() {
            return new PutHitsIntoListModel(this);
        }
        
        public void put(PutHitsIntoListModel putter, IndexedPatch patch) {
            addElement(patch);
        }
        
    }
    
    private static class IndexedPatchListCellRenderer extends DefaultListCellRenderer {

        private Searcher searcher;

        public IndexedPatchListCellRenderer(Searcher searcher) {
            this.searcher = searcher;
            
        }
        
        public Component getListCellRendererComponent(
            JList list,
            Object value,
            int index,
            boolean isSelected,
            boolean cellHasFocus) {
            super.getListCellRendererComponent(list, "", index, isSelected, cellHasFocus);
            
            IndexedPatch patch = (IndexedPatch) value;
            PatchMetadata metadata = getMetadata(patch);
            
            if (metadata == null) {
                setText("[Unavailable]");
            } else {
                String title = metadata.getTitle();
                if (title == null || title.trim().length() == 0) {
                    title = "Title: -unknown-";
                }
                String tags = metadata.getTags().toString();
                setText("<html><b>"+title+"</b><br>Tags: "+tags+"</html>");
            }
            
            return this;
        }

        private PatchMetadata getMetadata(IndexedPatch patch) {
            try {
                Document document = searcher.doc(patch.docid);
                return PatchDocument.ToMetadata(document);
            } catch (Exception ex) {
                return null;
            }
        }
        
    }
    
}
