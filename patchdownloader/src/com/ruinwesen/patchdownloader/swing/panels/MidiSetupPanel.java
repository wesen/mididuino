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
package com.ruinwesen.patchdownloader.swing.panels;

import java.awt.Dimension;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import javax.swing.Action;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;

import name.cs.csutils.CSUtils;
import name.cs.csutils.IconLoader;
import name.cs.csutils.RedirectAction;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendException;
import com.ruinwesen.patchdownloader.swing.SwingPatchdownloader;
import com.ruinwesen.patchdownloader.swing.misc.BoxLayoutBuilder;

public class MidiSetupPanel  {
    
    private static Log log = LogFactory.getLog(MidiSetupPanel.class);

    private JPanel container;
    private SwingPatchdownloader patchdownloader;
    private JComboBox cbMidiIn ;
    private JComboBox cbMidiOut ;
    private boolean isUpdating = false;
    
    public MidiSetupPanel(SwingPatchdownloader patchdownloader) {
        this.patchdownloader = patchdownloader;

        cbMidiIn = new JComboBox();
        cbMidiOut = new JComboBox();
        refreshMidideviceLists();

        /*
        Font smallLabelFont = CSUtils.changeFontSize(cbMidiIn.getFont(), 0.8f); 
        Font smallComboboxFont =  CSUtils.changeFontSize(cbMidiIn.getFont(), 0.8f); 
        cbMidiIn.setFont(smallLabelFont);
        cbMidiOut.setFont(smallLabelFont);
        cbMidiIn.setFont(smallComboboxFont);
        cbMidiOut.setFont(smallComboboxFont);
        */
        Action changeMidiDevice = new RedirectAction(this, "cbNewMidiDeviceSelected");
        cbMidiIn.addActionListener(changeMidiDevice);
        cbMidiOut.addActionListener(changeMidiDevice);

        JLabel labelMidiIn = new JLabel("In:");
        JLabel labelMidiOut = new JLabel("Out:");
       // labelMidiIn.setFont(smallLabelFont);
       // labelMidiOut.setFont(smallLabelFont);
        labelMidiIn.setLabelFor(cbMidiIn);
        labelMidiOut.setLabelFor(cbMidiOut);
        useMaxPreferredSize(labelMidiIn, labelMidiOut);

        Action refreshMidideviceLists = new RedirectAction(this,"refreshMidideviceLists");
        IconLoader.update(refreshMidideviceLists, "menu.midi.updatedevicelist");
        refreshMidideviceLists.putValue(Action.NAME, "");
        JButton btnRefresh = new JButton(refreshMidideviceLists);
        
        SectionBorder border = new SectionBorder("MIDI setup");
        container = new JPanel();
        container.setBorder(border);
        container.setLayout(new BoxLayout(container, BoxLayout.X_AXIS));
        
        BoxLayoutBuilder builder = new BoxLayoutBuilder(container);

        // left column
        builder.beginVerticalBox();
        {
            // midi in
            builder.beginHorizontalBox();
            builder.add(labelMidiIn);
            builder.addStrut(2);
            builder.add(cbMidiIn);
            builder.endContainer();
    
            // midi out
            builder.beginHorizontalBox();
            builder.add(labelMidiOut);
            builder.addStrut(2);
            builder.add(cbMidiOut);
            builder.endContainer();
        }
        builder.endContainer();
        // second column: refresh button
        builder.add(btnRefresh);
    }
    
    public void cbNewMidiDeviceSelected() {
        if (!isUpdating) {
            MidiSend midisend = patchdownloader.getMidiSend();
            midisend.setInputDevice(((MidiDeviceItem)cbMidiIn.getSelectedItem()).dev);
            midisend.setOutputDevice(((MidiDeviceItem)cbMidiOut.getSelectedItem()).dev);
        }
    }
    
    public void refreshMidideviceLists() {
        isUpdating = true; // prevent the eventhandler to act on the combobox changes
        try {
            MidiSend midiSend = patchdownloader.getMidiSend();
    
            List<MidiDevice> inputList;
            List<MidiDevice> outputList;
            try {
                inputList = midiSend.getInputDeviceList();
            } catch (MidiSendException ex) {
                inputList = Collections.emptyList();
                if (log.isWarnEnabled()) {
                    log.warn("could not retrieve input device list", ex);
                }
            }
            try {
                outputList = midiSend.getOutputDeviceList();
            } catch (MidiSendException ex) {
                outputList = Collections.emptyList();
                if (log.isWarnEnabled()) {
                    log.warn("could not retrieve output device list", ex);
                }
            }

            MidiDevice selectedMidiIn = midiSend.getInputDevice();
            MidiDevice selectedMidiOut = midiSend.getOutputDevice();
            if (selectedMidiIn == null && !inputList.isEmpty()) {
                selectedMidiIn = inputList.get(0);
                midiSend.setInputDevice(selectedMidiIn);
            }
            if (selectedMidiOut == null && !outputList.isEmpty()) {
                selectedMidiOut = outputList.get(0);
                midiSend.setOutputDevice(selectedMidiOut);
            }

            
            
            cbMidiIn.setModel(new DefaultComboBoxModel(wrap(inputList)) );
            cbMidiOut.setModel(new DefaultComboBoxModel(wrap(outputList)) );
            cbMidiIn.setSelectedItem(selectedMidiIn);
            cbMidiOut.setSelectedItem(selectedMidiOut);
        } finally {
            isUpdating = false;
        }
    }
    
    private static Object[] wrap(List<MidiDevice> list)  {
        List<MidiDeviceItem> alist = new ArrayList<MidiDeviceItem>(list.size());
        for (MidiDevice dev: list) {
            alist.add(new MidiDeviceItem(dev));
        }
        return alist.toArray();
    }
    
    private void useMaxPreferredSize(JComponent a, JComponent b) {
        Dimension da = a.getPreferredSize();
        Dimension db = b.getPreferredSize();
        da.width = Math.max(da.width, db.width);
        da.height = Math.max(da.height, db.height);
        a.setPreferredSize(da);
        b.setPreferredSize(da);
    }

    public JComponent getContainer() {
        return container;
    }
    
    private static class MidiDeviceItem {
        private MidiDevice dev;

        public MidiDeviceItem(MidiDevice dev) {
            this.dev = dev;
        }
        
        @Override
        public String toString() {
            return CSUtils.limitString(dev.toString(), 20);
        }
        
    }

}
