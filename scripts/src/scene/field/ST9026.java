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
    int attacker0_bic;
    int attacker0_pos;
    int attacker1_bic;
    int attacker1_pos;
    int attacker2_bic;
    int attacker2_pos;

    JumpBattle() {
    }

    void EOB(int n) {
        System.println("ボス戦闘後後処理");
        if (n == 0) {
            System.println("0に勝利");
        } else if (n == 1) {
            System.println("＊＊＊＊＊バトルメンバーを元に戻します＊＊＊＊＊");
        }
        Runtime.setPartyData(0x1010000, this.attacker0_bic);
        Runtime.setPartyData(65538, this.attacker0_pos);
        Runtime.setPartyData(0x1010004, this.attacker1_bic);
        Runtime.setPartyData(65542, this.attacker1_pos);
        Runtime.setPartyData(0x1010008, this.attacker2_bic);
        Runtime.setPartyData(65546, this.attacker2_pos);
        System.println("1に勝利");
        System.println("ジャンプします");
        Runtime.charAllRecovery();
        System.println("*********全回復しました**************");
        Runtime.jumpCF(1751, 2);
    }

    void Final_init(int n) {
        System.println("バトルメンバーをケイオスのみにします");
        Runtime.setPartyData(0x1010000, 1);
        Runtime.setPartyData(65538, 2);
        Runtime.setPartyData(0x1010004, 0);
        Runtime.setPartyData(65542, 1);
        Runtime.setPartyData(0x1010008, 0);
        Runtime.setPartyData(65546, 3);
        this.enemy1.kickEnepc(14, 1, 0);
    }

    void init() {
        System.println("＊＊＊＊＊現在のバトルメンバーの保存をします＊＊＊＊＊");
        this.attacker0_bic = Runtime.getPartyData(0x1010000);
        this.attacker0_pos = Runtime.getPartyData(65538);
        this.attacker1_bic = Runtime.getPartyData(0x1010004);
        this.attacker1_pos = Runtime.getPartyData(65542);
        this.attacker2_bic = Runtime.getPartyData(0x1010008);
        this.attacker2_pos = Runtime.getPartyData(65546);
        Runtime.setRegister(0, this.attacker0_bic);
        Runtime.setRegister(1, this.attacker0_pos);
        Runtime.setRegister(2, this.attacker1_bic);
        Runtime.setRegister(3, this.attacker1_pos);
        Runtime.setRegister(4, this.attacker2_bic);
        Runtime.setRegister(5, this.attacker2_pos);
        System.println("attacker0_bic: /[$0]");
        System.println("attacker0_pos: /[$1]");
        System.println("attacker1_bic: /[$2]");
        System.println("attacker1_pos: /[$3]");
        System.println("attacker2_bic: /[$4]");
        System.println("attacker2_pos: /[$5]");
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
        this.enemy1.setGroup(1);
        this.enemy1.setParams(0, 0, 1, 3);
        this.enemy1.setVisible(false);
        this.enemy1.setBatEvent(12);
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

