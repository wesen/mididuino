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

import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;

import name.cs.csutils.i18n.I18N;

public class NoAction extends AbstractAction {

    /**
     * 
     */
    private static final long serialVersionUID = -7234337774156909187L;

    public NoAction(String title, String resourceKey) {
        putValue(NAME, title);
        I18N.update(this, resourceKey);
        Icons.update(this, resourceKey);
    }
    
    @Override
    public void actionPerformed(ActionEvent e) {
        // no op
    }

}
