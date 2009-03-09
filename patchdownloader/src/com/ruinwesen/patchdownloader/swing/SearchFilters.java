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
import java.awt.event.ActionEvent;
import java.util.LinkedList;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.ButtonGroup;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

import org.apache.lucene.search.Query;

public class SearchFilters {

    private static final String KEY_FILTERNAME = "filter.name";
    
    public static final class SearchFilter extends AbstractAction {
        
        private String name;
        private SearchFilters owner;
        private Query query;
        
        public SearchFilter(SearchFilters owner, String name, Query query) {
            setName(name);
            this.owner = owner;
            this.query = query;
        }
        
        public void setName(String name) {
            if (name == null) {
                name = "";
            }
            putValue(NAME, name);
            this.name = name;
        }
        
        public String getName() {
            return name;
        }
        
        public String toString() {
            return getName();
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            selectFilter();
        }
        
        public void selectFilter() {
            owner.selectFilter(this);
        }
        
    }
    
    private JMenu menuFilters;
    private SearchFilter searchAll;
    private SearchFilter searchRepository;
    private SearchFilter searchWorkspace;
    private ButtonGroup menuButtonGroup = new ButtonGroup();
    private List<SearchFilter> userFilters = new LinkedList<SearchFilter>();
    
    public SearchFilters() {
        init();
    }
    
    private void init() {
        searchAll = new SearchFilter(this, "All", null);
        searchRepository = new SearchFilter(this, "Repository", null);
        searchWorkspace = new SearchFilter(this, "Workspace", null);

        menuFilters = new JMenu("Filter");
    }

    public void selectFilter(SearchFilter filter) {
        if (filter.owner == this) {
            JMenuItem item = menuItemFor(filter);
            if (item != null) {
                item.setSelected(false);
            }
        }
    }
    
    private JMenuItem menuItemFor(SearchFilter filter) {
        for (Component c: menuFilters.getMenuComponents()) {
            if (c instanceof JMenuItem &&
                    filter.getName().equals(
                    ((JMenuItem)c).getClientProperty(KEY_FILTERNAME))) {
                return (JMenuItem) c;
            }
        }
        return null;
    }

}

