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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.BorderFactory;
import javax.swing.GroupLayout;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

import name.cs.csutils.i18n.I18N;

import com.ruinwesen.patchmanager.swing.SwingPatchManager;
public class SearchPanel  {

    private JTextField textQueryField;
    private JPanel container;
    private JComboBox cbOrder;
    private SwingPatchManager patchmanager;
    private String previousQueryString = "";
    private boolean controlsEnabled = true;
    
    public SearchPanel(SwingPatchManager patchmanager) {
        this.patchmanager = patchmanager;
        textQueryField = new JTextField();
        textQueryField.setColumns(10);

        new TextPopup().installAt(textQueryField);
        
        JLabel labelSearch = new JLabel(I18N.translate("translation.search", "Search")+":");
        labelSearch.setLabelFor(textQueryField);

        final String RELEVANCE = I18N.translate("translation.sortby.relevance", "Relevance");
        final String DATE_ADDED = I18N.translate("translation.sortby.dateadded", "Date");
        final String TITLE = I18N.translate("translation.sortby.title", "Title");
        final String AUTHOR = I18N.translate("translation.sortby.author", "Author");
        cbOrder = new JComboBox(new Object[]{RELEVANCE, DATE_ADDED, TITLE, AUTHOR});

        JLabel labelSortby = new JLabel(I18N.translate("translation.sortby", "Sort by")+":");
        labelSortby.setLabelFor(cbOrder);
        
        container = new JPanel();
        container.setBorder(BorderFactory.createEmptyBorder(2,5,2,5));
        
        GroupLayout ly = new GroupLayout(container);
        container.setLayout(ly);
        
        ly.setAutoCreateContainerGaps(false);
        ly.setAutoCreateGaps(true);
        ly.setHorizontalGroup(ly.createSequentialGroup()
                .addComponent(labelSearch)
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.CENTER)
                        .addComponent(textQueryField)
                )
                .addComponent(labelSortby)
                .addComponent(cbOrder, GroupLayout.PREFERRED_SIZE, GroupLayout.PREFERRED_SIZE, GroupLayout.PREFERRED_SIZE)
        );
        ly.setVerticalGroup( ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                .addComponent(labelSearch)
                .addComponent(textQueryField)
                .addComponent(labelSortby)
                .addComponent(cbOrder)
        );
        new EventHandler().install();
    }

    public void setControlsEnabled(boolean enabled) {
        if (this.controlsEnabled != enabled) {
            controlsEnabled = enabled;
            textQueryField.setEnabled(controlsEnabled);
            cbOrder.setEnabled(controlsEnabled);
        }
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
        }
    }
    
    private class EventHandler implements
        ActionListener, DocumentListener {

        javax.swing.Timer timer;
        long mostRecentQueryChange = 0;
        static final long queryChangeAcceptTime = 500; 
        public void install() {
            cbOrder.addActionListener(this);
            textQueryField.getDocument().addDocumentListener(this);
            timer = new javax.swing.Timer(0, this);
            timer.setCoalesce(true);
            timer.setDelay(100);
            timer.setRepeats(true);
            timer.stop();
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            if (cbOrder == e.getSource()) {
                /*
                SearchController controller = patchdownloader.getSearchController();
                switch (cbOrder.getSelectedIndex()) {
                case 0: controller.setOrder(Order.BY_RELEVANCE); break;
                case 1: controller.setOrder(Order.BY_DATE); break;
                case 2: controller.setOrder(Order.BY_TITLE); break;
                case 3: controller.setOrder(Order.BY_AUTHOR); break;
                }
                controller.update();*/
            } else if (e.getSource() == timer 
                    && timer.isRunning()
                    && System.currentTimeMillis()>mostRecentQueryChange+queryChangeAcceptTime) {
                timer.stop();
                notifyQueryAccepted();
            }
        }

        @Override
        public void changedUpdate(DocumentEvent e) {
            // no op
        }

        @Override
        public void insertUpdate(DocumentEvent e) {
            notifyQueryChanged();
        }

        @Override
        public void removeUpdate(DocumentEvent e) {
            notifyQueryChanged();
        }

        private void notifyQueryChanged() {
            mostRecentQueryChange = System.currentTimeMillis();
            if (!timer.isRunning())
                timer.start();
        }
        
        private void notifyQueryAccepted() {
            String s = getQueryString();
            if (!previousQueryString.equals(s)) {
                previousQueryString = s;
                patchmanager.executeQuery();
            }
        }

    }

}
