package xeno;

import xeno.Stage;
import xeno.Unit;
import xeno.util.Toolkit;

public class MAPUnit
        extends Unit {
    public void init(int n) {
        this.id = n;
        this.setAlgorithm(3);
        Toolkit.getPeer(this);
    }

    public void setVisible(boolean bl) {
        Stage.setVisible(this.id, bl);
    }
}

