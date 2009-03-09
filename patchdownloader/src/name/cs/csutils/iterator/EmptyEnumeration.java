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
import java.util.NoSuchElementException;

public class EmptyEnumeration<E> implements Enumeration<E> {
    
    private static final Enumeration<Object> INSTANCE = new EmptyEnumeration<Object>();
    
    @SuppressWarnings("unchecked")
    public static <E> EmptyEnumeration<E> getInstance() {
        return (EmptyEnumeration<E>) INSTANCE;
    }

    @Override
    public boolean hasMoreElements() {
        return false;
    }

    @Override
    public E nextElement() {
        throw new NoSuchElementException();
    }

}
