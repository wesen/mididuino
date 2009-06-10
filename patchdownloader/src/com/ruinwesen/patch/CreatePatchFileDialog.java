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
package com.ruinwesen.patch;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.io.File;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.patchdownloader.swing.misc.TextPopup;

import name.cs.csutils.CSUtils;
import name.cs.csutils.RedirectAction;
import name.cs.csutils.RedirectAction.RedirectActionMeta;

public class CreatePatchFileDialog extends JDialog implements DeviceIDConstants {

    private static Log log = LogFactory.getLog(CreatePatchFileDialog.class);
    
    /**
     * 
     */
    private static final long serialVersionUID = -5263619233574690938L;

    private JTextField textAuthor;
    private JTextField textTitle;
    private JComboBox cbDeviceId;
    private JTextField textLastModifiedDate;
    private JTextArea textTags;
    private JTextArea textComment;
    private JTextField textSourcecode;
    private JTextField textMidifile;
    private JTextField textOutputFile;
    private JButton btnSelectSourcecode;
    private JButton btnSelectMidifile;
    private JButton btnSelectOutputFile;
    private JButton btnGenerate;
    private JFileChooser chooser = new JFileChooser();
    
    public CreatePatchFileDialog() {
        super();
        setTitle("PatchBuilder");
        init();
    }
    
    private void setDimensions(JComponent field) {
        Dimension d = field.getPreferredSize();
        d.width = Math.max(Short.MAX_VALUE, d.width);
        field.setMaximumSize(d);
        d.width = 200;
        field.setPreferredSize(d);
    }
    
    private void init() {
        JPanel container = new JPanel();
        container.setBorder(BorderFactory.createEmptyBorder(4,5,5,4));
        setContentPane(container);
        container.setLayout(new BorderLayout());
        
        List<JLabel> column1 = new LinkedList<JLabel>();
        JPanel north = new JPanel();
        north.setLayout(new BoxLayout(north, BoxLayout.Y_AXIS));
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        JPanel south = new JPanel();
        south.setLayout(new BoxLayout(south, BoxLayout.Y_AXIS));
        
        
        {
            JLabel label = new JLabel("Title");
            textTitle = new JTextField();
            label.setLabelFor(textTitle);
            column1.add(label);
            setDimensions(textTitle);

            Box box = Box.createHorizontalBox();   
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textTitle);
            box.add(Box.createHorizontalGlue());
            
            north.add(box);
        }
        {
            JLabel label = new JLabel("Author");
            textAuthor = new JTextField();
            label.setLabelFor(textAuthor);
            column1.add(label);
            setDimensions(textAuthor);

            Box box = Box.createHorizontalBox();      
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textAuthor);
            box.add(Box.createHorizontalGlue());
            north.add(box);
        }
        {
            JLabel label = new JLabel("Last Modified Date");
            textLastModifiedDate = new JTextField(CSUtils.dateToString(CSUtils.now()));
            label.setLabelFor(textLastModifiedDate);
            setDimensions(textLastModifiedDate);
            column1.add(label);

            Box box = Box.createHorizontalBox();      
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textLastModifiedDate);
            box.add(Box.createHorizontalGlue());
            north.add(box);
        }
        {
            JLabel label = new JLabel("Device-Id");
            Object[] defaults = new Object[] {
                    DEVICE_ID_MINICOMMAND_1_0
            };
            
            cbDeviceId = new JComboBox(defaults);
            cbDeviceId.setEditable(true);
            label.setLabelFor(cbDeviceId);
            column1.add(label);
            setDimensions(cbDeviceId);

            Box box = Box.createHorizontalBox();   
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(cbDeviceId);
            box.add(Box.createHorizontalGlue());
            
            north.add(box);
        }
        {
            JLabel label = new JLabel("Comment");
            textComment = new JTextArea();
            label.setLabelFor(textComment);
            column1.add(label);

            JScrollPane scroll = new JScrollPane(textComment);
            textComment.setPreferredSize(new Dimension(100, 60));
            Box box = Box.createHorizontalBox();      
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(scroll);
            center.add(box);
        }
        {
            JLabel label = new JLabel("Tags");
            textTags = new JTextArea();
            label.setLabelFor(textTags);
            column1.add(label);

            JScrollPane scroll = new JScrollPane(textTags);
            textTags.setPreferredSize(new Dimension(100, 40));
            Box box = Box.createHorizontalBox();      
            box.add(label);   
            box.add(Box.createHorizontalStrut(10));
            box.add(scroll);
            south.add(box);
        }
        {
            JLabel label = new JLabel("Source Code");
            textSourcecode = new JTextField();
            label.setLabelFor(textSourcecode);
            btnSelectSourcecode = new JButton(new RedirectAction(this, "btnSelectSourcecodeClick"));
            column1.add(label);

            Box box = Box.createHorizontalBox();    
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textSourcecode);
            box.add(Box.createHorizontalStrut(10));
            box.add(btnSelectSourcecode);
            box.add(Box.createHorizontalGlue());
            south.add(box);
        }
        {
            JLabel label = new JLabel("Midi file");
            textMidifile = new JTextField();
            label.setLabelFor(textMidifile);
            btnSelectMidifile = new JButton(new RedirectAction(this, "btnSelectMidiFileClick"));
            column1.add(label);

            Box box = Box.createHorizontalBox();      
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textMidifile);
            box.add(Box.createHorizontalStrut(10));
            box.add(btnSelectMidifile);
            box.add(Box.createHorizontalGlue());
            south.add(box);
        }
        {
            JSeparator sep = new JSeparator(SwingConstants.HORIZONTAL);
            sep.setMaximumSize(new Dimension(Short.MAX_VALUE, 5));
            south.add(sep);
        }
        {
            JLabel label = new JLabel("Output");
            textOutputFile = new JTextField();
            label.setLabelFor(textOutputFile);
            btnSelectOutputFile = new JButton(new RedirectAction(this, "btnSelectOutputfileClick"));
            column1.add(label);

            Box box = Box.createHorizontalBox();      
            box.add(label);
            box.add(Box.createHorizontalStrut(10));
            box.add(textOutputFile);
            box.add(Box.createHorizontalStrut(10));
            box.add(btnSelectOutputFile);
            box.add(Box.createHorizontalGlue());
            south.add(box);
        }
        {
            JSeparator sep = new JSeparator(SwingConstants.HORIZONTAL);
            sep.setMaximumSize(new Dimension(Short.MAX_VALUE, 5));
            south.add(sep);
        }
        {
            btnGenerate = new JButton(new RedirectAction(this, "generate"));
            btnGenerate.setAlignmentX(RIGHT_ALIGNMENT);
            Box box = Box.createHorizontalBox();      
            box.add(Box.createHorizontalGlue());
            box.add(btnGenerate);
            south.add(box);
        }
        Dimension size = new Dimension(0,0);
        for (JComponent c: column1) {
            Dimension pref = c.getPreferredSize();
            size.width = Math.max(size.width, pref.width);
            size.height = Math.max(size.height, pref.height);
        }
        if (size.width == 0) size.width = 100;
        if (size.height == 0) size.height = 20;
        for (JComponent c: column1) {
            c.setPreferredSize(size);
            c.setMinimumSize(size);
        }

        container.add(north, BorderLayout.NORTH);
        container.add(center, BorderLayout.CENTER);
        container.add(south, BorderLayout.SOUTH);


        new TextPopup().installAt(textAuthor);
        new TextPopup().installAt(textTitle);
        new TextPopup().installAt(textLastModifiedDate);
        new TextPopup().installAt(textTags);
        new TextPopup().installAt(textComment);
        new TextPopup().installAt(textSourcecode);
        new TextPopup().installAt(textMidifile);
        new TextPopup().installAt(textOutputFile);
        
    }

    @RedirectActionMeta(title="File...")
    public void btnSelectSourcecodeClick() {
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        File file = getSourcecodeFile();
        if (file != null) {
            chooser.setSelectedFile(file);
        }
        
        int code = chooser.showOpenDialog(this);
        if (code == JFileChooser.APPROVE_OPTION) {
            file = chooser.getSelectedFile();
            textSourcecode.setText(file == null ? "" : file.getAbsolutePath());
        }
    }
    @RedirectActionMeta(title="File...")
    public void btnSelectMidiFileClick() {
        chooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        File file = getMidiFile();
        if (file != null) {
            chooser.setSelectedFile(file);
        }
        int code = chooser.showOpenDialog(this);
        if (code == JFileChooser.APPROVE_OPTION) {
            file = chooser.getSelectedFile();
            textMidifile.setText(file == null ? "" : file.getAbsolutePath());
        }
    }
    @RedirectActionMeta(title="File...")
    public void btnSelectOutputfileClick() {
        chooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        File file = getOutputFile();
        if (file != null) {
            chooser.setSelectedFile(file);
        }
        int code = chooser.showOpenDialog(this);
        if (code == JFileChooser.APPROVE_OPTION) {
            file = chooser.getSelectedFile();
            textOutputFile.setText(file == null ? "" : file.getAbsolutePath());
        }
    }
    
    public File getMidiFile() {
        return getFile(textMidifile.getText(), true);
    }

    public File getSourcecodeFile() {
        return getFile(textSourcecode.getText(), false);
    }

    public File getOutputFile() {
        return getFile(textOutputFile.getText(), false);
    }

    private File getFile(String text, boolean aDirectory) {
        if (text == null || text.trim().length() == 0) {
            return null;
        }
        
        return new File(text);
    }
    
    public PatchFileBuilder createPatchFileBuilder() {
        PatchFileBuilder builder = new PatchFileBuilder();
        builder.setMidifile(getMidiFile());
        builder.setOutput(getOutputFile());
        builder.setSourceCode(getSourcecodeFile());
        PatchMetadata metadata = builder.getMetadata();
        metadata.setAuthor(textAuthor.getText());
        metadata.setComment(textComment.getText());
        metadata.setLastModifiedDate(CSUtils.parseDate(textLastModifiedDate.getText()));
        metadata.setTags(Tagset.parseTags(textTags.getText()));
        metadata.setTitle(textTitle.getText());
        
        String deviceIdString = (String) cbDeviceId.getSelectedItem();
        metadata.setDeviceId(deviceIdString);
        return builder;
    }
    
    @RedirectActionMeta(title="Generate")
    public void generate() {
        try {
            buildJarFile();
        } catch (Exception ex) {
            if (log.isDebugEnabled()) {
                log.debug("generate() failed", ex);
            }
            
            String message = ex.getMessage();
              
            JOptionPane.showMessageDialog(this, message,
                    "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
    
    public void buildJarFile() throws IOException, ParserConfigurationException, 
    TransformerException, PatchMetadataValidationException {
        File outputfile = getOutputFile();
        if (outputfile != null && outputfile.isFile()) {
            int code = JOptionPane.showConfirmDialog(this, 
                    "Replace file: "+outputfile.getName()+"?",
                    "Replace", 
                    JOptionPane.YES_NO_OPTION);
            if (code == JOptionPane.NO_OPTION) {
                return;
            }
        }
        
        Object deviceId = (Object) cbDeviceId.getSelectedItem();
        Object[] devideIDList = new Object[] {
                DEVICE_ID_MINICOMMAND_1_0
        };
        
        boolean found = false;
        for (Object realId: devideIDList) {
            if (realId.equals(deviceId)) {
                found = true;
                break;
            }
        }
        if (!found) {
            int code = JOptionPane.showConfirmDialog(this, 
                    "The device id is unknown:'"+deviceId+
                    "'.\nDo you wish to continue anyway ?",
                    "Unknown device-id", 
                    JOptionPane.YES_NO_OPTION);
            if (code == JOptionPane.NO_OPTION) {
                return;
            }
        }
        
        createPatchFileBuilder().build();
    }
    
}
 