/*
 * OV01 original TU 14: 0x00a2e4b0..0x00a31340 (64 functions)
 */
#include "common.h"

/* Debug menu: enters the battle debug interface. */
extern void debugBattle(void);

void debugEntry(void)
{
    debugBattle();
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugWpnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugCmdPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugThinkRegName);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugThinkPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugBattle);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugStatDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugParaDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugSpecDisp);

extern int plMuteki; /* debug menu: player invincibility flag */

int plMutekiGet(void)
{
    return plMuteki;
}

extern int enMuteki; /* debug menu: enemy invincibility flag */

int enMutekiGet(void)
{
    return enMuteki;
}

extern int plIchigeki; /* debug menu: player one-hit-kill flag */

int plIchigekiGet(void)
{
    return plIchigeki;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugItemSet);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugEtherSet);

void debugSkillSet(void) {

}

extern int calcAgwsEquipOrg(int agwsId, int charaId);
extern int calcAttEquipOrg(int charaId, int slot, int attId);
extern int calcWpnEquipOrg(int charaId, int slot, int wpnId, int variant);

/* Debug menu: forces a fixed test loadout onto character 0x1A. */
int debugEquSet(void)
{
    calcAgwsEquipOrg(6, 0x1A);
    calcWpnEquipOrg(0x1A, 0, 0x4E, 0x19);
    calcWpnEquipOrg(0x1A, 1, 0x4E, 0x21);
    calcAttEquipOrg(0x1A, 0, 1);
    return calcAttEquipOrg(0x1A, 1, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugBattleConfig);

int configStart(void) {
    return 1;
}

/* Debug menu: cycles the character assigned to party slot `slot`. */
extern int configPlayer(int slot);

int configPlayer1(void) {
    return configPlayer(0);
}

int configPlayer2(void) {
    return configPlayer(1);
}

int configPlayer3(void) {
    return configPlayer(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configPlayer);

/* Debug menu: cycles the formation position of party slot `slot`. */
extern int configPlPos(int slot);

int configPlPos1(void) {
    return configPlPos(0);
}

int configPlPos2(void) {
    return configPlPos(1);
}

int configPlPos3(void) {
    return configPlPos(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configPlPos);

/* Debug menu left/right adjuster: steps *value by step (d-pad) or by
   10 * step (shoulder buttons), wrapping from past max to min and from below
   min to max. Returns nonzero when *value changed. */
extern int valLR(int *value, int min, int max, int step);

extern void monsSetNoSet(int monsSetNo);
extern int monsSet; /* debug menu's selected monster-set number */

int configMonsSet(void) {
    valLR(&monsSet, 0, 0x63, 1);
    monsSetNoSet(monsSet);
    return 0;
}

extern void mapNoSet(int mapNo);
extern int mapNo; /* debug menu's selected map number */

int configMap(void) {
    valLR(&mapNo, 0, 0x63, 1);
    mapNoSet(mapNo);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configCamera);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configStat);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configEqu);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configGain);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configPlMuteki);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configEnMuteki);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configPlIchigeki);

extern void cfEncountSet(int cfEncount);
extern int cfEncount; /* debug menu's CF encounter value */

int configCFEncount(void) {
    valLR(&cfEncount, 0, 0xFFFF, 1);
    cfEncountSet(cfEncount);
    return 0;
}

extern void cfEventSet(int cfEvent);
extern int cfEvent; /* debug menu's CF event value */

int configCFEvent(void) {
    valLR(&cfEvent, 0, 0xFFFF, 1);
    cfEventSet(cfEvent);
    return 0;
}

extern void thinkNoSet(int thinkNo);
extern int D_00A5AA60; /* debug menu's selected think (AI) number */

int configThink(void) {
    valLR(&D_00A5AA60, 0, 0x63, 1);
    thinkNoSet(D_00A5AA60);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configYadoya);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", valUD);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", valLR);

extern int equipId;

int equipChar(void) {
    valLR(&equipId, 1, 0x20, 1);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipAgws);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipEngine);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipFrame);

/* Debug equip menu: cycles the weapon in equipment slot `slot` of the
   character selected by equipId. */
extern int equipWpn(int slot);

int equipWpn0(void) {
    return equipWpn(0);
}

int equipWpn1(void) {
    return equipWpn(1);
}

int equipWpn2(void) {
    return equipWpn(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipWpn);

/* Debug equip menu: cycles the attachment in equipment slot `slot` of the
   character selected by equipId. */
extern int equipAtt(int slot);

int equipAtt0(void) {
    return equipAtt(0);
}

int equipAtt1(void) {
    return equipAtt(1);
}

int equipAtt2(void) {
    return equipAtt(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipAtt);

/* Debug equip menu: cycles the accessory in equipment slot `slot` of the
   character selected by equipId. */
extern int equipAcc(int slot);

int equipAcc0(void) {
    return equipAcc(0);
}

int equipAcc1(void) {
    return equipAcc(1);
}

int equipAcc2(void) {
    return equipAcc(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipAcc);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipSpec);

extern int equipEther(int firstSlot, int lastSlot);

int equipEther0(void) {
    return equipEther(0, 3);
}

int equipEther1(void) {
    return equipEther(4, 7);
}

int equipEther2(void) {
    return equipEther(8, 0xB);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipEther);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipSkill);
