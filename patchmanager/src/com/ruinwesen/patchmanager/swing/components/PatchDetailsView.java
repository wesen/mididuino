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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;
import java.text.DateFormat;
import java.util.Date;

import org.jdesktop.layout.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JViewport;
import javax.swing.Scrollable;
import javax.swing.text.Caret;
import javax.swing.text.DefaultCaret;

import com.ruinwesen.patch.DefaultPatch;
import name.cs.csutils.CSAction;
import name.cs.csutils.SwingActionData;
import name.cs.csutils.debug.Debug;
import name.cs.csutils.debug.DebugFactory;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;
import com.ruinwesen.patchmanager.swing.wizards.TextEditorWizard;

public class PatchDetailsView {

    private static Log log = LogFactory.getLog(PatchDetailsView.class);
    private static Debug debug = DebugFactory.getDebug(PatchDetailsView.class);
    
    private Patch patch;
    private JPanel panel;
    
    private JLabel lblAuthor;    
    private JLabel lblTitle;
    private JLabel lblDeviceId;
    private JLabel lblDate;
    private JLabel lblEnvironmentId;
    private JTextArea taComment;
    private JTextArea taTags;
    private JButton btnReadDocumentation;
    private JButton btnSaveSourceAs;
    private JButton btnSaveMidiFileAs;
    private CSAction acReadDocumentation;
    
    public PatchDetailsView() {
        init();
    }
    
    private static final class ScrollablePanel extends JPanel implements Scrollable {

        /**
         * 
         */
        private static final long serialVersionUID = -7088442654053116936L;

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

        public int getScrollableBlockIncrement(Rectangle visibleRect,
                int orientation, int direction) {
            return 10;
        }

        public boolean getScrollableTracksViewportHeight() {
            if (getParent() instanceof JViewport) {
                return (((JViewport)getParent()).getHeight() > getPreferredSize().height);
            }
            return false;
        }

        public boolean getScrollableTracksViewportWidth() {
            // the viewport should always match the panels width
            if (getParent() instanceof JViewport) {
                return (((JViewport)getParent()).getWidth() > getPreferredSize().width);
            }
            return false;
        }

        public int getScrollableUnitIncrement(Rectangle visibleRect,
                int orientation, int direction) {
            return 10;
        }
        
    }

    public JButton getSaveSourceButton() {
        return btnSaveSourceAs;
    }

    public JButton getSaveMidifileButton() {
        return btnSaveMidiFileAs;
    }
    
    private void init() {
        panel = new ScrollablePanel();
        panel.setBackground(new Color(0xDEE4EA));
        GroupLayout ly = new GroupLayout(panel);
        panel.setLayout(ly);
        
        // create components
        lblAuthor = new JLabel();
        lblTitle = new JLabel();
        lblDeviceId = new JLabel();
        lblEnvironmentId = new JLabel();
        lblDate = new JLabel();
        taComment = new JTextArea();
        taComment.setEditable(false);
        taComment.setLineWrap(true);
        taComment.setWrapStyleWord(true);
        taTags = new JTextArea();
        taTags.setEditable(false);
        taTags.setLineWrap(true);
        taTags.setWrapStyleWord(true);

        taTags.setBackground(panel.getBackground());
        taComment.setBackground(panel.getBackground());
        taComment.setBorder(null);
        taTags.setBorder(null);
        
        Font labelFont = lblTitle.getFont();
        lblTitle.setFont(new Font(labelFont.getName(), Font.BOLD, labelFont.getSize()));
        
        taComment.setColumns(10);
        taTags.setColumns(10);
        configCaret(taComment.getCaret());
        configCaret(taTags.getCaret());
        
        acReadDocumentation = new CSAction("Read...")
        .useInvokationTarget(this, "readDocumentation");

        btnSaveSourceAs = new JButton();        
        btnSaveMidiFileAs = new JButton();
        btnReadDocumentation = new JButton(acReadDocumentation);

        // labels
        JLabel lblDeviceIdLabel = new JLabel("Device:");
        JLabel lblEnvironmentIdLabel = new JLabel("Environment:");
        JLabel lblTagsLabel = new JLabel("Tags:");
        JLabel lblReadDocuLabel = new JLabel("Documentation:");
        JLabel lblMidiFileSaveAsLabel = new JLabel("Midi File:");
        JLabel lblSourceSaveAsLabel = new JLabel("Source:");
        
        ly.setAutocreateContainerGaps(true);
        ly.setAutocreateGaps(true);
        
        ly.setHorizontalGroup(ly.createSequentialGroup()
                .add(ly.createParallelGroup()
                        .add(lblTitle)
                        .add(ly.createSequentialGroup()
                                .add(lblAuthor)
                                .add(lblDate)
                        )
                        .add(ly.createSequentialGroup()
                                .add(ly.createParallelGroup()
                                        .add(lblDeviceIdLabel)
                                        .add(lblEnvironmentIdLabel)
                                )
                                .add(
                                        ly.createParallelGroup()
                                        .add(lblDeviceId)
                                        .add(lblEnvironmentId)
                                )
                        )
                        .add(taComment)
                        .add(ly.createSequentialGroup()
                                .add(lblTagsLabel)
                                .add(taTags)
                        )
                        .add(ly.createSequentialGroup()
                                .add(ly.createParallelGroup()
                                        .add(lblReadDocuLabel)
                                        .add(lblSourceSaveAsLabel)
                                        .add(lblMidiFileSaveAsLabel)
                                )
                                .add(
                                        ly.createParallelGroup(GroupLayout.TRAILING)
                                        .add(btnReadDocumentation)
                                        .add(GroupLayout.TRAILING, btnSaveSourceAs)
                                        .add(GroupLayout.TRAILING, btnSaveMidiFileAs)
                                )
                        )
                )
        );
        
        ly.setVerticalGroup(ly.createSequentialGroup()
                .add(lblTitle)
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblAuthor)
                        .add(lblDate)
                )
                .add(taComment)
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblTagsLabel)
                        .add(taTags, GroupLayout.DEFAULT_SIZE, 
                                GroupLayout.PREFERRED_SIZE,
                                GroupLayout.PREFERRED_SIZE)
                )
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblDeviceIdLabel)
                        .add(lblDeviceId)
                )
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblEnvironmentIdLabel)
                        .add(lblEnvironmentId)
                )
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblReadDocuLabel)
                        .add(btnReadDocumentation)
                )
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblSourceSaveAsLabel)
                        .add(btnSaveSourceAs)
                )
                .add(
                        ly.createParallelGroup(GroupLayout.BASELINE)
                        .add(lblMidiFileSaveAsLabel)
                        .add(btnSaveMidiFileAs)
                )
        );
        
        if (!debug.isDebugEnabled()) {
            lblSourceSaveAsLabel.setVisible(false);
            btnSaveSourceAs.setVisible(false);
        }
        
        updateUI();
        ly.linkSize(
                new Component[]{lblDeviceIdLabel, 
                lblEnvironmentIdLabel,
                lblSourceSaveAsLabel,
                lblMidiFileSaveAsLabel},
                GroupLayout.HORIZONTAL);
        ly.linkSize(
                new Component[]{btnSaveMidiFileAs, btnSaveSourceAs,
                btnReadDocumentation},
                GroupLayout.HORIZONTAL);
    }
    
    public void readDocumentation() {
        String text = null;
        if (patch != null) {
            try {
                text = DefaultPatch.readDocumentationFile(patch);
            } catch (Exception ex) {
                if (log.isDebugEnabled()) {
                    log.debug("Could not read documentation", ex);
                }
            }
        }
        
        TextEditorWizard wiz = new TextEditorWizard(false);
        wiz.setText(text == null ? "" : text);
        wiz.setModal(false);
        wiz.showDialog(getContainer());
    }
    
    private void configCaret(Caret caret) {
        if (caret instanceof DefaultCaret) {
            ((DefaultCaret)caret).setUpdatePolicy(DefaultCaret.NEVER_UPDATE);
        }
    }

    @SwingActionData("Save As...")
    public void btnSaveMidiFileAsClick() {
        if (patch == null) {
            return;
        }
        
        SwingPatchManagerUtils.showStoreMidiFileDialog(panel, patch);
    }

    @SwingActionData("Save As...")
    public void btnSaveSourceAsClick() {
        
    }
    
    public JComponent getContainer() {
        return panel;
    }

    public void setPatch(Patch patch) {
        Patch oldValue = this.patch;
        
        if (oldValue != patch || (oldValue !=null && !oldValue.equals(patch))) {
            this.patch = patch;
            updateUI();
        }
    }

    /**
     * The dateformat used to create the date string.
     */
    private DateFormat dateFormat = DateFormat.getDateInstance(DateFormat.LONG);
    
    /**
     * Returns a formated string containing day,month and year of the specified date.
     * @param date a date
     * @return a formated string containing day,month and year of the specified date
     */
    private String dateToString(Date date) {
        return dateFormat.format(date);
    }
    
    private void updateUI() {
        Patch patch = this.patch;
        PatchMetadata meta;
        try {
            meta = patch.getMetadata();
        } catch (Exception ex) {
            meta = null;
        }
        
        if (meta == null) {
            lblTitle.setText("- no patch selected -");
            lblAuthor.setText("");
            lblEnvironmentId.setText("-");
            lblDeviceId.setText("-");
            taComment.setText("");
            taTags.setText("");
            lblDate.setText("");
            acReadDocumentation.setEnabled(false);
        } else {
            setText(lblAuthor, meta.getAuthor()+", ");
            setText(lblTitle, meta.getTitle());
            setText(lblDeviceId, meta.getDeviceId());
            setText(lblEnvironmentId, meta.getEnvironmentId());

            acReadDocumentation.setEnabled(meta.getPath(PatchMetadata.TEXT_DOCUMENTATION_PATH_NAME) != null);
            
            lblDate.setText(dateToString(meta.getLastModifiedDate())); 
            taComment.setText(meta.getComment());
            taTags.setText(meta.getTags()
                    .filterPrefix(PatchMetadata.CATEGORY_TAG_PREFIX, false, true)
                    .toSortedString());
        }
    }
    
    private void setText(JLabel lbl, Object value) {
        if (value == null) {
            lbl.setText("-");
            return;
        } 
        String str = String.valueOf(value).trim();
        if (str.length()==0) {
            lbl.setText("-");
        } else {
            lbl.setText(str);
        }
        
    }
    
}
 