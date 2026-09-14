package java.lang;

import xeno.util.Format;
import xeno.vm.System;

public class StringBuffer {
    String[] strings = new String[4];
    int count = 0;

    public StringBuffer() {
    }

    public StringBuffer(String string) {
        this.append(string);
    }

    public StringBuffer append(char c) {
        return this.append(Format.toString(c));
    }

    public StringBuffer append(float f) {
        return this.append(Format.toString(f));
    }

    public StringBuffer append(int n) {
        return this.append(Format.toString(n));
    }

    public StringBuffer append(Object object) {
        return this.append(object.toString());
    }

    public StringBuffer append(String string) {
        if (string == null) {
            return this;
        }
        if (this.strings.length == this.count) {
            String[] stringArray = new String[this.strings.length * 2];
            System.arraycopy(this.strings, 0, stringArray, 0, this.strings.length);
            this.strings = stringArray;
        }
        this.strings[this.count++] = string;
        return this;
    }

    public StringBuffer append(boolean bl) {
        return this.append(Format.toString(bl));
    }

    public StringBuffer append(byte[] byArray) {
        return this.append(new String(byArray));
    }

    public void setLength(int n) {
        this.count = 0;
    }

    public String toString() {
        return new String(this.strings, this.count);
    }
}

