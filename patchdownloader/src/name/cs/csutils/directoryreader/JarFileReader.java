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

import java.util.jar.JarFile;

/**
 * Reads files and directories in a jar file. 
 * 
 * @author chresan
 */
public class JarFileReader extends ZipFileReader<JarFile> {

    /**
     * Reads files and directories in the specified jar file.
     * @param jarfile the jar file
     */
    public JarFileReader(JarFile jarfile) {
        super(jarfile);
    }
    
}
