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
import java.awt.event.ActionEvent;
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

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patchdownloader.PatchDownloader;
import com.ruinwesen.patchdownloader.patch.DefaultPatchMetadata;
import com.ruinwesen.patchdownloader.patch.PatchMetadata;
import com.ruinwesen.patchdownloader.patch.Tagset;
import com.ruinwesen.patchdownloader.repository.StoredPatch;

public class PatchDetailsView {

    private Log log = LogFactory.getLog(PatchDetailsView.class);
    
    private JPanel detailsPane;
    
    private JPanel panel = new JPanel();
    private StoredPatch patch;
    private PatchMetadata metadata = new DefaultPatchMetadata();

    private PDFrame patchdownloader;
    private MidiBar midiBar;
    
    
    private JLabel labelTitle = new JLabel();
    private JLabel labelAuthor = new JLabel();
    private JLabel labelDate = new JLabel();
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
                metadata = patch.getMetadata();
            } catch (IOException e) {
                metadata = new DefaultPatchMetadata();
            }
        }
        updateView();
    }

    public JComponent getComponent() {
        return panel;
    }
    
    public PatchDetailsView(PDFrame patchdownloader) {
        this.patchdownloader = patchdownloader;
        panel.setLayout(new BorderLayout());
        HeaderPaneBuilder hpb = new HeaderPaneBuilder(I18N.translate("translation.details","Details"));
        panel.add(hpb.headerPane,BorderLayout.NORTH);
        
        init();
        
        panel.add(new JScrollPane(detailsPane),BorderLayout.CENTER);
        
        midiBar = new MidiBar(patchdownloader);
        panel.add(midiBar.getContainer(), BorderLayout.SOUTH);
       
        
        updateView();
    }

    private void init() {
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

        textTags.setBackground(detailsPane.getBackground());
        textCategory.setBackground(detailsPane.getBackground());
        textTags.setOpaque(false);
        textCategory.setOpaque(false);
        
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
        
        detailsPane.setLayout(new BoxLayout(detailsPane, BoxLayout.Y_AXIS));
        {
            labelTitle.setFont(CSUtils.changeFontStyle(labelTitle.getFont(), Font.BOLD));
            
            Box hbox = Box.createHorizontalBox();
            hbox.add(labelTitle);
            hbox.add(Box.createHorizontalGlue());
            hbox.add(this.btnSend=new JButton(new RedirectAction(this, "sendPatch")));
            detailsPane.add(hbox);
        }
        {
            Box hbox = Box.createHorizontalBox();
            hbox.add(labelAuthor);
            hbox.add(new JLabel(", "));
            hbox.add(labelDate);
            hbox.add(Box.createHorizontalGlue());
            detailsPane.add(hbox);
        }
        /* {
            Box hbox = Box.createHorizontalBox();
            hbox.add(new JLabel(I18N.translate("translation.rating", "Rating")+": "));
            RatingBar ratingBar = new RatingBar();
            ratingBar.useDefaultSmallIconSet();
            hbox.add(Box.createHorizontalGlue());
            hbox.add(ratingBar);
            detailsPane.add(hbox);
        }*/
        {
            Box hbox = Box.createHorizontalBox();
            JLabel label = new JLabel(I18N.translate("translation.comment", "Comment")+": ");
            label.setForeground(paler);
            hbox.add(label);
            hbox.add(Box.createHorizontalGlue());
            detailsPane.add(hbox);
        }
        
        detailsPane.add(textComment);
        detailsPane.add(Box.createVerticalGlue());

        {
            Box hbox = Box.createHorizontalBox();
            JLabel label = new JLabel(I18N.translate("translation.category", "Category")+": ");
            label.setAlignmentX(Component.LEFT_ALIGNMENT);
            textCategory.setAlignmentX(Component.LEFT_ALIGNMENT);
            label.setForeground(paler);
            hbox.add(label);
            hbox.add(textCategory);
            detailsPane.add(hbox);
            label.setAlignmentY(Component.TOP_ALIGNMENT);
            textCategory.setAlignmentY(Component.TOP_ALIGNMENT);
        }
        {
            Box hbox = Box.createHorizontalBox();
            JLabel label = new JLabel(I18N.translate("translation.tags", "Tags")+": ");
            label.setAlignmentX(Component.LEFT_ALIGNMENT);
            textTags.setAlignmentX(Component.LEFT_ALIGNMENT);
            label.setForeground(paler);
            hbox.add(label);
            hbox.add(textTags);
            detailsPane.add(hbox);
            label.setAlignmentY(Component.TOP_ALIGNMENT);
            textTags.setAlignmentY(Component.TOP_ALIGNMENT);
        }
        {
            Box vbox = Box.createVerticalBox();

            Box hbox1 = Box.createHorizontalBox();
            hbox1.add(new JLabel(I18N.translate("translation.patchdetails.midifile.label", "Midi file")+": "));
            hbox1.add(Box.createHorizontalGlue());
            hbox1.add(this.btnSaveMidifileAs=new JButton(new RedirectAction(this, "saveMidifileAs")));

            Box hbox2 = Box.createHorizontalBox();
            hbox2.add(new JLabel(I18N.translate("translation.patchdetails.sourcecode.label", "Source code")+": "));
            hbox2.add(Box.createHorizontalGlue());
            hbox2.add(this.btnSavesourceAs=new JButton(new RedirectAction(this, "saveSourcecodeAs")));

            vbox.add(hbox1);
            vbox.add(hbox2);
            
            detailsPane.add(vbox);
        }
    }
    
    @RedirectActionMeta(title="Save as...", resourcekey="patchdetails.midifile.saveas")
    public void saveMidifileAs() {
        saveMetadataPathAs(PatchMetadata.PATH_MIDIFILE);
    }

    @RedirectActionMeta(title="Save as...", resourcekey="patchdetails.sourcecode.saveas")
    public void saveSourcecodeAs() {
        saveMetadataPathAs(PatchMetadata.PATH_SOURCECODE);
    }

    public void actionPerformed(ActionEvent actionEvent) {
        if ("btn.send".equals(actionEvent.getActionCommand())) {
            sendPatch();
        } 
    }

    private DateFormat dateFormat = DateFormat.getDateInstance(DateFormat.LONG);
    private String dateToString(Date date) {
        return dateFormat.format(date);
    }

    private void updateView() {
        String author = "";
        String date = "";
        String title = "";
        String tags = "";
        String categories = "";
        String comment = "";
        if (metadata != null) {
            if (metadata.getAuthor() != null) {
                author = metadata.getAuthor().trim();
            }
            if (metadata.getLastModifiedDate() != null) {
                date = dateToString(metadata.getLastModifiedDate());
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
        
        labelAuthor.setText(author);
        labelDate.setText(date);
        labelTitle.setText(title);
        textTags.setText(tags);
        textCategory.setText(categories);
        textComment.setText(comment);

        updateSize(textCategory);
        updateSize(textTags);
    }
    
    private void updateSize(JTextArea area) {
        Dimension d = area.getMinimumSize();
        d.setSize(detailsPane.getWidth(), d.height);
        area.setMaximumSize(d);
    }

    public MidiBar getMidiBar() {
        return midiBar;
    }
    
    /*
    private void setHTML() {
        if (patch == null) {
            htmlField.setText("");
            return;
        } 
        
        StringBuilder html = new StringBuilder();

        boolean hasSource = metadata.getPath(PatchMetadata.PATH_SOURCECODE) != null;
        boolean hasMidiFile =  metadata.getPath(PatchMetadata.PATH_MIDIFILE) != null;
        
        html.append("<html background=\""+htmlBackground+"\">");
        if (hasMidiFile) {
            html.append("<table><tr><td><b>Send to device:</b> </td><td><input id=\"btn.send\" type=\"submit\" value=\"Send\"></td></tr></table>");
        }
        html.append("<br>");
        html.append("<div><b>Title:</b> "+metadata.getTitle()+" </div>");
        html.append("<div><b>Author:</b> "+metadata.getAuthor()+"</div>");
        html.append("<div><b>Tags:</b> "+metadata.getTags().toSortedString()+"</div>");
        
        html.append("<br>");
        html.append("<div><b>Comment:</b> </div>");
        if (metadata.getComment() != null) {
            html.append("<p>"+metadata.getComment()+"</p>");
        }
        html.append("<table>");
        if (hasSource) {
            html.append("<tr><td><b>Source code:</b> </td><td><input id=\"btn.sourcecode\" type=\"submit\" value=\"Save as...\"></td></tr>");
        }
        if (hasMidiFile) {
            html.append("<tr><td><b>Midi file:</b> </td><td><input id=\"btn.midifile\" type=\"submit\" value=\"Save as...\"></td></tr>");
        } else {
            html.append("<tr><td><b>Midi file:</b> </td><td><i>not available</i></td></tr>");
        }
        html.append("</table>");
        html.append("</html>");
        
        htmlField.setEditorKit(new MyHTMLEditorKit(this));
        htmlField.setText(html.toString());
    }*/

    @RedirectActionMeta(title="Send", resourcekey="patchdetails.send")
    public void sendPatch() {
        try {
            __sendPatch();
        } catch (Exception ex) {
            if (log.isDebugEnabled()) {
                log.debug("sendPatch() failed", ex);
            }
            JOptionPane.showMessageDialog(null, 
                    "Could not send patch: "+ex.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
    
    public void __sendPatch() throws Exception {
        
        if (!(patch != null && metadata != null)) {
            return;
        }
        String midifilepath = metadata.getPath(PatchMetadata.PATH_MIDIFILE);
        if (midifilepath == null) {
            return;
        }

        DirectoryReader reader;
        File tmp = null;
        try {
            reader = patch.getDirectoryReader();
            if (reader == null) {
                throw new IOException("no reader available for patch: "+patch.getPath());
            }
            try {
                tmp = File.createTempFile("send", CSUtils.getLastPathComponent(midifilepath));
            } catch (IOException ex) {
                throw new IOException("could not create temporary file", ex);
            }
            reader.copyFile(midifilepath, tmp);
            reader.close();
            
            PatchDownloader.getSharedInstance().getMidiSend().send(tmp);
        } finally {
            if (tmp != null)
                tmp.delete();
        }
    }
    
    private void saveMetadataPathAs(String key) {
        if (patch == null) {
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
