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

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Collections;
import java.util.List;

import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import name.cs.csutils.CSUtils;
import name.cs.csutils.IconLoader;
import name.cs.csutils.RedirectAction;

import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendException;

public class MidiBar  {
    
    private static Log log = LogFactory.getLog(MidiBar.class);

    private JPanel container;
    private PDFrame patchdownloader;
    private JComboBox cbMidiIn ;
    private JComboBox cbMidiOut ;
    private boolean isUpdating = false;
    
    public MidiBar(PDFrame patchdownloader) {
        this.patchdownloader = patchdownloader;

        cbMidiIn = new JComboBox();
        cbMidiOut = new JComboBox();
        refreshMidideviceLists();

        Font smallLabelFont = CSUtils.changeFontSize(cbMidiIn.getFont(), 0.8f); 
        Font smallComboboxFont =  CSUtils.changeFontSize(cbMidiIn.getFont(), 0.8f); 
        cbMidiIn.setFont(smallLabelFont);
        cbMidiOut.setFont(smallLabelFont);
        cbMidiIn.setFont(smallComboboxFont);
        cbMidiOut.setFont(smallComboboxFont);
        
        HeaderPaneBuilder hpb = new HeaderPaneBuilder("MIDI setup");
        container = new JPanel();
        container.setBorder(BorderFactory.createEmptyBorder(2,6,2,2));
        container.setLayout(new BoxLayout(container, BoxLayout.Y_AXIS));
        {
            container.add(hpb.container);
        }
        {
            JLabel labelMidiIn = new JLabel("In:");
            JLabel labelMidiOut = new JLabel("Out:");
            labelMidiIn.setFont(smallLabelFont);
            labelMidiOut.setFont(smallLabelFont);
            labelMidiIn.setLabelFor(cbMidiIn);
            labelMidiOut.setLabelFor(cbMidiOut);
            useMaxPreferredSize(labelMidiIn, labelMidiOut);
            
            Box midiInBox = Box.createHorizontalBox();
            midiInBox.add(labelMidiIn);
            midiInBox.add(Box.createHorizontalStrut(2));
            midiInBox.add(cbMidiIn);
            Box midiOutBox = Box.createHorizontalBox();
            midiOutBox.add(labelMidiOut);
            midiOutBox.add(Box.createHorizontalStrut(2));
            midiOutBox.add(cbMidiOut);

            Box box = Box.createVerticalBox();
            box.add(Box.createVerticalStrut(2));
            box.add(midiInBox);
            box.add(Box.createVerticalStrut(2));
            box.add(midiOutBox);
            box.add(Box.createVerticalGlue());

            Action refreshMidideviceLists = new RedirectAction(this,"refreshMidideviceLists");
            IconLoader.update(refreshMidideviceLists, "menu.midi.updatedevicelist");
            refreshMidideviceLists.putValue(Action.NAME, "");
            JButton btnRefresh = new JButton(refreshMidideviceLists);
            
            Box boxLR = Box.createHorizontalBox();
            boxLR.add(box);
            boxLR.add(btnRefresh);
            
            container.add(boxLR);
        }
        container.add(Box.createHorizontalGlue());
        new EventHandler().install();
    }
    
    private class EventHandler implements ActionListener {
        public void install() {
            cbMidiIn.addActionListener(this);
            cbMidiOut.addActionListener(this);
        }
        public void actionPerformed(ActionEvent e) {
            if (!isUpdating) {
                if (e.getSource() == cbMidiIn) {
                    patchdownloader.getMidiSend().setInputDevice((MidiDevice)cbMidiIn.getSelectedItem());
                } else if (e.getSource() == cbMidiOut) {
                    patchdownloader.getMidiSend().setOutputDevice((MidiDevice)cbMidiOut.getSelectedItem());
                }
            }
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

            cbMidiIn.setModel(new DefaultComboBoxModel(inputList.toArray()) );
            cbMidiOut.setModel(new DefaultComboBoxModel(outputList.toArray()) );
            cbMidiIn.setSelectedItem(selectedMidiIn);
            cbMidiOut.setSelectedItem(selectedMidiOut);
        } finally {
            isUpdating = false;
        }
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
    

}
