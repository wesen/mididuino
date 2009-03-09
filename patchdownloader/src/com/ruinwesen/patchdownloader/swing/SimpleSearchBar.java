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

import java.awt.BorderLayout;
import java.beans.PropertyChangeSupport;

import javax.swing.Action;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTextField;

import name.cs.csutils.swing.PerformActionOnEnterKey;
import name.cs.csutils.swing.RedirectAction;
import name.cs.csutils.swing.RedirectAction.RedirectActionMeta;

import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.index.Term;
import org.apache.lucene.queryParser.MultiFieldQueryParser;
import org.apache.lucene.queryParser.ParseException;
import org.apache.lucene.search.FuzzyQuery;
import org.apache.lucene.search.MatchAllDocsQuery;
import org.apache.lucene.search.Query;

import com.ruinwesen.patchdownloader.patch.PatchDocument;

public class SimpleSearchBar {

    private PropertyChangeSupport changeSupport;
    private JButton btnSearch;
    private JTextField textQueryField;
    private JPanel container;
    private PDFrame patchdownloader;
    
    public SimpleSearchBar(PDFrame patchdownloader) {
        this.patchdownloader = patchdownloader;
        Action performSearch = new RedirectAction(this, "performSearch");
        btnSearch = new JButton(performSearch);
        textQueryField = new JTextField();
        container = new JPanel();
        new PerformActionOnEnterKey(performSearch).installTo(textQueryField);
        container.setLayout(new BorderLayout());
        container.add(textQueryField, BorderLayout.CENTER);
        container.add(btnSearch, BorderLayout.EAST);
        changeSupport = new PropertyChangeSupport(this);
    }
    
    public JComponent getContainer() {
        return container;
    }
    
    public String getQueryString() {
        return norm(textQueryField.getText());
    }
    
    private String norm(String str) {
        return str == null || str.trim().length() == 0 ? "" : str;    
    }
    
    public void setQueryString(String text) {
        String oldQueryString = getQueryString();
        String newQueryString = norm(text);
        if (!newQueryString.equals(oldQueryString)) {
            textQueryField.setText(newQueryString);
            fireQueryStringChanged(oldQueryString, newQueryString);
        }
    }
    
    private void fireQueryStringChanged(String oldValue, String newValue) {
        changeSupport.firePropertyChange("query", oldValue, newValue);
    }

    @RedirectActionMeta(title="Search",resourcekey="button.perform.search")
    public void performSearch() {
        Query query = createQuery();
        patchdownloader.getPatchListView().setQuery(query);
    }

    public Query createQuery() {
        String queryString = getQueryString();
        if (queryString.length() == 0) {
            return new MatchAllDocsQuery();
        }
        String[] fields = new String[] {
                PatchDocument.AUTHOR,
                PatchDocument.COMMENT,
                PatchDocument.LASTMODIFIED,
                PatchDocument.NAME,
                PatchDocument.TAGS,
                PatchDocument.TITLE
        };
        
        MultiFieldQueryParser queryparser = new MultiFieldQueryParser(fields,
                new StandardAnalyzer());

        try {
            return queryparser.parse(queryString);
        } catch (ParseException ex) {
            ex.printStackTrace();
            return new FuzzyQuery(new Term(PatchDocument.TAGS));
        }
        /*
        try {
            return queryparser.parse(queryString);
        } catch (ParseException ex) {
            return null;
        }
        */
    }
    
}
