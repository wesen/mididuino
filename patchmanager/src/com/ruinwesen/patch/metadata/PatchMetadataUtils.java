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
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import name.cs.csutils.CSUtils;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

import com.ruinwesen.patch.PatchDataException;

public final class PatchMetadataUtils {
    
    private PatchMetadataUtils() {
        super();
    }
    
    public static String toXMLString(PatchMetadata metadata) throws PatchDataException {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        writeXML(metadata, os);
        return new String(os.toByteArray());
    }

    public static void writeXML(PatchMetadata metadata, File file) throws PatchDataException {
        try {
            OutputStream out = new BufferedOutputStream(new FileOutputStream(file));
            try {
            writeXML(metadata, out);
            } finally {
                out.flush();
                out.close();
            }
        } catch (IOException ex) {
            throw new PatchDataException(ex);
        }
    }
    
    public static void writeXML(PatchMetadata metadata,
            OutputStream out) throws PatchDataException {
        XMLPatchMetadata xml;
        if (metadata instanceof XMLPatchMetadata) {
            xml = (XMLPatchMetadata) metadata;
        } else {
            xml = new XMLPatchMetadata(metadata);
        }

        try {
            Transformer serializer =
                TransformerFactory.newInstance().newTransformer();
            
            serializer.setOutputProperty(OutputKeys.ENCODING,"UTF-8");
            //serializer.setOutputProperty(OutputKeys.DOCTYPE_SYSTEM,"users.dtd");
            serializer.setOutputProperty(OutputKeys.INDENT,"yes");
            serializer.transform(new DOMSource(xml.getDocument()), new StreamResult(out));
        } catch (TransformerException ex) {
            throw new PatchDataException(ex);
        }
    }

    public static void setAll(PatchMetadata dst, PatchMetadata src) {
        if (dst == src) {
            return;
        }
        dst.setAuthor(src.getAuthor());
        dst.setComment(src.getComment());
        dst.setName(src.getName());
        dst.setTitle(src.getTitle());
        dst.setVersion(src.getVersion());
        dst.setPaths(src.getPaths());
        dst.setEnvironmentId(src.getEnvironmentId());
        dst.setDeviceId(src.getDeviceId());
        dst.setLastModifiedDate(src.getLastModifiedDate());
        dst.setTags(src.getTags());
        dst.setPatchId(src.getPatchId());
        
    }

    static Document parseDocument(File file) throws PatchDataException {
        DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
        docFactory.setNamespaceAware(true);
        try {
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc = docBuilder.parse(file);
            return doc;
        } catch (IOException ex) {
            throw new PatchDataException(ex);
        } catch (ParserConfigurationException ex) {
            throw new PatchDataException(ex);
        } catch (SAXException ex) {
            throw new PatchDataException(ex);
        }
    }

    static Document parseDocument(InputStream in) throws PatchDataException {
        DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
        docFactory.setNamespaceAware(true);
        try {
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc = docBuilder.parse(in);
            return doc;
        } catch (IOException ex) {
            throw new PatchDataException(ex);
        } catch (ParserConfigurationException ex) {
            throw new PatchDataException(ex);
        } catch (SAXException ex) {
            throw new PatchDataException(ex);
        }
    }
    
    public static Document createDocument() throws PatchDataException {
        DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
        docFactory.setNamespaceAware(true);
        
        DocumentBuilder docBuilder;
        try {
            docBuilder = docFactory.newDocumentBuilder();
        } catch (ParserConfigurationException ex) {
            throw new PatchDataException(ex);
        }
        Document doc = docBuilder.newDocument();
        
        doc.setXmlStandalone(true);
        doc.setXmlVersion("1.0");

        Element elemRoot = doc.createElementNS(PatchMetadata.NS_PATCH_METDATA,"patch-metadata");
        elemRoot.setAttribute("version", "1.0");
        doc.appendChild(elemRoot);

        return doc;
    }
    
    public static Document createDocument(PatchMetadata metadata) throws PatchDataException {
        Document doc = createDocument();
        Element elemRoot = doc.getDocumentElement();

        elemRoot.appendChild(createTextElement(doc, "name", nullToEmptyString(metadata.getName())));
        elemRoot.appendChild(createTextElement(doc, "title", nullToEmptyString(metadata.getTitle())));
        elemRoot.appendChild(createTextElement(doc, "author", nullToEmptyString(metadata.getAuthor())));
        elemRoot.appendChild(createTextElement(doc, "comment", nullToEmptyString(metadata.getComment())));
        elemRoot.appendChild(createTextElement(doc, "tags", nullToEmptyString(metadata.getTags())));
        elemRoot.appendChild(createTextElement(doc, "environment-id", nullToEmptyString(metadata.getEnvironmentId())));
        elemRoot.appendChild(createTextElement(doc, "device-id", nullToEmptyString(metadata.getDeviceId())));
        String patchId = metadata.getPatchId();
        if (patchId != null && patchId.length()!=0) {
            elemRoot.appendChild(createTextElement(doc, "patch-id", patchId));
        }
        
        Date date = metadata.getLastModifiedDate();
        if (date == null) {
            date = CSUtils.now();
        }
        elemRoot.appendChild(createTextElement(doc, "last-modified-date", CSUtils.dateToString(date)));
        
        Map<String, Path> paths = metadata.getPaths();
        if (!paths.isEmpty()) {
            Element elemPathList = doc.createElementNS(PatchMetadata.NS_PATCH_METDATA,"path-list");
            elemRoot.appendChild(elemPathList);
            
            for (String name: paths.keySet()) {
                Path path = paths.get(name);
                Element elemPath = createTextElement(doc, "path", path.getPath());
                elemPath.setAttribute("name", name);
                elemPathList.appendChild(elemPath);
            }
        }
        return doc;
    }
    
    private static String nullToEmptyString(Object s) {
        return s != null ? s.toString() : "";
    }
    
    static Element createTextElement(Document doc, String elementName, String textContent) {
        Element elem = doc.createElementNS(PatchMetadata.NS_PATCH_METDATA,elementName);
        elem.appendChild(doc.createTextNode(textContent));
        return elem;
    }
    
    public static String toString(PatchMetadata m) {
        return m.getClass().getName()+"[author='"+m.getAuthor()+"',title='"+m.getTitle()+"',tags='"+m.getTags()+"',...]";
    }

    public static void setPaths(PatchMetadata dst, Collection<Path> paths) {
        Map<String, Path> map = new HashMap<String, Path>();
        for (Path path: paths) {
            map.put(path.getName(), path);
        }
        dst.setPaths(map);
    }

    public static InputStream createXMLInputStream(PatchMetadata meta) throws PatchDataException {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        writeXML(meta, os);
        return new ByteArrayInputStream(os.toByteArray());
    }

}
