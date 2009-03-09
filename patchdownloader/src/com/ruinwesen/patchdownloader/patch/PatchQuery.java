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

import java.io.File;
import java.io.IOException;

import name.cs.csutils.CSUtils;

import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.index.Term;
import org.apache.lucene.queryParser.MultiFieldQueryParser;
import org.apache.lucene.queryParser.ParseException;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.FuzzyQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.BooleanClause.Occur;

public abstract class PatchQuery {

    public abstract Query getQuery() throws ParseException;
    
    public void save(File file) throws IOException {
        throw CSUtils.NotImplementedYet();
    }

    public static Query author(String value) {
        return new FuzzyQuery(new Term(PatchDocument.AUTHOR, value));
    }

    public static Query title(String value) {
        return new FuzzyQuery(new Term(PatchDocument.TITLE, value));
    }
    
    /**
     * TODO
     * @author chresan
     */
    public static class AdvancedQuery extends PatchQuery {
        
        private String title = null;
        private String author = null;
        // private String device = null;
        private String tags = null;

        @Override
        public Query getQuery() throws ParseException {
            BooleanQuery query = new BooleanQuery();
            if (author != null) {
                query.add(new TermQuery(new Term(PatchDocument.AUTHOR, author)), Occur.SHOULD);
            }
            if (title != null) {
                query.add(new TermQuery(new Term(PatchDocument.TITLE, title)), Occur.SHOULD);
            }
            if (true) throw CSUtils.NotImplementedYet();
            return query;
        }
        
    }
    
    public static class PlainTextQuery extends PatchQuery {
        private String queryString;
        private MultiFieldQueryParser parser;

        public PlainTextQuery(String queryString) {
            String[] fields = new String[] {
                    PatchDocument.AUTHOR,
                    PatchDocument.COMMENT,
                    PatchDocument.TITLE
            };
            parser = new MultiFieldQueryParser(fields,
                    new StandardAnalyzer());
            this.queryString = queryString;
        }
        
        public String getQueryString() {
            return queryString;
        }

        @Override
        public Query getQuery() throws ParseException {
            return parser.parse(queryString);
        }
    }
    
}
