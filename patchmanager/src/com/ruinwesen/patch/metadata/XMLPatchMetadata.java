/**
 * Copyright (c) 2009, Christian Schneider
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the names of the authors nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package com.ruinwesen.patch.metadata;
import static com.ruinwesen.patch.metadata.PatchMetadataUtils.createTextElement;

import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import javax.xml.XMLConstants;
import javax.xml.namespace.NamespaceContext;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import name.cs.csutils.CSUtils;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import com.ruinwesen.patch.PatchDataException;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.DeviceId;
import com.ruinwesen.patch.metadata.PatchMetadataIDInfo.EnvironmentId;

public class XMLPatchMetadata implements PatchMetadata {
    
    private static Log log = LogFactory.getLog(XMLPatchMetadata.class);

    static {
        initXPath();
    }

    private static XPathExpression xpAuthor;
    private static XPathExpression xpTitle;
    private static XPathExpression xpPatchId;
    private static XPathExpression xpName;
    private static XPathExpression xpComment;
    private static XPathExpression xpDeviceId;
    private static XPathExpression xpEnvironmentId;
    private static XPathExpression xpPathNodeList;
    private static XPathExpression xpPathList;
    private static XPathExpression xpTags;
    private static XPathExpression xpLastModifiedDate;
    
    
    private static class NSContext implements NamespaceContext {
        @Override
        public String getNamespaceURI(String prefix) {
            if ( "pm".equals(prefix)) {
                return NS_PATCH_METDATA;
            }
            return XMLConstants.NULL_NS_URI;
        }

        @Override
        public String getPrefix(String namespaceURI) {
            if (namespaceURI.equals(NS_PATCH_METDATA)) {
                return "pm";
            }
            return null;
        }

        @Override
        public Iterator<String> getPrefixes(String namespaceURI) {
            List<String> list = new LinkedList<String>();
            if (namespaceURI.equals(NS_PATCH_METDATA)) {
                list.add("pm");
            }
            return list.iterator();
        }
        
    }
    
    private static void initXPath() {
        XPath xpath = XPathFactory.newInstance().newXPath();
        
        xpath.setNamespaceContext(new NSContext());
        
        try {
            xpPatchId = xpath.compile("/pm:patch-metadata/pm:patch-id");
            xpAuthor = xpath.compile("/pm:patch-metadata/pm:author");
            xpTitle = xpath.compile("/pm:patch-metadata/pm:title");
            xpName = xpath.compile("/pm:patch-metadata/pm:name");
            xpComment = xpath.compile("/pm:patch-metadata/pm:comment");
            xpDeviceId = xpath.compile("/pm:patch-metadata/pm:device-id");
            xpEnvironmentId = xpath.compile("/pm:patch-metadata/pm:environment-id");
            xpPathList = xpath.compile("/pm:patch-metadata/pm:path-list");
            xpPathNodeList = xpath.compile("/pm:patch-metadata/pm:path-list/pm:path");
            xpTags = xpath.compile("/pm:patch-metadata/pm:tags");
            xpLastModifiedDate = xpath.compile("/pm:patch-metadata/pm:last-modified-date");
        } catch (XPathExpressionException ex) {
            if (log.isErrorEnabled()) {
                log.error("compiling xpath expressions", ex);
            }
        }
    }
    
    private Document doc;
    
    public XMLPatchMetadata() throws PatchDataException {
        this(PatchMetadataUtils.createDocument());
    }

    public XMLPatchMetadata(File file) throws PatchDataException {
        this(PatchMetadataUtils.parseDocument(file));
        doc.setDocumentURI(file.toURI().toString());
    }

    public XMLPatchMetadata(InputStream in) throws PatchDataException {
        this(PatchMetadataUtils.parseDocument(in));
    }
    
    public XMLPatchMetadata(PatchMetadata src) throws PatchDataException {
        this(PatchMetadataUtils.createDocument(src));
    }
    
    XMLPatchMetadata(Document doc) {
        if (doc == null) {
            throw new NullPointerException();
        }
        this.doc = doc;
        String ns = doc.getDocumentElement().getNamespaceURI();
        if (!NS_PATCH_METDATA.equals(ns)) {
            throw new IllegalArgumentException("unsupported xml namespace:'"+ns+"'");
        }
        if (!VERSION_1_0.equals(getVersion())) {
            throw new IllegalArgumentException("unsupported document version: "+getVersion());
        }
    }
    
    public Document getDocument() {
        return doc;
    }
    
    public PatchMetadata copy() {
        return new XMLPatchMetadata((Document)doc.cloneNode(true));
    }
    
    private final String getTextContent(XPathExpression xp, String defaultValue) {
        try {
            return (String) xp.evaluate(doc, XPathConstants.STRING);
        } catch (XPathExpressionException ex) {
            return defaultValue;
        }
    }
    
    @Override
    public String getAuthor() {
        return getTextContent(xpAuthor,"");
    }

    @Override
    public String getComment() {
        return getTextContent(xpComment,"");
    }

    @Override
    public String getName() {
        return getTextContent(xpName,"");
    }

    @Override
    public String getTitle() {
        return getTextContent(xpTitle,"");
    }

    @Override
    public String getVersion() {
        return doc.getDocumentElement().getAttribute("version");
    }

    @Override
    public DeviceId getDeviceId() {
        String str = getTextContent(xpDeviceId,null);
        if (str == null || str.isEmpty()) {
            return null;
        }
        return PatchMetadataIDInfo.getDeviceId(str);
    }

    @Override
    public EnvironmentId getEnvironmentId() {
        String str = getTextContent(xpEnvironmentId,null);
        if (str == null || str.isEmpty()) {
            return null;
        }
        return PatchMetadataIDInfo.getEnvironmentId(str);
    }

    @Override
    public void setAuthor(String value) {
        setNode(xpAuthor, "author", value);
    }

    @Override
    public void setComment(String value) {
        setNode(xpComment, "comment", value);
    }

    @Override
    public void setDeviceId(DeviceId value) {
        if (value == null)
            deleteNode(xpDeviceId);
        else
            setNode(xpDeviceId, "device-id", value.value());
    }

    @Override
    public void setEnvironmentId(EnvironmentId value) {
        if (value == null)
            deleteNode(xpEnvironmentId);
        else
            setNode(xpEnvironmentId, "environment-id", value.value());
    }
    
    private void setNode(XPathExpression xpElement, String name, String value) {
        Element elem;
        try {
            elem = (Element)xpElement.evaluate(doc, XPathConstants.NODE);
        } catch (XPathExpressionException ex) {
            return;
        }

        if (elem != null) {
            elem.setTextContent(value == null ? "" : value);
        } else {
            if (value != null) { // insert
                doc
                .getDocumentElement()
                .appendChild(createTextElement(doc, name, value));
            }
        }        
    }

    private void deleteNode(XPathExpression xpElement) {
        Element elem;
        try {
            elem = (Element)xpElement.evaluate(doc, XPathConstants.NODE);
        } catch (XPathExpressionException ex) {
            return;
        }

        if (elem != null) {
            elem.getParentNode().removeChild(elem);
        }
    }

    @Override
    public void setName(String value) {
        setNode(xpName, "name", value);
    }

    @Override
    public void setTitle(String value) {
        setNode(xpTitle, "title", value);
    }

    @Override
    public void setVersion(String value) {
        if (value == null) {
            return;
        }
        doc.getDocumentElement().setAttribute("version", value);
    }

    @Override
    public Map<String, Path> getPaths() {
        NodeList nodelist;
        try {
            nodelist = (NodeList) xpPathNodeList
                .evaluate(doc.getDocumentElement(), XPathConstants.NODESET);
            if (nodelist == null) {
                return Collections.emptyMap();
            }
        } catch (XPathExpressionException ex) {
            return Collections.emptyMap();
        }

        Map<String, Path> map = new HashMap<String, Path>();
        for (int i=0;i<nodelist.getLength();i++) {
            Element elem = (Element) nodelist.item(i);
            String name = elem.hasAttribute("name") ? elem.getAttribute("name") : null;
            String value = elem.getTextContent();
            map.put(name, new Path(name, value));
        }
        return Collections.unmodifiableMap(map);
    }

    @Override
    public Path getPath(String name) {
        return getPaths().get(name);
    }

    @Override
    public void removePath(String name) {
        setPath(name, null);
    }

    @Override
    public void setPath(String name, Path value) {
        HashMap<String, Path> map = new HashMap<String, Path>(getPaths());
        if (value == null) {
            map.remove(name);
        } else {
            map.put(name, value);
        }
        setPaths(map);
    }

    @Override
    public void setPaths(Map<String, Path> pathsArg) {
        // use a copy we can modify
        HashMap<String, Path> paths = new HashMap<String, Path>(pathsArg);
        
        Element elemPathList = null;
        try {
            elemPathList = (Element) xpPathList
                .evaluate(doc.getDocumentElement(), XPathConstants.NODE);
        } catch (XPathExpressionException ex) {
        }
        
        // a list of already existing path-elements
        List<Element> existingPathElements;
        if (elemPathList == null) {
            elemPathList = doc.createElementNS(NS_PATCH_METDATA, "path-list");
            doc.getDocumentElement().appendChild(elemPathList);
            existingPathElements = Collections.emptyList();
        } else {
            NodeList childNodes = elemPathList.getChildNodes();
            existingPathElements = new ArrayList<Element>(childNodes.getLength());
            for (int i=0;i<childNodes.getLength();i++) {
                Node item = childNodes.item(i);
                if (item.getNodeType() != Node.ELEMENT_NODE) {
                    continue;
                }
                Element elem = (Element) item;
                if ("path".equals(elem.getNodeName())) {
                    existingPathElements.add(elem);
                } 
            }
        }
        
        for (Element existingPathElement: existingPathElements) {
            String pathName = existingPathElement.getAttribute("name");
            
            if (!paths.containsKey(pathName)) { // delete
                elemPathList.removeChild(existingPathElement);
            } else { // update
                Path path = paths.remove(pathName);
                existingPathElement.setTextContent(path.getPath());
            }
        }
        // add
        for (String pathName: paths.keySet()) {
            Path path = paths.get(pathName);
            Element pathElement = createTextElement(doc, "path", path.getPath());
            pathElement.setAttribute("name", path.getName());
            elemPathList.appendChild(pathElement);
        }
    }
    @Override
    public void setAll(PatchMetadata src) {
        PatchMetadataUtils.setAll(this,src);
    }

    @Override
    public void addPath(Path path) {
        setPath(path.getName(), path);
    }

    @Override
    public void addTag(Tag tag) {
        Tagset t = getTags();
        t.add(tag);
        setTags(t);
    }

    @Override
    public void addTag(String tagname) {
        Tagset t = getTags();
        t.add(tagname);
        setTags(t);
    }
    
    @Override
    public Tagset getTags() {
        String str = getTextContent(xpTags,null);
        if (str == null) {
            return new Tagset();
        } else {
            return Tagset.parseTags(str);
        }
    }

    @Override
    public void setTags(Tagset set) {
        setNode(xpTags, "tags", set.toString());
    }

    @Override
    public String getLastModifiedDateString() {
        return CSUtils.dateToString(getLastModifiedDate());
    }

    @Override
    public Date getLastModifiedDate() {
        String str = getTextContent(xpLastModifiedDate,null);
        if (str == null) {
            return CSUtils.now();
        }
        Date date = CSUtils.parseDate(str);
        if (date == null) {
            date = CSUtils.now();
        }
        return date;
    }

    @Override
    public void setLastModifiedDate(Date d) {
        if (d == null) {
            d = CSUtils.now();
        }
        setNode(xpLastModifiedDate, "last-modified-date", CSUtils.dateToString(d));
    }
    
    @Override
    public String toString() {
        return PatchMetadataUtils.toString(this);
    }

    @Override
    public void setPaths(Collection<Path> paths) {
        PatchMetadataUtils.setPaths(this, paths);
    }

    @Override
    public String getPatchId() {
        String id = getTextContent(xpPatchId, null);
        if (id != null && id.isEmpty()) {
            id = null;
        }
        return id ;
    }

    @Override
    public void setPatchId(String patchId) {
        if (patchId != null && patchId.isEmpty()) {
            patchId = null;
        }
        if (patchId == null) {
            deleteNode(xpPatchId);
        } else {
            setNode(xpPatchId, "patch-id", patchId);
        }
    }
 
}
 