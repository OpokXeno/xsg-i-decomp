package xeno.util;

public class Menu {
    public native void addItem(String var1);

    public native void addQuery(String var1);

    public native void addQuery(String[] var1, int var2);

    public static native Menu create();

    public static Menu createYN() {
        Menu menu = Menu.create();
        menu.addItem("はい\nいいえ");
        return menu;
    }

    public native int getSelected();

    public native void setCursor(int var1);

    public native void setLocation(int var1, int var2);

    public native void setVisible(boolean var1);
}

