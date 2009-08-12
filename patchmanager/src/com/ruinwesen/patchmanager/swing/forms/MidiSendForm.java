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
package com.ruinwesen.patchmanager.swing.forms;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import javax.swing.DefaultComboBoxModel;
import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.xml.parsers.ParserConfigurationException;

import name.cs.csutils.CSAction;
import name.cs.csutils.CSFileSelectionContext;
import name.cs.csutils.CSUtils;
import name.cs.csutils.InvokationTarget;
import name.cs.csutils.SwingActionData;
import name.cs.csutils.concurrent.SimpleSwingWorker;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.xml.sax.SAXException;

import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendCallback;
import com.ruinwesen.midisend.MidiSendException;
import com.ruinwesen.midisend.RWMidiSend;
import com.ruinwesen.patch.DefaultPatch;
import com.ruinwesen.patch.Patch;
import com.ruinwesen.patch.directory.Directory;
import com.ruinwesen.patch.metadata.PatchMetadata;
import com.ruinwesen.patch.metadata.Path;
import com.ruinwesen.patch.utils.HexFileValidator;
import com.ruinwesen.patch.utils.Validator;
import com.ruinwesen.patchmanager.swing.SwingPatchManagerUtils;

public class MidiSendForm extends Form implements DocumentListener, ItemListener,
    MidiSendCallback {
    
    private static Log log = LogFactory.getLog(MidiSendForm.class);
    
    private JPanel panel;
    private JLabel lblAtionStatusValue;
    private JProgressBar progressBar;
    private CSAction acClose;
    private CSAction acSend;
    private CSAction acUpdateMidiDevices;
    private JComboBox cbMidiIn;
    private JComboBox cbMidiOut;
    private MidiSend midiSend ;
    private JTextField tfMidiFile;
    private CSAction acChooseMidiFile;
    
    public MidiSendForm() {
        super();
        init();
    }
    
    private void init() {
        panel = new JPanel();
        GroupLayout ly = new GroupLayout(panel);
        panel.setLayout(ly);

        midiSend = new RWMidiSend();
        midiSend.setCallback(this);

        acClose = new CSAction(this, "btnCloseClick");
        acSend = new CSAction(this, "btnSendClick");
        acUpdateMidiDevices = new CSAction(this, "btnUpdateMidiDevicesClick").useInvokeLater();
        acChooseMidiFile = new CSAction(this, "btnChooseMidiFileClick");
        
        // create components
        lblAtionStatusValue = new JLabel("-");
        progressBar = new JProgressBar();
        cbMidiIn = new JComboBox();
        cbMidiOut = new JComboBox();
        tfMidiFile = new JTextField();
        tfMidiFile.setColumns(20);

        tfMidiFile.setText("/home/christian/test/chresan.hex");
        
        cbMidiIn.addItemListener(this);
        cbMidiOut.addItemListener(this);
        tfMidiFile.getDocument().addDocumentListener(this);
        
        // labels
        JLabel lblActionStatus = new JLabel("Status:");
        JLabel lblProgress = new JLabel("Progress:");

        JLabel lblMidiIn = new JLabel("Midi-In:");
        JLabel lblMidiOut = new JLabel("Midi-Out:");
        JLabel lblMidiFile = new JLabel("File:");

        JButton btnUpdateMidiDevices = new JButton(acUpdateMidiDevices);
        JButton btnClose = new JButton(acClose);
        JButton btnSend = new JButton(acSend);
        JButton btnChooseMidiFile = new JButton(acChooseMidiFile); 
        
        ly.setAutoCreateContainerGaps(true);
        ly.setAutoCreateGaps(true);
        
        ly.setHorizontalGroup(ly.createSequentialGroup()
                .addGroup(ly.createParallelGroup()
                        .addComponent(lblMidiFile)
                        .addComponent(lblActionStatus)
                        .addComponent(lblMidiIn)
                        .addComponent(lblMidiOut)
                        .addComponent(lblProgress)
                )
                .addGroup(ly.createParallelGroup()
                        .addComponent(tfMidiFile)
                        .addComponent(lblAtionStatusValue)
                        .addComponent(cbMidiIn)
                        .addComponent(cbMidiOut)
                        .addComponent(progressBar)
                )
                .addGroup(ly.createParallelGroup()
                        .addComponent(btnChooseMidiFile)
                        .addComponent(btnUpdateMidiDevices)
                        .addComponent(btnSend)
                        .addComponent(btnClose)
                )
        );
        
        ly.setVerticalGroup(ly.createSequentialGroup()
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblMidiFile)
                        .addComponent(tfMidiFile)
                        .addComponent(btnChooseMidiFile)
                )
                .addGap(20)
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblMidiIn)
                        .addComponent(cbMidiIn)
                )
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblMidiOut)
                        .addComponent(cbMidiOut)
                        .addComponent(btnUpdateMidiDevices, GroupLayout.Alignment.TRAILING)
                )
                .addGap(20)
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblProgress)
                        .addComponent(progressBar, GroupLayout.Alignment.CENTER)
                        .addComponent(btnSend)
                )
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(lblActionStatus)
                        .addComponent(lblAtionStatusValue)
                )
                .addGap(20)
                .addGroup(ly.createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(btnClose)
                )
        );
        
        SwingUtilities.invokeLater(new InvokationTarget(this, "updateMidiDevices"));
        
        ly.linkSize(SwingConstants.HORIZONTAL, 
                btnChooseMidiFile,
                btnClose,
                btnSend,
                btnUpdateMidiDevices
        );

        ly.linkSize(SwingConstants.HORIZONTAL, 
                cbMidiIn,
                cbMidiOut
        );
        
        
        panel.setMinimumSize(new Dimension(480,120));
        
        /*
        panel.add(lblValidationStatus);
        panel.add(lblValidationStatusValue);
        panel.add(lblActionStatus);
        panel.add(lblAtionStatusValue);
        
        panel.add(lblMidiIn);
        panel.add(cbMidiIn);
        panel.add(lblMidiOut);
        panel.add(cbMidiOut);

        panel.add(progressBar);
        
*/      
        
        SwingUtilities.invokeLater(new InvokationTarget(this, "updateSendAction"));
        
    }

    public File getMidiFile() {
        String str = tfMidiFile.getText();
        if (str != null && !str.trim().isEmpty()) {
            return new File(str.trim());
        }
        return null;
    }
    
    public void updateSendAction () {
        boolean sendOptionEnabled = true;
        if (cbMidiIn.getSelectedItem() == null || cbMidiOut.getSelectedItem() == null) {
            sendOptionEnabled = false;
        } else {
            File f = getMidiFile();
            if (f == null || !f.exists()) {
                sendOptionEnabled = false;
            }
        }
        acSend.setEnabled(sendOptionEnabled);
    }
    
    @SwingActionData("Close")    
    public void btnCloseClick() {
        ((RWMidiSend)midiSend).cancel(); // dirty hack :(
        closeDialog();
    }

    @SwingActionData("Send")
    public void btnSendClick() {
        MidiDevice din = (MidiDevice) cbMidiIn.getSelectedItem();
        MidiDevice dout = (MidiDevice) cbMidiOut.getSelectedItem();
        File file = getMidiFile();
        
        if (din == null || dout == null) {
            return; // TODO show error
        }

        if (file == null || !file.exists()) {
            lblAtionStatusValue.setText("Error: file not found:"+file);
            return;
        }

        // validate hex file + unpack hex file from patch
        final byte[] hexFileData;
        try {
            hexFileData = getHexFileData(new DefaultPatch(file));
        } catch (IOException ex) {
            return;
        }

        if (!validate(hexFileData)) {
            return;
        }
        
        
        lblAtionStatusValue.setText("Sending ...");
        
        midiSend.setInputDevice(din);
        midiSend.setOutputDevice(dout);
        
        acSend.setEnabled(false);
        
        new Thread(new SendThread(hexFileData)).start();
    }
    
    
    private class SendThread extends SimpleSwingWorker {
        private byte[] data;
        public SendThread(byte[] data) {
            this.data = data;
        }

        @Override
        protected void setup() {
            acSend.setEnabled(false);
            progressBar.setIndeterminate(true);
        }
        
        @Override
        protected void process() throws MidiSendException {
            midiSend.send(data);
        }

        @Override
        protected void cleanup() {
            progressBar.setIndeterminate(false);
            if (getException() != null) {
                lblAtionStatusValue.setText("");
                SwingPatchManagerUtils.showErrorDialog(panel, 
                        "Error: "+getException()
                );
            }
            acSend.setEnabled(true);
        }
        
    }
    
    private byte[] getHexFileData(Patch patch) throws IOException {
        PatchMetadata meta;
        try {
            meta = patch.getMetadata();
        } catch (ParserConfigurationException ex) {
            throw new IOException(ex);
        } catch (SAXException ex) {
            throw new IOException(ex);
        }
        
        Path hexfilepath = meta.getPath(PatchMetadata.DEFAULT_MIDIFILE_PATH_NAME);
        if (hexfilepath == null) {
            throw new IOException("patch contains no Intel HEX file");
        }
        String hexfilepathString = hexfilepath.getPath();
        if (hexfilepathString != null && hexfilepathString.trim().isEmpty()) {
            throw new IOException("invalid patch file");
        }
        
        Directory dir = patch.openDirectory();
        try {
            // hex file is not local, we have to extract it
            InputStream is = dir.getInputStream(hexfilepathString);
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            try {
                CSUtils.copy(is, out); 
            } finally {
                is.close();
            }
            return out.toByteArray();
        } finally {
            try {
                dir.close();
            } catch (IOException ex) {
                // ignore
            }
        }
    }
    
    private boolean validate(byte[] data) {
        try {
            Validator validator = new HexFileValidator(true);
            validator.update(data);
            validator.validate();
            return true;
        } catch (IOException ex) {
            // ignore
            lblAtionStatusValue.setText("Error:"+ex.getMessage());
        }
        return false;
    }
    
    @SwingActionData("Update")
    public void btnUpdateMidiDevicesClick() {
        updateMidiDevices();
    }

    @SwingActionData("File...")
    public void btnChooseMidiFileClick() {
        JFileChooser fc = new JFileChooser();
        fc.setMultiSelectionEnabled(false);
        fc.setAcceptAllFileFilterUsed(true);
        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
        CSFileSelectionContext.getDefaultContext().beforeFileSelection(fc);
        fc.setSelectedFile(getMidiFile());
        
        if (fc.showOpenDialog(panel) == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            CSFileSelectionContext.getDefaultContext().afterFileSelection(fc);
            String name = file != null ? file.getAbsolutePath() : "";
            tfMidiFile.setText(name);  
        } 
        
    }
    
    @Override
    public Component getForm() {
        return panel;
    }

    public void updateMidiDevices() {
        try {
            cbMidiIn.setModel(new DefaultComboBoxModel(midiSend.getInputDeviceList().toArray()));
            cbMidiOut.setModel(new DefaultComboBoxModel(midiSend.getOutputDeviceList().toArray()));
        } catch (MidiSendException ex) {
            if (log.isDebugEnabled()) {
                log.debug("set model failed", ex);
            }
        }
    }
    
    public static void main(String[] args) {
        new MidiSendForm().showDialog(null, "midi-send");
    }

    @Override
    public void changedUpdate(DocumentEvent e) {
        if (e.getDocument() == tfMidiFile.getDocument()) {
            updateSendAction();
        }
    }

    @Override
    public void insertUpdate(DocumentEvent e) {
        if (e.getDocument() == tfMidiFile.getDocument()) {
            updateSendAction();
        }
    }
    
    public void showDialog(Component parentComponent) {
        super.showDialog(parentComponent, "midi-send");
    }

    @Override
    public void removeUpdate(DocumentEvent e) {
        if (e.getDocument() == tfMidiFile.getDocument()) {
            updateSendAction();
        }
    }

    @Override
    public void itemStateChanged(ItemEvent e) {
        if ((e.getSource() == cbMidiIn || e.getSource() == cbMidiOut)) {
            updateSendAction();
        }
    }

    @Override
    public void midisendCompleted() {
        progressBar.setValue(100);
        lblAtionStatusValue.setText("Success.");
        progressBar.setIndeterminate(false);
    }

    @Override
    public void midisendProgress(float progress) {
        progressBar.setValue((int)(100*progress));
        progressBar.setIndeterminate(false);
    }

    public void setMidiFile(File file) {
        tfMidiFile.setText(file == null ? "" : file.getAbsolutePath());
    }

    public void setSelectMidiFileEnabled(boolean value) {
        tfMidiFile.setEditable(value);
        tfMidiFile.setEnabled(value);
        acChooseMidiFile.setEnabled(value);
    }
    
}
