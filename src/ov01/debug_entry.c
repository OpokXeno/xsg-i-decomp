/*
 * OV01 original TU 14: 0x00a2e4b0..0x00a31340 (64 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/battle_init.h"
#include "main/act_1.h"
#include "ov01/calc.h"
#include "debug_entry_data.h"

/*
 * Debug menu entry: page 0, 1 or 2 selects the status, parameter or spec
 * display.
 */
extern void debugBattle(int page);

void debugEntry(int page)
{
    debugBattle(page);
}

const char *debugNameGet(int charaId) {
    if (charaId < DEBUG_NAME_COUNT) {
        return nameTbl[charaId];
    }
    return err;
}

const char *debugWpnGet(int wpnId) {
    if (wpnId < DEBUG_WPN_COUNT) {
        return wpnTbl[wpnId];
    }
    return err;
}

extern int printf(const char *format, ...);
extern int *unitCmdPtrGet(BattleUnit *unit);
extern int frameCntGet(void);
extern unsigned char *dataItmGet(int index);
extern int dataTecGet(int tecId);

extern const char debugEntryAPTidParaText[];
extern const char debugEntryAPText[];
extern const char debugEntryAPPosText[];
extern const char debugEntryAPMotText[];
extern const char debugEntryAPTidTgtText[];
extern const char debugEntryALLText[];
extern const char debugEntryJpText[];
extern const char debugEntryAPPosIdText[];
extern const char debugEntryAPERRORText[];

/* Prints one command-trace line for unit, with the format selected by cmd. */
void debugCmdPrint(BattleUnit *unit, int cmd)
{
    int *cmdRec;
    int tid;
    BattleUnit *tgt;
    int para;
    unsigned char flag;

    cmdRec = unitCmdPtrGet(unit);
    tid = cmdRec[1];
    tgt = (BattleUnit *)cmdRec[2];
    para = cmdRec[3];
    switch (cmd) {
    case 1:
    case 2:
    case 3:
        printf(debugEntryAPTidParaText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd], tid, para);
        break;
    case 15: case 17: case 19: case 21: case 23:
    case 25: case 27: case 29: case 31: case 33:
        printf(debugEntryAPText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd]);
        break;
    case 5: case 6: case 16: case 18: case 20: case 22:
    case 24: case 26: case 28: case 30: case 32: case 34:
        printf(debugEntryAPPosText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd], tid);
        break;
    case 13:
        printf(debugEntryAPMotText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd], tid);
        break;
    case 4: case 8: case 9: case 10: case 11: case 12:
    case 37: case 38: case 39: case 40: case 41: case 42:
    case 44: case 45: case 46: case 47: case 48:
        printf(debugEntryAPText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd]);
        break;
    case 14:
        if (cmdRec[4] == 3) {
            flag = dataItmGet(para)[8];
        } else {
            flag = *(unsigned char *)dataTecGet(tid);
        }
        printf(debugEntryAPTidTgtText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd], tid, unitNoGet(tgt),
               debugNameGet(calcUPGet(tgt)->charaId), (flag & 0x10) ? debugEntryALLText : debugEntryJpText);
        break;
    case 43:
        printf(debugEntryAPPosIdText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
               calcUPGet(unit)->dblActKind, cmdStrTbl[cmd], tid, cmdRec[2]);
        break;
    default:
        do {
            printf(debugEntryAPERRORText, frameCntGet(), unitNoGet(unit), debugNameGet(calcUPGet(unit)->charaId),
                   calcUPGet(unit)->dblActKind, cmdRec[0]);
        } while (0);
        break;
    }
}

#define DEBUG_THINK_REG_VALID 0x8000
#define DEBUG_THINK_REG_TEXT 0x4000
#define DEBUG_THINK_REG_INDEX_MASK 0x3FFF
#define DEBUG_THINK_NUMERIC_LIMIT 16
#define CMD_STR_TBL_REG_NAMES 0x28

/*
 * Returns the printable name of a think register code, or an empty string
 * when the code is not valid.
 */
char *debugThinkRegName(int reg_code)
{
    int reg_index;

    reg_index = reg_code & DEBUG_THINK_REG_INDEX_MASK;
    if (reg_code & DEBUG_THINK_REG_VALID) {
        if (reg_code & DEBUG_THINK_REG_TEXT) {
            if (reg_index < DEBUG_THINK_NUMERIC_LIMIT) {
                sprintf(str_0, debugEntryRText, reg_index);
            } else {
                sprintf(str_0, debugEntryRText2, cmdStrTbl[CMD_STR_TBL_REG_NAMES + reg_index]);
            }
        } else {
            if (reg_index < DEBUG_THINK_NUMERIC_LIMIT) {
                sprintf(str_0, debugEntryRText3, reg_index);
            } else {
                sprintf(str_0, debugEntryRText4, cmdStrTbl[CMD_STR_TBL_REG_NAMES + reg_index]);
            }
        }
    } else {
        str_0[0] = '\0';
    }

    return str_0;
}

extern short thinkRegNo(short pc);
extern short thinkRegGet(unsigned short regIndex);
extern short cmdNum(short pc);
static const char *thinkStrTbl[99];
static const char *thinkExpTbl[6];
extern const char debugEntryJpText2[];
extern const char debugEntryHText[];
extern const char debugEntryHText2[];
extern const char debugEntryHText3[];
extern const char debugEntryJpText3[];
extern const char debugEntryJpText4[];
extern const char debugEntryJpText5[];
extern const char debugEntryJpText6[];
extern const char debugEntryJpText7[];
extern const char debugEntryJpText8[];

/* Prints one think command at pc, selected by cmd, with its register and operand values. */
void debugThinkPrint(int cmd, short pc, short jmp)
{
    int regCode, a, b, c;
    char *name;

    printf(debugEntryJpText2, pc, thinkStrTbl[cmd]);
    switch (cmd) {
    case 1:
    case 2:
        printf(debugEntryHText, jmp);
        break;
    case 4:
        pc += 2;
        regCode = cmdNum(pc);
        pc += 2;
        b = cmdNum(pc);
        c = cmdNum(pc + 2);
        printf(debugEntryHText2, regCode, thinkExpTbl[b], c, jmp);
        break;
    case 5:
    case 6:
        a = cmdNum(pc + 2);
        printf(debugEntryHText3, a, jmp);
        break;
    case 7:
        pc += 2;
        regCode = thinkRegNo(pc);
        a = cmdNum(pc + 2);
        name = debugThinkRegName(regCode);
        printf(debugEntryJpText3, name, a);
        break;
    case 8:
    case 9:
    case 18:
    case 19:
        pc += 2;
        printf(debugEntryJpText4, debugThinkRegName(thinkRegNo(pc)), cmdNum(pc));
        break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 20:
        pc += 2;
        regCode = thinkRegNo(pc);
        pc += 2;
        a = cmdNum(pc);
        jmp = cmdNum(pc - 2);
        name = debugThinkRegName(regCode);
        printf(debugEntryJpText5, name, a, jmp);
        break;
    case 21:
        a = cmdNum(pc + 2);
        printf(debugEntryJpText6, a);
        break;
    case 22:
        regCode = thinkRegNo(pc + 2);
        jmp = thinkRegGet(regCode);
        name = debugThinkRegName(regCode);
        printf(debugEntryJpText4, name, jmp);
        break;
    case 23:
    case 24:
        pc += 2;
        regCode = thinkRegNo(pc);
        pc += 2;
        a = cmdNum(pc);
        b = cmdNum(pc + 2);
        jmp = thinkRegGet(regCode);
        name = debugThinkRegName(regCode);
        printf(debugEntryJpText7, name, a, b, jmp);
        break;
    default:
        printf(debugEntryJpText8);
        regCode = 0;
        break;
    }
}

extern int debugStatDisp(void);
extern int debugSpecDisp(void);
extern int debugParaDisp(void);

void debugBattle(int page)
{
    if (page == 0) {
        debugStatDisp();
    } else if (page == 1) {
        debugParaDisp();
    } else if (page == 2) {
        debugSpecDisp();
    }
}

extern const char debugEntryUNITSTATText[];
extern const char debugEntrySTText[];
extern const char debugEntryJpText9[];
extern const char debugEntrySTAText[];
extern const char debugEntrySTPText[];
extern const char debugEntrySTTText[];
extern const char debugEntryJpText10[];

/* Prints a status dump for every non-null battle unit. */
int debugStatDisp(void)
{
    BattleUnit **units;
    CalcUnitParam *up;
    unsigned char *base;
    unsigned char *p;
    int count;
    int i;
    int j;
    int k;

    count = unitTblGet(2, &units);
    printf(debugEntryUNITSTATText);
    for (i = 0; i < count; i++) {
        if (units[i] != 0) {
            up = calcUPGet(units[i]);
            base = (unsigned char *)up + 0xC;
            printf(debugEntryLVHPEPText, unitNoGet(units[i]), debugNameGet(up->charaId),
                   up->level, (short)up->currentHp, up->maxHp, (short)up->currentEp, up->maxEp);
            printf(debugEntrySTText);
            for (j = 0; j < 8; j++) {
                printf(debugEntryJpText9, up->statActiveMask[j]);
            }
            printf(debugEntryJpText8);
            printf(debugEntrySTAText);
            for (j = 0; j < 8; j++) {
                printf(debugEntryJpText9, up->statAltMask[j]);
            }
            printf(debugEntryJpText8);
            printf(debugEntrySTPText);
            for (j = 0; j < 8; j++) {
                printf(debugEntryJpText9, up->statProtect[j]);
            }
            printf(debugEntryJpText8);
            printf(debugEntrySTTText);
            for (j = 0; j < 8; j++) {
                p = base + j * 16 + 0xC0;
                for (k = 0; k < 16; k++) {
                    if (*p != 0) {
                        printf(debugEntryJpText10, *p);
                    }
                    p++;
                }
            }
            printf(debugEntryJpText8);
        }
    }
    return 1;
}

/* Prints a parameter dump for every non-null battle unit. */
int debugParaDisp(void)
{
    BattleUnit **units;
    CalcUnitParam *up;
    int count;
    int i;

    count = unitTblGet(2, &units);
    printf(debugEntryUNITPARAText);
    for (i = 0; i < count; i++) {
        if (units[i] != 0) {
            up = calcUPGet(units[i]);
            printf(debugEntryLVHPEPText, unitNoGet(units[i]), debugNameGet(up->charaId),
                   up->level, (short)up->currentHp, up->maxHp, (short)up->currentEp, up->maxEp);
            printf(debugEntrySTRVITEATKEDEFHITEVAText, up->baseAtk, up->phyDefense, up->magAttack, up->magDefense,
                   (signed char)up->hitStat, (signed char)up->evadeStat);
            printf(debugEntryAGLBCBPMVTYPECRATEText, up->agility, up->bcValue, up->bpGauge, up->moveKindId, up->counterRate);
            printf(debugEntryWEAKSPECINFOOBJSTATText, up->conditions, up->specDef, up->flags, units[i]->work->flags);
            printf(debugEntryWCTAGLPLUSETHERDISWPNATKADJText, up->waitCount, up->waitCountBase, up->turnBias,
                   up->etherDisableFlags, up->wpnAtkAdj);
        }
    }
    return 1;
}

typedef struct DebugSpecSlot {
    short id;
    short spd;
    short pow;
    short agl;
    short use;
    short kill;
} DebugSpecSlot;

extern char *dataPlChaGet(int cid);
extern const char debugEntrySPECPARAText[];
extern const char debugEntryTidSpdPowAglUseKillText[];

/* Prints the special-slot table of every non-null battle unit whose work flags lack 0x40. */
int debugSpecDisp(void)
{
    BattleUnit **units;
    CalcUnitParam *up;
    DebugSpecSlot *sp;
    int count;
    int i;
    int j;

    count = unitTblGet(2, &units);
    printf(debugEntrySPECPARAText);
    for (i = 0; i < count; i++) {
        if (units[i] != 0 && !(units[i]->work->flags & 0x40)) {
            up = calcUPGet(units[i]);
            sp = (DebugSpecSlot *)(dataPlChaGet(up->charaId) + 0x48);
            printf(debugEntryLVHPEPText, unitNoGet(units[i]), debugNameGet(up->charaId),
                   up->level, (short)up->currentHp, up->maxHp, (short)up->currentEp, up->maxEp);
            for (j = 0; j < 8; j++) {
                printf(debugEntryTidSpdPowAglUseKillText, j, sp->id, sp->spd, sp->pow, sp->agl, sp->use, sp->kill);
                sp++;
            }
        }
    }
    return 1;
}

int plMutekiGet(void)
{
    return plMuteki;
}

int enMutekiGet(void)
{
    return enMuteki;
}

int plIchigekiGet(void)
{
    return plIchigeki;
}

extern int dataItmBoxInc(int id);

void debugItemSet(void)
{
    int pass;
    int item_id;

    for (pass = 0; pass < 10; pass++) {
        for (item_id = 1; item_id < 36; item_id++) {
            dataItmBoxInc(item_id);
        }
    }
}

extern int calcEtherEquipOrg(int charaId, int slot, int etherId);

void debugEtherSet(void)
{
    int slot;

    for (slot = 0; slot < 12; slot++) {
        calcEtherEquipOrg(3, slot, slot + 1);
    }
    for (slot = 0; slot < 10; slot++) {
        calcEtherEquipOrg(1, slot, slot + 0x1B);
    }
    for (slot = 0; slot < 10; slot++) {
        calcEtherEquipOrg(2, slot, slot + 0x11);
    }
    for (slot = 0; slot < 12; slot++) {
        calcEtherEquipOrg(6, slot, slot + 0x25);
    }
    for (slot = 0; slot < 10; slot++) {
        calcEtherEquipOrg(7, slot, slot + 0x31);
    }
    for (slot = 0; slot < 8; slot++) {
        calcEtherEquipOrg(5, slot, slot + 0x48);
    }
}

void debugSkillSet(void) {

}

extern int calcAgwsEquipOrg(int agwsId, int charaId);
extern int calcAttEquipOrg(int charaId, int slot, int attId);
extern int calcWpnEquipOrg(int charaId, int slot, int wpnId, int variant);

/* Debug menu: forces a fixed test loadout onto DEBUG_EQU_TEST_CHARA. */
#define DEBUG_EQU_TEST_CHARA 0x1A

int debugEquSet(void)
{
    calcAgwsEquipOrg(6, DEBUG_EQU_TEST_CHARA);
    calcWpnEquipOrg(DEBUG_EQU_TEST_CHARA, 0, 0x4E, 0x19);
    calcWpnEquipOrg(DEBUG_EQU_TEST_CHARA, 1, 0x4E, 0x21);
    calcAttEquipOrg(DEBUG_EQU_TEST_CHARA, 0, 1);
    return calcAttEquipOrg(DEBUG_EQU_TEST_CHARA, 1, 1);
}

/*
 * plUnitTblGet returns the base of the party roster. configPlayer cycles the
 * player field (+0x00) over 0..0x1F and configPlPos cycles the position
 * field (+0x02) over 1..9. plUnitTblSet writes both fields back for one slot.
 */
typedef struct DebugPartyUnit {
    short player;   /* +0x00 */
    short position; /* +0x02 */
    unsigned char unmodeled_04[4];
} DebugPartyUnit;

extern DebugPartyUnit *plUnitTblGet(void);
extern void plUnitTblSet(int slot, int player, int position);
extern int valLR(int *value, int min, int max, int step);

/*
 * dataUnitOrgGet returns character chrNo's original stat record;
 * calcTotalParaMenu returns a recalculated copy of the same record. The
 * equip screens below index its agws/engine/frame/accessory/attachment/
 * special/ether/skill ids by the character calcXxxEquipOrg is told to refit.
 */
typedef struct UnitOrgData {
    short maxHp;               /* +0x00 */
    short maxEp;                /* +0x02 */
    short str;                  /* +0x04 */
    short vit;                  /* +0x06 */
    short eatk;                 /* +0x08 */
    short edef;                 /* +0x0A */
    signed char hit;            /* +0x0C */
    signed char eva;            /* +0x0D */
    signed char agility;        /* +0x0E */
    signed char moveKindId;     /* +0x0F */
    short atkRange;             /* +0x10 */
    short defRange;             /* +0x12 */
    signed char level;          /* +0x14 */
    signed char size;           /* +0x15 */
    signed char cRate;          /* +0x16 */
    signed char ecap;           /* +0x17 */
    unsigned short weak;        /* +0x18 */
    unsigned short spec;        /* +0x1A */
    unsigned short flags;       /* +0x1C */
    short weight;               /* +0x1E */
    signed char bRate;          /* +0x20 */
    signed char bNum;           /* +0x21 */
    unsigned char unmodeled_22[0x24 - 0x22];
    unsigned short protect[8];  /* +0x24 */
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    short charaId;               /* +0x38 */
    unsigned char unmodeled_3a[0x54 - 0x3A];
    short agwsId;                /* +0x54 */
    short engineId;               /* +0x56 */
    short frameId;                /* +0x58 */
    signed char weaponVariant[3]; /* +0x5A */
    unsigned char unmodeled_5d;
    short weaponId[3];            /* +0x5E */
    short accessory[3];           /* +0x64 */
    short attachment[3];          /* +0x6A */
    unsigned char unmodeled_70[0x76 - 0x70];
    short norm[6];                /* +0x76 */
    short special[6];             /* +0x82 */
    short ether[12];              /* +0x8E */
    short skill[3];                /* +0xA6 */
} UnitOrgData;

extern UnitOrgData *calcTotalParaMenu(int chrNo, int *attack, int *defense);
extern UnitOrgData *dataUnitOrgGet(int chrNo);

extern PadPrefix PadData;
/*
 * Gain record returned by dataGainGet: experience, money, the TP/EP/SP values
 * and the layer, group and item counts.
 */
typedef struct DebugGainRecord {
    int exp;                      /* +0x00: "EXP   =%8d" */
    unsigned short money;         /* +0x04: "MONEY =%8d" */
    unsigned char tp;             /* +0x06: "TP    =%4d" */
    unsigned char ep;             /* +0x07: "EP    =%4d" */
    unsigned char sp;             /* +0x08: "SP    =%4d" */
    unsigned char unmodeled_09;
    unsigned char layer[3];       /* +0x0A: "LAY =%3d" */
    unsigned char group[3];       /* +0x0D: "GRP =%3d" */
    unsigned char item[3];        /* +0x10: "ITEM =%3d" */
} DebugGainRecord;

typedef struct DebugGrowRecord {
    int exp;                      /* +0x00: "EXP      =%8d" */
    int nextExp;                  /* +0x04: "NEXT EXP =%8d" */
    short weaponOffset;           /* +0x08: "W OFS    =%4d" */
    short weaponCount;            /* +0x0A: "W CNT    =%4d" */
    int tp;                       /* +0x0C: "TP       =%4d" */
    int ep;                       /* +0x10: "EP       =%4d" */
    int sp;                       /* +0x14: "SP       =%4d" */
    short grow[8];                /* +0x18: "GROW PARA=%4d %4d %4d %4d" x2 */
} DebugGrowRecord;

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglSleep(void);
extern void dataThinkLoad(int thinkNo);
extern int cfEncountGet(void);
extern int cfEventGet(void);
extern int mapNoGet(void);
extern int monsSetNoGet(void);
extern int thinkNoGet(void);
extern int plMutekiGet(void);
extern int enMutekiGet(void);
extern int plIchigekiGet(void);
extern int cameraFlagGet(void);
extern void debugItemSet(void);
extern void debugEtherSet(void);
extern void debugSkillSet(void);
extern int debugEquSet(void);
extern UnitOrgData *dataUnitInitGet(int chrNo);
extern short *dataNormInitGet(int chrNo);
extern short *dataEtherInitGet(int chrNo);
extern DebugGainRecord *dataGainGet(int gainId);
extern char *dataPlChaGet(int cid);
extern int valUD(int *value, int min, int max, int step);
static int configPos;
static int equipPos;
static int (*configCmdTbl[21])(int *);
extern int (*equipCmdTbl[])();
static const char *posName[10];
static const char *szName[5];

extern const char debugEntryJpText11[];
extern const char debugEntryBATTLECONFIG20011001Text[];
extern const char debugEntryBATTLESTARTText[];
extern const char debugEntryTHINKText[];
extern const char debugEntryMONSTERSETText[];
extern const char debugEntryMAPText[];
extern const char debugEntryPLAYER1Text[];
extern const char debugEntryPLAYER2Text[];
extern const char debugEntryPLAYER3Text[];
extern const char debugEntryPLAYER1POSText[];
extern const char debugEntryPLAYER2POSText[];
extern const char debugEntryPLAYER3POSText[];
extern const char debugEntryCAMERAFLAGText[];
extern const char debugEntryPLMUTEKIText[];
extern const char debugEntryENMUTEKIText[];
extern const char debugEntryPLICHIGEKIText[];
extern const char debugEntryCFENCOUNTText[];
extern const char debugEntryCFEVENTText[];
extern const char debugEntrySTATText[];
extern const char debugEntryEQUIPText[];
extern const char debugEntryGAINText[];
extern const char debugEntryYADOYAText[];
extern const char debugEntrySTATText2[];
extern const char debugEntryIDText2[];
extern const char debugEntryHPMAXHPText[];
extern const char debugEntryEPMAXEPText[];
extern const char debugEntrySTRVITText[];
extern const char debugEntryEATKEDEFText[];
extern const char debugEntryHITEVAText[];
extern const char debugEntryAGLMVTYPEText[];
extern const char debugEntryAAREADAREAText[];
extern const char debugEntryLVSIZEText[];
extern const char debugEntryCOUNTECAPText[];
extern const char debugEntryWEAKSPECText[];
extern const char debugEntryINFOWEIGHTText[];
extern const char debugEntryBRATEBNUMText[];
extern const char debugEntryNORMText[];
extern const char debugEntryJpText12[];
extern const char debugEntryETHERText[];
extern const char debugEntryJpText13[];
extern const char debugEntryPROTCText[];
extern const char debugEntryJpText14[];
extern const char debugEntryEQUIPText2[];
extern const char debugEntryAGWSText[];
extern const char debugEntryENGINEText[];
extern const char debugEntryFRAMEText[];
extern const char debugEntryWPN0WIDText[];
extern const char debugEntryWPN1WIDText[];
extern const char debugEntryWPN2WIDText[];
extern const char debugEntryATT0Text[];
extern const char debugEntryATT1Text[];
extern const char debugEntryATT2Text[];
extern const char debugEntryACC0Text[];
extern const char debugEntryACC1Text[];
extern const char debugEntryACC2Text[];
extern const char debugEntrySPECText[];
extern const char debugEntryETH0Text[];
extern const char debugEntryETH4Text[];
extern const char debugEntryETH8Text[];
extern const char debugEntrySKILLText[];
extern const char debugEntryGAINText2[];
extern const char debugEntryEXPText[];
extern const char debugEntryMONEYText[];
extern const char debugEntryTPText[];
extern const char debugEntryEPText[];
extern const char debugEntrySPText[];
extern const char debugEntryLAYGRPITEMText[];
extern const char debugEntryEXPText2[];
extern const char debugEntryNEXTEXPText[];
extern const char debugEntryWOFSText[];
extern const char debugEntryWCNTText[];
extern const char debugEntryTPText2[];
extern const char debugEntryEPText2[];
extern const char debugEntrySPText2[];
extern const char debugEntryGROWPARAText[];
extern const char debugEntryJpText15[];

/*
 * Battle configuration debug screen. Page 0 is the settings list and pages 1
 * to 3 show the stat, equipment and gain records. Cross returns from a page to
 * the list; Cross on the list, or Circle on an entry whose command returns 1,
 * leaves the screen.
 */
void debugBattleConfig(void)
{
    DebugPartyUnit *party;
    UnitOrgData *rec;
    DebugGainRecord *gain;
    DebugGrowRecord *grow;
    short *norm;
    short *ether;
    int cmdCount;
    int lastCmd;
    int equipCount;
    int cmdResult;
    int hp;
    int row;
    int h = 9;
    int x = 12;
    int w = 9;
    int column;
    int ep;
    int i;
    int y;
    int *pos;

    party = plUnitTblGet();
    for (cmdCount = 0; configCmdTbl[cmdCount] != 0; cmdCount++) {
    }
    mapNo = mapNoGet();
    monsSet = monsSetNoGet();
    cfEncount = cfEncountGet();
    cfEvent = cfEventGet();
    debugThinkNo = thinkNoGet();
    debugItemSet();
    debugEtherSet();
    debugSkillSet();
    debugEquSet();
    lastCmd = cmdCount - 1;
    for (;;) {
        if (configSeq == 0) {
            /* Page 0: settings list. */
            row = 0;
            x = 12;
            valUD(&configPos, 0, lastCmd, 1);
            {
                int cursorY = h * (configPos + 1) + x;
                float word = *(float *)party;

                *(float *)party = word;
                xglFontDebugPrintf(12, cursorY, debugEntryJpText11);
            }
            { union { int i; float f; } *view = (void *)configCmdTbl; float f = view->f; view->f = f; }
            pos = &configPos;
            cmdResult = configCmdTbl[*pos](pos);
            pos = 0;
            pos = &configPos;
            xglFontDebugPrintf(12, h * row++ + x, debugEntryBATTLECONFIG20011001Text);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryBATTLESTARTText);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryTHINKText, debugThinkNo);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryMONSTERSETText, monsSet);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryMAPText, mapNo);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER1Text, debugNameGet(party[0].player));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER2Text, debugNameGet(party[1].player));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER3Text, debugNameGet(party[2].player));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER1POSText, posName[party[0].position]);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER2POSText, posName[party[1].position]);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLAYER3POSText, posName[party[2].position]);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryCAMERAFLAGText, cameraFlagGet());
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLMUTEKIText, plMutekiGet());
            xglFontDebugPrintf(12, h * row++ + x, debugEntryENMUTEKIText, enMutekiGet());
            xglFontDebugPrintf(12, h * row++ + x, debugEntryPLICHIGEKIText, plIchigekiGet());
            xglFontDebugPrintf(12, h * row++ + x, debugEntryCFENCOUNTText, cfEncount);
            xglFontDebugPrintf(12, h * row++ + x, debugEntryCFEVENTText, cfEvent);
            xglFontDebugPrintf(12, h * row++ + x, debugEntrySTATText, debugNameGet(statId));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryEQUIPText, debugNameGet(equipId));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryGAINText, debugNameGet(gainId));
            xglFontDebugPrintf(12, h * row++ + x, debugEntryYADOYAText);
            if (cmdResult == 1 && (PadData.half_2a & PAD_CIRCLE)) {
                goto done;
            }
            pos = 0;
            if (PadData.half_2a & PAD_CROSS) {
                goto done;
            }
        } else if (configSeq == 1) {
            /* Page 1: stat record of the selected character. */
            row = 0;
            valLR(&statId, 1, DEBUG_CHARA_ID_MAX, 1);
            if (statId < 0x21) {
                rec = dataUnitOrgGet(statId);
                hp = rec->hp;
                ep = rec->ep;
                norm = rec->norm;
                ether = rec->ether;
            } else {
                rec = dataUnitInitGet(statId);
                hp = rec->maxHp;
                ep = rec->maxEp;
                norm = dataNormInitGet(statId);
                ether = dataEtherInitGet(statId);
            }
            xglFontDebugPrintf(12, h * row++ + 12, debugEntrySTATText2);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryIDText2, debugNameGet(statId));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryHPMAXHPText, hp, rec->maxHp);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryEPMAXEPText, ep, rec->maxEp);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntrySTRVITText, rec->str, rec->vit);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryEATKEDEFText, rec->eatk, rec->edef);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryHITEVAText, rec->hit, rec->eva);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryAGLMVTYPEText, rec->agility, rec->moveKindId);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryAAREADAREAText, rec->atkRange, rec->defRange);
            { const char *sz = szName[rec->size]; xglFontDebugPrintf(12, h * row++ + 12, debugEntryLVSIZEText, rec->level, sz); }
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryCOUNTECAPText, rec->cRate, rec->ecap);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryWEAKSPECText, rec->weak, rec->spec);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryINFOWEIGHTText, rec->flags, rec->weight);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryBRATEBNUMText, rec->bRate, rec->bNum);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryNORMText, norm[0], norm[1], norm[2]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryJpText12, norm[3], norm[4], norm[5]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryETHERText, ether[0], ether[1], ether[2], ether[3]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryJpText13, ether[4], ether[5], ether[6], ether[7]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryJpText13, ether[8], ether[9], ether[10], ether[11]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryPROTCText, rec->protect[0], rec->protect[1],
                               rec->protect[2], rec->protect[3]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryJpText14, rec->protect[4], rec->protect[5],
                               rec->protect[6], rec->protect[7]);
            if (PadData.half_2a & PAD_CROSS) {
                configSeq = 0;
            }
        } else if (configSeq == 2) {
            /* Page 2: equipment of the selected character. */
            row = 0;
            for (equipCount = 0; equipCmdTbl[equipCount] != 0; equipCount++) {
            }
            if (valUD(&equipPos, 0, equipCount - 1, 1) == 1) {
                slot = 0;
                equipPosX = 0;
            }
            column = w;
            xglFontDebugPrintf(column * equipPosX + 12, h * (equipPos + 1) + 12, debugEntryJpText11);
            equipCmdTbl[equipPos]();
            rec = dataUnitOrgGet(equipId);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryEQUIPText2);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryIDText2, debugNameGet(equipId));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryAGWSText, debugNameGet(rec->agwsId));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryENGINEText, rec->engineId);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryFRAMEText, rec->frameId);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryWPN0WIDText, rec->weaponVariant[0],
                               debugWpnGet(rec->weaponId[0]));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryWPN1WIDText, rec->weaponVariant[1],
                               debugWpnGet(rec->weaponId[1]));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryWPN2WIDText, rec->weaponVariant[2],
                               debugWpnGet(rec->weaponId[2]));
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryATT0Text, rec->attachment[0]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryATT1Text, rec->attachment[1]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryATT2Text, rec->attachment[2]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryACC0Text, rec->accessory[0]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryACC1Text, rec->accessory[1]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryACC2Text, rec->accessory[2]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntrySPECText, rec->special[0], rec->special[1],
                               rec->special[2], rec->special[3], rec->special[4], rec->special[5]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryETH0Text, rec->ether[0], rec->ether[1],
                               rec->ether[2], rec->ether[3]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryETH4Text, rec->ether[4], rec->ether[5],
                               rec->ether[6], rec->ether[7]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryETH8Text, rec->ether[8], rec->ether[9],
                               rec->ether[10], rec->ether[11]);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntrySKILLText, rec->skill[0], rec->skill[1],
                               rec->skill[2]);
            if (PadData.half_2a & PAD_CROSS) {
                configSeq = 0;
            }
        } else if (configSeq == 3) {
            /* Page 3: gain record for ids from 0x21, grow record for ids below 0x11. */
            valLR(&gainId, 1, DEBUG_CHARA_ID_MAX, 1);
            row = 0;
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryGAINText2);
            xglFontDebugPrintf(12, h * row++ + 12, debugEntryIDText2, debugNameGet(gainId));
            if (gainId >= 0x21) {
                gain = dataGainGet(gainId);
                xglFontDebugPrintf(12, h * row++ + 12, debugEntryEXPText, gain->exp);
                xglFontDebugPrintf(12, h * row++ + 12, debugEntryMONEYText, gain->money);
                xglFontDebugPrintf(12, h * row++ + 12, debugEntryTPText, gain->tp);
                xglFontDebugPrintf(12, h * row++ + 12, debugEntryEPText, gain->ep);
                xglFontDebugPrintf(12, h * row++ + 12, debugEntrySPText, gain->sp);
                for (i = 0; i < 3; i++) {
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryLAYGRPITEMText, gain->layer[i],
                                       gain->group[i], gain->item[i]);
                }
            } else {
                if (gainId < 0x11) {
                    grow = (DebugGrowRecord *)dataPlChaGet(gainId);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryEXPText2, grow->exp);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryNEXTEXPText, grow->nextExp);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryWOFSText, grow->weaponOffset);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryWCNTText, grow->weaponCount);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryTPText2, grow->tp);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryEPText2, grow->ep);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntrySPText2, grow->sp);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryGROWPARAText, grow->grow[0], grow->grow[1],
                                       grow->grow[2], grow->grow[3]);
                    xglFontDebugPrintf(12, h * row++ + 12, debugEntryJpText15, grow->grow[4], grow->grow[5],
                                       grow->grow[6], grow->grow[7]);
                }
            }
            if (PadData.half_2a & PAD_CROSS) {
                configSeq = 0;
            }
        }
        xglSleep();
    }
done:
    dataThinkLoad(debugThinkNo);
}

int configStart(int *pos) {
    return 1;
}

/* Debug menu: cycles the character assigned to party slot `slot`. */
extern int configPlayer(int slot);

int configPlayer1(int *pos) {
    return configPlayer(0);
}

int configPlayer2(int *pos) {
    return configPlayer(1);
}

int configPlayer3(int *pos) {
    return configPlayer(2);
}

int configPlayer(int slot)
{
    DebugPartyUnit *units;
    int player;

    units = plUnitTblGet();
    player = units[slot].player;
    valLR(&player, 0, 31, 1);
    plUnitTblSet(slot, player, units[slot].position);
    return 0;
}

/* Debug menu: cycles the formation position of party slot `slot`. */
extern int configPlPos(int slot);

int configPlPos1(int *pos) {
    return configPlPos(0);
}

int configPlPos2(int *pos) {
    return configPlPos(1);
}

int configPlPos3(int *pos) {
    return configPlPos(2);
}

int configPlPos(int slot)
{
    DebugPartyUnit *units;
    int position;

    units = plUnitTblGet();
    position = units[slot].position;
    valLR(&position, 1, 9, 1);
    plUnitTblSet(slot, units[slot].player, position);
    return 0;
}

extern void monsSetNoSet(int monsSetNo);
int configMonsSet(int *pos) {
    valLR(&monsSet, 0, 0x63, 1);
    monsSetNoSet(monsSet);
    return 0;
}

extern void mapNoSet(int mapNo);
int configMap(int *pos) {
    valLR(&mapNo, 0, 0x63, 1);
    mapNoSet(mapNo);
    return 0;
}

extern int cameraFlagGet(void);
extern void cameraFlagSet(int flag);

int configCamera(int *pos) {
    int flag;

    flag = cameraFlagGet();
    valLR(&flag, 0, 1, 1);
    cameraFlagSet(flag);
    return 0;
}

int configStat(int *pos)
{
    extern PadPrefix PadData;

    valLR(&statId, 1, DEBUG_CHARA_ID_MAX, 1);
    if (PadData.half_2a & PAD_CIRCLE) {
        configSeq = 1;
    }
    return 0;
}

extern PadPrefix PadData;

int configEqu(int *pos) {
    valLR(&equipId, 1, 0x20, 1);
    if (PadData.half_2a & PAD_CIRCLE) {
        configSeq = 2;
        equipPosX = 0;
    }
    return 0;
}

int configGain(int *pos)
{
    extern PadPrefix PadData;

    valLR(&gainId, 1, DEBUG_CHARA_ID_MAX, 1);
    if (PadData.half_2a & PAD_CIRCLE) {
        configSeq = 3;
        equipPosX = 0;
    }
    return 0;
}

int configPlMuteki(int *pos) {
    int flag;

    flag = plMutekiGet();
    valLR(&flag, 0, 1, 1);
    plMuteki = flag;
    return 0;
}

int configEnMuteki(int *pos) {
    int flag;

    flag = enMutekiGet();
    valLR(&flag, 0, 1, 1);
    enMuteki = flag;
    return 0;
}

int configPlIchigeki(int *pos) {
    int flag;

    flag = plIchigekiGet();
    valLR(&flag, 0, 1, 1);
    plIchigeki = flag;
    return 0;
}

extern void cfEncountSet(int cfEncount);
int configCFEncount(int *pos) {
    valLR(&cfEncount, 0, 0xFFFF, 1);
    cfEncountSet(cfEncount);
    return 0;
}

extern void cfEventSet(int cfEvent);
int configCFEvent(int *pos) {
    valLR(&cfEvent, 0, 0xFFFF, 1);
    cfEventSet(cfEvent);
    return 0;
}

extern void thinkNoSet(int thinkNo);
int configThink(int *pos) {
    valLR(&debugThinkNo, 0, 0x63, 1);
    thinkNoSet(debugThinkNo);
    return 0;
}

int configYadoya(int *pos) {
    int chrNo;
    UnitOrgData *total;
    UnitOrgData *rec;

    if (PadData.half_2a & PAD_CIRCLE) {
        for (chrNo = 1; chrNo <= 0x20; chrNo++) {
            total = calcTotalParaMenu(chrNo, 0, 0);
            rec = dataUnitOrgGet(chrNo);
            rec->hp = total->maxHp;
            rec->ep = total->maxEp;
        }
    }
    return 0;
}

/*
 * PadData+0x2C, the button word; the shared PadPrefix leaves it unmodeled,
 * so the selectors read it through this view of the same object. Bits as
 * src/main/game_debug_menu.h.
 */
typedef struct DebugPadView {
    unsigned char unmodeled_00[0x2C];
    unsigned short buttons; /* +0x2C */
} DebugPadView;

/*
 * Debug menu up/down adjuster: steps *value by step, wrapping at both ends.
 * Returns nonzero when *value changed.
 */
int valUD(int *value, int min, int max, int step)
{
    int old = *value;
    unsigned short buttons = ((DebugPadView *)&PadData)->buttons;

    if (buttons & PAD_DOWN) {
        *value += step;
        if (*value > max) {
            *value = min;
        }
    } else if (buttons & PAD_UP) {
        *value -= step;
        if (*value < min) {
            *value = max;
        }
    }
    return *value != old;
}

/*
 * Debug menu left/right adjuster: steps *value by step (Left/Right) or by
 * 10 * step (L1/R1), wrapping from past max to min and from below min to max.
 * Returns nonzero when *value changed.
 */
int valLR(int *value, int min, int max, int step)
{
    int old = *value;

    if (((DebugPadView *)&PadData)->buttons & PAD_R1) {
        *value += step * 10;
        if (*value > max) {
            *value = min;
        }
    }
    if (((DebugPadView *)&PadData)->buttons & PAD_L1) {
        *value -= step * 10;
        if (*value < min) {
            *value = max;
        }
    }
    if (((DebugPadView *)&PadData)->buttons & PAD_RIGHT) {
        *value += step;
        if (*value > max) {
            *value = min;
        }
    } else if (((DebugPadView *)&PadData)->buttons & PAD_LEFT) {
        *value -= step;
        if (*value < min) {
            *value = max;
        }
    }
    return *value != old;
}

int equipChar(void) {
    valLR(&equipId, 1, 0x20, 1);
    return 0;
}

int equipAgws(void) {
    UnitOrgData *unit;
    int agwsId;

    unit = dataUnitOrgGet(equipId);
    agwsId = unit->agwsId;
    valLR(&agwsId, 0x11, 0x20, 1);
    calcAgwsEquipOrg(unit->charaId, agwsId);
    return 0;
}

extern int calcEngineEquipOrg(int charaId, int engineId);

int equipEngine(void) {
    UnitOrgData *unit;
    int engineId;

    unit = dataUnitOrgGet(equipId);
    engineId = unit->engineId;
    valLR(&engineId, 0, 0x28, 1);
    calcEngineEquipOrg(unit->charaId, engineId);
    return 0;
}

extern int calcFrameEquipOrg(int charaId, int frameId);

int equipFrame(void) {
    UnitOrgData *unit;
    int frameId;

    unit = dataUnitOrgGet(equipId);
    frameId = unit->frameId;
    valLR(&frameId, 0, 0x29, 1);
    calcFrameEquipOrg(unit->charaId, frameId);
    return 0;
}

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

int equipWpn(int slot)
{
    UnitOrgData *unit;
    int weapon_id;
    int weapon_variant;

    unit = dataUnitOrgGet(equipId);
    weapon_id = unit->weaponId[slot];
    weapon_variant = unit->weaponVariant[slot];
    if (equipPosX == 0) {
        valLR(&weapon_variant, 0x18, 0x27, 1);
    } else {
        valLR(&weapon_id, 0, 0x73, 1);
    }
    calcWpnEquipOrg(unit->charaId, slot, weapon_id, weapon_variant);
    if (PadData.half_2a & PAD_CIRCLE) {
        equipPosX = (equipPosX != 0) ? 0 : 9;
    }
    return 0;
}

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

int equipAtt(int slot) {
    UnitOrgData *unit;
    int attId;

    unit = dataUnitOrgGet(equipId);
    attId = unit->attachment[slot];
    valLR(&attId, 0, 0xB2, 1);
    calcAttEquipOrg(unit->charaId, slot, attId);
    return 0;
}

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

extern int calcAccEquipOrg(int charaId, int slot, int accId);

int equipAcc(int slot) {
    UnitOrgData *unit;
    int accId;

    unit = dataUnitOrgGet(equipId);
    accId = unit->accessory[slot];
    valLR(&accId, 0, 0xB4, 1);
    calcAccEquipOrg(unit->charaId, slot, accId);
    return 0;
}

extern int calcSpecEquipOrg(int charaId, int slot, int specId);
extern int slot; /* shared debug cursor for the spec/ether/skill equip screens */
extern int specPos[6]; /* screen cursor column per spec slot */

/*
 * Edits the special-action id of the spec slot selected by slot. Circle moves
 * to the next slot.
 */
int equipSpec(void) {
    UnitOrgData *unit;
    int specId;

    unit = dataUnitOrgGet(equipId);
    specId = unit->special[slot];
    valLR(&specId, 0, 0x74, 1);
    calcSpecEquipOrg(unit->charaId, slot, specId);
    if (PadData.half_2a & PAD_CIRCLE) {
        slot += 1;
        if (slot >= 6) {
            slot = 0;
        }
        equipPosX = specPos[slot];
    }
    return 0;
}

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


extern int etherPos[4]; /* screen cursor column per ether sub-slot */

/*
 * Edits the ether id of the slot selected by slot within firstSlot..lastSlot.
 * Circle moves to the next slot, wrapping back to firstSlot after lastSlot.
 */
int equipEther(int firstSlot, int lastSlot) {
    UnitOrgData *unit;
    int etherId;

    unit = dataUnitOrgGet(equipId);
    etherId = unit->ether[slot + firstSlot];
    valLR(&etherId, 0, 0xB3, 1);
    calcEtherEquipOrg(unit->charaId, slot + firstSlot, etherId);
    if (PadData.half_2a & PAD_CIRCLE) {
        slot += 1;
        if ((lastSlot - firstSlot) < slot) {
            slot = 0;
        }
        equipPosX = etherPos[slot];
    }
    return 0;
}

extern int calcSkillEquipOrg(int charaId, int slot, int skillId);
extern int skillPos[3]; /* screen cursor column per skill slot */

/*
 * Edits the skill id of the skill slot selected by slot. Circle moves to the
 * next of the three slots.
 */
int equipSkill(void) {
    UnitOrgData *unit;
    int skillId;

    unit = dataUnitOrgGet(equipId);
    skillId = unit->skill[slot];
    valLR(&skillId, 0, 0x5B, 1);
    calcSkillEquipOrg(unit->charaId, slot, skillId);
    if (PadData.half_2a & PAD_CIRCLE) {
        slot += 1;
        if (slot >= 3) {
            slot = 0;
        }
        equipPosX = skillPos[slot];
    }
    return 0;
}

extern const char debugEntryNEText[8];
extern const char debugEntryGTText[8];
extern const char debugEntryLTText[8];
extern const char debugEntryLEText[8];
extern const char debugEntryGEText[8];
extern const char debugEntryEQText[8];
extern const char debugEntryUNITText[8];
extern const char debugEntryCMDText[8];
extern const char debugEntryIDText[8];
extern const char debugEntryTGTText[8];
extern const char debugEntryUNITATKText[16];
extern const char debugEntryCMDATKText[8];
extern const char debugEntryTIDATKText[8];
extern const char debugEntryPARAATKText[16];
extern const char debugEntryRRTText[8];
extern const char debugEntryPLNUMText[8];
extern const char debugEntryENNUMText[8];
extern const char debugEntryCAMMODEText[16];
extern const char debugEntryCAMSETACT3Text[16];
extern const char debugEntryCAMCENTERLENGTHText[24];
extern const char debugEntryRSUBText[8];
extern const char debugEntryRPOPText[8];
extern const char debugEntryCAMMOVEText[16];
extern const char debugEntryCAMPERSMOVEMODEText[24];
extern const char debugEntryMAPMULSETText[16];
extern const char debugEntryUNITCREATEText[16];
extern const char debugEntryCAMSHAKEText[16];
extern const char debugEntryUNITPARAGETText[16];
extern const char debugEntryCFEVENTGETText[16];
extern const char debugEntryEVENTText[8];
extern const char debugEntryRORText[8];
extern const char debugEntryMSGPOSText[8];
extern const char debugEntryCAMLOCKText[16];
extern const char debugEntryCFENCOUNTGETText[16];
extern const char debugEntryLIGHTDIRText[16];
extern const char debugEntryLIGHTAMBText[16];
extern const char debugEntryCAMREFMODEText[16];
extern const char debugEntryRRRNDText[8];
extern const char debugEntryATKTBLText[8];
extern const char debugEntryWAITMSGText[16];
extern const char debugEntryATKTBLSEARCHText[16];
extern const char debugEntryCAMCOPYText[16];
extern const char debugEntryRPUTText[8];
extern const char debugEntryCAMREFMOVEMODEText[24];
extern const char debugEntryTHINKSETText[16];
extern const char debugEntryONGOSUBText[8];
extern const char debugEntryTHINKSET2Text[16];
extern const char debugEntryMEMDUMPText[8];
extern const char debugEntryWAITEVENTText[16];
extern const char debugEntryCAMUNITHEIGHTText[16];
extern const char debugEntryTHINKNOGETText[16];
extern const char debugEntryATKSETText[8];
extern const char debugEntryCAMPERSText[16];
extern const char debugEntryCAMCAMMOVEMODEText[24];
extern const char debugEntryCAMSETPOSText[16];
extern const char debugEntryCMDPUTONText[16];
extern const char debugEntryRMULText[8];
extern const char debugEntryDEADTIMEText[16];
extern const char debugEntryATKTBLSORTText[16];
extern const char debugEntryGOText[8];
extern const char debugEntryCAMOFFSANGText[16];
extern const char debugEntrySTATSETText[16];
extern const char debugEntryRSETText[8];
extern const char debugEntryCAMBANKMOVEMODEText[24];
extern const char debugEntryLIGHTCOLText[16];
extern const char debugEntryWAITCNTText[16];
extern const char debugEntryRINCText[8];
extern const char debugEntryRETURNText[8];
extern const char debugEntryTHINKCAMEVENTText[16];
extern const char debugEntryCMDPUTOFFText[16];
extern const char debugEntryTECPARAGETText[16];
extern const char debugEntryBOOSTText[8];
extern const char debugEntryRDIVText[8];
extern const char debugEntrySTATGETText[16];
extern const char debugEntryUNITTBLText[8];
extern const char debugEntryWAITCAMMOVEText[16];
extern const char debugEntryCAMMOVESTOPText[16];
extern const char debugEntrySTATRESETText[16];
extern const char debugEntryHPPERGETText[16];
extern const char debugEntryCAMSETACTText[16];
extern const char debugEntryGOSUBText[8];
extern const char debugEntryONGOText[8];
extern const char debugEntryTBLGETText[8];
extern const char debugEntryATKTBLSORTREVText[16];
extern const char debugEntryRMODText[8];
extern const char debugEntrySTATCHKText[8];
extern const char debugEntryUNITPARASETText[16];
extern const char debugEntryTBLSETText[8];
extern const char debugEntryVPADSETText[16];
extern const char debugEntryNULLText[8];
extern const char debugEntryMONSSETNOGETText[16];
extern const char debugEntryRXORText[8];
extern const char debugEntryPRINTText[8];
extern const char debugEntryUNITCHANGEText[16];
extern const char debugEntryRADDText[8];
extern const char debugEntryCAMCAMMODEText[16];
extern const char debugEntryVPADDISABLEText[16];
extern const char debugEntryRNEGText[8];
extern const char debugEntryCAMOFFSPOSText[16];
extern const char debugEntryCAMCOORDGETText[16];
extern const char debugEntryRANDText[8];
extern const char debugEntryEVENTSLOTGETText[16];
extern const char debugEntryBRAText[8];
extern const char debugEntryRDECText[8];
extern const char debugEntryEXITText[8];
extern const char debugEntryCAMPERSMODEText[16];
extern const char debugEntryCAMBANKMODEText[16];
extern const char debugEntryVPADENEBLEText[16];
extern const char debugEntryPARTYSIZEGETText[16];
extern const char debugEntryCAMSETACT2Text[16];
extern const char debugEntryRNOTText[8];
extern const char debugEntryCAMBANKText[16];
extern const char debugEntryMSGText[8];
extern const char debugEntryCAMTBLSETText[16];
extern const char debugEntryLINECHKText[8];
extern const char debugEntryRPUSHText[8];
extern const char debugEntryA3Text[8];
extern const char debugEntryA2Text[8];
extern const char debugEntryP3Text[8];
extern const char debugEntryP1Text[8];
extern const char debugEntryP2Text[8];
extern const char debugEntryA1Text[8];
extern const char debugEntryP5Text[8];
extern const char debugEntryP6Text[8];
extern const char debugEntryP4Text[8];
extern const char debugEntryMText[8];
extern const char debugEntryLLText[8];
extern const char debugEntrySText[8];
extern const char debugEntryLText[8];
extern const char debugEntrySSText[8];
extern int equipEngine(void);
extern int equipWpn1(void);
extern int equipWpn0(void);
extern int equipAgws(void);
extern int equipAcc1(void);
extern int equipEther0(void);
extern int equipWpn2(void);
extern int equipAcc0(void);
extern int equipAtt1(void);
extern int equipChar(void);
extern int equipAtt2(void);
extern int equipSkill(void);
extern int equipAtt0(void);
extern int equipFrame(void);
extern int equipEther1(void);
extern int equipSpec(void);
extern int equipEther2(void);
extern int equipAcc2(void);

static const char *thinkExpTbl[6] = { debugEntryEQText, debugEntryNEText, debugEntryGTText, debugEntryGEText, debugEntryLTText, debugEntryLEText };

static const char *regName[12] = {
    debugEntryUNITText, debugEntryCMDText, debugEntryIDText, debugEntryTGTText,
    debugEntryUNITATKText, debugEntryCMDATKText, debugEntryTIDATKText, debugEntryPARAATKText,
    debugEntryRRTText, debugEntryPLNUMText, debugEntryENNUMText, debugEntryCAMMODEText
};

static const char *thinkStrTbl[99] = { debugEntryEXITText, debugEntryGOText, debugEntryGOSUBText, debugEntryRETURNText, debugEntryBRAText, debugEntryONGOText, debugEntryONGOSUBText, debugEntryRSETText, debugEntryRINCText, debugEntryRDECText, debugEntryRADDText, debugEntryRSUBText, debugEntryRMULText, debugEntryRDIVText, debugEntryRMODText, debugEntryRANDText, debugEntryRORText, debugEntryRXORText, debugEntryRNOTText, debugEntryRNEGText, debugEntryRRRNDText, debugEntryRPUSHText, debugEntryRPOPText, debugEntryTBLGETText, debugEntryTBLSETText, debugEntryRPUTText, debugEntryPRINTText, debugEntryMEMDUMPText, debugEntryCMDPUTONText, debugEntryCMDPUTOFFText, debugEntryNULLText, debugEntryNULLText, debugEntryMSGPOSText, debugEntryMSGText, debugEntryTHINKSETText, debugEntryTHINKSET2Text, debugEntryBOOSTText, debugEntrySTATCHKText, debugEntryLINECHKText, debugEntryUNITPARAGETText, debugEntryUNITPARASETText, debugEntryUNITTBLText, debugEntryUNITCREATEText, debugEntryUNITCHANGEText, debugEntryATKTBLText, debugEntryATKSETText, debugEntryATKTBLSORTText, debugEntryATKTBLSORTREVText, debugEntryATKTBLSEARCHText, debugEntryTECPARAGETText, debugEntryMONSSETNOGETText, debugEntryPARTYSIZEGETText, debugEntryLIGHTAMBText, debugEntryLIGHTCOLText, debugEntryLIGHTDIRText, debugEntryCAMREFMODEText, debugEntryCAMCAMMODEText, debugEntryCAMPERSMODEText, debugEntryCAMBANKMODEText, debugEntryCAMREFMOVEMODEText, debugEntryCAMCAMMOVEMODEText, debugEntryCAMPERSMOVEMODEText, debugEntryCAMBANKMOVEMODEText, debugEntryCAMLOCKText, debugEntryCAMCOPYText, debugEntryCAMSETPOSText, debugEntryCAMSETACTText, debugEntryCAMSETACT2Text, debugEntryCAMSETACT3Text, debugEntryCAMOFFSPOSText, debugEntryCAMOFFSANGText, debugEntryCAMMOVEText, debugEntryCAMSHAKEText, debugEntryCAMCENTERLENGTHText, debugEntryCAMUNITHEIGHTText, debugEntryCAMCOORDGETText, debugEntryCAMBANKText, debugEntryCAMPERSText, debugEntryCAMTBLSETText, debugEntryCAMMOVESTOPText, debugEntryEVENTText, debugEntryWAITEVENTText, debugEntryWAITCNTText, debugEntryWAITCAMMOVEText, debugEntryWAITMSGText, debugEntryVPADENEBLEText, debugEntryVPADDISABLEText, debugEntryVPADSETText, debugEntryMAPMULSETText, debugEntryHPPERGETText, debugEntryEVENTSLOTGETText, debugEntryTHINKCAMEVENTText, debugEntryCFENCOUNTGETText, debugEntryCFEVENTGETText, debugEntryTHINKNOGETText, debugEntrySTATGETText, debugEntrySTATSETText, debugEntrySTATRESETText, debugEntryDEADTIMEText };

static int configPos = 0;

static int equipPos = 0;

static const char *posName[10] = { debugEntryNULLText, debugEntryP1Text, debugEntryP2Text, debugEntryP3Text, debugEntryP4Text, debugEntryP5Text, debugEntryP6Text, debugEntryA1Text, debugEntryA2Text, debugEntryA3Text };

static const char *szName[5] = { debugEntrySSText, debugEntrySText, debugEntryMText, debugEntryLText, debugEntryLLText };

static int (*configCmdTbl[21])(int *) = {
    configStart, configThink, configMonsSet,
    configMap, configPlayer1, configPlayer2,
    configPlayer3, configPlPos1, configPlPos2,
    configPlPos3, configCamera, configPlMuteki,
    configEnMuteki, configPlIchigeki, configCFEncount,
    configCFEvent, configStat, configEqu,
    configGain, configYadoya, 0
};

static int (*equipCmdTbl[19])() = { equipChar, equipAgws, equipEngine, equipFrame, equipWpn0, equipWpn1, equipWpn2, equipAtt0, equipAtt1, equipAtt2, equipAcc0, equipAcc1, equipAcc2, equipSpec, equipEther0, equipEther1, equipEther2, equipSkill, 0 };

const char debugEntryLEText[8] = "LE";

const char debugEntryLTText[8] = "LT";

const char debugEntryGEText[8] = "GE";

const char debugEntryGTText[8] = "GT";

const char debugEntryNEText[8] = "NE";

const char debugEntryEQText[8] = "EQ";

const char debugEntryCAMMODEText[16] = "CAM_MODE";

const char debugEntryENNUMText[8] = "EN_NUM";

const char debugEntryPLNUMText[8] = "PL_NUM";

const char debugEntryRRTText[8] = "RRT";

const char debugEntryPARAATKText[16] = "PARA_ATK";

const char debugEntryTIDATKText[8] = "TID_ATK";

const char debugEntryCMDATKText[8] = "CMD_ATK";

const char debugEntryUNITATKText[16] = "UNIT_ATK";

const char debugEntryTGTText[8] = "TGT";

const char debugEntryIDText[8] = "ID";

const char debugEntryCMDText[8] = "CMD";

const char debugEntryUNITText[8] = "UNIT";

const char debugEntryDEADTIMEText[16] = "DEAD_TIME";

const char debugEntrySTATRESETText[16] = "STAT_RESET";

const char debugEntrySTATSETText[16] = "STAT_SET";

const char debugEntrySTATGETText[16] = "STAT_GET";

const char debugEntryTHINKNOGETText[16] = "THINKNO_GET";

const char debugEntryCFEVENTGETText[16] = "CF_EVENT_GET";

const char debugEntryCFENCOUNTGETText[16] = "CF_ENCOUNT_GET";

const char debugEntryTHINKCAMEVENTText[16] = "THINK_CAM_EVENT";

const char debugEntryEVENTSLOTGETText[16] = "EVENT_SLOT_GET";

const char debugEntryHPPERGETText[16] = "HP_PER_GET";

const char debugEntryMAPMULSETText[16] = "MAP_MULSET";

const char debugEntryVPADSETText[16] = "VPAD_SET";

const char debugEntryVPADDISABLEText[16] = "VPAD_DISABLE";

const char debugEntryVPADENEBLEText[16] = "VPAD_ENEBLE";

const char debugEntryWAITMSGText[16] = "WAIT_MSG";

const char debugEntryWAITCAMMOVEText[16] = "WAIT_CAM_MOVE";

const char debugEntryWAITCNTText[16] = "WAIT_CNT";

const char debugEntryWAITEVENTText[16] = "WAIT_EVENT";

const char debugEntryEVENTText[8] = "EVENT";

const char debugEntryCAMMOVESTOPText[16] = "CAM_MOVE_STOP";

const char debugEntryCAMTBLSETText[16] = "CAM_TBL_SET";

const char debugEntryCAMPERSText[16] = "CAM_PERS";

const char debugEntryCAMBANKText[16] = "CAM_BANK";

const char debugEntryCAMCOORDGETText[16] = "CAM_COORD_GET";

const char debugEntryCAMUNITHEIGHTText[16] = "CAM_UNIT_HEIGHT";

const char debugEntryCAMCENTERLENGTHText[24] = "CAM_CENTER_LENGTH";

const char debugEntryCAMSHAKEText[16] = "CAM_SHAKE";

const char debugEntryCAMMOVEText[16] = "CAM_MOVE";

const char debugEntryCAMOFFSANGText[16] = "CAM_OFFS_ANG";

const char debugEntryCAMOFFSPOSText[16] = "CAM_OFFS_POS";

const char debugEntryCAMSETACT3Text[16] = "CAM_SET_ACT3";

const char debugEntryCAMSETACT2Text[16] = "CAM_SET_ACT2";

const char debugEntryCAMSETACTText[16] = "CAM_SET_ACT";

const char debugEntryCAMSETPOSText[16] = "CAM_SET_POS";

const char debugEntryCAMCOPYText[16] = "CAM_COPY";

const char debugEntryCAMLOCKText[16] = "CAM_LOCK";

const char debugEntryCAMBANKMOVEMODEText[24] = "CAM_BANK_MOVE_MODE";

const char debugEntryCAMPERSMOVEMODEText[24] = "CAM_PERS_MOVE_MODE";

const char debugEntryCAMCAMMOVEMODEText[24] = "CAM_CAM_MOVE_MODE";

const char debugEntryCAMREFMOVEMODEText[24] = "CAM_REF_MOVE_MODE";

const char debugEntryCAMBANKMODEText[16] = "CAM_BANK_MODE";

const char debugEntryCAMPERSMODEText[16] = "CAM_PERS_MODE";

const char debugEntryCAMCAMMODEText[16] = "CAM_CAM_MODE";

const char debugEntryCAMREFMODEText[16] = "CAM_REF_MODE";

const char debugEntryLIGHTDIRText[16] = "LIGHT_DIR";

const char debugEntryLIGHTCOLText[16] = "LIGHT_COL";

const char debugEntryLIGHTAMBText[16] = "LIGHT_AMB";

const char debugEntryPARTYSIZEGETText[16] = "PARTYSIZE_GET";

const char debugEntryMONSSETNOGETText[16] = "MONSSETNO_GET";

const char debugEntryTECPARAGETText[16] = "TECPARA_GET";

const char debugEntryATKTBLSEARCHText[16] = "ATKTBL_SEARCH";

const char debugEntryATKTBLSORTREVText[16] = "ATKTBL_SORT_REV";

const char debugEntryATKTBLSORTText[16] = "ATKTBL_SORT";

const char debugEntryATKSETText[8] = "ATKSET";

const char debugEntryATKTBLText[8] = "ATKTBL";

const char debugEntryUNITCHANGEText[16] = "UNIT_CHANGE";

const char debugEntryUNITCREATEText[16] = "UNIT_CREATE";

const char debugEntryUNITTBLText[8] = "UNITTBL";

const char debugEntryUNITPARASETText[16] = "UNITPARA_SET";

const char debugEntryUNITPARAGETText[16] = "UNITPARA_GET";

const char debugEntryLINECHKText[8] = "LINECHK";

const char debugEntrySTATCHKText[8] = "STATCHK";

const char debugEntryBOOSTText[8] = "BOOST";

const char debugEntryTHINKSET2Text[16] = "THINKSET2";

const char debugEntryTHINKSETText[16] = "THINKSET";

const char debugEntryMSGText[8] = "MSG";

const char debugEntryMSGPOSText[8] = "MSGPOS";

const char debugEntryNULLText[8] = "NULL";

const char debugEntryCMDPUTOFFText[16] = "CMDPUTOFF";

const char debugEntryCMDPUTONText[16] = "CMDPUTON";

const char debugEntryMEMDUMPText[8] = "MEMDUMP";

const char debugEntryPRINTText[8] = "PRINT";

const char debugEntryRPUTText[8] = "RPUT";

const char debugEntryTBLSETText[8] = "TBLSET";

const char debugEntryTBLGETText[8] = "TBLGET";

const char debugEntryRPOPText[8] = "RPOP";

const char debugEntryRPUSHText[8] = "RPUSH";

const char debugEntryRRRNDText[8] = "RRRND";

const char debugEntryRNEGText[8] = "R_NEG";

const char debugEntryRNOTText[8] = "R_NOT";

const char debugEntryRXORText[8] = "R_XOR";

const char debugEntryRORText[8] = "R_OR";

const char debugEntryRANDText[8] = "R_AND";

const char debugEntryRMODText[8] = "RMOD";

const char debugEntryRDIVText[8] = "RDIV";

const char debugEntryRMULText[8] = "RMUL";

const char debugEntryRSUBText[8] = "RSUB";

const char debugEntryRADDText[8] = "RADD";

const char debugEntryRDECText[8] = "RDEC";

const char debugEntryRINCText[8] = "RINC";

const char debugEntryRSETText[8] = "RSET";

const char debugEntryONGOSUBText[8] = "ONGOSUB";

const char debugEntryONGOText[8] = "ONGO";

const char debugEntryBRAText[8] = "BRA";

const char debugEntryRETURNText[8] = "RETURN";

const char debugEntryGOSUBText[8] = "GOSUB";

const char debugEntryGOText[8] = "GO";

const char debugEntryEXITText[8] = "EXIT";

const char debugEntryAPTidParaText[40] = "%6d: %1d)%s AP=%2d %s tid=%d para=%d\n";

const char debugEntryAPText[24] = "%6d: %1d)%s AP=%2d %s\n";

const char debugEntryAPPosText[32] = "%6d: %1d)%s AP=%2d %s pos=%d\n";

const char debugEntryAPMotText[32] = "%6d: %1d)%s AP=%2d %s mot=%d\n";

const char debugEntryAPTidTgtText[48] = "%6d: %1d)%s AP=%2d %s tid=%d tgt=%1d)%s %s\n";

const char debugEntryALLText[8] = "ALL";

const char debugEntryJpText[8] = "";

const char debugEntryAPPosIdText[40] = "%6d: %1d)%s AP=%2d %s pos=%d id=%d\n";

const char debugEntryAPERRORText[40] = "%6d: %1d)%s AP=%2d ERROR=%d\n";

const char debugEntryJpText2[16] = "%04X: %s ";

const char debugEntryHText[8] = "%Xh\n";

const char debugEntryHText2[24] = "%d %s %d -> %Xh\n";

const char debugEntryHText3[8] = "%d %Xh\n";

const char debugEntryJpText3[8] = "%s %d\n";

const char debugEntryJpText4[16] = "%s -> %d\n";

const char debugEntryJpText5[16] = "%s %d -> %d\n";

const char debugEntryJpText6[8] = "%d\n";

const char debugEntryJpText7[16] = "%s %d %d -> %d\n";

const char debugEntryJpText8[8] = "\n";

const char debugEntryUNITSTATText[24] = "** UNIT STAT **\n";

const char debugEntrySTText[8] = " ST  ";

const char debugEntryJpText9[8] = "%04X ";

const char debugEntrySTAText[8] = " STA ";

const char debugEntrySTPText[8] = " STP ";

const char debugEntrySTTText[8] = " STT ";

const char debugEntryJpText10[8] = "%d ";

const char debugEntrySPECPARAText[24] = "** SPEC PARA **\n";

const char debugEntryTidSpdPowAglUseKillText[56] = "%1d: tid=%3d spd=%d pow=%d agl=%d use=%d kill=%d\n";

const char debugEntryA3Text[8] = "A3";

const char debugEntryA2Text[8] = "A2";

const char debugEntryA1Text[8] = "A1";

const char debugEntryP6Text[8] = "P6";

const char debugEntryP5Text[8] = "P5";

const char debugEntryP4Text[8] = "P4";

const char debugEntryP3Text[8] = "P3";

const char debugEntryP2Text[8] = "P2";

const char debugEntryP1Text[8] = "P1";

const char debugEntryLLText[8] = "LL";

const char debugEntryLText[8] = "L";

const char debugEntryMText[8] = "M";

const char debugEntrySText[8] = "S";

const char debugEntrySSText[8] = "SS";

const char debugEntryJpText11[8] = ">";

const char debugEntryBATTLECONFIG20011001Text[32] = "\013 --BATTLE CONFIG 2001/10/01--";

const char debugEntryBATTLESTARTText[24] = "\014\200\000\000 BATTLE START";

const char debugEntryTHINKText[16] = "\013 THINK= %2d";

const char debugEntryMONSTERSETText[24] = "\013 MONSTER SET= %2d";

const char debugEntryMAPText[16] = "\013 MAP= %2d";

const char debugEntryPLAYER1Text[16] = "\013 PLAYER1= %s";

const char debugEntryPLAYER2Text[16] = "\013 PLAYER2= %s";

const char debugEntryPLAYER3Text[16] = "\013 PLAYER3= %s";

const char debugEntryPLAYER1POSText[24] = "\013 PLAYER1 POS= %s";

const char debugEntryPLAYER2POSText[24] = "\013 PLAYER2 POS= %s";

const char debugEntryPLAYER3POSText[24] = "\013 PLAYER3 POS= %s";

const char debugEntryCAMERAFLAGText[24] = "\013 CAMERA FLAG= %1d";

const char debugEntryPLMUTEKIText[24] = "\013 PL MUTEKI= %1d";

const char debugEntryENMUTEKIText[24] = "\013 EN MUTEKI= %1d";

const char debugEntryPLICHIGEKIText[24] = "\013 PL ICHIGEKI= %1d";

const char debugEntryCFENCOUNTText[24] = "\013 CF ENCOUNT= %4x";

const char debugEntryCFEVENTText[16] = "\013 CF EVENT= %4x";

const char debugEntrySTATText[16] = "\014\200\000\000 STAT = %s";

const char debugEntryEQUIPText[16] = "\014\200\000\000 EQUIP= %s";

const char debugEntryGAINText[16] = "\014\200\000\000 GAIN = %s";

const char debugEntryYADOYAText[16] = "\014\200\000\000 YADOYA";

const char debugEntrySTATText2[16] = "\013 --STAT--";

const char debugEntryIDText2[8] = "\013 ID=%s";

const char debugEntryHPMAXHPText[24] = "\013 HP   =%4d MAX HP=%4d";

const char debugEntryEPMAXEPText[24] = "\013 EP   =%4d MAX EP=%4d";

const char debugEntrySTRVITText[24] = "\013 STR  =%4d VIT   =%4d";

const char debugEntryEATKEDEFText[24] = "\013 EATK =%4d EDEF  =%4d";

const char debugEntryHITEVAText[24] = "\013 HIT  =%4d EVA   =%4d";

const char debugEntryAGLMVTYPEText[24] = "\013 AGL  =%4d MVTYPE=%4d";

const char debugEntryAAREADAREAText[24] = "\013 AAREA=%4d DAREA =%4d";

const char debugEntryLVSIZEText[24] = "\013 LV   =%4d SIZE  =%s";

const char debugEntryCOUNTECAPText[24] = "\013 COUNT=%4d ECAP  =%4d";

const char debugEntryWEAKSPECText[24] = "\013 WEAK =%4x SPEC  =%4x";

const char debugEntryINFOWEIGHTText[24] = "\013 INFO =%4x WEIGHT=%4d";

const char debugEntryBRATEBNUMText[24] = "\013 BRATE=%4d BNUM  =%4d";

const char debugEntryNORMText[24] = "\013 NORM =%4d %4d %4d";

const char debugEntryJpText12[24] = "\013       %4d %4d %4d";

const char debugEntryETHERText[24] = "\013 ETHER=%4d %4d %4d %4d";

const char debugEntryJpText13[24] = "\013       %4d %4d %4d %4d";

const char debugEntryPROTCText[24] = "\013 PROTC=%4x %4x %4x %4x";

const char debugEntryJpText14[24] = "\013       %4x %4x %4x %4x";

const char debugEntryEQUIPText2[16] = "\013 --EQUIP--";

const char debugEntryAGWSText[16] = "\013 AGWS  =%s";

const char debugEntryENGINEText[16] = "\013 ENGINE=%3d";

const char debugEntryFRAMEText[16] = "\013 FRAME =%3d";

const char debugEntryWPN0WIDText[24] = "\013 WPN0  =%3d WID =%s";

const char debugEntryWPN1WIDText[24] = "\013 WPN1  =%3d WID =%s";

const char debugEntryWPN2WIDText[24] = "\013 WPN2  =%3d WID =%s";

const char debugEntryATT0Text[16] = "\013 ATT0  =%3d";

const char debugEntryATT1Text[16] = "\013 ATT1  =%3d";

const char debugEntryATT2Text[16] = "\013 ATT2  =%3d";

const char debugEntryACC0Text[16] = "\013 ACC0  =%3d";

const char debugEntryACC1Text[16] = "\013 ACC1  =%3d";

const char debugEntryACC2Text[16] = "\013 ACC2  =%3d";

const char debugEntrySPECText[40] = "\013 SPEC  =%3d %3d %3d %3d %3d %3d";

const char debugEntryETH0Text[32] = "\013 ETH0  =%4d %4d %4d %4d";

const char debugEntryETH4Text[32] = "\013 ETH4  =%4d %4d %4d %4d";

const char debugEntryETH8Text[32] = "\013 ETH8  =%4d %4d %4d %4d";

const char debugEntrySKILLText[24] = "\013 SKILL =%4d %4d %4d";

const char debugEntryGAINText2[16] = "\013 --GAIN--";

const char debugEntryEXPText[16] = "\013 EXP   =%8d";

const char debugEntryMONEYText[16] = "\013 MONEY =%8d";

const char debugEntryTPText[16] = "\013 TP    =%4d";

const char debugEntryEPText[16] = "\013 EP    =%4d";

const char debugEntrySPText[16] = "\013 SP    =%4d";

const char debugEntryLAYGRPITEMText[32] = "\013 LAY =%3d GRP =%3d ITEM =%3d";

const char debugEntryEXPText2[16] = "\013 EXP      =%8d";

const char debugEntryNEXTEXPText[16] = "\013 NEXT EXP =%8d";

const char debugEntryWOFSText[16] = "\013 W OFS    =%4d";

const char debugEntryWCNTText[16] = "\013 W CNT    =%4d";

const char debugEntryTPText2[16] = "\013 TP       =%4d";

const char debugEntryEPText2[16] = "\013 EP       =%4d";

const char debugEntrySPText2[16] = "\013 SP       =%4d";

const char debugEntryGROWPARAText[32] = "\013 GROW PARA=%4d %4d %4d %4d";

const char debugEntryJpText15[32] = "\013          =%4d %4d %4d %4d";
