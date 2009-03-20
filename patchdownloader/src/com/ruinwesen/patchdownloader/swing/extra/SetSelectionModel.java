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
package com.ruinwesen.patchdownloader.swing.extra;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.TreeSet;

import javax.swing.JList;
import javax.swing.ListSelectionModel;
import javax.swing.event.EventListenerList;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

public class SetSelectionModel implements ListSelectionModel {

    protected EventListenerList listenerList = new EventListenerList();
    private TreeSet<Integer> selection = new TreeSet<Integer>();
    private boolean flagValueIsAdjusting = false;
    private int specialIndex = -1;
    
    private SetSelectionEventHandler eventHandler = new SetSelectionEventHandler();
    private JList listView;

    public SetSelectionModel(JList listView) {
        this.listView = listView;
        installEventHandlers();
    }

    protected void notifySelectionChanged(int index) {
        // no op
    }
    
    protected void fireValueChanged(int firstIndex, int lastIndex, boolean isAdjusting) {
        Object[] listeners = listenerList.getListenerList();
        ListSelectionEvent e = null;
        for (int i = listeners.length - 2; i >= 0; i -= 2) {
            if (listeners[i] == ListSelectionListener.class) {
                if (e == null) {
                    e = new ListSelectionEvent(this, firstIndex, lastIndex, isAdjusting);
                }
                ((ListSelectionListener)listeners[i+1]).valueChanged(e);
            }
        }
    }
    
    protected void fireValueChanged(int index) {
        fireValueChanged(index, index, flagValueIsAdjusting);
    }

    public void setAdd(int index) {
        if (index >= 0 && index < listView.getModel().getSize()
                && !selection.contains(index) && selection.add(index)) {
            if (specialIndex != -1) {
                int min = Math.min(specialIndex, index);
                int max = Math.max(specialIndex, index);
                specialIndex = index;
                fireValueChanged(min, max, flagValueIsAdjusting);
            } else  {
                specialIndex = index;
                fireValueChanged(index);
            }
            notifySelectionChanged(index);
        }
    }

    public void setRemove(int index) {
        if (index >= 0 && index < listView.getModel().getSize()
                && selection.contains(index) && selection.remove(index)) {
            fireValueChanged(index);
            notifySelectionChanged(index);
        }
    }

    public void setSwap(int index) {
        if (selection.contains(index)) {
            setRemove(index);
        } else {
            setAdd(index);
        }
    }
    
    protected void installEventHandlers() {
        if (listView != null) {
            listView.addMouseListener(eventHandler);
            listView.addKeyListener(eventHandler);
        }
    }

    public void uninstall() {
        if (listView != null) {
            listView.removeMouseListener(eventHandler);
            listView.removeKeyListener(eventHandler);
        }
    }

    private class SetSelectionEventHandler extends MouseAdapter implements KeyListener {
        @Override
        public void mousePressed(MouseEvent e) {
            setSwap(listView.locationToIndex(e.getPoint()));
        }

        @Override
        public void keyPressed(KeyEvent e) {
            if (e.getKeyCode() == KeyEvent.VK_SPACE) {
                setSwap(specialIndex);
            } else if (e.getKeyCode() == KeyEvent.VK_DOWN) {
                moveSpecialIndex(+1);
            } else if (e.getKeyCode() == KeyEvent.VK_UP) {
                moveSpecialIndex(-1);
            }
        }

        @Override
        public void keyReleased(KeyEvent e) {
            // no op
        }

        @Override
        public void keyTyped(KeyEvent e) {
            // no op
        }
    }
    
    @Override
    public void addListSelectionListener(ListSelectionListener l) {
        listenerList.add(ListSelectionListener.class, l);
    }

    @Override
    public void removeListSelectionListener(ListSelectionListener l) {
        listenerList.remove(ListSelectionListener.class, l);
    }

    @Override
    public int getMaxSelectionIndex() {
        return selection.isEmpty() ? -1 : selection.last();
    }

    @Override
    public int getMinSelectionIndex() {
        return selection.isEmpty() ? -1 : selection.first();
    }

    @Override
    public int getSelectionMode() {
        return MULTIPLE_INTERVAL_SELECTION;
    }

    @Override
    public boolean isSelectionEmpty() {
        return selection.isEmpty();
    }

    @Override
    public boolean getValueIsAdjusting() {
        return flagValueIsAdjusting;
    }
    
    @Override
    public void setValueIsAdjusting(boolean valueIsAdjusting) {
        this.flagValueIsAdjusting = valueIsAdjusting;
    }
    
    @Override
    public void clearSelection() {
        selection.clear();
        specialIndex = -1;
    }

    @Override
    public int getAnchorSelectionIndex() {
        return specialIndex;
    }

    @Override
    public int getLeadSelectionIndex() {
        return getSpecialIndex();
    }

    private void moveSpecialIndex(int inc) {
        if (specialIndex == -1) {
            return;
        }
        int oldIndex = specialIndex;
        specialIndex = specialIndex+inc;

        if (oldIndex >= 0 && oldIndex < listView.getModel().getSize())
            fireValueChanged(oldIndex);
        if (oldIndex != specialIndex && specialIndex >= 0 
                && specialIndex < listView.getModel().getSize())
            fireValueChanged(specialIndex);
    }
    
    private int getSpecialIndex() {
        if (specialIndex>=listView.getModel().getSize()) {
            specialIndex = -1;
        }
        return specialIndex;
    }

    @Override
    public void setAnchorSelectionIndex(int index) {
        // no op
    }

    @Override
    public void setLeadSelectionIndex(int index) {
        // no op
    }

    @Override
    public boolean isSelectedIndex(int index) {
        return selection.contains(index);
    }

    @Override
    public void setSelectionMode(int selectionMode) {
        // no op
    }

    @Override
    public void addSelectionInterval(int index0, int index1) {
        // no op
    }


    @Override
    public void insertIndexInterval(int index, int length, boolean before) {
        // no op
    }

    @Override
    public void removeIndexInterval(int index0, int index1) {
        // no op
    }

    @Override
    public void removeSelectionInterval(int index0, int index1) {
        // no op
    }

    @Override
    public void setSelectionInterval(int index0, int index1) {
        // no op
    }

}