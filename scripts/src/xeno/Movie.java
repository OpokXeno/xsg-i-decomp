package xeno;

public class Movie {
    public int id;
    public int loop;
    public int frame;
    public int transparent;
    public int alpha;

    public void init() {
        this.init(256);
    }

    public native void init(int var1);

    public native void start(int var1);

    public native void stop();

    public native void update();
}

