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
package com.ruinwesen.patchdownloader.patch;

import java.util.Date;
import java.util.Map;

public interface PatchMetadata {

    public static final String PATH_SOURCECODE = "sourcecode";
    public static final String PATH_MIDIFILE = "midifile";

    String getMetadataFormatVersion();
    String getComment();
    void setComment(String comment);
    String getTitle();
    void setTitle(String title);
    void setLastModifiedDate(Date date);
    Date getLastModifiedDate();
    String getName();
    void setName(String name);
    String getAuthor();
    void setAuthor(String author);
    void setMetadata(PatchMetadata metadata);
    Tagset getTags();
    void setTags(Tagset tag);
    void removeTag(String tag);
    void removeAllTags(Tagset tag);
    void addAllTags(Tagset tag);

    String getPath(String name);
    void setPath(String name, String path);
    
    Map<String,String> getPaths();
    
}
