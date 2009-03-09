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

import org.apache.lucene.document.DateTools;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.Field.Store;

public class PatchDocument {

    public static final String TAGS = "tags";
    public static final String TITLE = "title";
    public static final String NAME = "name";
    public static final String AUTHOR = "author";
    public static final String LASTMODIFIED = "lastmodified";
    public static final String COMMENT = "comment";
    // public static final String PATH = "patch.path";
    
    private PatchDocument() {}
    
    private static boolean containsText(String str) {
        return str != null && str.trim().length() != 0;
    }
    
    public static Document Document(PatchMetadata patch) {
        Document doc = new Document();
        String tmp; // used to cache the method results - in case
        // patch is a XMLPatchMetadata object where these calls are expensive

        
        if (containsText(tmp=patch.getTitle())) {
            add(doc, TITLE, tmp);
        }
        if (containsText(patch.getName())) {
            add(doc, NAME, tmp);
        }
        if (containsText(tmp=patch.getAuthor())) {
            add(doc, AUTHOR, tmp);
        }
        
        Date lastmodified = patch.getLastModifiedDate();
        if (lastmodified != null) {
            doc.add(new Field(LASTMODIFIED, 
                    DateTools.timeToString(lastmodified.getTime(), 
                            DateTools.Resolution.DAY),
                    Field.Store.YES, Field.Index.NOT_ANALYZED));
        }
        if (containsText(tmp=patch.getComment())) {
            doc.add(new Field(COMMENT, tmp, Field.Store.YES,
                Field.Index.ANALYZED));
        }

        tmp = patch.getTags().toString();
        if (tmp.length()>0) {
            doc.add(new Field(TAGS, tmp, Field.Store.YES, Field.Index.ANALYZED));
        }
        
        return doc;
    }
    
    public static PatchMetadata ToMetadata(Document doc) {
        PatchMetadata metadata = new DefaultPatchMetadata();
        metadata.setTitle(doc.get(TITLE));
        metadata.setName(doc.get(NAME));
        metadata.setAuthor(doc.get(AUTHOR));
        String tags = doc.get(TAGS);
        if (tags != null) metadata.setTags(Tagset.parseTags(tags));
        return metadata;
    }

    private static void add(Document doc, String field, String value) {
        if (value != null) {
            doc.add(new Field(field, norm(value), Store.YES, Field.Index.NOT_ANALYZED));
        }
    }

    private static String norm(String str) {
        return str.trim().toLowerCase();
    }
    
}
