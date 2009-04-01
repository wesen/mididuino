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
import java.awt.Font;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.UIDefaults;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;

import name.cs.csutils.CSUtils;
import name.cs.csutils.I18N;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.RedirectAction.RedirectActionMeta;
import name.cs.csutils.directoryreader.DirectoryReader;

import com.ruinwesen.patchdownloader.patch.DefaultPatchMetadata;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.Tagset;
import com.ruinwesen.patchdownloader.repository.StoredPatch;
import com.ruinwesen.patchdownloader.swing.misc.BoxLayoutBuilder;
import com.ruinwesen.patchdownloader.swing.misc.TextPopup;
import com.ruinwesen.patchdownloader.swing.panels.MidiSetupPanel;
import com.ruinwesen.patchdownloader.swing.panels.SectionBorder;
import com.ruinwesen.patchdownloader.tasks.SendMidifileTask;

public class PatchDetailsView {

    private JPanel detailsPane;
    
    private JPanel panel = new JPanel();
    private StoredPatch patch;
    private PatchMetadata metadata = null;

    private SwingPatchdownloader patchdownloader;
    private MidiSetupPanel midiBar;
    
    private JLabel labelTitle = new JLabel();
    private JLabel labelAuthorAndDate = new JLabel();
    private JTextArea textTags = new JTextArea();
    private JTextArea textCategory = new JTextArea();
    private JTextArea textComment = new JTextArea();

    private JButton btnSend;

    private JButton btnSaveMidifileAs;

    private JButton btnSavesourceAs;
    
    public void setPatch(StoredPatch patch, PatchMetadata metadata) {
        this.patch = patch;
        this.metadata = metadata;
        if (metadata == null && patch != null) {
            try {
                this.metadata = patch.getMetadata();
            } catch (IOException e) {
                this.metadata = new DefaultPatchMetadata();
            }
        }
        updateView();
    }

    public JComponent getComponent() {
        return panel;
    }
    
    public PatchDetailsView(SwingPatchdownloader patchdownloader) {
        this.patchdownloader = patchdownloader;
        init();
    }

    private void init() {
        panel.setLayout(new BorderLayout());
        SectionBorder sectionborder = new SectionBorder(
                I18N.translate("translation.details","Details"));
        panel.setBorder(sectionborder);
        panel.setMinimumSize(new Dimension(150, 150));
        UIDefaults uidefaults = UIManager.getDefaults();
        detailsPane = new JPanel();
        detailsPane.setOpaque(true);
        Border border = uidefaults.getBorder("List.border");
        if (border == null) {
            border = detailsPane.getBorder();
        } 
        border = new CompoundBorder(border,
                BorderFactory.createEmptyBorder(2,4,2,4));
        detailsPane.setBorder(border);
        Font font = uidefaults.getFont("List.font");
        /*
        Color bglist = uidefaults.getColor("List.background");
        Color fglist = uidefaults.getColor("List.foreground");
        if (bglist != null && fglist != null) {
            detailsPane.setBackground(new Color(bglist.getRGB()));
            detailsPane.setForeground(new Color(fglist.getRGB()));
        } else {
            bglist = detailsPane.getBackground();
            fglist = detailsPane.getForeground();
        }*/
        if (font != null) {
            detailsPane.setFont(font);
        }

        Color fglist = detailsPane.getForeground();
        Color bluer = CSUtils.change(fglist, 0, 0, 0.8f);
        Color paler = CSUtils.change(fglist, .67f);

        new TextPopup().installAt(textComment);
        textTags.setOpaque(true);
        textCategory.setOpaque(true);
        
        textComment.setEditable(false);
        textComment.setWrapStyleWord(true);
        textComment.setLineWrap(true);
        
        textTags.setEditable(false);
        textTags.setWrapStyleWord(true);
        textTags.setLineWrap(true);
        
        textCategory.setEditable(false);
        textCategory.setWrapStyleWord(true);
        textCategory.setLineWrap(true);

        textCategory.setForeground(bluer);
        textCategory.setBorder(null);
        textCategory.setFocusable(false);
        textTags.setForeground(bluer);
        textTags.setBorder(null);
        textTags.setFocusable(false);
        Color paneBackground = detailsPane.getBackground();
        fixBackground(textTags, paneBackground);
        fixBackground(textCategory, paneBackground);
        
        labelTitle.setFont(CSUtils.changeFontStyle(labelTitle.getFont(), Font.BOLD));
        btnSend=new JButton(new RedirectAction(this, "sendPatch"));
        
        detailsPane.setLayout(new BorderLayout());

        // We use these three panels so we can add the comment text area
        // to the center pane. the center pane gets most of the space
        // in the borderlayout thus the comment text area has the most space.
        // This fixes a problem where the comment area, tag area, category area
        // all have the same height and the layout looks disrupted. 
        JPanel north = new JPanel();
        north.setLayout(new BoxLayout(north, BoxLayout.Y_AXIS));
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        JPanel south = new JPanel();
        south.setLayout(new BoxLayout(south, BoxLayout.Y_AXIS));
        detailsPane.add(north, BorderLayout.NORTH);
        detailsPane.add(center, BorderLayout.CENTER);
        detailsPane.add(south, BorderLayout.SOUTH);
        
        BoxLayoutBuilder northBuilder = new BoxLayoutBuilder(north);
        northBuilder.beginHorizontalBox();
        {
            northBuilder.add(labelTitle);
            northBuilder.addGlue();
            northBuilder.add(btnSend);
        }
        northBuilder.endContainer();
        northBuilder.beginHorizontalBox();
        {
            northBuilder.add(labelAuthorAndDate);
            northBuilder.addGlue();
        }
        northBuilder.endContainer();
        
        BoxLayoutBuilder centerBuilder = new BoxLayoutBuilder(center);
        centerBuilder.beginHorizontalBox();
        {
            JLabel label = new JLabel(I18N.translate("translation.comment", "Comment")+": ");
            label.setForeground(paler);
            centerBuilder.add(label);
            centerBuilder.addGlue();
        }
        centerBuilder.endContainer();
        centerBuilder.add(new JScrollPane(textComment));

        BoxLayoutBuilder southBuilder = new BoxLayoutBuilder(south);
        southBuilder.beginHorizontalBox();
        {
            JLabel label = new JLabel(I18N.translate("translation.category", "Category")+": ");
            label.setForeground(paler);
            label.setAlignmentX(Component.LEFT_ALIGNMENT);
            label.setAlignmentY(Component.TOP_ALIGNMENT);
            textCategory.setAlignmentX(Component.LEFT_ALIGNMENT);
            textCategory.setAlignmentY(Component.TOP_ALIGNMENT);

            southBuilder.add(label);
            southBuilder.add(textCategory);
        }
        southBuilder.endContainer();

        southBuilder.beginHorizontalBox();
        {
            JLabel label = new JLabel(I18N.translate("translation.tags", "Tags")+": ");
            label.setForeground(paler);
            label.setAlignmentX(Component.LEFT_ALIGNMENT);
            label.setAlignmentY(Component.TOP_ALIGNMENT);
            textTags.setAlignmentX(Component.LEFT_ALIGNMENT);
            textTags.setAlignmentY(Component.TOP_ALIGNMENT);

            southBuilder.add(label);
            southBuilder.add(textTags);
        }
        southBuilder.endContainer();
        
        southBuilder.beginVerticalBox();
        {
            southBuilder.beginHorizontalBox();
            {
                southBuilder.add(new JLabel(I18N.translate("translation.patchdetails.midifile.label", "Midi file")+": "));
                southBuilder.add(Box.createHorizontalGlue());
                southBuilder.add(this.btnSaveMidifileAs=new JButton(new RedirectAction(this, "saveMidifileAs")));
            }
            southBuilder.endContainer();
            southBuilder.beginHorizontalBox();
            {
                southBuilder.add(new JLabel(I18N.translate("translation.patchdetails.sourcecode.label", "Source code")+": "));
                southBuilder.add(Box.createHorizontalGlue());
                southBuilder.add(this.btnSavesourceAs=new JButton(new RedirectAction(this, "saveSourcecodeAs")));
            }
            southBuilder.endContainer();
        }
        southBuilder.endContainer();
        
        
        textComment.setAlignmentY(0);
        //textComment.setPreferredSize(new Dimension(Short.MAX_VALUE, Short.MAX_VALUE));
        panel.add(detailsPane,BorderLayout.CENTER);
        
        midiBar = new MidiSetupPanel(patchdownloader);
        panel.add(midiBar.getContainer(), BorderLayout.SOUTH);
        panel.setPreferredSize(new Dimension(260,100));
        
        updateView();
    }
    
    private void fixBackground(JTextArea textComponent, Color bgcolor) {
        // don't pass javax.swing.plaf.ColorUIResource as argument
        // see also: http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=6464612
        textComponent.setBackground(new Color(bgcolor.getRGB()));
        textComponent.setEditable(false);
    }

    @RedirectActionMeta(title="Save as...", resourcekey="patchdetails.midifile.saveas")
    public void saveMidifileAs() {
        saveMetadataPathAs(PatchMetadata.PATH_MIDIFILE);
    }

    @RedirectActionMeta(title="Save as...", resourcekey="patchdetails.sourcecode.saveas")
    public void saveSourcecodeAs() {
        saveMetadataPathAs(PatchMetadata.PATH_SOURCECODE);
    }

    private DateFormat dateFormat = DateFormat.getDateInstance(DateFormat.LONG);
    private String dateToString(Date date) {
        return dateFormat.format(date);
    }

    private void updateView() {
        String author_date = "";
        String title = "";
        String tags = "-";
        String categories = "-";
        String comment = "";
        if (metadata != null) {
            if (metadata.getAuthor() != null) {
                author_date = metadata.getAuthor().trim();
            }
            if (metadata.getLastModifiedDate() != null) {
                if (!author_date.isEmpty()) {
                    author_date = author_date+", ";
                }
                author_date += dateToString(metadata.getLastModifiedDate());
            }
            if (metadata.getTitle() != null) {
                title = metadata.getTitle().trim();
            }

            Tagset tagset = metadata.getTags();
            if (!tagset.isEmpty()) {
                final String PREFIX = "category:";
                Tagset tagsOnly = tagset.filterPrefix(PREFIX, false, true);
                Tagset categoryTags = tagset.filterPrefix(PREFIX, true, true);
                
                if (!tagsOnly.isEmpty())
                    tags = tagsOnly.toSortedString();
                if (!categoryTags.isEmpty())
                    categories = categoryTags.toSortedString();
            }
            
            if (metadata.getComment() != null) {
                comment = metadata.getComment().trim();
            }
            
        }

        btnSend.setEnabled(metadata != null && metadata.getPath(PatchMetadata.PATH_MIDIFILE)!=null);
        btnSaveMidifileAs.setEnabled(metadata != null && metadata.getPath(PatchMetadata.PATH_MIDIFILE)!=null);
        btnSavesourceAs.setEnabled(metadata != null && metadata.getPath(PatchMetadata.PATH_SOURCECODE)!=null);
        
        labelAuthorAndDate.setText(author_date);
        labelTitle.setText(title);
        textTags.setText(tags);
        textCategory.setText(categories);
        textComment.setText(comment);
    }
    
    public MidiSetupPanel getMidiBar() {
        return midiBar;
    }
    
    @RedirectActionMeta(title="Send", resourcekey="patchdetails.send")
    public void sendPatch() {
        new SendMidifileTask(patch, metadata).start();
    }
    
    private void saveMetadataPathAs(String key) {
        if (patch == null || metadata == null) {
            return;
        }
        String path = metadata.getPath(key);
        if (path == null) {
            return ;
        }
        File dir = patchdownloader.getLastFileChooserLocation();
        if (dir != null && dir.isFile()) {
            dir = dir.getParentFile();
        }
        JFileChooser chooser = new JFileChooser();
        if (dir != null) {
            chooser.setCurrentDirectory(dir);
        } else {
            dir = chooser.getCurrentDirectory();
        }

        try {
            DirectoryReader reader = patch.getDirectoryReader();
            try {
                boolean isFile = reader.isFile(path);
                if (!isFile) {
                    isFile = !reader.isDirectory(path);
                    if (isFile) {
                        throw new FileNotFoundException("path does not exist: "+path);
                    }
                }
                String name = CSUtils.getLastPathComponent(path);
                chooser.setSelectedFile(new File(dir, name));
                chooser.setVisible(true);
                chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                int code = chooser.showSaveDialog(panel);
                if (code == JFileChooser.APPROVE_OPTION) {
                    patchdownloader.setLastFileChooserLocation(chooser);
                    File dst = chooser.getSelectedFile();
                    
                    boolean go_on = true;
                    if (dst.exists()) {
                        if (isFile) {
                            boolean replace = 
                                JOptionPane.OK_OPTION ==
                                JOptionPane.showConfirmDialog(panel, 
                                    "Replace file ?\n"+dst.getName(),
                                    "Replace",
                                    JOptionPane.YES_NO_OPTION);
                            if (!replace) go_on = false;
                        } else if (!isFile) {

                            // see which files we would create
                            List<String> targetList = new ArrayList<String>();
                            
                            Enumeration<? extends name.cs.csutils.directoryreader.Entry> enumeration = 
                                reader.entryEnumeration(path);
                            
                            while (enumeration.hasMoreElements()) {
                                String targetPath = enumeration.nextElement().getPath();
                                File target = new File(dst, targetPath);
                                if (target.isFile() && target.exists()) {
                                    targetList.add(targetPath);
                                }
                            }
                            
                            if (!targetList.isEmpty()) {
                                StringBuilder msg = new StringBuilder();
                                msg.append("Replace file(s) ?");
                                for (String targetPath: targetList) {
                                    msg.append('\n').append(targetPath);
                                }

                                boolean replace = 
                                    JOptionPane.OK_OPTION ==
                                    JOptionPane.showConfirmDialog(panel, 
                                         msg.toString(),
                                        "Replace",
                                        JOptionPane.YES_NO_OPTION);
                                if (!replace) go_on = false;
                            }
                            
                            
                        }
                    } else {
                        if (!isFile && !dst.mkdir()) {
                            throw new IOException("could not create directory: "+dst.getAbsolutePath());
                        }
                    }
                    if (go_on) {
                        if (isFile) {
                            reader.copyFile(path, dst);
                        } else {
                            reader.copyTo(path, dst);
                        }
                    }
                }
            } finally {
                reader.close();
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

}
