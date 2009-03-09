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
package name.cs.csutils.directoryreader;

/**
 * A file or directory entry.
 * 
 * @author chresan
 */
public interface Entry {

    /**
     * Returns the path name of this entry.
     * @return the path name of this entry
     */
    String getPath();
    
    /**
     * Returns the name of this entry.
     * @return the name of this entry
     */
    String getName();
    
    /**
     * Returns true if this entry is a file, otherwise false.
     * @return true if this entry is a file, otherwise false
     */
    boolean isFile();

    /**
     * Returns true if this entry is a directory, otherwise false.
     * @return true if this entry is a directory, otherwise false
     */
    boolean isDirectory();
    
    /**
     * Returns the size of the file or -1 if the size is not known.
     * @return the size of the file or -1 if the size is not known
     */
    long size();
    
    /**
     * Returns the path name of this entry.
     * @return the path name of this entry
     */
    String toString();
    
}
