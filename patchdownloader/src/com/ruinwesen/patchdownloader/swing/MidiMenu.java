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

import java.awt.Component;

import javax.swing.ButtonGroup;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;

import com.ruinwesen.midisend.JavaMidiSend;
import com.ruinwesen.midisend.MidiDevice;
import com.ruinwesen.midisend.MidiSend;
import com.ruinwesen.midisend.MidiSendException;
import com.ruinwesen.midisend.RWMidiSend;
import com.ruinwesen.patchdownloader.PatchDownloader;

import name.cs.csutils.i18n.I18N;
import name.cs.csutils.swing.RedirectAction;
import name.cs.csutils.swing.RedirectAction.RedirectActionMeta;

public class MidiMenu {

    private JMenu menuMidi;
    
    public MidiMenu() {
        this.init();
    }
    
    public JMenu getMenu() {
        return menuMidi;
    }
    
    protected MidiSend getMidiSend() {
        return PatchDownloader.getSharedInstance().getMidiSend();
    }
    
    private void init() {

        menuMidi = new JMenu(I18N.action("Midi", "menu.midi"));
       JRadioButtonMenuItem rbRWMidiSend = new JRadioButtonMenuItem(new RedirectAction(this, "useRWMidiSend"));
       JRadioButtonMenuItem rbjavaMidiSend = new JRadioButtonMenuItem(new RedirectAction(this, "useJavaMidiSend"));
       ButtonGroup midisendGroup = new ButtonGroup();
       midisendGroup.add(rbjavaMidiSend);
       midisendGroup.add(rbRWMidiSend);
       menuMidi.add(rbRWMidiSend);
       menuMidi.add(rbjavaMidiSend);
       menuMidi.addSeparator();
       menuMidi.add(new RedirectAction(this,"updateMidiSendDevices"));
       menuMidi.addSeparator();

       if (getMidiSend() instanceof RWMidiSend) {
           midisendGroup.setSelected(rbRWMidiSend.getModel(), true); 
       } else {
           midisendGroup.setSelected(rbjavaMidiSend.getModel(), true);
       }
       updateMidiSendDevices();
    }

    @RedirectActionMeta(title="midi-send")
    public void useRWMidiSend() {
        setMidiSend(new RWMidiSend());
    }

    @RedirectActionMeta(title="java")
    public void useJavaMidiSend() {
        setMidiSend(new JavaMidiSend());
    }

    final String MIDI_DEVICE_KEY = "midi.device.key";
    @RedirectActionMeta(title="Update midi device list",
            resourcekey="menu.midi.updatedevicelist")
    public void updateMidiSendDevices() {
        // remove old entries
        
        for (Component c: menuMidi.getMenuComponents()) {
            if (c instanceof JComponent
                    && ((JComponent)c).getClientProperty(MIDI_DEVICE_KEY)!=null) {
                menuMidi.remove(c);
            }
        }
        
        MidiSend midiSend = getMidiSend();
        
        ButtonGroup group = new ButtonGroup();
        try {
            for (MidiDevice dev: midiSend.getInputDeviceList()) {
                makeMidiDeviceMenuItem(group,midiSend.getInputDevice(),dev);
            }
        } catch (MidiSendException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
        if (group.getButtonCount()!=0) {
            JComponent sep = new JPopupMenu.Separator();
            sep.putClientProperty(MIDI_DEVICE_KEY,MIDI_DEVICE_KEY);
            menuMidi.add(sep);
        }
        group = new ButtonGroup();
        try {
            for (MidiDevice dev: midiSend.getOutputDeviceList()) {
                makeMidiDeviceMenuItem(group,midiSend.getOutputDevice(),dev);
            }
        } catch (MidiSendException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    protected void setMidiSend(MidiSend midiSend) {
        PatchDownloader.getSharedInstance().setMidiSend(midiSend);
        updateMidiSendDevices();
    }

    private JRadioButtonMenuItem makeMidiDeviceMenuItem(ButtonGroup group,
            MidiDevice usedDevice,
            MidiDevice device) {
        
        RedirectAction action =
            new RedirectAction(this, "useMidiDevice", MidiDevice.class, device);
        
        action.actionPerformed(null);
        action.putValue(RedirectAction.NAME, (device.isInput() ? "In: " : "Out: ")+device.getName());
        JRadioButtonMenuItem menuItem = new JRadioButtonMenuItem(action);
        menuMidi.add(menuItem);
        menuItem.putClientProperty(MIDI_DEVICE_KEY,MIDI_DEVICE_KEY);
        group.add(menuItem);
        if (usedDevice == device ||
                usedDevice != null && usedDevice.equals(device)) {
            menuItem.setSelected(true);
        }
        return menuItem;
    }

    public void useMidiDevice(MidiDevice device) {
        PatchDownloader.getSharedInstance().getMidiSend().setDevice(device);
    }
    
}
