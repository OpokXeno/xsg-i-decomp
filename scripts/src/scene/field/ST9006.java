import xeno.Camera;
import xeno.Chr;
import xeno.Enepc;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class JumpBattle
        extends Stage
        implements XenoConstants,
        CfConstants {
    Player player;
    Camera cam1;
    Enepc enemy1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;

    JumpBattle() {
    }

    void EOB(int n) {
        System.println("ボス戦闘後処理――勝利");
        System.println("もとにもどるっす。");
        Runtime.setFlags(1021, 1, 1);
        Runtime.setFlags(1022, 1, 0);
        Runtime.charAllRecovery();
        System.println("*********全回復しました**************");
        System.sleep(1);
        Runtime.evsExit();
    }

    void EOB_Always(int n) {
        System.println("ボス戦闘後処理――ALWAYS");
        System.println("もとにもどるっす。");
        Runtime.setFlags(1021, 1, 0);
        Runtime.setFlags(1022, 1, 1);
        Runtime.charAllRecovery();
        System.println("*********全回復しました**************");
        System.sleep(1);
        Runtime.evsExit();
    }

    void Final_init(int n) {
        this.enemy1.kickEnepc(14, 1, 0);
    }

    void init() {
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(-28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(4);
        this.enemy1.setParams(0, 0, 1, 3);
        this.enemy1.setVisible(false);
        this.enemy1.setBatEvent(72);
    }

    class Player
            extends Chr {
        Player() {
        }

        void init() {
            this.setPlayer();
        }
    }
}

