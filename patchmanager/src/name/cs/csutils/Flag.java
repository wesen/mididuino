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
package name.cs.csutils;

import java.io.IOException;
import java.io.Serializable;
import java.util.concurrent.atomic.AtomicBoolean;

public abstract class Flag implements Serializable {

    private static final long serialVersionUID = 2819376108971218264L;

    public final static class LogicOr extends Flag {
        private static final long serialVersionUID = -7474472416832049980L;
        private Flag a;
        private Flag b;        
        private int setindex;
        public LogicOr(Flag a, Flag b, int setindex) {
            this.a = a;
            this.b = b;
            this.setindex = setindex;
            this.init();
        }
        @Override
        public void set(boolean set) {
            if (setindex == 0)
                a.set(set);
            else if (setindex==1)
                b.set(set);
        }
        @Override
        public boolean isset() {
            return a.isset() || b.isset();
        }   
    }
    public final static class Atomic extends Flag {
        /**
         * 
         */
        private static final long serialVersionUID = 2496339285094835722L;
        private AtomicBoolean flag;        
        public Atomic() {
            this(false);
        }

        public Atomic(boolean set) {
            this.flag = new AtomicBoolean(set);
            this.init();
        }

        @Override
        public void set(boolean set) {
            flag.set(set);
        }
        @Override
        public boolean isset() {
            return flag.get();
        }   
    }

    private final static class InverseFlag extends Flag {
        private static final long serialVersionUID = -5601964329071124069L;
        private Flag flag;
        public InverseFlag(Flag flag) {
            this.flag = flag;
            this.init();
        }
        @Override
        public boolean isset() {
            return !flag.isset();
        }

        @Override
        public void set(boolean set) {
            flag.set(!set);
        }

        @Override
        public Flag inverse() {
            return flag;
        }

        @Override
        protected Flag newInverse() {
            return flag;
        }
         
    }

    private transient Flag inverse;
    
    public Flag() {
        this.init();
    }
    
    protected void init() {
        this.inverse = newInverse();
    }
    
    protected Flag newInverse() {
        return new InverseFlag(this);
    }
    
    public void set() {
        set(true);
    }
    
    public abstract void set(boolean set);

    public void unset() {
        set(false); 
    }
    
    public abstract  boolean isset();

    public Flag inverse() {
        return inverse;
    }

    private void writeObject(java.io.ObjectOutputStream out)
        throws IOException {
        out.defaultWriteObject();
    }
    
    private void readObject(java.io.ObjectInputStream in)
         throws IOException, ClassNotFoundException {
        in.defaultReadObject();
        this.init();
    }
}
