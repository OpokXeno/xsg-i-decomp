package xeno.util;

import xeno.util.Format;

public class Runtime {
    public static native void AGWSAllRecovery();

    public static native void CaptureEnd();

    public static native void CaptureStart(String var0, int var1);

    public static native boolean addGold(int var0);

    public static native boolean addItem(int var0, int var1);

    public static native boolean addItemWin(int var0, int var1);

    public static native void battleChangeParty(int var0);

    public static native void charAllRecovery();

    public static native int checkFriend(int var0);

    public static native int checkGold();

    public static native int checkItem(int var0, int var1);

    public static native int checkLockParty(int var0);

    public static native int checkOutFriend(int var0);

    public static native int checkTakeAgws(int var0);

    public static native void disable(int var0);

    public static native void enable(int var0);

    public static native void enterShop(int var0);

    public static native void etherTecSet(int var0);

    public static native void evsExit();

    public static native void evsSetRetPoint();

    public static native void execBattle(int var0, int var1);

    public static native int getEntrance();

    public static native int getFlags(int var0, int var1);

    public static native int getGameState();

    public static native String getItemName(int var0, int var1);

    public static native int getLeader();

    public static native int getLocation();

    public static native int getPartyData(int var0);

    public static native int getRegister(int var0);

    public static void jumpCF(int n) {
        Runtime.jumpCF(n, -1);
    }

    public static native void jumpCF(int var0, int var1);

    public static native void jumpEvent(int var0);

    public static int mailArriveCheck(int n) {
        return Runtime.mailFlag(n);
    }

    public static int mailArriveSet(int n) {
        return Runtime.mailFlag(0x4000000 + n);
    }

    public static int mailAttachCheck(int n) {
        return Runtime.mailFlag(0x2000000 + n);
    }

    public static void mailExec() {
        Runtime.mailExec(0);
    }

    public static native void mailExec(int var0);

    public static native int mailFlag(int var0);

    public static int mailGetTotal(int n) {
        return Runtime.mailFlag(0x5000000 + n);
    }

    public static int mailReplyCheck(int n) {
        return Runtime.mailFlag(0x3000000 + n);
    }

    public static int mailViewCheck(int n) {
        return Runtime.mailFlag(0x1000000 + n);
    }

    public static native void minigameExec(int var0);

    public static native void mpeg2(String var0);

    public static native void mpeg2AfterCrossFade(int var0);

    public static native void progressEffect(int var0);

    public static native boolean removeGold(int var0);

    public static native boolean removeItem(int var0, int var1);

    public static native void resetFriend(int var0);

    public static native void resetLockParty(int var0);

    public static native void resetOutFriend(int var0);

    public static native void resetTakeAgws(int var0);

    public static native void setDefocus(int var0, int var1, int[] var2);

    public static native void setDefocusParam(int var0, int var1, int var2);

    public static native void setDefocusQuick(int var0, int var1, int var2, int var3);

    public static void setDirectionalWind(float f, float f2, float f3, float f4) {
        Runtime.setWindParam(2, f, f2, f3, f4);
    }

    public static native void setEventTimer(String var0, int var1);

    public static native void setFlags(int var0, int var1, int var2);

    public static native void setFriend(int var0);

    public static native void setIdLightCol(int var0, int var1, float var2, float var3, float var4);

    public static native void setIdLightDir(int var0, int var1, float var2, float var3, float var4);

    public static native void setIdLightVec(int var0, int var1, float var2, float var3, float var4);

    public static void setLocation(int n) {
        Runtime.setLocation(n, 0);
    }

    public static void setLocation(int n, int n2) {
        Runtime.setLocation(n, -1, n2);
    }

    public static native void setLocation(int var0, int var1, int var2);

    public static native void setLockParty(int var0);

    public static native void setMap(int var0, int var1);

    public static native void setMenuLock();

    public static native void setOutFriend(int var0);

    public static native void setPartyData(int var0, int var1);

    public static native void setPlayerControl(boolean var0);

    public static native void setPlayerMoveParam(float var0, float var1, float var2);

    public static void setPointWind(float f, float f2, float f3, float f4) {
        Runtime.setWindParam(3, f, f2, f3, f4);
    }

    public static void setRegister(int n, float f) {
        Runtime.setRegister(n, Format.floatToIntBits(f));
    }

    public static native void setRegister(int var0, int var1);

    public static native void setShootFlag(boolean var0);

    public static native void setShootHeightCheck(boolean var0);

    public static native void setShootIDCheck(boolean var0);

    public static native void setShootRange(float var0);

    public static native void setShootUwaCheck(boolean var0);

    public static native void setTakeAgws(int var0);

    public static native void setWindParam(int var0, float var1, float var2, float var3, float var4);

    public static void setWindShake(float f, float f2) {
        Runtime.setWindParam(1, f, f2, 0.0f, 0.0f);
    }

    public static void stopWind() {
        Runtime.setWindParam(0, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

