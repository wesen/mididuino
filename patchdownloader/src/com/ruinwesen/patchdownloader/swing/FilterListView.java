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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.util.LinkedList;
import java.util.List;

import javax.swing.AbstractListModel;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListCellRenderer;
import javax.swing.ListModel;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;

import com.ruinwesen.patchdownloader.indexer.Category;
import com.ruinwesen.patchdownloader.patch.Tagset;
import com.ruinwesen.patchdownloader.swing.extra.SetSelectionModel;
import com.ruinwesen.patchdownloader.swing.panels.SectionBorder;

public class FilterListView {

    private Category[] allCategories;
 //   private BitSet selectedCategories ;
    private Category[] visibleCategories;
    
    public JList listView = new JList();
    private JComponent container;
    private SwingPatchdownloader patchdownloader;
    private MySetSelectionModel selectionModel;
    private boolean selectionModelUpdating = false;
    
    public FilterListView(SwingPatchdownloader patchdownloader) {
        this.patchdownloader = patchdownloader;
        JPanel pane = new JPanel(new BorderLayout());
        SectionBorder border = new SectionBorder(I18N.translate("translation.categories", "Categories"));
        pane.setBorder(border);

        listView.setModel(createModel());
        listView.setCellRenderer(new FilterListCellRenderer());
        selectionModel = new MySetSelectionModel();
        listView.setSelectionModel(selectionModel);
        
        JScrollPane scrollPane = new JScrollPane(listView);
        pane.add(scrollPane, BorderLayout.CENTER);
        container = pane;
        
        visibleCategories = new Category[0];
        allCategories = patchdownloader.getPatchIndex().getCategories();
  //      selectedCategories = new BitSet(allCategories.length);
    }
    
    private int indexOf(Category c) {
        for (int i=0;i<allCategories.length;i++) {
            if (allCategories[i].equals(c)) {
                return i;
            }
        }
        return -1;
    }

    /*
    private void setAll(List<Category> dst, 
            BitSet set, Category[] list, boolean value) {
        for (int i=0;i<list.length;i++) {
            int idx = indexOf(list[i]);
            if (idx>=0 && idx<set.length()) {
                if (dst != null) {
                    dst.add(list[i]);
                }
                set.set(idx, value);
            }
        }
    }*/
    
    private class MySetSelectionModel extends SetSelectionModel {

        public MySetSelectionModel() {
            super(listView);
        }

        @Override
        protected void notifySelectionChanged(int index) {
            if (selectionModelUpdating ) {
                return;
            }
            if (index>=0 && index<visibleCategories.length) {
              //  int idx = indexOf(visibleCategories[index]);
            //    selectedCategories.set(idx, isSelectedIndex(index));
                System.out.println(getFilteringTags());
                patchdownloader.getSearchController().setDisplayedCategories(getFilteringTags());
                patchdownloader.getSearchController().update();
            }
        }
        
    }
    
    public Tagset getFilteringTags() {
        Tagset set = new Tagset();
        for (int i=0;i<visibleCategories.length;i++) {
            if (selectionModel.isSelectedIndex(i)) {
                set.add(visibleCategories[i].tag());
            }
        }
        return set;
    }
    
    public JComponent getComponent() {
        return container;
    }

    private ListModel createModel() {
        return new AbstractListModel() {
            private static final long serialVersionUID = 2858943808230551393L;
            public int getSize() { return visibleCategories.length; }
            public Object getElementAt(int i) { return visibleCategories[i]; }
          };
    }
    
    public void setDisplayedCategories(Category[] categories) {
        if (categories == null) {
            throw new IllegalArgumentException("categories==null");
        }

        List<Category> selected = new LinkedList<Category>();
        for (int i=0;i<visibleCategories.length;i++) {
            if (selectionModel.isSelectedIndex(i)) {
                selected.add(visibleCategories[i]);
            }
        }
        
        selectionModelUpdating = true;
        this.visibleCategories = categories;
        listView.setModel(createModel()); // new model so list will update

        for (Category cat: selected) {
            for (int i=0;i<visibleCategories.length;i++) {
                if (cat.equals(visibleCategories[i])) {
                    selectionModel.setAdd(i);
                }
            }
        }
        
        selectionModelUpdating = false;
        listView.repaint(); // TODO fire events
    }
    
    private static class FilterListCellRenderer implements ListCellRenderer {
       
        DefaultListCellRenderer delegate = new DefaultListCellRenderer();
    
        public Component getListCellRendererComponent(
            JList list,
            Object value,
            int index,
            boolean isSelected,
            boolean cellHasFocus) {
            Color foreground = list.getForeground();
            if (foreground == null) {
                foreground = list.getSelectionForeground();
                if (foreground == null)
                    foreground = Color.BLACK;
            }
            String bluer = CSUtils.toHTMLColor(isSelected ? list.getSelectionForeground() : CSUtils.change(foreground, 0, 0, 0.8f));
            Category cat = (Category) value;
            String text = "<html>"+cat.name()+" (<span style=\"color:"+bluer+"\">"+cat.size()+"</span>)";
            
            delegate.getListCellRendererComponent(list, text, index, isSelected, cellHasFocus);
            Dimension pref = delegate.getUI().getPreferredSize(delegate);
            delegate.setPreferredSize(new Dimension(pref.width, 30));
            return delegate;
        }

    }

}
