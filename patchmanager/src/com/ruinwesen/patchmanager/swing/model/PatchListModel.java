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
package com.ruinwesen.patchmanager.swing.model;

import java.awt.EventQueue;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

import javax.swing.AbstractListModel;
import javax.swing.ListModel;
import javax.swing.SwingUtilities;

import name.cs.csutils.collector.Collector;

import com.ruinwesen.patch.Patch;

public class PatchListModel<K extends Patch> extends AbstractListModel implements ListModel, Collector<K> {

    /**
     * 
     */
    private static final long serialVersionUID = -2640916455139766635L;
    private Comparator<K> comparator;
    private boolean elementsInOrder = true;
    private List<K> elements = new ArrayList<K>();
    private List<K> collectedElements = new LinkedList<K>();
    private final Object COLLECTING_LOCK = new Object();
    
    public PatchListModel(Comparator<K> comparator) {
        super();
        setOrder(comparator);
    }

    public synchronized void setOrder(Comparator<K> comparator) {
        if (comparator == null) {
            throw new IllegalArgumentException("comparator==null");
        }
        if (this.comparator != comparator) {
            this.comparator = comparator;
            this.elementsInOrder = false;
        }
    }

    @Override
    public synchronized Object getElementAt(int index) {
        return elements.get(index);
    }

    @Override
    public synchronized int getSize() {
        return elements.size();
    }

    @Override
    public int collect(K item) {
        add(item);
        return Collector.ACCEPTED_MORE;
    }

    public void add(K item) {
        synchronized (COLLECTING_LOCK) {
            collectedElements.add(item);
        }
        notifyElementsAdded();
    }

    private void notifyElementsAdded() {
        int size = getSize();
        if ((size == 20) || (size >0 && (size % 1000==0))) {
            update();
        }
    }

    public void setList(Collection<K> c) {
        clear();
        addAll(c);
        update();
    }

    public void addAll(Collection<K> c) {
        synchronized (COLLECTING_LOCK) {
            collectedElements.addAll(c);
        }
        notifyElementsAdded();
    }

    public synchronized void clear() {
        synchronized (COLLECTING_LOCK) {
            int size = elements.size();
            elements.clear();
            collectedElements.clear();
            if (size>0) {
                fireIntervalRemoved(this, 0, size-1);
            }
        }
    }
    
    public synchronized void update() {
        // get collected
        List<K> collected = Collections.emptyList();
        synchronized (COLLECTING_LOCK) {
            if (!collectedElements.isEmpty()) {
                collected = collectedElements;
                collectedElements = new LinkedList<K>();
            }
        }
        
        if (!elementsInOrder) {
            elements.addAll(collected);
            Collections.sort(elements, comparator);
            elementsInOrder = true;
            fireIntervalAdded(this, 0, elements.size());
        } else { // elements are in order: use insertion sort
            if (collected.isEmpty()) { 
                return; // nothing to insert
            }

            // user array for faster sorting
            collected = new ArrayList<K>(collected);
            // sort list of elements we are going to insert
            Collections.sort(collected, comparator);
            
            // now both lists are sorted - we can use insertion sort
            ListIterator<K> idst = elements.listIterator();
            ListIterator<K> isrc = collected.listIterator();

            int minChangeIndex = -1;
            int maxChangeIndex = -1;
            
            while (isrc.hasNext()) {
                K ksrc = isrc.next();
                
                while (idst.hasNext()) {
                    K kdst = idst.next();
                    int cmp = comparator.compare(ksrc, kdst);
                    if (cmp<0) { // add before
                        minChangeIndex = idst.previousIndex();
                        idst.previous();
                        idst.add(ksrc);
                        ksrc = null;
                        break;
                    }
                }
                
                if (ksrc != null) { // indicates the element was not inserted yet
                    // inserting completed, remaining elements
                    // can be appended to the dst-list (including ksrc)
                    int fromIndex = isrc.nextIndex()-1;
                    elements.addAll(collected.subList(fromIndex, collected.size()));
                    maxChangeIndex = elements.size()-1;
                    break;
                }
                
            }
            
            if (maxChangeIndex < 0) {
                maxChangeIndex = minChangeIndex;
            }
            if (minChangeIndex>=0) {
                fireIntervalAdded(this, minChangeIndex, maxChangeIndex);
            }
        }
    }

    @Override
    protected void fireIntervalRemoved(Object source, int index0, int index1) {
        fireIntervalChanged(source, index0, index1, false);
    }

    @Override
    protected void fireIntervalAdded(Object source, int index0, int index1) {
        fireIntervalChanged(source, index0, index1, true);
    }

    private final void fireIntervalChanged(Object source, int index0, int index1, boolean added) {
        if (EventQueue.isDispatchThread()) {
            if (added) {
                super.fireIntervalAdded(source, index0, index1);
            } else {
                super.fireIntervalRemoved(source, index0, index1);
            }
        } else {
            SwingUtilities.invokeLater(new FireIntervalChangeEvent(source, index0, index1, added));
        }
        
    }
    
    private final class FireIntervalChangeEvent implements Runnable {
        private Object source;
        private int index0; 
        private int index1;
        private boolean added;
        public FireIntervalChangeEvent(Object source, int index0, int index1, boolean added) {
            this.source = source;
            this.index0 = index0;
            this.index1 = index1;
            this.added = added;
        }

        public void run() {
            if (added) {
                PatchListModel.super.fireIntervalAdded(source, index0, index1);
            } else {
                PatchListModel.super.fireIntervalRemoved(source, index0, index1);
            }
        }
    }
    
}
