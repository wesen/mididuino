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

import java.awt.Component;
import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.Timer;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import name.cs.csutils.I18N;

import com.ruinwesen.patchdownloader.indexer.Doc;
import com.ruinwesen.patchdownloader.indexer.Query;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.repository.StoredPatch;

public class PatchListView implements ListSelectionListener {

    private JComponent container;
    private JList listView;
    private PDFrame patchdownloader;
    private Timer listSelectionTimer;
    private long lastSelectionEventTime = 0;
    private long acceptSelectionEventDelay = 250; // 1/2 second until a selection is 'accepted'
    private JLabel headerTitleLabel;
    private int minCellWidth = 200;
    private LargeListModel<Doc,List<Doc>> listmodel;
    private int patchResultCounter = 0;
    private PatchCellRenderer cellRenderer;
    
    public PatchListView(PDFrame patchdownloader) {
        super();
        this.patchdownloader = patchdownloader;
        
        listSelectionTimer = new Timer(0, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (isSelectionTimeAccepted()) {
                    listSelectionTimer.stop();
                    notifySelectionTimeAccepted();
                }
            }  
            });
        listSelectionTimer.setDelay(100);
        listSelectionTimer.setCoalesce(true);
        listSelectionTimer.setInitialDelay(100);
        listSelectionTimer.setRepeats(true);
        
        init();
    }
    
    public void setQuery(Query query) {
        cellRenderer.setQuery(query);
    }
    
    public LargeListModel<Doc, List<Doc>> getListModel() {
        return listmodel;
    }
    
    private void updateTitleLabel() {
        String text =  I18N.format("translation.patchlistview.headerlabel",
                "Patches (%d)", patchResultCounter);
        headerTitleLabel.setText(text);
    }
    
    private void notifySelectionTimeAccepted() {
        if (listSelectionTimer.isRunning()) {
            return;
        }
        PatchDetailsView details = patchdownloader.getPatchDetailsView();
        int index = listView.getSelectedIndex();
        if (index<0 || index>=listmodel.getSize()) {
            details.setPatch(null, null);
        } else {
            Doc patch = listmodel.getElementAt(index);
            File file = new File(patchdownloader.getRemoteRepositoryBackup().getBaseDir(), patch.path());
            
            PatchMetadata metadata = patchdownloader.getMetadataCache().getMetadata(patch, true);
            details.setPatch(new StoredPatch.JarFilePatch(file), metadata);
            
        }
    }
    
    private void notifySelectionChanged() {
        lastSelectionEventTime = System.currentTimeMillis();
        if (!listSelectionTimer.isRunning()) {
            listSelectionTimer.start(); 
        }
    }
    
    private boolean isSelectionTimeAccepted() {
        return System.currentTimeMillis() > lastSelectionEventTime+acceptSelectionEventDelay;
    }
    
    private void init() {
        listmodel = new LargeListModel<Doc, List<Doc>>(new ArrayList<Doc>());
        listView = new JList(listmodel);
        cellRenderer = new PatchCellRenderer(patchdownloader.getMetadataCache());
        listView.setCellRenderer(cellRenderer);
        listView.addListSelectionListener(this);
        //Font font = CSUtils.changeFontSize(UIManager.getFont("TextPane.font"), 0.8f);
        // listView.setFont(font); 
        container = new JPanel(new BorderLayout());

        HeaderPaneBuilder hpb = new HeaderPaneBuilder("Patches");
        this.headerTitleLabel = hpb.headerTitleLabel;
        
        updateTitleLabel();
        JScrollPane listViewScroller = new JScrollPane(listView); 
        container.add(hpb.headerPane, BorderLayout.NORTH);
        container.add(listViewScroller, BorderLayout.CENTER);

        SearchBar topBar;
        topBar = new SearchBar(patchdownloader);
        container.add(topBar.getContainer(), BorderLayout.SOUTH);

        // TODO don't use this approximation because different fonts
        // require a different cell height 
        
        Font listViewFont = listView.getFont();
        FontMetrics metrics = listView.getFontMetrics(listViewFont);
        
        int[] widths = metrics.getWidths();
        int maxCharWidth = 6;
        for (int w: widths)
            maxCharWidth = Math.max(maxCharWidth, w);
        
        minCellWidth = maxCharWidth*20;
        
        
        
        listView.setFixedCellHeight((int)(metrics.getHeight()*4
                +metrics.getLeading()+metrics.getAscent()+
                metrics.getDescent())); 
        // TODO set the fixed cell width depending on the component width
        listViewScroller.addComponentListener(new FixedCellWidthUpdater());
        updateListViewFixedCellWidth();
        
    }
    
    private void updateListViewFixedCellWidth() {
        Component parent = listView.getParent();
        if (parent != null && parent instanceof JComponent) {
            Insets insets = ((JComponent)parent).getInsets();
            int w = parent.getWidth()-insets.left-insets.right;
            insets = listView.getInsets(insets);
            w += -insets.left-insets.right;

            listView.setFixedCellWidth(Math.max(minCellWidth,w));
        } else {
            Insets insets = listView.getInsets();
            listView.setFixedCellWidth(Math.max(minCellWidth,listView.getWidth()-insets.left-insets.right));
        }
    }
    
    private class FixedCellWidthUpdater extends ComponentAdapter {
        @Override
        public void componentResized(ComponentEvent e) {
            updateListViewFixedCellWidth();
        }
    }
    
    public JComponent getComponent() {
        return container;
    }
    
    public void setPatchResultCounter(int newcount) {
        if (newcount != patchResultCounter) {
            patchResultCounter = newcount;
            updateTitleLabel();
        }
    }
    
    public int getPatchResultCounter() {
        return patchResultCounter;
    }
    
    /*
    private class IndexedPatchListCellRenderer extends DefaultListCellRenderer {

        private StringBuilder html = new StringBuilder();
        private DateFormat dateFormat = DateFormat.getDateInstance(DateFormat.MEDIUM);
        
        private String dateToString(Date date) {
            return dateFormat.format(date);
        }
            
        public Component getListCellRendererComponent(
            JList list,
            Object value,
            int index,
            boolean isSelected,
            boolean cellHasFocus) {
            super.getListCellRendererComponent(list, "", index, isSelected, cellHasFocus);
            
            Color foreground = list.getForeground();
            if (foreground == null) {
                foreground = list.getSelectionForeground();
                if (foreground == null)
                    foreground = Color.BLACK;
            }
            
            String bluer = CSUtils.toHTMLColor(isSelected ? list.getSelectionForeground() : CSUtils.change(foreground, 0, 0, 0.8f));
            String paler = CSUtils.toHTMLColor(CSUtils.change(foreground, .75f));
            
            Doc patch = (Doc) value;
            PatchMetadata metadata = patchdownloader.getMetadataCache().getMetadata(patch, true);
            
            if (metadata == null) {
                setText("[Unavailable]");
            } else {
                String title = metadata.getTitle();
                if (title == null || title.trim().length() == 0) {
                    title = "Title: -unknown-";
                } else {
                    title = CSUtils.limitString(title.trim(), 40);
                }
                String comment = metadata.getComment();
                if (comment != null) {
                    if (comment.trim().length() == 0) comment = null;
                    else { comment = CSUtils.limitString(comment.trim(), 40);}
                }
                
                html.setLength(0);
                html.append("<html>");
                html.append("<div><b>").append(title).append("</b>");

                if (metadata.getLastModifiedDate() != null) {
                    html.append(" <span style=\"font-size:.8em;\"> - "+dateToString(metadata.getLastModifiedDate())+"</span>");
                }
                html.append("</div>\n");
               
                //html.append(" <span style=\"font-size:0.8em;\">(score:"+patch.score+")</span>");
                if (comment != null) {
                    html.append("<div style=\"color:"+paler+"\">"+comment+"</div>\n");
                }

                html.append("<div style=\"font-size:.9em;\">");
                html.append("Tags: ");
                Tagset tagset = metadata.getTags();
                if (!tagset.isEmpty()) {
                    String tags = tagset.toSortedString();
                    String hltags = query.highlight(tags, "<b>","</b>");
                    html.append("<span style=\"color:"+bluer+";\">"+hltags+"</span>");
                }
                html.append("</div>\n");
                html.append("</html>");
                setText(html.toString()); 
            }
            
            return this;
        }
        
    }
    */

    @Override
    public void valueChanged(ListSelectionEvent e) {
        this.notifySelectionChanged();
    }

}
