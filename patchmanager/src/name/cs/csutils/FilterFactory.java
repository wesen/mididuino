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

import java.io.File;
import java.io.FileFilter;

public class FilterFactory {

    private static final Filter<Object> TRUE = new Constant(true);
    private static final Filter<Object> FALSE = new Constant(false);
    private static final Filter<Object> TEST_NULL = new TestNull();
    
    @SuppressWarnings("unchecked")
    public static <K> Filter<K> constant(boolean value) {
        Filter<Object> filter = value ? TRUE:FALSE;
        return (Filter<K>) filter;
    }

    public static <K> Filter<K> trueConstant() {
        return constant(true);
    }

    public static <K> Filter<K> falseConstant() {
        return constant(false);
    }

    @SuppressWarnings("unchecked")
    public static <K> Filter<K> isNull() {
        return (Filter<K>) TEST_NULL;
    }

    public Filter<File> mapFromFileFilter(FileFilter filter) {
        if (filter instanceof MapToFileFilter) {
            return ((MapToFileFilter)filter).filter;
        }
        return new MapFromFileFilter(filter);
    }

    public FileFilter mapToFileFilter(Filter<File> filter) {
        if (filter instanceof MapFromFileFilter) {
            return ((MapFromFileFilter)filter).filter;
        }
        return new MapToFileFilter(filter);
    }
    
    public static <K> Filter<K> isNotNull() {
        return negate(FilterFactory.<K>isNull());
    }

    public static <K> Filter<K> and(Filter<K> a, Filter<K> b) {
        return new LogicOp<K>(LogicOp.AND, a, b);
    }

    public static <K> Filter<K> or(Filter<K> a, Filter<K> b) {
        return new LogicOp<K>(LogicOp.OR, a, b);
    }

    public static <K> Filter<K> xor(Filter<K> a, Filter<K> b) {
        return new LogicOp<K>(LogicOp.XOR, a, b);
    }

    public static <K> Filter<K> equivalence(Filter<K> a, Filter<K> b) {
        return new LogicOp<K>(LogicOp.EQUIVALENCE, a, b);
    }

    public static <K> Filter<K> negate(Filter<K> filter) {
        if (filter instanceof NegateFilter) { // double negation
            return ((NegateFilter<K>)filter).filter;
        }
        return new NegateFilter<K>(filter);
    }

    public static Filter<String> hasSuffix(String suffix) {
        if (suffix == null) {
            throw new IllegalArgumentException("suffix==null");
        }
        return new HasSuffix(suffix); 
    }

    public static Filter<String> hasPrefix(String prefix) {
        if (prefix == null) {
            throw new IllegalArgumentException("prefix==null");
        }
        return new HasPrefix(prefix); 
    }

    private static final class TestNull implements Filter<Object> {
        @Override
        public boolean test(Object item) {
            return item==null;
        }        
    }

    private static final class MapFromFileFilter implements Filter<File> {
        private FileFilter filter;
        public MapFromFileFilter(FileFilter filter) {
            this.filter = filter;
        }
        @Override
        public boolean test(File item) {
            return filter.accept(item);
        }
    }

    private static final class MapToFileFilter implements FileFilter {
        private Filter<File> filter;
        public MapToFileFilter(Filter<File> filter) {
            this.filter = filter;
        }
        @Override
        public boolean accept(File pathname) {
            return filter.test(pathname);
        }
    }
    
    private static abstract class UnaryFilter<K> implements Filter<K> {
        protected Filter<K> filter;
        public UnaryFilter(Filter<K> filter) {
            this.filter = filter;
        }
    }

    private static abstract class BinaryFilter<K> implements Filter<K> {
        protected Filter<K> a;
        protected Filter<K> b;
        public BinaryFilter(Filter<K> a, Filter<K> b) {
            this.a = a;
            this.b = b;
        }
    }

    private static class LogicOp<K> extends BinaryFilter<K> {
        public static final int AND = 0;
        public static final int OR = 1;
        public static final int XOR = 2;
        public static final int EQUIVALENCE = 3;
        private int op;
        public LogicOp(int op, Filter<K> a, Filter<K> b) {
            super(a,b);
            this.op = op;
        }

        @Override
        public boolean test(K item) {
            switch (op) {
            // note this is not actually the logic and operation because
            // if a is false then b is not called 
            case AND: return a.test(item) && b.test(item);
            // note this is not actually the logic or operation because
            // if a is true then b is not called 
            case OR: return a.test(item) || b.test(item);
            case XOR: return a.test(item) ^ b.test(item);
            case EQUIVALENCE: return a.test(item) == b.test(item);
            default:
                throw new IllegalStateException("illegal operator: "+op);
            }
        }
    }
    
    
    
    private static final class NegateFilter<K> extends UnaryFilter<K> {
        public NegateFilter(Filter<K> filter) {
            super(filter);
        }
        @Override
        public boolean test(K item) {
            return !filter.test(item);
        }    
    }
    
    private static final class HasSuffix implements Filter<String> {
        private String suffix;
        public HasSuffix(String suffix) {
            this.suffix = suffix;
        }
        @Override
        public boolean test(String item) {
            return item.endsWith(suffix);
        }        
    }
    
    private static final class HasPrefix implements Filter<String> {
        private String prefix;
        public HasPrefix(String prefix) {
            this.prefix = prefix;
        }
        @Override
        public boolean test(String item) {
            return item.startsWith(prefix);
        }        
    }
    
    private static final class Constant implements Filter<Object> {
        private boolean value;
        public Constant(boolean value) {
            this.value = value;
        }
        public boolean test(Object o) {
            return value;
        }
    }
    
}
