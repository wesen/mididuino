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
package name.cs.csutils.iterator;

import java.util.Enumeration;
import java.util.Iterator;

public class IteratorToEnumeration<E> implements Enumeration<E> {

    private Iterator<E> iter;

    public IteratorToEnumeration(Iterator<E> iter) {
        this.iter = iter;
    }

    @Override
    public boolean hasMoreElements() {
        return iter.hasNext();
    }

    @Override
    public E nextElement() {
        return iter.next();
    }
    
}