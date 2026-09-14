package java.lang;

import xeno.util.Format;
import xeno.vm.System;

public class String {
    byte[] chars;

    public String() {
        this.chars = new byte[0];
    }

    public String(String string) {
        this.chars = string.chars;
    }

    public String(byte[] byArray) {
        this(byArray, 0, byArray.length);
    }

    public String(byte[] byArray, int n, int n2) {
        byte[] byArray2 = new byte[n2];
        System.arraycopy(byArray, n, byArray2, 0, n2);
        this.chars = byArray2;
    }

    protected String(String[] stringArray, int n) {
        int n2 = 0;
        int n3 = 0;
        while (n3 < n) {
            n2 += stringArray[n3].chars.length;
            ++n3;
        }
        byte[] byArray = new byte[n2];
        int n4 = 0;
        int n5 = 0;
        while (n5 < n) {
            int n6 = stringArray[n5].chars.length;
            System.arraycopy(stringArray[n5].chars, 0, byArray, n4, n6);
            n4 += n6;
            ++n5;
        }
        this.chars = byArray;
    }

    public char charAt(int n) {
        return (char) this.chars[n];
    }

    public String concat(String string) {
        if (string == null || string.chars.length == 0) {
            return this;
        }
        return this + string;
    }

    public boolean equals(Object object) {
        if (object instanceof String) {
            String string = (String) object;
            if (this.chars.length != string.chars.length) {
                return false;
            }
            int n = 0;
            while (n < this.chars.length) {
                if (this.chars[n] != string.chars[n]) {
                    return false;
                }
                ++n;
            }
        }
        return true;
    }

    public int length() {
        return this.chars.length;
    }

    public String substring(int n, int n2) {
        return new String(this.chars, n, n2 - n);
    }

    public byte[] toByteArray() {
        int n = this.length();
        byte[] byArray = new byte[n];
        System.arraycopy(this.chars, 0, byArray, 0, n);
        return byArray;
    }

    public String toString() {
        return this;
    }

    public static String valueOf(char c) {
        return Format.toString(c);
    }

    public static String valueOf(float f) {
        return Format.toString(f);
    }

    public static String valueOf(int n) {
        return Format.toString(n);
    }

    public static String valueOf(Object object) {
        if (object == null) {
            return "null";
        }
        if (object instanceof String) {
            return (String) object;
        }
        return "?";
    }

    public static String valueOf(boolean bl) {
        return Format.toString(bl);
    }
}

