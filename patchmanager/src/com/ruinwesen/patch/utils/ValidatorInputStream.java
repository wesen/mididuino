package com.ruinwesen.patch.utils;

import java.io.IOException;
import java.io.InputStream;

public class ValidatorInputStream extends InputStream {

    private InputStream is;
    private Validator validator;

    public ValidatorInputStream(InputStream is, Validator v) {
        this.is = is;
        this.validator =v;
    }
    
    @Override
    public int read() throws IOException {
        int r = is.read();
        validator.update(r);
        return r;
    }

}
