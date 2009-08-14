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
package com.ruinwesen.patchmanager.swing.components;

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Insets;
import java.awt.LayoutManager2;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.swing.AbstractButton;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JViewport;
import javax.swing.Scrollable;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.event.ChangeListener;
import javax.swing.event.EventListenerList;

import name.cs.csutils.collector.Collector;
import name.cs.csutils.event.EventUtils;
import name.cs.csutils.i18n.I18N;

import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Tag;
import com.ruinwesen.patch.metadata.Tagset;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;
import com.ruinwesen.patchmanager.client.index.IndexedPatch;
import com.ruinwesen.patchmanager.client.index.PatchIndex;
import com.ruinwesen.patchmanager.client.index.Query;
import com.ruinwesen.patchmanager.swing.model.Order;

public class SearchOptionsControl implements ActionListener, ItemListener {

    private Map<Object, Filter> allFiltersMap 
        = new HashMap<Object, Filter>();
    
    private JComponent container;
    private boolean controlsEnabled = true;
    private EventListenerList eventListenerList = new EventListenerList();
    
    private JComboBox cbSearchOrder;
    private String SearchOptionByRelevance;
    private String SearchOptionByDateAdded;
    private String SearchOptionByTitle;
    private String SearchOptionByAuthor;

    public SearchOptionsControl() {
        container = new ScrollablePanel();
        container.setLayout(new FilterBoxLayout());
        container.setBorder(BorderFactory.createEmptyBorder(3,6,3,6));
        container.setOpaque(true);
        container.setBackground(new Color(0xDEE4EA));

        SearchOptionByRelevance = I18N.translate("search.order.by-relevance", "Relevance");
        SearchOptionByDateAdded = I18N.translate("search.order.by-date-added", "Date Added");
        SearchOptionByTitle = I18N.translate("search.order.by-title", "Title");
        SearchOptionByAuthor = I18N.translate("search.order.by-author", "Author");
        
        cbSearchOrder = new JComboBox(
                new String[]{
                        SearchOptionByRelevance,
                        SearchOptionByDateAdded,
                        SearchOptionByTitle,
                        SearchOptionByAuthor } );
        
        cbSearchOrder.addItemListener(this);
        
        // this will create the heading labels
        rebuildFilterUI((FilterDataCollector)null);
    }

    public Comparator<IndexedPatch> getOrder() {
        Object selection = cbSearchOrder.getSelectedItem();
        if (selection == SearchOptionByAuthor) {
            return Order.BY_AUTHOR;
        } else if (selection == SearchOptionByDateAdded) {
            return Order.BY_DATE;
        } else if (selection == SearchOptionByTitle) {
            return Order.BY_TITLE;
        } else {
            return Order.BY_RELEVANCE;
        }
    }

    private static final class ScrollablePanel extends JPanel implements Scrollable {

        /**
         * 
         */
        private static final long serialVersionUID = -179591354285360683L;

        @Override
        public Dimension getPreferredScrollableViewportSize() {
            Dimension size = getPreferredSize();
            Component c = getParent();
            if (c instanceof JViewport) {
                c = c.getParent();
                if (c instanceof JScrollPane) {
                    JScrollBar bar = ((JScrollPane)c).getVerticalScrollBar();
                    size.width+= bar.getPreferredSize().width;
                }
            }
            return size;
        }

        @Override
        public int getScrollableBlockIncrement(Rectangle visibleRect,
                int orientation, int direction) {
            return 10;
        }

        @Override
        public boolean getScrollableTracksViewportHeight() {
            if (getParent() instanceof JViewport) {
                return (((JViewport)getParent()).getHeight() > getPreferredSize().height);
            }
            return false;
        }

        @Override
        public boolean getScrollableTracksViewportWidth() {
            // the viewport should always match the panels width
            if (getParent() instanceof JViewport) {
                return (((JViewport)getParent()).getWidth() > getPreferredSize().width);
            }
            return false;
        }

        @Override
        public int getScrollableUnitIncrement(Rectangle visibleRect,
                int orientation, int direction) {
            return 10;
        }
        
    }
    
    public JComponent getContainer() {
        return container;
    }
    
    public void setControlsEnabled(boolean enabled) {
        if (this.controlsEnabled != enabled) {
            this.controlsEnabled = enabled;
            cbSearchOrder.setEnabled(enabled);
            for (Filter filter: allFiltersMap.values()) {
                filter.component.setEnabled(enabled);
            }
        }
    }

    public void enableFiltersInQuery(Query query) {
        Tagset enabledCategories = new Tagset();
        Tagset disabledCategories = new Tagset();
        Set<EnvironmentId> envIdSet = new HashSet<EnvironmentId>();
        Set<DeviceId> devIdSet = new HashSet<DeviceId>();

        envIdSet.add(null);
        devIdSet.add(null);
        for (Filter filter: allFiltersMap.values()) {
            switch (filter.type) {
            case Filter.CATEGORY_TAG: {
                if (filter.component.isSelected()) {
                    enabledCategories.add(((Tag)filter.value));
                }   else {
                    disabledCategories.add(((Tag)filter.value));
                }
                break;
            }
            case Filter.DEVICE_ID: {
                if (filter.component.isSelected()) {
                    devIdSet.add((DeviceId)filter.value);
                }
                break;
            }
            case Filter.ENVIRONMENT_ID: {
                if (filter.component.isSelected()) {
                    envIdSet.add((EnvironmentId)filter.value);
                }
                break;
            }
            default: {
                break;
            }
            }
            
        }
        
        query.setExpectedDeviceIds(devIdSet.isEmpty()?null:devIdSet);
        query.setExpectedEnvironmentIds(envIdSet.isEmpty()?null:envIdSet);
   //     query.setExpectedTags(enabledCategories.isEmpty() ? null : enabledCategories);
        query.setForbiddenTags(disabledCategories.isEmpty()?null:disabledCategories);
        
        
        
    }

    public void rebuildFilterUI(PatchIndex index) {
        FilterIndexer collector = new FilterIndexer();
        index.collect(collector);
        collector.rebuildFilterUI();
    }

    public void addChangeListener(ChangeListener l) {
        eventListenerList.add(ChangeListener.class, l);
    }

    public void removeChangeListener(ChangeListener l) {
        eventListenerList.remove(ChangeListener.class, l);
    }
    
    private void fireChangeEvent() {
        EventUtils.fireChangeEvent(eventListenerList, this);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        fireChangeEvent();
    }

    @Override
    public void itemStateChanged(ItemEvent e) {
        fireChangeEvent();
    }

    private void rebuildFilterUI(FilterDataCollector indexer) {
        // remove all previous data / components / listener-configurations
        for (Filter filter: allFiltersMap.values()) {
            filter.component.removeActionListener(this);
        }
        allFiltersMap.clear();
        container.removeAll();
        
        container.add(newLabel("Sort results by"));
        container.add(cbSearchOrder);
        
        // rebuild 
        List<Filter> filterList;
        if (indexer != null) {
            filterList = createFilterList(indexer.filterset);
            indexer.filterset.remove(null);
        } else {
            filterList = Collections.emptyList();
        }
        
        container.add(newLabel("Categories"));
        addToContainer(filterList, Filter.CATEGORY_TAG);
        container.add(Box.createVerticalStrut(10));
        container.add(newLabel("Device"));
        addToContainer(filterList, Filter.DEVICE_ID);
        container.add(Box.createVerticalStrut(10));
        container.add(newLabel("Environment"));
        addToContainer(filterList, Filter.ENVIRONMENT_ID);
        container.invalidate();
        container.revalidate();
        container.repaint();
    }
    
    private void addToContainer(List<Filter> filterList, int type) {
        for (Filter filter: filterList) {
            if (filter.type == type) {
                container.add(filter.component);
            }
        }        
    }
    
    private List<Filter> createFilterList(Collection<Object> src) {
        List<Filter> list = new ArrayList<Filter>(src.size());
        
        for (Object filterValue: src) {
            Filter filter;
            if (filterValue instanceof Tag) {
                filter = new Filter((Tag)filterValue);
            } else if (filterValue instanceof DeviceId) {
                filter = new Filter((DeviceId)filterValue);
            } else if (filterValue instanceof EnvironmentId) {
                filter = new Filter((EnvironmentId)filterValue);
            } else {
                continue;
            }
            
            allFiltersMap.put(filterValue, filter);
            filter.component.setEnabled(controlsEnabled);
            filter.component.addActionListener(this);
            list.add(filter);
        } 
        
        Collections.sort(list);
        return list;
    }
    
    private Font cachedLabelFont;

    private JLabel newLabel(String title) {
        if (cachedLabelFont == null) {
            Font f = UIManager.getFont("Label.font");
            if (f == null) {
                f = container.getFont();
            }
            cachedLabelFont = new Font(f.getName(), Font.BOLD, f.getSize());
        }
        JLabel label = new JLabel(title);
        label.setFont(cachedLabelFont);
        
        label.setAlignmentX(Component.LEFT_ALIGNMENT);
        return label;
    }

    private void updateFilterUI(FilterDataCollector data) {
        Set<Object> visibleFilters = data.filterset;
        boolean componentsChanged = false;
        for (Map.Entry<Object, Filter> entry: allFiltersMap.entrySet()) {
            JCheckBox component = entry.getValue().component;
            boolean wasVisible = component.isVisible();
            boolean isVisible = visibleFilters.contains(entry.getKey());
            if (wasVisible != isVisible) {
                component.setVisible(isVisible);
                componentsChanged = true;
            }
        }
        if (componentsChanged) {
            // update layout
            container.revalidate();
            container.repaint();
        }
    }

    public FilterUpdater createFilterUpdater() {
        return new FilterUpdater();
    }

    public FilterIndexer createFilterIndexer() {
        return new FilterIndexer();
    }
    
    /**
     * Class to update existing filters.
     */
    public final class FilterUpdater extends FilterDataCollector implements Runnable {
        public void updateFilterUI() {
            if (EventQueue.isDispatchThread()) {
                run();
            } else {
                SwingUtilities.invokeLater(this);
            } 
        }
        public void run() {
            SearchOptionsControl.this.updateFilterUI(this);
        }
    }
    
    /**
     * Class to create new filters.
     */
    public final class FilterIndexer extends FilterDataCollector implements Runnable {
        public void rebuildFilterUI() {
            if (EventQueue.isDispatchThread()) {
                run();
            } else {
                SwingUtilities.invokeLater(this);
            }
        }
        public void run() {
            SearchOptionsControl.this.rebuildFilterUI(this);
        }
    }
    
    /**
     * Filter data collector.
     */
    private static class FilterDataCollector implements Collector<IndexedPatch> {
        private Set<Object> filterset = new HashSet<Object>();
        @Override
        public final int collect(IndexedPatch item) {
            PatchMetadata meta = item.getMetadata();
            // collect category tags
            filterset.addAll(meta.getTags().filterPrefix(
                            PatchMetadata.CATEGORY_TAG_PREFIX,true, false));
            // collect environment ids
            if (meta.getEnvironmentId() != null)
                filterset.add(meta.getEnvironmentId());
            // collect device ids
            if (meta.getDeviceId() != null)
                filterset.add(meta.getDeviceId());
            
            return 0;
        }        
    }
    
    private static final String labelForTag(Tag tag) {
        String name = tag.value();
        int idx = name.lastIndexOf(':');
        if (idx>=0) name = name.substring(idx+1);
        return name;
    }
    
    private static final class Filter implements Comparable<Filter> {
        public static final int ENVIRONMENT_ID = 0;
        public static final int DEVICE_ID = 1;
        public static final int CATEGORY_TAG = 2; 
        public int type;
        public Object value;
        public JCheckBox component;
        public Filter(EnvironmentId environmentId) {
            init(environmentId, environmentId.value(), ENVIRONMENT_ID);
        }
        public Filter(DeviceId deviceId) {
            init(deviceId, deviceId.value(), DEVICE_ID);
        }
        public Filter(Tag tag) {
            init(tag, labelForTag(tag), CATEGORY_TAG);
        }
        public void init(Object value, String label, int type) {
            this.type = type;
            this.value = value;
            this.component = newCheckbox(label);
        }
        public String label() {
            return component.getText();
        }
        @Override
        public int compareTo(Filter b) {
            return label().compareTo(b.label());
        }
        private JCheckBox newCheckbox(String label) {
            JCheckBox c = new JCheckBox(label);
            c.setSelected(true);
            c.setBorderPainted(false);
            return c;
        }
        
    }

    private static final class FilterBoxLayout implements LayoutManager2 {

        final int BUTTON_PADDING_LEFT = 10;
        final int LINE_PADDING = 3;
        private Dimension preferredSize;
        private Dimension maximumSize;
        
        @Override
        public void layoutContainer(Container parent) {
            int parentwidth = parent.getWidth();
            Insets is = parent.getInsets();
            parentwidth -= is.left+is.right;
            synchronized (parent.getTreeLock()) {
                int y = is.top;
                final int minx = is.left;
                for (int i=0;i<parent.getComponentCount();i++) {
                    Component c = parent.getComponent(i);
                    if (!c.isVisible()) {
                        continue;
                    }
                    Dimension d = c.getPreferredSize();
                    int x = minx;
                    if (c instanceof AbstractButton) {
                        x += BUTTON_PADDING_LEFT; 
                    } else if (c instanceof JLabel) {
                        y+=LINE_PADDING;
                    }
                    c.setBounds(x, y, Math.max(parentwidth-x, d.width), d.height);
                    y+=d.height+LINE_PADDING;
                    if (c instanceof JLabel) {
                        y+=LINE_PADDING;
                    }
                }
            }
        }

        @Override
        public void invalidateLayout(Container target) {
            preferredSize = null;
            maximumSize = null;
        }

        private Dimension computePreferredLayoutSize(Container parent, boolean ignoreInvisibleComponents) {
            Dimension preferredSize = new Dimension(0,0);
            int extraWidth = 0;
            synchronized (parent.getTreeLock()) {
                // include vertical scrollbar width
                /*
                Container parentparent = parent.getParent();
                if (parentparent instanceof JViewport) {
                    parentparent = ((JViewport)parentparent).getParent();
                    if (parentparent instanceof JScrollPane) {
                        extraWidth = 
                            ((JScrollPane)parentparent).getVerticalScrollBar().getWidth();
                    }
                }*/
                for (int i=parent.getComponentCount()-1;i>=0;i--) {
                    Component c = parent.getComponent(i);
                    if (ignoreInvisibleComponents && !c.isVisible()) {
                        continue;
                    }
                    Dimension d = c.getPreferredSize();
                    if (c instanceof AbstractButton) {
                        d.width+=BUTTON_PADDING_LEFT; 
                    } else if (c instanceof JLabel) {
                        d.height+=LINE_PADDING;
                    }
                    preferredSize.width = Math.max(preferredSize.width, d.width);
                    preferredSize.height += d.height;
                    preferredSize.height += LINE_PADDING;
                }
            }
            Insets insets = parent.getInsets();
            preferredSize.width = Math.max(preferredSize.width, 100);
            preferredSize.width+=insets.left+insets.right+extraWidth;
            preferredSize.height+=insets.top+insets.bottom;
            preferredSize.height += LINE_PADDING;
            return preferredSize;
        }
        
        @Override
        public Dimension preferredLayoutSize(Container parent) {
            if (this.preferredSize == null) {
                this.preferredSize = computePreferredLayoutSize(parent, false);
            }
            return new Dimension(preferredSize);
        }

        @Override
        public Dimension minimumLayoutSize(Container parent) {
            return preferredLayoutSize(parent);
        }

        @Override
        public Dimension maximumLayoutSize(Container parent) {
            if (maximumSize == null) {
                Dimension dim = this.preferredLayoutSize(parent);
                maximumSize = new Dimension(dim.width, Short.MAX_VALUE);
                return dim;
            } else {
                return new Dimension(maximumSize);
            }
        }

        @Override
        public void addLayoutComponent(String name, Component comp) {
            // no op
        }

        @Override
        public void addLayoutComponent(Component comp, Object constraints) {
            // no op
        }

        @Override
        public float getLayoutAlignmentX(Container target) {
            return 0;
        }

        @Override
        public float getLayoutAlignmentY(Container target) {
            return 0;
        }

        @Override
        public void removeLayoutComponent(Component comp) {
            // no op
        }
        
    }

}
