package xeno;

public class Light {
    public int id;
    public Object peer;

    public Light(int n) {
        this.id = n;
        this.peer = null;
    }

    public native void setColor(float var1, float var2, float var3);

    public void setColor(int n, float f, float f2, float f3) {
        this.id = n;
        this.setColor(f, f2, f3);
    }

    public native void setDirection(float var1, float var2, float var3);

    public void setDirection(int n, float f, float f2, float f3) {
        this.id = n;
        this.setDirection(f, f2, f3);
    }

    public native void setDirection2(float var1, float var2, float var3);

    public void setDirection2(int n, float f, float f2, float f3) {
        this.id = n;
        this.setDirection2(f, f2, f3);
    }

    public native void setGlobalPointLightCol(int var1, float var2, float var3, float var4);

    public native void setGlobalPointLightPos(int var1, float var2, float var3, float var4);

    public native void setGlobalPointLightReset();
}

