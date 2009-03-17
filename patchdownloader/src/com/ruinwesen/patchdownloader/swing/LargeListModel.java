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
package com.ruinwesen.patchdownloader.swing;

import java.util.Collection;
import java.util.List;

import javax.swing.AbstractListModel;

public class LargeListModel<E, L extends List<E>> extends AbstractListModel {

    private static final long serialVersionUID = 3945207488598386179L;
    protected L delegate;
    private Tracker tracker;

    protected LargeListModel() {
        this.delegate = null;
        this.tracker = new Tracker(this);
    }
    
    public void setItems(Collection<? extends E> c) {
        tracker.remove(0, delegate.size()-1);
        delegate.clear();
        delegate.addAll(c);
        tracker.add(0, c.size());
    }
    
    public LargeListModel(L list) {
        this.delegate = list;
        this.tracker = new Tracker(this);
    }
    
    @Override
    public E getElementAt(int index) {
        return delegate.get(index);
    }

    @Override
    public int getSize() {
        return delegate.size();
    }
    
    public boolean isEmpty() {
        return delegate.isEmpty();
    }

    public void add(E element) {
        delegate.add(element);
        tracker.add(delegate.size());
    }

    public void add(int index, E element) {
        delegate.add(index, element);
        tracker.add(index);
    }

    public void addAll(int index, Collection<? extends E> c) {
        delegate.addAll(index, c);
        tracker.add(index, c.size()-1);
    }
    
    public void remove(int index) {
        delegate.remove(index);
        tracker.remove(index);
    }
    
    public void removeRange(int fromIndex, int toIndex) {
        if (fromIndex > toIndex) {
            throw new IllegalArgumentException("fromIndex must be <= toIndex");
        }
        for (int i=toIndex;i>=fromIndex;i--) {
            delegate.remove(i);
        }
        tracker.remove(fromIndex, toIndex);
    }

    public void contentsChanged(int index) {
        tracker.change(index);
    }

    public void contentsChanged(int fromIndex, int toIndex) {
        tracker.change(fromIndex, toIndex);
    }
    
    public void fireCachedEvents() {
        tracker.fireEvents();
    }
    
    /**
     * The tracker class keeps track of add,remove and change operations on
     * intervals [fromIndex,toIndex]. In specific cases events may cancel previous
     * events out which are then either removed or their range (interval) 
     * becomes smaller or is split into two intervals.
     * 
     * In the end the tracker can generate the necessary listmodel events
     * but the number of these events is the same or likely much smaller 
     * then the number of events without this tracker class.
     * 
     * The benefit of less list modification events is a more responsive ui
     * since the list component does not have to update and repaint so frequently.
     * 
     * Especially lists with large numbers of elements (1000,10000 or more)
     * will benefit from this.
     */
    private static final class Tracker {
        
        private LargeListModel<?,?> listmodel;
        private int lastListSize;
        private int minChangeIndex;
        private int maxChangeIndex;
        
        public Tracker(LargeListModel<?,?> listmodel) {
            super();
            this.listmodel = listmodel;
            reset();
        }
        
        private void reset() {
            lastListSize = listmodel.getSize();
            minChangeIndex = Integer.MAX_VALUE;
            maxChangeIndex = Integer.MIN_VALUE;
        }

        public void remove(int index) {
            remove(index, index);
        }

        public void add(int index) {
            add(index, index);
        }

        public void change(int index) {
            change(index, index);
        }
        
        private boolean containsChanges() {
            return minChangeIndex<=maxChangeIndex;
        }

        public void change(int fromIndex, int toIndex) {
            minChangeIndex = Math.min(fromIndex, minChangeIndex);
            maxChangeIndex = Math.max(toIndex, maxChangeIndex);
        }

        public void add(int fromIndex, int toIndex) {
            change(fromIndex, toIndex);
        }
        
        public void remove(int fromIndex, int toIndex) {
            change(fromIndex, toIndex);
        }

        public void fireEvents() {
            if (!containsChanges()) {
                // no events to send
                reset();
                return;
            }
            if (listmodel.isEmpty()) {
                if (lastListSize > 0) {
                    // only a single remove interval event
                    listmodel.fireIntervalRemoved(listmodel, 0, lastListSize-1);
                }
                reset();
                return;
            }
            if (listmodel.getSize()<lastListSize) {
                listmodel.fireIntervalRemoved(listmodel, listmodel.getSize(), lastListSize-1);
                if (listmodel.getSize()<=minChangeIndex) {
                    // no change events
                } else { // minChangeIndex<listModel.getSize()
                    maxChangeIndex = Math.min(maxChangeIndex, listmodel.getSize()-1);
                    listmodel.fireContentsChanged(listmodel, minChangeIndex, maxChangeIndex);
                }
                reset();
                return;
            } else if (listmodel.getSize() == lastListSize) {
                listmodel.fireContentsChanged(listmodel, minChangeIndex, maxChangeIndex);
                reset();
                return;
            } else { // listmodel.getSize() > lastListSize
                listmodel.fireIntervalAdded(listmodel, lastListSize, listmodel.getSize()-1);
                if (minChangeIndex>=lastListSize) {
                    // no change events
                } else {
                    maxChangeIndex = Math.min(maxChangeIndex, lastListSize-1);
                    listmodel.fireContentsChanged(listmodel, minChangeIndex, maxChangeIndex);
                }
                reset();
                return;
            }
        }

    }
    
}
