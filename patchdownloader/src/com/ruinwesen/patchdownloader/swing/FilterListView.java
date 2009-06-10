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
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import javax.swing.AbstractListModel;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListCellRenderer;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;

import com.ruinwesen.patch.Tag;
import com.ruinwesen.patch.Tagset;
import com.ruinwesen.patchdownloader.indexer.TagStats;
import com.ruinwesen.patchdownloader.swing.panels.SectionBorder;

public class FilterListView implements ListSelectionListener {

    public static final class Category {
        private String name;
        private int size;

        public Category(Tag tag, int size) {
            this.name = tag.normalizedValue().substring("category:".length());
            this.size = size;
        }
        
        public Category(String name, int size) {
            this.name = name;
            this.size = size;
        }
        public String name() {
            return name;
        }
        public Tag tag() {
            return new Tag("category:"+name);
        }
        public int size() {
            return size;
        }
        @Override
        public boolean equals(Object o) {
            if (o instanceof Category) {
                return ((Category)o).name.equalsIgnoreCase(name);
            }
            return false;
        }
    }
    
    
    public JList listView = new JList();
    private JComponent container;
    private SwingPatchdownloader patchdownloader;
    private boolean isUpdatingCategories = false;
    
    public FilterListView(SwingPatchdownloader patchdownloader) {
        this.patchdownloader = patchdownloader;
        JPanel pane = new JPanel(new BorderLayout());
        SectionBorder border = new SectionBorder(I18N.translate("translation.categories", "Categories"));
        pane.setBorder(border);

        listView.setCellRenderer(new FilterListCellRenderer());
        listView.setModel(new SimpleListModel(Collections.<Category>emptyList()));
        
        JScrollPane scrollPane = new JScrollPane(listView);
        pane.add(scrollPane, BorderLayout.CENTER);
        container = pane;
        
        listView.addListSelectionListener(this);
        new EventHandler().install();
    }
    
    public JComponent getComponent() {
        return container;
    }

    private int insert(List<Category> list, Category item) {
        for (int i=0;i<list.size();i++) {
            Category cmp = list.get(i);
            if (cmp.name().compareToIgnoreCase(item.name())>=0) {
                list.add(i, item);
                return i;
            }
        }
        list.add(item);
        return list.size()-1;
    }

    @Override
    public void valueChanged(ListSelectionEvent e) {
        ListSelectionModel selectionModel = listView.getSelectionModel();
        if (!selectionModel.getValueIsAdjusting() && !isUpdatingCategories) {
            Category selected = (Category) listView.getSelectedValue();
            SearchController sc = patchdownloader.getSearchController();
            
            Tagset tags = new Tagset();
            if (selected != null) {
                tags.add(selected.tag());
            }
            
            sc.setDisplayedCategories(tags);
            sc.update();
        }
    }

    public void update(TagStats stats) {
        List<Category> list = new LinkedList<Category>();
        for (Tag tag: stats.tags()) {
            if (tag.normalizedValue().startsWith("category:")
                    && stats.getCount(tag)>0) {
                Category cat = new Category(tag, stats.getCount(tag));
                list.add(cat);
            }
        }
        setDisplayedCategories(list.toArray(new Category[list.size()]));
    }
    
    public synchronized void setDisplayedCategories(Category[] categories) {
        if (categories == null) {
            throw new IllegalArgumentException("categories==null");
        }
        
        // the categories which contain currently displayed patches
        List<Category> displayedCategories = new ArrayList<Category>(
                Arrays.asList(categories));
        
        if (listView.getModel().getSize() == displayedCategories.size()) {
            List<Category> current = ((SimpleListModel)listView.getModel()).list;
            if (current.containsAll(displayedCategories)) {
            // we only have to update the count values
            for (int i = 0;i<current.size();i++) {
                current.remove(i);
                current.add(i, categories[i]);
            }
            ((SimpleListModel)listView.getModel()).fireContentsChanged();
            return;
            }
        }
        
        isUpdatingCategories = true;
        // remove categories which are not in the displayedCategories list
        // and which are not selected (keepSelectionAt)
        Category selected = (Category) listView.getSelectedValue();
        // insert selected in to the displayedCategories list if
        // it is not null or already present
        int newSelectedIndex = -1;
        if (selected != null) {
            newSelectedIndex = displayedCategories.indexOf(selected);
            if (newSelectedIndex<0) {
                // the category is empty
                selected = new Category(selected.tag(), 0);
                newSelectedIndex = insert(displayedCategories, selected);
            }
        }
        
        SimpleListModel model = new SimpleListModel(displayedCategories);
        listView.setModel(model);
        listView.setSelectedIndex(newSelectedIndex);
        isUpdatingCategories = false;

        // all available categories
        // List<Category> allCategories  = new ArrayList<Category>(
        //        Arrays.asList(patchdownloader.getPatchIndex().getCategories()));
    }
    
    private class EventHandler extends MouseAdapter implements ListSelectionListener {

        private int mousePressedSelectionIndex = -1;
        private int mouseReleasedSelectionIndex = -1;
        private boolean valueChanged = false;

        @Override
        public void mousePressed(MouseEvent e) {
            mousePressedSelectionIndex = listView.getSelectedIndex();
            valueChanged = false;
        }

        @Override
        public void mouseReleased(MouseEvent e) {
            mouseReleasedSelectionIndex = listView.getSelectedIndex();
            if (mousePressedSelectionIndex == mouseReleasedSelectionIndex
                    && !valueChanged) {
                listView.clearSelection();
            }
            valueChanged = false;
        }

        public void install() {
            listView.addMouseListener(this);
            listView.getSelectionModel().addListSelectionListener(this);
        }

        @Override
        public void valueChanged(ListSelectionEvent e) {
            if (!e.getValueIsAdjusting()) {
                valueChanged = true;
            }
        }
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

    private static class SimpleListModel extends AbstractListModel {

        private static final long serialVersionUID = 4798765566936658850L;
        private List<Category> list;
        
        public SimpleListModel(List<Category> list) {
            this.list = list;
        }
        
        public void fireContentsChanged() {
            fireContentsChanged(this, 0, list.size()-1);
        }
        
        @Override
        public Object getElementAt(int index) {
            return list.get(index);
        }

        @Override
        public int getSize() {
            return list.size();
        }
        
    }

}
