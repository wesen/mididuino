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
package name.cs.csutils.swing;

import java.awt.Component;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.Action;

import name.cs.csutils.platform.Platform;


public class PerformActionOnEnterKey extends KeyAdapter {

    private Action action;

    public PerformActionOnEnterKey(Action action) {
        this.action = action;
    }
    
    @Override
    public void keyPressed(KeyEvent e) {
        handleKeyPressedOrReleasedEvent(e);
    }

    @Override
    public void keyReleased(KeyEvent e) {
        handleKeyPressedOrReleasedEvent(e);
    }

    private void handleKeyPressedOrReleasedEvent(KeyEvent e) {
        if (Platform.canPerformKeyAction(e)) {
            if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                performAction(e);
            }
        }
    }

    protected void performAction(KeyEvent e) {
        action.actionPerformed(null);
    }

    public void installTo(Component component) {
        component.addKeyListener(this);
    }

    
}
