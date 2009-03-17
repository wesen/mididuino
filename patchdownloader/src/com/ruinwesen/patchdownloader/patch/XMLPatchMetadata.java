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
package com.ruinwesen.patchdownloader.patch;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import name.cs.csutils.CSUtils;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * Stores patch meta data in a XML document.
 * 
 * For performance reasons do not use this class directly instead for
 * reading and writing from and to a stream/file.
 * 
 * Reading metadata from a file:
 *  PatchMetadata data =
 *      new DefaultPatchMetadata(new XMLPatchMetadata("[the file]"));
 * 
 * Writing metadata to a file:
 *  PatchMetadata data = ...; // the data
 *  new XMLPatchMetadata(data).write("[the file]");
 * 
 * This class allows non-destructive editing of a XML file, unknown
 *  XML elements are kept intact.
 *  PatchMetadata data = ...; // the data
 *  String file = "[the file]"
 *  // read the contents of the XML file 
 *  XMLPatchMetadata edit = new XMLPatchMetadata(file);
 *  // update the XML document
 *  edit.setMetadata(data);
 *  // write the modified XML document
 *  edit.write(file);
 * 
 * @author chresan
 */
public class XMLPatchMetadata implements PatchMetadata {

    private static final String EL_PATCH_METADATA = "patch-metadata";
    private static final String ATT_METADATA_VERSION = "version";
    private static final String ATT_PATH_NAME = "name";
    private static final String EL_TITLE = "title";
    private static final String EL_NAME = "name";
    private static final String EL_AUTHOR = "author";
    private static final String EL_LASTMODIFIED = "lastmodified";
    private static final String EL_COMMENT = "comment";
    private static final String EL_TAGS = "tags";
    private static final String EL_TAG = "tag";
    private static final String EL_PATH = "path";
    
    private static final String VERSION = "1.0";
    private Document doc;

    public XMLPatchMetadata() throws ParserConfigurationException {
        super();
        createEmptyDocument();
    }
    
    public XMLPatchMetadata(PatchMetadata metadata) throws ParserConfigurationException {
        super();
        createEmptyDocument();
        setMetadata(metadata);
    }

    public XMLPatchMetadata(InputStream stream) throws IOException, ParserConfigurationException {
        this(stream, false);
    }
    
    private XMLPatchMetadata(InputStream stream, boolean close) throws IOException, ParserConfigurationException {
        super();
        try {
            this.doc = newDocumentBuilder().parse(stream);
            validateRoot();
        } catch (SAXException ex) {
            throw new IOException("could not read file", ex);
        } finally {
            stream.close();
        }
    }
    
    public XMLPatchMetadata(File file) throws IOException, ParserConfigurationException { 
        this(new BufferedInputStream(new FileInputStream(file)), true);
    }

    public XMLPatchMetadata(String file) throws IOException, ParserConfigurationException { 
        this(new File(file));
    }
    
    public Document getDocument() {
        return doc;
    }
    
    
    private void validateRoot() throws IOException {
        String name = doc.getDocumentElement().getNodeName();
        if (!EL_PATCH_METADATA.equals(name)) {
            throw new IOException("Unknown format (root element:"+name+")");
        }
        String version = getMetadataFormatVersion();
        if (!VERSION.equals(version)) {
            throw new IOException("Unsupported format version: "+version);
        }
    }

    private DocumentBuilder newDocumentBuilder() throws ParserConfigurationException {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        return factory.newDocumentBuilder();
    }

    private void createEmptyDocument() throws ParserConfigurationException {
        this.doc = newDocumentBuilder().newDocument();
        Element root = doc.createElement(EL_PATCH_METADATA);
        root.setAttribute(ATT_METADATA_VERSION, VERSION);
        doc.appendChild(root);
        doc.getDocumentElement().appendChild(doc.createTextNode("\n"));
    }

    public void write(String file) throws IOException, ParserConfigurationException, TransformerException {
        write(new File(file));
    }
    
    public void write(File file) throws IOException, ParserConfigurationException, TransformerException {
        OutputStream out = new BufferedOutputStream(new FileOutputStream(file));
        try {
            write(out);
        } finally {
            out.flush();
            out.close();
        }
    }
    
    public void write(OutputStream out) throws IOException,
        ParserConfigurationException, TransformerException {
        TransformerFactory factory = TransformerFactory.newInstance();
        Transformer transformer = factory.newTransformer();
        transformer.transform(new DOMSource(doc), new StreamResult(out)); 
    }
    
    private void removeAllChildElements(Element parent, String name) {
        NodeList list = parent.getChildNodes();
        for (int i=0;i<list.getLength();i++) {
            Node node = list.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE
                    && name.equals(node.getNodeName())) {
                parent.removeChild(node);
            }
        }
    }

    private Element getChildElement(Element parent, String name) {
        NodeList list = parent.getChildNodes();
        for (int i=0;i<list.getLength();i++) {
            Node node = list.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE
                    && name.equals(node.getNodeName())) {
                return (Element) node;
            }
        }
        return null;
    }

    private Element getChildElementRemoveFollowing(Element parent, String name) {
        Element elem = null;
        NodeList list = parent.getChildNodes();
        for (int i=0;i<list.getLength();i++) {
            Node node = list.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE
                    && name.equals(node.getNodeName())) {
                if (elem == null) {
                    elem = (Element) node;
                } else {
                    parent.removeChild(node);
                }
            }
        }
        return elem;
    }
    
    private void setTag(Element parent, String name, String value) {
        if (value == null) {
            removeAllChildElements(parent, name);
            return;
        }
        Element enew = doc.createElement(name);
        enew.appendChild(doc.createTextNode(value));
        Element elem = getChildElementRemoveFollowing(parent, name);
        if (elem == null) {
            parent.appendChild(enew);
        } else {
            parent.insertBefore(enew, elem);
            parent.removeChild(elem);
        }
    }
    
    private void setTag(String name, String value) {
        setTag(doc.getDocumentElement(), name, value);
    }
    
    private String getTag(Element parent, String name, String defaultValue) {
        Element elem = getChildElement(parent, name);
        String value = elem != null ? elem.getTextContent() : null;
        return value != null ? value : defaultValue;
    }
    
    private String getTag(String name, String defaultValue) {
        return getTag(doc.getDocumentElement(), name, defaultValue);
    }

    private String emptyToNull(String value) {
        return value == null || value.trim().length() == 0 ? null : value;
    }

    @Override
    public String getAuthor() {
        return emptyToNull(getTag(EL_AUTHOR, null));
    }

    @Override
    public String getComment() {
        return emptyToNull(getTag(EL_COMMENT, null));
    }

    @Override
    public Date getLastModifiedDate() {
        String dateStr = emptyToNull(getTag(EL_LASTMODIFIED, null));
        Date date = dateStr == null ? null : CSUtils.parseDate(dateStr);
        return date;
    }

    @Override
    public String getMetadataFormatVersion() {
        return getTag(ATT_METADATA_VERSION, VERSION);
    }

    @Override
    public String getName() {
        return emptyToNull(getTag(EL_NAME, null));
    }

    @Override
    public String getTitle() {
        return emptyToNull(getTag(EL_TITLE, null));
    }

    @Override
    public void setAuthor(String author) {
        setTag(EL_AUTHOR, convertEmptyString(author,""));
    }

    @Override
    public void setComment(String comment) {
        setTag(EL_COMMENT, convertEmptyString(comment,null));
    }

    @Override
    public void setLastModifiedDate(Date date) {
        String dateStr = date == null ? null : CSUtils.dateToString(date);
        if (dateStr == null) {
            dateStr = CSUtils.dateToString(CSUtils.now());
        }
        setTag(EL_LASTMODIFIED, dateStr);
    }

    @Override
    public void setName(String name) {
        setTag(EL_NAME, convertEmptyString(name,""));
    }

    @Override
    public void setTitle(String title) {
        setTag(EL_TITLE, convertEmptyString(title,""));
    }


    private String convertEmptyString(String value, String defaultString) {
        return value == null || value.trim().length() == 0 ? defaultString
                : value;
    }

    @Override
    public void setMetadata(PatchMetadata metadata) {
        if (metadata == this) {
            return;
        }
        setTitle(metadata.getTitle());
        setName(metadata.getName());
        setAuthor(metadata.getAuthor());
        setLastModifiedDate(metadata.getLastModifiedDate());
        setComment(metadata.getComment());
        setTags(metadata.getTags());
        for (Map.Entry<String,String> entry:metadata.getPaths().entrySet()) {
            setPath(entry.getKey(), entry.getValue());
        }
        // we don't take the format version value
    }

    @Override
    public void addAllTags(Tagset tagset) {
        Tagset t = getTags();
        t.addAll(tagset);
        setTags(t);
    }

    @Override
    public Tagset getTags() {
        Tagset set = new Tagset();
        Element elem = getChildElement(doc.getDocumentElement(), EL_TAGS);
        if (elem != null) {
            NodeList list = elem.getChildNodes();
            for (int i=0;i<list.getLength();i++) {
                Node node = list.item(i);
                if (node.getNodeType() == Node.ELEMENT_NODE &&
                        EL_TAG.equals(node.getNodeName())) {
                    String tag = convertEmptyString(((Element)node).getTextContent(), null);
                    if (tag != null) {
                        set.add(tag);
                    }
                }
            }
        }  
        return set;
    }

    @Override
    public void removeAllTags(Tagset tag) {
        Tagset t = getTags();
        t.addAll(tag);
        setTags(t);
    }

    @Override
    public void removeTag(String tag) {
        Tagset t = getTags();
        t.add(tag);
        setTags(t);
    }

    @Override
    public void setTags(Tagset set) {
        Element elem = getChildElementRemoveFollowing(doc.getDocumentElement(), EL_TAGS);
        if (elem != null) {
            NodeList list = elem.getChildNodes();
            for (int i=0;i<list.getLength();i++) {
                elem.removeChild(list.item(i));
            }
            if (set.isEmpty()) {
                return;
            }
        } else {
            if (set.isEmpty()) {
                return;
            }
            elem = doc.createElement(EL_TAGS);
            doc.getDocumentElement().appendChild(elem);
        }
        
        for (Tag t: set) {
            Element et = doc.createElement(EL_TAG);
            et.appendChild(doc.createTextNode(t.value()));
            elem.appendChild(et);
        }
    }

    @Override
    public String getPath(String name) {
        if (name == null) {
            throw new IllegalArgumentException("name must not be null");
        }
        
        return getPaths().get(name);
    }

    
    private transient Map<String,String> cachedPaths = null;
    @Override
    public Map<String, String> getPaths() {
        if (cachedPaths != null) {
            return Collections.unmodifiableMap(new HashMap<String,String>(cachedPaths));
        }
        Map<String,String> paths = new HashMap<String, String>();
        NodeList list = doc.getDocumentElement().getChildNodes();
        for (int i=0;i<list.getLength();i++) {
            Node node = list.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE &&
                    EL_PATH.equals(node.getNodeName())) {
                Element elem = (Element) node;
                String name = getAttr(elem,ATT_PATH_NAME);
                if (name != null) {
                    String path = elem.getTextContent();
                    if (path != null) {
                        paths.put(name, path);
                    } 
                }
            }
        }
        cachedPaths = paths;
        return Collections.unmodifiableMap(new HashMap<String,String>(cachedPaths));
    }

    @Override
    public void setPath(String name, String path) {
        if (name == null) {
            throw new IllegalArgumentException("name must not be null");
        }

        if (cachedPaths != null) {
            if (path == null) {
                cachedPaths.remove(name);
            } else {
                cachedPaths.put(name, path);
            }
        }
        NodeList list = doc.getDocumentElement().getChildNodes();
        for (int i=0;i<list.getLength();i++) {
            Node node = list.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE &&
                    EL_PATH.equals(node.getNodeName())) {
                Element elem = (Element) node;
                
                String attr = getAttr(elem,ATT_PATH_NAME);
                if (attr != null && name.equals(attr)) {
                    elem.setTextContent(path);
                    System.out.print("settextcontent:"+elem.getTextContent());
                    return;
                }
            }
        }
        
        // create new path element
        Element elemPath = doc.createElement(EL_PATH);
        elemPath.setTextContent(path);
        elemPath.setAttribute(ATT_PATH_NAME, name);
        doc.getDocumentElement().appendChild(elemPath);
    }
    
    private String getAttr(Element elem,String name) {
        NamedNodeMap map = elem.getAttributes();
        for (int i=0;i<map.getLength();i++) {
            Node node = map.item(i);
            if (name.equals(node.getNodeName())) {
                return node.getNodeValue();
            }
        }
        return null;
    }
    
     
}
