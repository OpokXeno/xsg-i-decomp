/*
 * OV01 original TU 3: 0x00a07770..0x00a10bb8 (150 functions)
 */
#include "common.h"
#include "ov01/actor.h"
#include "shared.h"
#include "battle_init.h"
#include "ov01/calc.h"
#include "ov01/unit_cmd.h"
#include "ov01/snd.h"
#include "ov01/menu.h"
#include "main/party.h"
#include "main/sef.h"

/* Defined here so the compiler emits its storage outside the inline asm
 * blocks of this TU. */
const char s_battleCtrlBoostEnFormat[32] = "** BOOST EN=%d %d%% (%d)\n";


/*
 * The enemy AI's own boost chance percentage: a signed byte at CalcUnitParam
 * +0x20, inside calc.h's unmodeled_1e span. The boost level both functions
 * log is calc.h's bcValue (+0x3C).
 */
#define CALC_PARAM_BOOST_CHANCE_OFF 0x20

static int escapeFlag;
extern int printf(const char *format, ...);

const char **dataItmNameGet(int index);
const char **dataWepNameGet(int index);
const char **dataBltNameGet(int index);
const char **dataAccNameGet(int index);

extern void battleMsgPut(int x, int y, int msgId, int frames);

/*
 * The battle actor object a unit's actor/weapon slots point to. src/main/
 * chr.h, src/main/near_dir.h, src/main/set_motion.h and src/ov01/unit_cmd.h
 * all restate this same 0x70-byte head verbatim (a TU-local header cannot be
 * included from another TU); this TU adds only the words its own code
 * touches: the float at +0xC0 (transWepIn/transWepOut set it to 0.0/1.0, the
 * same "transparency" member src/ov01/unit_cmd.h restates at that offset)
 * and the flags word at +0x6F0 (hairWindOn ORs in HAIR_WIND_BIT there for
 * every other unit's actor and hairWindOff clears it again).
 */
/* The motion actor's current motion id; +0x84 holds the CalcUnitParam pointer
 * on the work actor instead. stat1Proc compares it with 20 and 21. */
typedef struct UnitMotionHead {
    unsigned char unmodeled_00[0x86];
    short motionId; /* +0x86 */
} UnitMotionHead;

#define HAIR_WIND_BIT 0x100000

#define WEAPON_HIDDEN_BIT 0x8

/*
 * The camera-event request record camEventExec (defined elsewhere) consumes:
 * camEventTurnStartSet writes type 6, the acting/target units and, in param,
 * whether the acting unit is an enemy. battleCtrlMan writes type 8 with only
 * unit/target, leaving param at whatever the record already held.
 */
typedef struct CamEvent {
    int type;           /* +0x00 */
    int param;           /* +0x04 */
    BattleUnit *unit;     /* +0x08 */
    BattleUnit *target;   /* +0x0C */
} CamEvent;

/*
 * One argument: camEventExec reads only $4, and scenarioCam's item branch
 * calls it without setting $5. entry_first_init.c declares it with one
 * argument as well.
 */
extern void camEventExec(CamEvent *event);

/*
 * The scenario-battle exec record the scenarioBatExecPhase* handlers work on
 * (scenarioBatExecPhase20 passes the same pointer to etehrCVGet and to
 * scenarioMtd, and every unit call there loads its first word). scenarioCam
 * reads the action type (compared with 3), the technique id (passed to
 * dataTecGet) and the item id (passed to dataItmGet). spWepDispOn reads the
 * action type (compared with 1), the technique id (compared with 8 and 0xB)
 * and, for a technique action, +0x0C as a weapon slot.
 */
typedef struct ScenarioBatExec {
    BattleUnit *unit; /* +0x00 */
    int type;         /* +0x04: 3 for an item action */
    int tecId;        /* +0x08 */
    int slotOrItemId; /* +0x0C: the item id for an item action (type 3),
                       * the weapon slot for a technique (type 1) */
} ScenarioBatExec;

int dataMtdRead(BattleUnit *unit, int trigger);
int dataDefWpnGet(BattleUnit *unit);

/*
 * Show (transWepIn) or hide (transWepOut) the weapon actor in the unit's
 * slot `slot & 3` by installing its fade draw callback.
 */
void transWepIn(BattleUnit *unit, int slot);
void transWepOut(BattleUnit *unit, int slot);

void spWepDispOn(ScenarioBatExec *exec, int weapon);
void spWepDispOff(ScenarioBatExec *exec, int weapon);
void spWepDispOnSub(BattleUnit *unit, int slot, int fade);
void hairWindOn(BattleUnit *unit);
void hairWindOff(BattleUnit *unit);
extern void ACT_setHand(Actor *actor, int hand);
extern Actor *ACT_setArms(Actor *actor, Actor *parent, unsigned int accessoryId, int flags);
/* Defined in data_unit_org_get.c; spWepDispOn only tests the returned
 * special-weapon entry against zero, so the entry type stays incomplete. */
extern struct WepSpEntry *dataTid2WepSp(int tecId);

extern void transWepInDraw(Actor *actor);
extern void transWepOutDraw(Actor *actor);

/*
 * dataWpnGet's record (src/ov01/data_unit_org_get.c, WpnRecord), as far as
 * keyBuffChk reads it; the name follows menu.c's MenuWpnData view.
 */
typedef struct KeyWpnData {
    unsigned char unmodeled_00[0x0F];
    signed char apCost;          /* +0x0F */
    unsigned char unmodeled_10[0x14 - 0x10];
    short tecId;                 /* +0x14: the command's technique id */
    short tecIdMultiHit;         /* +0x16: keyCmd2tid's technique for commands 3, 5, 10 */
    short tecIdDblAct;           /* +0x18: keyCmd2tid's technique for command 19 */
} KeyWpnData;

/* Functions from other translation units, or defined later in this file,
 * that are called before their definition. */
extern KeyWpnData *dataWpnGet(int weaponId);
extern void actStockInit(void);
extern int actStockRemove(BattleUnit *unit, int *index);
extern void scenarioMtd();
extern unsigned char *dataGainGet(int entry);
extern int dataBoxInc(int group, int item);
extern const char *itemNameGet(int group, int index);
extern int sefIsHitActor(Actor *actor);

/* The battle AI script identifier. */
static int thinkNo;

/*
 * The unit currently executing a battle action / the unit that most
 * recently acted.
 */
static BattleUnit *pLastUnit;
BattleUnit **tgtUnitAdrGet(void);
BattleUnit *tgtUnitGet(void);

static ManWork manWk;

/*
 * battleCtrl's state (BatCtrl.field00): choose the acting unit's command,
 * pick the next actor from the action stock, or wait until the queued
 * command has run and the battle message is done. Names follow what each
 * case does.
 */
enum BattleCtrlState {
    BATTLE_CTRL_ACT_SELECT = 1,
    BATTLE_CTRL_STOCK_PICK = 2,
    BATTLE_CTRL_ACT_WAIT = 3
};

/*
 * battleExec's phase (BatCtrl.field04). Names follow each case's work, and
 * BatCtrl.field08 holds the phase the WAIT phases resume.
 */
enum BattlePhase {
    BATTLE_PHASE_EXEC_QUEUE = 1,       /* battleExecNorm: run the next queued command */
    BATTLE_PHASE_SCENARIO_START = 2,   /* scenarioDstGet(0): scenario batch starts */
    BATTLE_PHASE_SCENARIO_RUN = 3,     /* scenarioBatExec until it reports done */
    BATTLE_PHASE_STAT_PROC_1 = 4,      /* stat1Proc on the statUnit record */
    BATTLE_PHASE_DEAD = 5,             /* battleExecDead */
    BATTLE_PHASE_STAT_PROC_2 = 6,      /* stat2Proc on the statUnit record */
    BATTLE_PHASE_WAIT_BUSY = 7,        /* wait while busyUnitChk(2) > 0, then field08 */
    BATTLE_PHASE_WAIT_UNIT = 8,        /* wait while pExecUnit's flag 0x2 is set */
    BATTLE_PHASE_WAIT_FRAMES = 9       /* wait while busyUnitChk(0) > 0 and field0C counts down */
};

/*
 * The battle-control record battleExec's phase machine steps through.
 */
typedef struct BatCtrl {
    int field00; /* +0x0: set to 1 by battleInit */
    int field04; /* +0x4: battle phase; set to 7 by battleInit */
    int field08; /* +0x8: phase to resume after a wait; set to 1 by battleInit */
    int field0C; /* +0xC: frame countdown tested by battleExec's case 9 */
} BatCtrl;
static BatCtrl batCtrl;

/*
 * Record ScenarioBatState.source points to. scenarioBatNext is the only
 * reference to it in this TU.
 */
typedef struct ScenarioStep {
    ScenarioBatExec exec; /* +0x00: the action record scenarioBatExecPhase40 casts this pointer to */
    int stride;           /* +0x10: added into ScenarioBatState.index by scenarioBatNext */
} ScenarioStep;
/*
 * Two-parameter spelling of the phase callback. It is not standards-correct
 * prototyping: scenarioBatExec's indirect call sets $a0 to scenarioSrcGet(0)'s
 * return and $a1 to scenarioDstGet(0)'s record, and only this parameter list
 * makes the compiler emit that set-up. scenarioBatExecPhase10 reads `source`
 * as the ScenarioBatExec record and hands `dest` to scenarioCam as its target
 * list.
 */
typedef struct ScenarioDstEntry ScenarioDstEntry;
typedef void (*ScenarioPhaseFunc)(int source, ScenarioDstEntry *dest);
/*
 * One destination record. scenarioDstSet appends a whole 0x28-byte
 * record built by unitJobTypeAtk/unitCmdItemSub (src/ov01/unit_cmd.c) at
 * dest[count]; scenarioDstGet hands out &dest[index + offset]. Only the words
 * this TU's callers of scenarioDstGet touch are modeled:
 *   +0x00 unit: scenarioBatEnd reads unit->work (+0x10) and clears bit 0x4000
 *         in its first word; scenarioBatExecMain passes the word at unit+0x14
 *         to sefIsSeSignal.
 *   +0x08: battleExec copies it into statUnit.hpBonus.
 *   +0x14: scenarioBatExecPhase30 zeroes it after scenarioEffCall.
 */
struct ScenarioDstEntry {
    BattleUnit *unit;                 /* +0x00 */
    unsigned char unmodeled_04[4];
    int word08;                       /* +0x08 */
    int word0c;                       /* +0x0C: scenarioBatEnd's dst[3] */
    unsigned char unmodeled_10[4];
    int word14;                       /* +0x14 */
    int word18;                       /* +0x18: scenarioBatEnd's dst[6] */
    int word1c;                       /* +0x1C: scenarioBatEnd's dst[7] */
    int word20;                       /* +0x20: scenarioBatEnd's dst[8] */
    int word24;                       /* +0x24: scenarioBatEnd's dst[9] */
};

/*
 * The record limit scenarioDstSet and scenarioDstGet both test (`slti 0xD`).
 * scenarioDst itself spans 0x1E0 bytes, twelve records; a thirteenth accepted
 * record would land on scenarioPtr.
 */
#define SCENARIO_DST_LIMIT 13

/*
 * Scenario-battle phase-dispatch and cursor record. scenarioBatInit and every
 * scenarioBatExecPhase* handler call scenarioPtrFuncSet to install the next
 * phase's handler and reset its frame counter; that function writes nextPhase
 * (+0x10, the callback the caller passes in $a0) and zeroes phaseTimer (+0x14)
 * on every phase transition. scenarioBatNext adds source->stride into index,
 * clamps index to count once their sum reaches it, and reports whether the
 * clamp fires.
 *
 * scenarioBatInit also writes +0x08, the destination buffer scenarioBatInit
 * pairs with source: both are set from the fixed scenarioSrc/scenarioDst
 * arrays.
 */
typedef struct ScenarioBatState {
    int count;                    /* +0x00: index is clamped to this */
    int index;                    /* +0x04: advanced by source->stride each call */
    ScenarioDstEntry *dest;       /* +0x08: scenarioDstSet appends at dest[count] */
    ScenarioStep *source;         /* +0x0C */
    ScenarioPhaseFunc nextPhase; /* +0x10 */
    int phaseTimer;              /* +0x14 */
} ScenarioBatState;

/*
 * Private battle state, in original BSS address order. Keeping these local
 * definitions together lets the guarded overlay linker map each exact object
 * to its original storage span.
 */
static ObjectTask *pTaskEnCur;
static BattleUnit *pActUnit;
static int oldCurGrp;
static int keyBuffIdx;
#define KEY_BUFF_COUNT 8
static int keyBuff[KEY_BUFF_COUNT];
static int batMainFlag;
static ScenarioStep scenarioSrc[8];
static ScenarioDstEntry scenarioDst[12];
static ScenarioBatState scenarioPtr;
static int helpFlag;
typedef struct StatUnit {
    BattleUnit *unit;
    int cmd;
    int hpBonus;
} StatUnit;
static StatUnit statUnit;
static CamEvent camEv;
static int batRetCode;

/*
 * batRetCode, read by battleAfter and by entry_first_init.c through
 * battleRetCodeGet. battleEndChk writes WIN (mode 1 with the enemy side
 * empty, which stores the mode itself), LOSE (the player side empty) or
 * ESCAPE (escapeFlag, or the cfEvent bits 0x10/0x20). battleAfter prints the
 * player-win text for WIN, the escape text for ESCAPE and the enemy-win text
 * for the remaining value.
 */
enum BattleResult {
    BATTLE_RESULT_NONE = 0,
    BATTLE_RESULT_WIN = 1,
    BATTLE_RESULT_LOSE = 2,
    BATTLE_RESULT_ESCAPE = 3
};

static int fifoCmd[6];
extern void fifoInit(void *fifo, int capacity);
static BattleUnit *pTgtUnit;
static BattleUnit *pTgtUnit2;
static BattleUnit *pExecUnit;
static BattleUnit *pManUnit;
static BattleUnit *pCounterUnit;
static int thinkWk;
static int menuFlag;
extern void objInit(void);
extern void unitInit(void);
extern void thinkSysInit(void);
extern void dataFileInfoInit(void);
extern void keyBuffClear(void);
extern void dataBattleInit(void);
extern void scenarioBatInit(void);
extern void monsGainInit(void);
extern void sndInit(void);
extern void MBattleCreate(void);
extern void menuInitBat(void);
extern void dataVPadModeSet(int enable);
extern ObjectTask *objEnCurCreate(float x, float y, float z);

void battleInit(void)
{
  ObjectTask *task;
  objInit();
  unitInit();
  thinkSysInit();
  dataFileInfoInit();
  task = objEnCurCreate(0.0f, 0.0f, 0.0f);
  *((int *) task->work) |= 4;
  pTaskEnCur = task;
  pActUnit = 0;
  pExecUnit = 0;
  pTgtUnit = 0;
  pTgtUnit2 = 0;
  pManUnit = 0;
  pCounterUnit = 0;
  pLastUnit = 0;
  fifoInit(&fifoCmd, 0x20);
  keyBuffClear();
  manWk.unitTask = 0;
  thinkWk = 0;
  batCtrl.field04 = BATTLE_PHASE_WAIT_BUSY;
  batMainFlag = 0;
  manWk.controlPhase = (int) 0;
  menuFlag = 0;
  helpFlag = 0;
  escapeFlag = 0;
  manWk.field04 = 0;
  manWk.tgtSelect = 0;
  batCtrl.field00 = BATTLE_CTRL_ACT_SELECT;
  batCtrl.field08 = BATTLE_PHASE_EXEC_QUEUE;
  dataBattleInit();
  actStockInit();
  if (!task->work)
  {
  }
  do
  {
    scenarioBatInit();
    monsGainInit();
    sndInit();
    MBattleCreate();
    menuInitBat();
  }
  while (0);
  dataVPadModeSet(0);
}

void battleCtrl(void);
void battleExec(void);
int battleEndChk(void);

/* Runs one battle tick: sets batMainFlag, then battleCtrl, battleExec and battleEndChk. */
void battleMain(void)
{
    /* Separate load keeps this store in its own register. */
    int flag = 1;
    batMainFlag = flag;
    battleCtrl();
    battleExec();
    battleEndChk();
}

extern const char battleInitPLWinText[];
extern const char battleInitPLEscText[];
extern const char battleInitENWinText[];
extern void menuStatDispRemove(void);
extern int busyUnitChk(int side);
extern int battleRetCodeGet(void);
void objCurRemove(void);

/* Plays the end-of-battle message for the battle result and tears down the battle display. */
int battleAfter(void)
{
  int rv;
  CmdEntry cmd;
  CamEvent *ev;
  BattleUnit **unitTable;
  int eventType;
  int count;
  int i;
  int ret;
  BattleUnit *unit;
  ret = battleRetCodeGet();
  if ((ev && ev) && ev)
  {
  }
  /* Win: the first call queues the win event; later calls wait for input or busy units. */
  if (ret == BATTLE_RESULT_WIN)
  {
    if (batMainFlag == ret)
    {
      char *msg;
      i = 0;
      printf(battleInitPLWinText);
      batMainFlag = 2;
      (&camEv)->unit = pLastUnit;
      eventType = 5;
      do
      {
        ev = &camEv;
        camEv.type = eventType;
        batMainFlag = 2;
        ((void (*)(CamEvent *, BattleUnit *))camEventExec)(ev, pLastUnit);
        count = unitTblGet(0, &unitTable);
      }
      while (0);
      if (count > 0)
      {
        do
        {
          unit = unitTable[i];
          i++;
          if ((unit != 0) && (!((*((int *) unit->work)) & 0x80)))
          {
            cmd.selector = CMD_BTED;
            unitCmdSet(unit, &cmd);
          }
        }
        while (i < count);
      }
      if ((battleInitENWinText && battleInitENWinText) && battleInitENWinText)
      {
      }
      menuStatDispRemove();
      objCurRemove();
      return 1;
    }
    if (dataPadRead(1) & PAD_CIRCLE)
    {
      goto zero;
    }
    if (busyUnitChk(2) != 0)
    {
      goto one;
    }
    goto zero;
    one:
    goto _sw_tl2;

    zero:
    return 0;

  }
  if (ret == BATTLE_RESULT_ESCAPE)
  {
    printf(battleInitPLEscText);
    batMainFlag = 2;
    menuStatDispRemove();
    objCurRemove();
    return 0;
  }
  printf(battleInitENWinText);
  batMainFlag = (double) 2;
  menuStatDispRemove();
  objCurRemove();
  ev = &camEv;
  return 0;
  _sw_tl2:
  return 1;

}

extern int dataPadRead(int padNo);
int battleCtrlHelp(void);
int battleCtrlBoostPl(void);
int battleCtrlConfusion(BattleUnit *unit);
int battleCtrlMan(BattleUnit *unit, int *inp);
int battleCtrlThink(BattleUnit *unit);
int battleCtrlJunk(BattleUnit *unit);
int unitLiveNumGet2(int kind);
int actUnitTblMake(int side, BattleUnit **actUnits);
int rnd(int max);
ManWork *manWorkGet(void);
int battleCtrlBoostEn(void);
BattleUnit *actStockPick(void);
void menuStockNext(void);
void menuStockStart(void);
void menuStockBoost(void);
void calcTurnStart(BattleUnit *unit);
void thinkTurnStartExec(BattleUnit *unit);
void camEventTurnStartSet(void);
int thinkExecChk(void);
int battleMsgEndChk(void);
/*
 * The action-stock record (0x24 bytes, the span actStockGet indexes).
 * actStockRemove compares the unit against the boost slots and the first
 * `max` (+0x18) stock words from +0x00. The actor words are pointers: only
 * with pointer-typed loads does the scheduler hoist the +0x10 load above the
 * `*index = -1` int store, as the original does. calc.c (CalcActStock,
 * src/ov01/calc.h) and menu.c (actStockGet()[4]/[5]) read the same storage
 * through views of their own.
 */
typedef struct ActStock {
    BattleUnit *stock[4]; /* +0x00 */
    BattleUnit *boost[2]; /* +0x10: calcBoost's per-mode boosting actor */
    int max;              /* +0x18: the limit actStockMaxGet returns */
    int pickCursor;       /* +0x1C: stock slots handed out so far (actStockPick) */
    BattleUnit *lastPicked; /* +0x20: the unit actStockPick returned last */
    int pickSource;       /* +0x24: 1 after a refill, 2 when a boost slot was taken */
} ActStock;
static ActStock actStock;

/*
 * One battle-control tick: runs the help toggle, then steps batCtrl.field00
 * (choose a command, pick the next actor or wait for the queued command).
 */
void battleCtrl(void)
{
    BatCtrl *bc;
    int cmd[8];
    int ret;
    int st;

    if (dataPadRead(1) & (PAD_L1 | PAD_L2)) {
        battleCtrlHelp();
    }
    if (battleCtrlBoostPl() != 0) {
        return;
    }
    switch (batCtrl.field00) {
    case BATTLE_CTRL_ACT_SELECT:
        bc = &batCtrl;
        if (unitLiveNumGet2(0) == 0) {
            break;
        }
        if (unitLiveNumGet2(1) == 0) {
            break;
        }
        if (actUnitTblMake(2, (BattleUnit **)cmd) == 0) {
            bc->field00 = BATTLE_CTRL_STOCK_PICK;
            manWorkGet()->unitTask = 0;
            break;
        }
        if (calcStatGet(pActUnit, 1, 0x4000) != 0 || calcStatGet(pActUnit, 5, 0x1000) != 0) {
            ret = battleCtrlConfusion(pActUnit);
        } else if (calcStatGet(pActUnit, 4, 0x40) != 0 && rnd(100) < 50) {
            ret = battleCtrlJunk(pActUnit);
        } else {
            goto normal;
        }
        if (ret == 0) {
            bc->field00 = BATTLE_CTRL_ACT_WAIT;
        }
        break;
    normal:
        /* Normal turn: the AI think routine (flag 0x20) or the player command menu picks the command. */
        if (pActUnit->work->flags & 0x20) {
            ret = ((int (*)(BattleUnit *, int *))battleCtrlThink)(pActUnit, &thinkWk);
        } else {
            ret = battleCtrlMan(pActUnit, (int *)manWorkGet());
        }
        if (ret == 0) {
            batCtrl.field00 = BATTLE_CTRL_ACT_WAIT;
        }
        break;
    case BATTLE_CTRL_STOCK_PICK:
        {
            BattleUnit *pick = actStockPick();
            pActUnit = pick;
            if (pick == 0) {
                break;
            }
        }
        if (actStock.pickSource == 0) {
            menuStockNext();
        } else if (actStock.pickSource == 1) {
            menuStockStart();
        } else {
            menuStockBoost();
        }
        pActUnit->work->flags |= 8;
        calcTurnStart(pActUnit);
        thinkTurnStartExec(pActUnit);
        camEventTurnStartSet();
        battleCtrlBoostEn();
        batCtrl.field00 = BATTLE_CTRL_ACT_SELECT;
        break;
    case BATTLE_CTRL_ACT_WAIT:
        if (pActUnit->work->flags & 8) {
            break;
        }
        st = batCtrl.field04;
        if (st != BATTLE_PHASE_EXEC_QUEUE) {
            break;
        }
        if (thinkExecChk() == 0) {
            break;
        }
        if (battleMsgEndChk() == 0) {
            break;
        }
        batCtrl.field00 = st;
        camEv.type = 7;
        camEv.target = 0;
        camEv.param = (*(int *)((union { void *p; int i; } *)&pActUnit->work)->p >> 6) & 1;
        camEv.unit = pActUnit;
        camEventExec(&camEv);
        break;
    }
}

typedef struct ConfTgt {
    int slot;
    int wpn;
    BattleUnit *tgt;
} ConfTgt;
static ConfTgt atkTbl_0[48];

/*
 * Confused unit's turn: queues an attack on a random living unit that one of
 * its weapons or techniques can reach, then the end-of-turn command. Returns 0.
 */
int battleCtrlConfusion(BattleUnit *unit)
{
    int cmd[8];
    BattleUnit **units;
    int cnt;
    int count;
    int slot;
    int k;
    int wpn;
    int no;
    int sp24;

    count = 0;
    cnt = unitTblGet(2, &units);
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        slot = 0;
        do {
            no = calcUPGet(unit)->weaponId[slot];
            if (no != 0) {
                wpn = dataWpnGet(no)->tecId;
                for (k = 0; k < cnt; k++) {
                    if (units[k] != 0 && !(units[k]->work->flags & ACTOR_FLAG_DEAD) && units[k] != unit && calcAtkChk(unit, units[k], wpn) != 0) {
                        atkTbl_0[count].slot = slot;
                        atkTbl_0[count].wpn = wpn;
                        atkTbl_0[count].tgt = units[k];
                        count++;
                    }
                }
            }
            slot++;
        } while (slot < 3);
    } else {
        if ((unit->work->flags & ACTOR_FLAG_ENEMY) || (calcUPGet(unit)->weaponId[0] == 0)) {
            sp24 = -1;
        } else {
            sp24 = 0;
        }
        slot = 0;
        do {
            wpn = calcUPGet(unit)->normalTechniqueId[slot];
            if (wpn != 0) {
                for (k = 0; k < cnt; k++) {
                    if (units[k] != 0 && !(units[k]->work->flags & ACTOR_FLAG_DEAD) && units[k] != unit && calcAtkChk(unit, units[k], wpn) != 0) {
                        atkTbl_0[count].slot = sp24;
                        atkTbl_0[count].wpn = wpn;
                        atkTbl_0[count].tgt = units[k];
                        count++;
                    }
                }
            }
            slot++;
        } while (slot < 6);
    }
    if (count > 0) {
        int r = rnd(count - 1);
        cmd[0] = CMD_ATK;
        cmd[1] = atkTbl_0[r].wpn;
        cmd[2] = (int)atkTbl_0[r].tgt;
        cmd[3] = atkTbl_0[r].slot;
        unitCmdListSet(unit, cmd);
    }
    cmd[0] = CMD_END;
    unitCmdListSet(unit, cmd);
    return 0;
}

/* 35-byte battle message buffer; the MsgText copy below is a block move. */
typedef struct MsgText {
    signed char text[0x23];
} MsgText;
static MsgText msgWork;
extern const MsgText battleInitUnableToMoveMsg;
void battleMsg(int msgId);

/* Shows the unable-to-move message and queues an end-of-turn command for the unit. */
int battleCtrlJunk(BattleUnit *unit)
{
    int cmd[8];

    msgWork = battleInitUnableToMoveMsg;
    battleMsg((int)&msgWork);
    cmd[0] = CMD_END;
    unitCmdListSet(unit, cmd);
    return 0;
}


extern const char battleInitMakeMaxNumText[];
extern const char battleInitUIDText[];
extern const char battleInitJpText[];
extern const char battleInitSP0Text[];
extern const char battleInitSP1Text[];
extern const char battleInitPickIDText[];
extern int calcWctDec(BattleUnit *unit);
extern int calcBattleChk(BattleUnit *unit);
extern int unitTblGet(int side, BattleUnit ***table);
extern const char battleInitCurEnProcNoTgtText[];
int actStockMaxGet(void);
void actStockMake(void);

/* Clears the action stock at battle start: the limit, the cursors and the slots. */
void actStockInit(void)
{
    int i;

    actStock.max = actStockMaxGet();
    actStock.pickCursor = 0;
    actStock.lastPicked = 0;
    actStock.pickSource = 0;
    for (i = 0; i < 4; i++) {
        actStock.stock[i] = 0;
    }
    for (i = 0; i < 2; i++) {
        actStock.boost[i] = 0;
    }
}

/*
 * The action-stock limit: half the units on side 2 still able to fight,
 * rounded up, capped at 4.
 */
int actStockMaxGet(void)
{
    BattleUnit **table;
    int count;
    int i;
    int fighters;

    fighters = 0;
    count = unitTblGet(2, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && calcBattleChk(table[i])) {
            fighters++;
        }
    }
    if (fighters > 0) {
        if (fighters * 5 % 10 >= 5) {
            fighters = fighters / 2 + 1;
        } else {
            fighters = fighters / 2;
        }
    }
    if (fighters > 4) {
        fighters = 4;
    }
    return fighters;
}

/*
 * Refills the action stock from the side-2 units that can still act: each
 * unit taken gets bit 0x10 of its actor flags, up to the stock limit. The
 * stock is printed afterwards, with each unit's character id.
 */
void actStockMake(void)
{
    BattleUnit **table;
    int count;
    int n;
    int i;
    BattleUnit **q;
    int *p;
    BattleUnit *unit;

    n = 0;
    i = 3;
    p = (int *)&actStock;
    p[6] = actStockMaxGet();
    p[7] = 0;
    count = unitTblGet(2, &table);
    p = (int *)&actStock;
    do {
        p[i] = 0;
        i--;
    } while (i >= 0);
    if (actStock.max > 0) {
        do {
            i = 0;
            if (count > 0) {
                do {
                    if (table[i] != 0 && calcBattleChk(table[i]) != 0
                        && (table[i]->work->flags & 0x10) == 0
                        && calcWctDec(table[i]) == 0) {
                        unit = table[i];
                        actStock.stock[n] = unit;
                        unit->work->flags |= 0x10;
                        n = n + 1;
                        if (n >= actStock.max) {
                            goto done;
                        }
                    }
                    i++;
                } while (i < count);
            }
        } while (n < actStock.max);
    }
done:
    printf(battleInitMakeMaxNumText, actStock.max, n);
    i = 0;
    if (n > 0) {
        q = actStock.stock;
        do {
            printf(battleInitUIDText, i, *q, calcUPGet(*q)->charaId);
            q++;
            i++;
        } while (i < n);
    }
    printf(battleInitJpText);
}

/*
 * Hands out the next acting unit: the boost slots first, then the stock in
 * order, refilling the stock when it runs out. A unit with bit 0x80 in its
 * actor flags is skipped. The picked unit is kept in lastPicked.
 */
BattleUnit *actStockPick(void)
{
    BattleUnit *unit;
    int c;

    actStock.pickSource = 0;
    do {
        if (actStock.max <= actStock.pickCursor && actStock.boost[0] == 0 && actStock.boost[1] == 0) {
            actStockMake();
            actStock.pickSource = 1;
        }
        if (actStock.boost[0] != 0) {
            unit = actStock.boost[0];
            actStock.pickSource = 2;
            actStock.boost[0] = 0;
            printf(battleInitSP0Text);
            if (actStock.pickCursor > 0) {
                actStock.stock[actStock.pickCursor - 1] = 0;
            }
        } else if (actStock.boost[1] != 0) {
            unit = actStock.boost[1];
            actStock.boost[1] = 0;
            actStock.pickSource = 2;
            printf(battleInitSP1Text);
            if (actStock.pickCursor > 0) {
                actStock.stock[actStock.pickCursor - 1] = 0;
            }
        } else {
            c = actStock.pickCursor;
            unit = actStock.stock[c];
            if (c > 0) {
                actStock.stock[c - 1] = 0;
            }
            actStock.pickCursor = c + 1;
        }
        if (unit == 0 || (unit->work->flags & ACTOR_FLAG_DEAD)) {
            unit = 0;
        }
    } while (unit == 0);
    printf(battleInitPickIDText, unit, calcUPGet(unit)->charaId, actStock.pickCursor, actStock.max);
    actStock.lastPicked = unit;
    return unit;
}

/*
 * Drop `unit` from the boost slots and the stock; *index receives its stock
 * position, or -1. Nonzero when any slot held it.
 */
int actStockRemove(BattleUnit *unit, int *index)
{
    int removed = 0;
    int i;

    *index = -1;
    if (actStock.boost[0] == unit) {
        actStock.boost[0] = 0;
        removed = 1;
    }
    if (actStock.boost[1] == unit) {
        actStock.boost[1] = 0;
        removed = 1;
    }
    for (i = 0; i < actStock.max; i++) {
        if (actStock.stock[i] == unit) {
            actStock.stock[i] = 0;
            removed = 1;
            *index = i;
            break;
        }
    }
    return removed;
}

/*
 * Returns the action-stock array. calc.c and menu.c read the same storage
 * through their own declarations.
 */
BattleUnit **actStockGet(void)
{
    return actStock.stock;
}

/* Number of battle help pages; battleCtrlHelp cycles helpFlag through them. */
#define BATTLE_HELP_PAGE_COUNT 3

int battleCtrlHelp(void)
{
    helpFlag = (helpFlag + 1) % BATTLE_HELP_PAGE_COUNT;
    return 1;
}

int battleCtrlHelpGet(void)
{
    return helpFlag;
}

extern int unitTblGet(int side, BattleUnit ***table);
extern int calcBoostChk(BattleUnit *unit, int mode);

/* Text: "** BOOST PL=%d (%d)\n" */
extern const char battleInitBOOSTPLText[];

extern int dataPadRead(int padNo);
extern void menuBoostDisp(int enabled);

/*
 * Player boost: while R1 or R2 is held, shows the boost menu; a face button
 * (square, triangle or circle) picks a unit whose boost check passes and boosts
 * it. Returns 1 when a boost was applied, 0 otherwise.
 */
int battleCtrlBoostPl(void)
{
    BattleUnit **table;
    int selection;
    int held;

    held = dataPadRead(1);
    if (dataPadRead(0) & (PAD_R1 | PAD_R2)) {
        menuBoostDisp(1);
        selection = -1;
        if (held & PAD_SQUARE) {
            selection = 0;
        } else if (held & PAD_TRIANGLE) {
            selection = 1;
        } else if (held & PAD_CIRCLE) {
            selection = 2;
        }

        unitTblGet(0, &table);
        if (selection != -1 && table[selection] != 0 &&
            calcBoostChk(table[selection], 1) != 0) {
            printf(battleInitBOOSTPLText, calcUPGet(table[selection])->charaId,
                   calcUPGet(table[selection])->bcValue);
            calcBoost(table[selection], 1);
            return 1;
        }
        return 0;
    }

    menuBoostDisp(0);
    return 0;
}

extern int rnd(int max);

/*
 * Enemy AI boost pass: boosts the first unit on side 1 whose boost check passes
 * and whose random roll falls under its boost chance. Returns 1 when a unit was
 * boosted, 0 otherwise.
 */
int battleCtrlBoostEn(void)
{
    BattleUnit **table;
    int count;
    int chance;
    int i;

    count = unitTblGet(1, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            chance = ((signed char *)calcUPGet(table[i]))[CALC_PARAM_BOOST_CHANCE_OFF];
            if (calcBoostChk(table[i], 1) != 0 && rnd(100) < chance) {
                printf(s_battleCtrlBoostEnFormat, calcUPGet(table[i])->charaId, chance,
                       calcUPGet(table[i])->bcValue);
                calcBoost(table[i], 1);
                return 1;
            }
        }
    }
    return 0;
}

/*
 * The ether record dataEthGet returns (src/ov01/data_unit_org_get.c), as far as
 * convKey2unit reads it: the technique id at +6.
 */
typedef struct EtherView {
    unsigned char unmodeled_00[6];
    short tecId; /* +6 */
} EtherView;
extern EtherView *dataEthGet(int etherId);

/*
 * dataTecGet's return, as far as this TU reads it: a byte at +0x00 whose
 * bit 0x10 selects between the selector-0x2C and selector-0xF commands in
 * syoukan1Sub and ends scenarioCam, and whose low two bits are scenarioCam's
 * range class; +0x11 is the signed byte data_unit_org_get.c's TecRecord names
 * `kind`. data_unit_org_get.c, dataTecGet's own TU, owns the layout.
 */
typedef struct TecFlagView {
    unsigned char flags;                      /* +0x00 */
    unsigned char unmodeled_01[0x02 - 0x01];
    u16 procFlags; /* +0x02: with bit 0x20 (TEC_PROC_FLOW_SPEC) set,
                    * scenarioSrcEndChk returns the caller's result; calc.c's
                    * TecKindView names the same u16 flags */
    unsigned char unmodeled_04[0x11 - 0x04];
    signed char kind;                         /* +0x11 */
    unsigned char unmodeled_12[0x16 - 0x12];
    short effectNo; /* +0x16: scenarioBatExecPhase50 waits on this effect,
                     * passed to sefIsFinishEffect */
} TecFlagView;

extern TecFlagView *dataTecGet(int tecId);

/*
 * dataItmGet's return as far as scenarioCam reads it: the item's
 * target-selection flags byte; calc.c's ItmTrgView names the same member.
 */
typedef struct ItmTrgView {
    unsigned char unmodeled_00[0x08];
    u8 trgFlags; /* +0x08 */
    unsigned char unmodeled_09[0x10 - 0x09];
    short effectNo; /* +0x10: the item's effect, passed to sefIsFinishEffect */
} ItmTrgView;

extern ItmTrgView *dataItmGet(int itmId);

extern void camEventExec(CamEvent *event);
/*
 * The unit's actor record read from ObjectTask.work as an int-sized word,
 * menu.c's menuStockWorkGet idiom: unitCmdListSet reloads the slot after the
 * int store that sets bit 0x100; read as the pointer, gcc keeps the first load.
 */
static __inline__ Actor *unitActorGet(ObjectTask *task)
{
    return (Actor *)*(u32 *)&task->work;
}

/*
 * Key-command ids: ManWork.field04 (the word battleCtrlMan reads as inp[1])
 * and the value keyCmdApGet/keyCmdChk take. convKey2unit decodes 0..19 as
 * weapon slots (ATK through keyCmd2wno), 20..25 as the command kinds below
 * and 26 as END. The names follow those decodes and the manInput states that
 * produce them; no debug string names them.
 */
enum BattleKeyCommand {
    KEY_CMD_NONE = 0,
    KEY_CMD_WEAPON_LIMIT = 20,     /* ids below this are weapon slots */
    KEY_CMD_ATTACK = 0x13,         /* the battle menu's attack entry */
    KEY_CMD_ETHER = 0x14,
    KEY_CMD_ITEM = 0x15,
    KEY_CMD_GUARD = 0x16,
    KEY_CMD_AGWS = 0x17,
    KEY_CMD_MOVE = 0x18,
    KEY_CMD_ITEM_WIDE = 0x19,      /* item on the wide target (two box uses) */
    KEY_CMD_END = 0x1A,
    KEY_CMD_CANCEL = 0x1B          /* no command this frame (also tested as 0) */
};

/*
 * ManWork.tgtSelect, the input state manInput switches on. Names follow what
 * each state does; the battle-menu entries map menuResultBat's results 1..6
 * to ETH, ITEM, GUARD, AGWS, MOVE and ATTACK targeting.
 */
enum ManSelectState {
    MAN_SELECT_ROOT = 0,           /* no menu: CROSS opens the battle menu, a key picks a command */
    MAN_SELECT_KEY_PICKED = 1,     /* a weapon key was chosen: read its AP cost */
    MAN_SELECT_BAT_OPEN = 2,       /* open the battle menu (also the CROSS return target) */
    MAN_SELECT_BAT_MENU = 3,       /* battle menu is open and reports its result */
    MAN_SELECT_COMMAND_DONE = 4,   /* command decided (or menu exit): report the key */
    MAN_SELECT_ETH_OPEN = 5,
    MAN_SELECT_ETH_MENU = 6,
    MAN_SELECT_ETH_TARGET = 7,
    MAN_SELECT_ITM_OPEN = 8,
    MAN_SELECT_ITM_MENU = 9,
    MAN_SELECT_ITM_TARGET = 0xA,
    MAN_SELECT_ITM_TARGET_WIDE = 0xB,
    MAN_SELECT_GUARD = 0xC,        /* GUARD key straight away */
    MAN_SELECT_AGWS_POS = 0xD,     /* position select for AGWS (curPosProc mode 5) */
    MAN_SELECT_MOVE_POS = 0xE,     /* position select for MOVE (curPosProc mode 6) */
    MAN_SELECT_ATTACK_TARGET = 0xF /* enemy target select for the attack entry */
};

extern int manInput(BattleUnit *unit, ManWork *work);
extern const char battleInitMENUEXITText[];
extern void menuMapHSet(int enable);
extern int sndSysSePlay(int soundId);
extern int calcDItemChk(BattleUnit *unit, int itmId);
extern void menuOpenBat(BattleUnit *unit);
extern void menuCloseBat(void);
extern int menuResultBat(void);
extern void menuOpenEth(BattleUnit *unit);
extern void menuCloseEth(void);
extern int menuResultEth(int *ethId);
extern void menuOpenItm(BattleUnit *unit);
extern void menuCloseItm(void);
extern int menuResultItm(int *itmId);
extern int keyCmdGet(BattleUnit *unit);
extern int keyCmdChk(BattleUnit *unit, BattleUnit *target, int command);
extern int curPosProcInit(BattleUnit *unit, int mode);
extern int curPosProc(BattleUnit *unit, int *posId);
extern int curTgtProcInit(BattleUnit **target, BattleUnit *self, int flags);
extern int curTgtProc(BattleUnit **target, int targetFlags);
extern int calcWpnUse(BattleUnit *unit, int idx);
extern int calcAp(BattleUnit *unit, int delta);

int menuChk(void);
void curSelGrpSet(u32 group, int flags);
int curEnProcInit(BattleUnit **target, int side);
int curEnProc(BattleUnit **target);
void tgtUnitSet(BattleUnit *unit);
int convKey2unit(BattleUnit *unit, int *command, int *input, int target);
void keyBuffClear(void);

/*
 * Player command input for one unit: runs manInput for the menu state and,
 * when a key command is chosen, queues it on the unit and charges its AP.
 * Returns 0 when the turn ends with the END command, 1 otherwise.
 */
int battleCtrlMan(BattleUnit *unit, int *inp)
{
  int bit;
  int enemy;
  BattleUnit *tgt;
  int cmd[8];
  int ap;
  bit = *(int *) unit->work & 0x40;
  enemy = bit == 0;
  tgt = tgtUnitGet();
  if (inp[0] != 1)
  {
    if (menuChk() == 0)
    {
      pManUnit = unit;
      pActUnit = unit;
      curSelGrpSet(1, 0);
      curEnProcInit(&tgt, enemy);
      if (curEnProc((BattleUnit **) &tgt) != 0)
      {
        CamEvent *ev = &camEv;
        ev->type = 8;
        ev->unit = pActUnit;
        ev->target = *(BattleUnit * volatile *) &tgt;
        camEventExec(ev);
      }
      tgtUnitSet(tgt);
      unitActorGet(pTaskEnCur)->flags &= ~4;
    }
  }
  ap = manInput(unit, (ManWork *) inp);
  if (inp[1] == KEY_CMD_END)
  {
    cmd[0] = CMD_END;
    unitCmdListSet(unit, cmd);
    keyBuffClear();
    inp[0] = 0;
    inp[3] = inp[1];
    return 0;
  }
  if ((inp[1] != KEY_CMD_NONE) && (inp[1] != KEY_CMD_CANCEL))
  {
    convKey2unit(unit, cmd, inp, (int) tgt);
    unitCmdListSet(unit, cmd);
    if (cmd[0] == CMD_ATK)
    {
      calcWpnUse(unit, cmd[3]);
    }
    calcAp(unit, -ap);
  }
  return 1;
}

/*
 * Input state machine for one unit's command: steps work->tgtSelect through the
 * key, battle, ether, item, target and position menus. Stores the chosen key in
 * work->field04 and returns the AP the command costs.
 */
int manInput(BattleUnit *unit, ManWork *work)
{
  int pad;
  int cmd;
  int ap;
  int trgFlags;
  int selection;
  int result;
  BattleUnit *tgt[1];
  BattleUnit *target;

  cmd = 0;
  ap = 0;
  pad = dataPadRead(1);
  if (work->unitTask != unit)
  {
    work->controlPhase = 0;
  }
  else if (work->field04 != KEY_CMD_CANCEL && work->field04 != KEY_CMD_NONE)
  {
    work->controlPhase = work->field04;
  }
  /* One case per ManWork.tgtSelect state (see enum ManSelectState). */
  switch (work->tgtSelect)
  {
  case MAN_SELECT_ROOT:
    menuMapHSet(0);
    unitActorGet(pTaskEnCur)->flags &= ~4;
    menuFlag = 0;
    if (pad & PAD_CROSS)
    {
      unitActorGet(pTaskEnCur)->flags |= 4;
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
      menuFlag = 1;
    }
    else
    {
      cmd = keyCmdGet(unit);
      if (cmd != KEY_CMD_CANCEL && cmd != KEY_CMD_NONE)
      {
        menuMapHSet(1);
        unitActorGet(pTaskEnCur)->flags |= 4;
        work->tgtSelect = MAN_SELECT_KEY_PICKED;
        ap = keyCmdApGet(unit, cmd);
      }
    }
    break;
  case MAN_SELECT_KEY_PICKED:
    cmd = keyCmdGet(unit);
    ap = keyCmdApGet(unit, cmd);
    break;
  case MAN_SELECT_BAT_OPEN:
    unitActorGet(pTaskEnCur)->flags |= 4;
    menuOpenBat(unit);
    work->tgtSelect = MAN_SELECT_BAT_MENU;
    camEv.type = 0x12;
    camEv.unit = pActUnit;
    camEventExec(&camEv);
    break;
  case MAN_SELECT_BAT_MENU:
    if (pad & PAD_CROSS)
    {
      menuCloseBat();
      work->tgtSelect = MAN_SELECT_ROOT;
      camEventTurnStartSet();
    }
    else
    {
      int command = menuResultBat();
      if (command != 0)
      {
        switch (command)
        {
        case 1:
          menuCloseBat();
          menuOpenEth(unit);
          work->tgtSelect = MAN_SELECT_ETH_OPEN;
          camEv.type = 0xD;
          break;
        case 2:
          menuCloseBat();
          menuOpenItm(unit);
          work->tgtSelect = MAN_SELECT_ITM_OPEN;
          camEv.type = 0xE;
          break;
        case 3:
          menuCloseBat();
          work->tgtSelect = MAN_SELECT_GUARD;
          camEv.type = 0xF;
          break;
        case 4:
          menuCloseBat();
          work->tgtSelect = MAN_SELECT_AGWS_POS;
          camEv.type = 0x10;
          break;
        case 5:
          menuCloseBat();
          work->tgtSelect = MAN_SELECT_MOVE_POS;
          camEv.type = 0x11;
          break;
        case 6:
          menuCloseBat();
          work->tgtSelect = MAN_SELECT_ATTACK_TARGET;
          camEv.type = 0x16;
          break;
        }
        camEv.unit = pActUnit;
        camEventExec(&camEv);
      }
    }
    break;
  case MAN_SELECT_COMMAND_DONE:
    cmd = KEY_CMD_END;
    printf(battleInitMENUEXITText);
    work->tgtSelect = MAN_SELECT_ROOT;
    menuFlag = 0;
    menuMapHSet(1);
    unitActorGet(pTaskEnCur)->flags |= 4;
    break;
  case MAN_SELECT_ETH_OPEN:
    menuResultEth(&selection);
    if (selection != -1)
    {
      work->tgtSelect = MAN_SELECT_ETH_MENU;
    }
    break;
  case MAN_SELECT_ETH_MENU:
    pad = menuResultEth(&selection);
    work->selection = selection;
    if ((pad & PAD_CIRCLE) && selection != 0)
    {
      menuCloseEth();
      tgt[0] = tgtUnitGet();
      camEv.type = 9;
      camEv.unit = pActUnit;
      camEv.target = tgt[0];
      camEventExec(&camEv);
      curSelGrpSet(-1, dataTecGet(dataEthGet(work->selection)->tecId)->flags);
      work->tgtSelect = MAN_SELECT_ETH_TARGET;
    }
    else if (pad & PAD_CROSS)
    {
      menuCloseEth();
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
    }
    break;
  case MAN_SELECT_ETH_TARGET:
    trgFlags = dataTecGet(dataEthGet(work->selection)->tecId)->flags;
    tgt[0] = tgtUnitGet();
    curTgtProcInit(tgt, unit, trgFlags);
    if (curTgtProc(tgt, trgFlags) != 0)
    {
      camEv.unit = pActUnit;
      camEv.type = 9;
      camEv.target = tgt[0];
      camEventExec(&camEv);
    }
    tgtUnitSet(tgt[0]);
    unitActorGet(pTaskEnCur)->flags &= ~4;
    if (pad & PAD_CIRCLE)
    {
      ap = keyCmdApGet(unit, KEY_CMD_ETHER);
      work->tgtSelect = MAN_SELECT_COMMAND_DONE;
      cmd = KEY_CMD_ETHER;
      unitActorGet(pTaskEnCur)->flags |= 4;
      sndSysSePlay(1);
    }
    else if (pad & PAD_CROSS)
    {
      work->tgtSelect = MAN_SELECT_ETH_OPEN;
      menuOpenEth(unit);
      camEv.type = 0xD;
      camEv.unit = pActUnit;
      camEventExec(&camEv);
      unitActorGet(pTaskEnCur)->flags |= 4;
      sndSysSePlay(2);
    }
    break;
  case MAN_SELECT_ITM_OPEN:
    menuResultItm(&selection);
    if (selection != -1)
    {
      work->tgtSelect = MAN_SELECT_ITM_MENU;
    }
    break;
  case MAN_SELECT_ITM_MENU:
    pad = menuResultItm(&selection);
    work->selection = selection;
    if ((pad & PAD_CIRCLE) && selection != 0)
    {
      menuCloseItm();
      tgt[0] = tgtUnitGet();
      camEv.type = 0xA;
      camEv.unit = pActUnit;
      camEv.target = tgt[0];
      camEventExec(&camEv);
      curSelGrpSet(-1, dataItmGet(work->selection)->trgFlags);
      work->tgtSelect = MAN_SELECT_ITM_TARGET;
    }
    else if ((pad & PAD_TRIANGLE) && selection != 0)
    {
      if (calcDItemChk(unit, selection) != 0)
      {
        menuCloseItm();
        tgt[0] = tgtUnitGet();
        camEv.type = 0xA;
        camEv.unit = pActUnit;
        camEv.target = tgt[0];
        camEventExec(&camEv);
        curSelGrpSet(-1, dataItmGet(work->selection)->trgFlags);
        work->tgtSelect = MAN_SELECT_ITM_TARGET_WIDE;
        sndSysSePlay(1);
      }
      else
      {
        sndSysSePlay(5);
      }
    }
    else if (pad & PAD_CROSS)
    {
      menuCloseItm();
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
    }
    break;
  case MAN_SELECT_ITM_TARGET:
  case MAN_SELECT_ITM_TARGET_WIDE:
    trgFlags = dataItmGet(work->selection)->trgFlags;
    if (work->tgtSelect == MAN_SELECT_ITM_TARGET_WIDE)
    {
      trgFlags &= 0x7F;
      trgFlags |= 0x50;
    }
    tgt[0] = tgtUnitGet();
    curTgtProcInit(tgt, unit, trgFlags);
    if (curTgtProc(tgt, trgFlags) != 0)
    {
      camEv.unit = pActUnit;
      camEv.type = 0xA;
      camEv.target = tgt[0];
      camEventExec(&camEv);
    }
    tgtUnitSet(tgt[0]);
    unitActorGet(pTaskEnCur)->flags &= ~4;
    if (pad & PAD_CIRCLE)
    {
      cmd = KEY_CMD_ITEM;
      if (work->tgtSelect != MAN_SELECT_ITM_TARGET)
      {
        cmd = KEY_CMD_ITEM_WIDE;
      }
      ap = keyCmdApGet(unit, cmd);
      work->tgtSelect = MAN_SELECT_COMMAND_DONE;
      unitActorGet(pTaskEnCur)->flags |= 4;
      sndSysSePlay(1);
    }
    else if (pad & PAD_CROSS)
    {
      work->tgtSelect = MAN_SELECT_ITM_OPEN;
      menuOpenItm(unit);
      camEv.type = 0xE;
      camEv.unit = pActUnit;
      camEventExec(&camEv);
      unitActorGet(pTaskEnCur)->flags |= 4;
      sndSysSePlay(2);
    }
    break;
  case MAN_SELECT_GUARD:
    cmd = KEY_CMD_GUARD;
    ap = keyCmdApGet(unit, KEY_CMD_GUARD);
    work->tgtSelect = MAN_SELECT_COMMAND_DONE;
    break;
  case MAN_SELECT_AGWS_POS:
    curSelGrpSet(0, 0);
    curPosProcInit(unit, 5);
    if (curPosProc(unit, &work->selection) != 0)
    {
      camEv.type = 0xB;
      camEv.unit = pActUnit;
      camEv.param = work->selection;
      camEv.target = 0;
      camEventExec(&camEv);
    }
    unitActorGet(pTaskEnCur)->flags &= ~4;
    if (pad & PAD_CIRCLE)
    {
      ap = keyCmdApGet(unit, KEY_CMD_AGWS);
      work->tgtSelect = MAN_SELECT_COMMAND_DONE;
      camEv.type = 0x18;
      camEv.unit = pActUnit;
      cmd = KEY_CMD_AGWS;
      camEv.param = work->selection;
      camEv.target = 0;
      camEventExec(&camEv);
      sndSysSePlay(1);
    }
    else if (pad & PAD_CROSS)
    {
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
      sndSysSePlay(2);
    }
    break;
  case MAN_SELECT_MOVE_POS:
    curSelGrpSet(0, 0);
    curPosProcInit(unit, 6);
    if (curPosProc(unit, &work->selection) != 0)
    {
      camEv.type = 0xC;
      camEv.unit = pActUnit;
      camEv.param = work->selection;
      camEv.target = 0;
      camEventExec(&camEv);
    }
    unitActorGet(pTaskEnCur)->flags &= ~4;
    if (pad & PAD_CIRCLE)
    {
      ap = keyCmdApGet(unit, KEY_CMD_MOVE);
      work->tgtSelect = MAN_SELECT_COMMAND_DONE;
      cmd = KEY_CMD_MOVE;
      sndSysSePlay(1);
    }
    else if (pad & PAD_CROSS)
    {
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
      sndSysSePlay(2);
    }
    break;
  case MAN_SELECT_ATTACK_TARGET:
    tgt[0] = tgtUnitGet();
    curSelGrpSet(1, 0);
    curEnProcInit(tgt, 1);
    if (curEnProc(tgt) != 0)
    {
      camEv.unit = pActUnit;
      camEv.type = 0x15;
      camEv.target = tgt[0];
      camEventExec(&camEv);
    }
    tgtUnitSet(tgt[0]);
    unitActorGet(pTaskEnCur)->flags &= ~4;
    if (pad & PAD_CIRCLE)
    {
      if (keyCmdChk(unit, tgt[0], KEY_CMD_ATTACK) == 0)
      {
        ap = keyCmdApGet(unit, KEY_CMD_ATTACK);
        work->tgtSelect = MAN_SELECT_COMMAND_DONE;
        cmd = KEY_CMD_ATTACK;
        sndSysSePlay(1);
      }
      else
      {
        sndSysSePlay(5);
      }
    }
    else if (pad & PAD_CROSS)
    {
      work->tgtSelect = MAN_SELECT_BAT_OPEN;
      sndSysSePlay(2);
    }
    break;
  }
  work->field04 = cmd;
  work->unitTask = unit;
  return ap;
}

void camEventTurnStartSet(void)
{
    camEv.type = 6;
    camEv.unit = pActUnit;
    camEv.target = tgtUnitGet();
    camEv.param = ((int)pActUnit->work->flags >> 6) & 1;
    camEventExec(&camEv);
}

/*
 * The two command-list buffers (fifoCmd and cmdList) that cmdListSet,
 * cmdListChange, unitCmdListSet and cmdListCansel operate on. cmdListPtrGet is
 * the only place both addresses are taken together.
 */
/*
 * The command-list record (0x14 bytes: five words) and its queue entry
 * (0x18 bytes: the unit, then the record). The queue cursors have the layout
 * of src/ov01/obj.h's Fifo (base, maxIndex, readIndex, writeIndex, count).
 */
typedef union CmdListBody {
    int w[5];
    CmdEntry entry;
} CmdListBody;

typedef struct CmdListEntry {
    int unit;
    CmdListBody body;
} CmdListEntry;

typedef struct CmdFifo {
    int base;
    int maxIndex;
    int readIndex;
    int writeIndex;
    int count;
} CmdFifo;

int fifoPush(CmdFifo *fifo);
int fifoPop(CmdFifo *fifo);
int fifoEdGet(CmdFifo *fifo);
int fifoStGet(CmdFifo *fifo);
int fifoNumGet(CmdFifo *fifo);

static int fifoCmd[6];
static int cmdList[0xC0];

void cmdListPtrGet(CmdFifo **fifo, CmdListEntry **list)
{
    *fifo = (CmdFifo *)fifoCmd;
    *list = (CmdListEntry *)cmdList;
}

/* Queues one command record on the unit at the write cursor. */
void cmdListSet(BattleUnit *unit, int *command)
{
    CmdFifo *fifo;
    CmdListEntry *list;
    CmdListEntry *entry;

    cmdListPtrGet(&fifo, &list);
    entry = (CmdListEntry *)((char *)(fifoEdGet(fifo) * sizeof(CmdListEntry)) + (int)list);
    entry->unit = (int)unit;
    entry->body = *(CmdListBody *)command;
    fifoPush(fifo);
}

/* Overwrites the entry at the read cursor with the unit and record. */
void cmdListChange(BattleUnit *unit, int *command)
{
    CmdFifo *fifo;
    CmdListEntry *list;
    CmdListEntry *entry;

    cmdListPtrGet(&fifo, &list);
    entry = (CmdListEntry *)((char *)(fifoStGet(fifo) * sizeof(CmdListEntry)) + (int)list);
    entry->unit = (int)unit;
    entry->body = *(CmdListBody *)command;
}

extern void dataItmBoxDec(int itemId);
int keyCmd2tid(BattleUnit *unit, int command);
int keyCmd2wno(BattleUnit *unit, int key);

/*
 * Turns the key input record (input[1] the command key, input[2] its
 * argument) into the command record: kind, id, target and slot. Keys below
 * 20 are battle commands; the higher keys are ether, item and state entries.
 */
int convKey2unit(BattleUnit *unit, int *command, int *input, int target)
{
    int tid;
    int etherId;

    if (input[1] != 0) {
        if (input[1] == KEY_CMD_END) {
            command[0] = CMD_END;
        } else if (input[1] < KEY_CMD_WEAPON_LIMIT) {
            tid = keyCmd2tid(unit, input[1]);
            command[0] = CMD_ATK;
            command[2] = target;
            command[1] = tid;
            command[3] = keyCmd2wno(unit, input[1]);
            if (tid == 0) {
                printf("** CMD ERR -> %d\n", input[1]);
            }
        } else {
            switch (input[1]) {
            case KEY_CMD_ETHER:
                command[2] = target;
                command[0] = CMD_ETHER;
                etherId = input[2];
                command[3] = etherId;
                command[1] = dataEthGet(etherId)->tecId;
                break;
            case KEY_CMD_ITEM:
                command[2] = target;
                command[0] = CMD_ITEM;
                {
                    int itemId = input[2];
                    command[1] = 0;
                    command[3] = itemId;
                }
                dataItmBoxDec(input[2]);
                break;
            case KEY_CMD_GUARD:
                command[0] = CMD_GUARD;
                break;
            case KEY_CMD_AGWS:
                command[0] = CMD_AGWS;
                command[1] = input[2];
                break;
            case KEY_CMD_MOVE:
                command[0] = CMD_MOVE;
                command[1] = input[2];
                break;
            case KEY_CMD_ITEM_WIDE:
                command[2] = target;
                command[0] = CMD_ITEM;
                {
                    int itemId = input[2];
                    command[1] = 2;
                    command[3] = itemId;
                }
                dataItmBoxDec(input[2]);
                dataItmBoxDec(input[2]);
                break;
            }
        }
    }
    return 1;
}


/* Queues one 0x14-byte command record on the unit (defined below). */
void cmdListSet(BattleUnit *unit, int *command);


/*
 * Queues a unit's command together with the records that bracket it. A
 * technique (kind 1 or 2) is preceded by a 0xE record carrying the same
 * words; a ranged technique adds the 0x2C or 0xF record its technique flags
 * select, an unranged one a 0x11 record once (actor bit 0x100), and every
 * technique sets actor bit 0x200 (and 0x1000 when its TecRecord kind is 2). An item
 * (kind 3) gets the 0xE record only. Kind 8 queues the 0x12 (bit 0x100) or
 * 0x10 (bit 0x200) record with the unit's position id and clears bits
 * 0x100, 0x200 and 0x1000. Always 1.
 */
int unitCmdListSet(BattleUnit *unit, int *command)
{
    CalcUnitParam *param = calcUPGet(unit);
    int record[5];
    int kind = command[0];
    int range;

    if (kind == 1 || kind == 2) {
        record[0] = 0xE;
        record[1] = command[1];
        record[2] = command[2];
        record[3] = command[3];
        record[4] = kind;
        cmdListSet(unit, record);
        range = dataTecGet(command[1])->flags & 3;
        if (range != 0) {
            if (dataTecGet(command[1])->flags & 0x10) {
                record[0] = 0x2C;
            } else {
                record[0] = 0xF;
            }
            cmdListSet(unit, record);
        }
        if (range == 0 && !(unitActorGet((ObjectTask *)unit)->flags & 0x100)) {
            record[0] = 0x11;
            cmdListSet(unit, record);
            unitActorGet((ObjectTask *)unit)->flags |= 0x100;
        }
        unitActorGet((ObjectTask *)unit)->flags |= 0x200;
        if (dataTecGet(command[1])->kind == 2) {
            unitActorGet((ObjectTask *)unit)->flags |= 0x1000;
        }
    } else if (kind == 3) {
        record[0] = 0xE;
        record[1] = command[1];
        record[2] = command[2];
        record[3] = command[3];
        record[4] = kind;
        cmdListSet(unit, record);
    } else if (kind == 8) {
        if (unitActorGet((ObjectTask *)unit)->flags & 0x100) {
            record[0] = 0x12;
            record[1] = param->posId;
            cmdListSet(unit, record);
        } else if (unitActorGet((ObjectTask *)unit)->flags & 0x200) {
            record[0] = 0x10;
            record[1] = param->posId;
            cmdListSet(unit, record);
        }
        unitActorGet((ObjectTask *)unit)->flags &= ~0x1300;
    }
    cmdListSet(unit, command);
    return 1;
}

/*
 * Drops the unit's queued records from the read cursor on, stopping at the
 * first record that belongs to another unit or is a kind 8 record.
 */
void cmdListCansel(BattleUnit *unit)
{
    CmdFifo *fifo;
    CmdListEntry *list;
    CmdListEntry *entry;

    cmdListPtrGet(&fifo, &list);
    while (fifoNumGet(fifo) != 0) {
        entry = (CmdListEntry *)((char *)(fifoStGet(fifo) * sizeof(CmdListEntry)) + (int)list);
        if (entry->unit != (int)unit || entry->body.w[0] == 8) {
            break;
        }
        fifoPop(fifo);
    }
}

/* Menu map-height toggle, defined by ov01/tu009 (src/ov01/menu.c). */
extern void menuMapHSet(int enable);

/* Enemy AI attack-selection entry point, defined by ov01/tu006.
 * thinkUnitAtkExec forwards this argument to calcUPGet as ObjectTask *. */
extern void thinkUnitAtkExec(BattleUnit *unit);

int battleCtrlThink(BattleUnit *unit)
{
    menuMapHSet(1);
    thinkUnitAtkExec(unit);
    return 0;
}

extern ScenarioDstEntry *scenarioDstGet(int offset);
extern int scenarioBatExec(void);
extern int busyUnitChk(int side);
extern int stat1Proc(StatUnit *stat);
extern int stat2Proc();
extern int battleExecDead(void);
extern void sndInit2(void);
extern int battleExecNorm(void);

/*
 * Steps the batCtrl.field04 phase machine: runs queued commands, scenario
 * batches, the status and death passes and the waits on busy units.
 */
void battleExec(void)
{
    CmdEntry cmd;

    switch (batCtrl.field04)
    {
    case BATTLE_PHASE_EXEC_QUEUE:
        battleExecNorm();
        break;
    case BATTLE_PHASE_SCENARIO_START:
        if (scenarioDstGet(0) == 0)
        {
            batCtrl.field04 = BATTLE_PHASE_WAIT_UNIT;
            batCtrl.field08 = BATTLE_PHASE_EXEC_QUEUE;
            break;
        }
        batCtrl.field04 = BATTLE_PHASE_SCENARIO_RUN;
        {
            int *dst = (int *)scenarioDstGet(0);
            statUnit.hpBonus = dst[2];
        }
    case BATTLE_PHASE_SCENARIO_RUN:
        if (scenarioBatExec() != 0)
        {
            scenarioBatInit();
            batCtrl.field04 = BATTLE_PHASE_EXEC_QUEUE;
            if (battleExecNorm() == 0)
            {
                if (pExecUnit != 0)
                {
                    cmd.selector = CMD_STAND;
                    unitCmdSet(pExecUnit, &cmd);
                }
            }
        }
        break;
    /* Status and death handling after a command: stat1Proc, the dead-unit pass, then stat2Proc. */
    case BATTLE_PHASE_STAT_PROC_1:
        sndInit2();
        if (stat1Proc(&statUnit) != 0)
        {
            sndInit2();
            batCtrl.field04 = BATTLE_PHASE_WAIT_BUSY;
            batCtrl.field08 = BATTLE_PHASE_DEAD;
            break;
        }
        batCtrl.field04 = BATTLE_PHASE_DEAD;
    case BATTLE_PHASE_DEAD:
        if (battleExecDead() != 0)
        {
            break;
        }
    case BATTLE_PHASE_STAT_PROC_2:
        menuTimeNext();
        sndInit2();
        if (stat2Proc(&statUnit) != 0)
        {
            batCtrl.field04 = BATTLE_PHASE_WAIT_BUSY;
            batCtrl.field08 = BATTLE_PHASE_EXEC_QUEUE;
            break;
        }
        batCtrl.field04 = BATTLE_PHASE_EXEC_QUEUE;
        break;
    case BATTLE_PHASE_WAIT_BUSY:
        if (busyUnitChk(2) > 0)
        {
            break;
        }
        batCtrl.field04 = batCtrl.field08;
        break;
    case BATTLE_PHASE_WAIT_UNIT:
        if (pExecUnit->work->flags & ACTOR_FLAG_CMD_PENDING)
        {
            break;
        }
        batCtrl.field04 = batCtrl.field08;
        break;
    case BATTLE_PHASE_WAIT_FRAMES:
        if (busyUnitChk(0) > 0)
        {
            break;
        }
        if (batCtrl.field0C-- > 0)
        {
            break;
        }
        batCtrl.field04 = batCtrl.field08;
        break;
    }
}

extern void menuCritDispOff(void);
extern int unitMotGet(BattleUnit *unit);
extern void thinkTurnEndExec(BattleUnit *unit);
extern void calcTurnEnd(BattleUnit *unit);

/*
 * Dispatches the command at the head of the command queue to its unit and pops
 * it. Returns 0 when the queue is empty or the unit is still moving, else 1.
 */
int battleExecNorm(void)
{
    CmdFifo *fifo;
    CmdListEntry *list;
    CmdListEntry *e;
    BattleUnit *unit;
    int *cmd;
    int cmdNo;

    cmdListPtrGet(&fifo, &list);
    if (fifoNumGet(fifo) == 0)
    {
        return 0;
    }
    e = (CmdListEntry *)((char *)(fifoStGet(fifo) * sizeof(CmdListEntry)) + (int)list);
    unit = (BattleUnit *)e->unit;
    cmd = e->body.w;
    if (unitMotGet(unit) == 2 && (unit->actor->moveFlags & 2))
    {
        return 0;
    }
    if (unit != pExecUnit)
    {
        menuCritDispOff();
        statUnit.unit = 0;
        statUnit.cmd = 0;
    }
    cmdNo = *cmd;
    if ((unsigned int)(cmdNo - 1) < 6)
    {
        statUnit.cmd = cmdNo;
        statUnit.unit = unit;
    }
    if (unit != 0 && !(unit->work->flags & ACTOR_FLAG_DEAD))
    {
        pExecUnit = unit;
        unitCmdSet(unit, &e->body.entry);
        if (*cmd == CMD_END)
        {
            thinkTurnEndExec(unit);
            calcTurnEnd(unit);
            pLastUnit = pExecUnit;
            pExecUnit = 0;
        }
    }
    fifoPop(fifo);
    cmdNo = (int)pExecUnit;
    if (cmdNo == 0)
    {
        menuCritDispOff();
        batCtrl.field04 = BATTLE_PHASE_WAIT_BUSY;
        batCtrl.field08 = BATTLE_PHASE_STAT_PROC_1;
    }
    else
    {
        batCtrl.field04 = BATTLE_PHASE_SCENARIO_START;
    }
    return 1;
}

extern int unitCmdDeadSet(BattleUnit *unit);
extern void monsGainSet(BattleUnit *unit);
extern void menuStockDead(int n);
extern void *dataSpecDataGet(int id, int tid);
extern const char battleInitDEADTIDText[];
extern void UmnDataBaseMonsterSet(void);
extern int menuTimeGet(void);
extern int menuTimeXGet(void);
extern void menuCritDispOn(int mode);

/*
 * Handles the units that died this turn: removes them from the action stock,
 * runs their death effect and camera event, and updates the enemy gain and
 * database records. Returns 1 when any unit died, 0 otherwise.
 */
int battleExecDead(void)
{
  BattleUnit **table;
  int count;
  int i;
  int flag;
  const char *deadFormat;
  int stock;
  BattleUnit *linked;
  char *bc;
  CamEvent *ev;
  const char *fmt;
  flag = 0;
  i = 0;
  count = unitTblGet(2, &table);
  if (count > 0)
  {
    bc = (char *) (&batCtrl);
    ev = &camEv;
    ;
    do
    {
      if (table[i] != 0)
      {
        if ((calcBattleChk(table[i]) == 0) || (((short) calcUPGet(table[i])->currentHp) == 0))
        {
          if ((actStockRemove(table[i], &stock) != 0) && (stock > 0))
          {
            menuStockDead(stock);
          }
        }
        if (unitCmdDeadSet(table[i]) != 0)
        {
          flag = 1;
          if (table[i]->work->flags & ACTOR_FLAG_ENEMY)
          {
            decoEffCall(table[i], 0x68);
            monsGainSet(table[i]);
            {
              short charaId = calcUPGet(table[i])->charaId;
              if ((((charaId != 0x22) && (charaId != 0x24)) && (charaId != 0x99)) && (((unsigned) (charaId - 0x6F)) >= 0x28))
              {
                UmnDataBaseMonsterSet();
              }
            }
            if (menuTimeGet() == 3)
            {
              int mode = menuTimeXGet();
              if (mode == 10)
              {
                mode = 3;
              }
              else
              {
                mode = ((mode ^ 4) != 0) ? (1) : (2);
              }
              menuCritDispOn(mode);
              *((int *) (bc + 0xC)) = 0x3C;
            }
            else
            {
              *((int *) (bc - -0xC)) = 0;
            }
          }
          linked = *((BattleUnit **) (((char *) calcUPGet(table[i])) + 0x150));
          if (linked != 0)
          {
            short cid = calcUPGet(linked)->charaId;
            void *spec = dataSpecDataGet(cid, *((int *) (((char *) calcUPGet(table[i])) + 0x154)));
            if (spec != 0)
            {
              if (((*((int *) (((char *) calcUPGet(table[i])) + 0x154))) != 0x61) || (calcUPGet(table[i])->flags & 0x2000))
              {
                *((unsigned short *) (((char *) spec) + 0xA)) = (*((unsigned short *) (((char *) spec) + 0xA))) + 1;
              }
            }
          }
          deadFormat = battleInitDEADTIDText;
          {
            short charaId = calcUPGet(table[i])->charaId;
            int tid = *((int *) (((char *) calcUPGet(table[i])) + 0x154));
            printf(deadFormat, charaId, tid);
          }
          {
            BattleUnit *ty = (BattleUnit *) 0x13;
            *(BattleUnit **) &ev->type = ty;
            ev->unit = table[i];
            ((void (*)(CamEvent *, BattleUnit *))camEventExec)(ev, ty);
          }
        }
      }
      i++;
    }
    while (i < count);
  }
  if (flag != 0)
  {
    batCtrl.field04 = 9;
    batCtrl.field08 = 6;
  }
  else
  {
    batCtrl.field04 = 6;
  }
  return flag;
}


extern const char battleInitStatDmgSAFEText[];

/* CalcUnitParam +0x150: a spec pointer (its type is not recovered), cleared
 * by the unit-damage path below; calc.h's unmodeled_14e span covers it. */
#define CALC_PARAM_SPEC_PTR_OFF 0x150

/*
 * Applies one unit's status-driven HP and EP changes for the stat record's
 * command number, queues the matching command and records the damage. Returns 1
 * when the unit's HP, EP or form changed, 0 otherwise.
 */
int stat1Proc(StatUnit *stat)
{
    BattleUnit *unit = stat->unit;
    int mode = stat->cmd;
    int hp;
    int ep;
    int alive;
    int neg;
    int dmg;
    int flags;
    CmdEntry cmd;
    struct
    {
        BattleUnit *unit;
        int pad;
        int dmg;
        int flags;
        int more[8];
    } rec;
    int curHp;
    int ret = 0;

    if (unit != 0)
    {
        hp = 0;
        curHp = (short) calcUPGet(unit)->currentHp;
        ep = 0;
        if (calcStatGet(unit, 1, 0x100) != 0
            && rnd(100) < (curHp % 10) * 5)
        {
            calcHp(unit, curHp);
            return 1;
        }
        if (calcStatGet(unit, 0, 0x800) != 0)
        {
            hp = calcUPGet(unit)->maxHp / 5;
        }
        if (mode == 1)
        {
            if (calcStatGet(unit, 1, 0x400) != 0)
            {
                hp += calcUPGet(unit)->maxHp / 2;
            }
            if (calcStatGet(unit, 1, 0x80) != 0)
            {
                hp += stat->hpBonus;
            }
        }
        if (mode == 2)
        {
            if (calcStatGet(unit, 1, 0x20) != 0)
            {
                ep = calcUPGet(unit)->maxEp / 8;
            }
        }
        if (calcStatGet(unit, 4, 0x80) != 0)
        {
            ep += calcUPGet(unit)->maxEp / 8;
        }
        if (mode == 4)
        {
            if (calcStatGet(unit, 6, 0x200) != 0)
            {
                hp -= calcUPGet(unit)->maxHp / 10;
            }
            if (calcStatGet(unit, 6, 0x100) != 0)
            {
                calcStatReset(unit, 4, 0x7FFF, 0);
                calcStatReset(unit, 5, 0x7FFF, 0);
            }
        }
        /* A nonzero HP change is queued as one damage or recovery command. */
        if (hp != 0)
        {
            unitCmdClear(unit);
            if (hp > 0)
            {
                if (unit->work->flags & ACTOR_FLAG_GUARD)
                {
                    cmd.selector = CMD_DEF;
                }
                else
                {
                    cmd.selector = CMD_DMG;
                }
                cmd.argument = 0;
            }
            else
            {
                cmd.selector = CMD_REC;
            }
            unitCmdSet(unit, &cmd);
            neg = -hp;
            alive = calcHp(unit, neg);
            curHp = (hp > 0) ? 1 : 2;
            if (alive == 0
                && calcStatGet(unit, 2, 1) != 0)
            {
                calcHp(unit, 1);
                calcStatReset(unit, 2, 1, 0);
                printf(battleInitStatDmgSAFEText);
            }
            dmg = neg;
            flags = curHp | 0x400;
            if (hp >= 0)
            {
                dmg = hp;
                alive = 0;
            }
            rec.dmg = dmg;
            rec.flags = flags;
            rec.unit = unit;
            dmgValPut(0, &rec.unit);
            *((int *) (((char *) calcUPGet(unit)) + CALC_PARAM_SPEC_PTR_OFF)) = 0;
        }
        if (ep != 0)
        {
            unitCmdClear(unit);
            cmd.selector = CMD_DMG;
            cmd.argument = 0;
            unitCmdSet(unit, &cmd);
            calcEp(unit, -ep);
            rec.flags = 0x403;
            rec.unit = unit;
            rec.dmg = ep;
            dmgValPut(0, &rec.unit);
        }
        /* Motion 20 or 21 without its status flag queues a unit change. */
        if ((((UnitMotionHead *)unit->actor)->motionId == 20 && calcStatGet(unit, 7, 2) == 0)
            || (((UnitMotionHead *)unit->actor)->motionId == 21 && calcStatGet(unit, 7, 1) == 0))
        {
            unitCmdClear(unit);
            cmd.selector = CMD_UNITCHANGE;
            cmd.argument = (calcUPGet(unit)->posId < 0)
                ? -calcUPGet(unit)->posId : calcUPGet(unit)->posId;
            cmd.extra = calcUPGet(unit)->charaId;
            unitCmdSet(unit, &cmd);
            cmd.selector = CMD_BTST;
            cmd.argument = 0;
            unitCmdSet(unit, &cmd);
            return 1;
        }
        if (hp == 0 && ep == 0)
        {
            ret = 0;
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

typedef struct { unsigned char c[12]; } Str12;
extern const char battleInitItemChangeText[];
extern const char battleInitChangedToText[];
extern const char battleInitJpText2[];

/*
 * Second status pass over the side-2 units: applies the pending HP changes and
 * the item-change steps. Returns 1 when any unit took an HP change, 0 otherwise.
 */
int stat2Proc(void)
{
  CmdEntry cmd;
  struct 
  {
    BattleUnit *unit;
    int pad;
    int dmg;
    int flags;
    int more[8];
  } rec;
  BattleUnit **table;
  BattleUnit *unit;
  int count;
  int i;
  int ret;
  int dmg;
  unsigned char *g;
  unsigned char b17;
  unsigned char b18;
  unsigned short t;
  char *name;
  ret = 0;
  i = 0;
  count = unitTblGet(2, &table);
  if (count > 0)
  {
    do
    {
      unit = table[i];
      dmg = 0;
      if (unit != 0)
      {
        if ((*((int *) unit->work)) & 0x80)
        {
          if (calcStatGet(unit, 2, 0x4000) != 0)
          {
            dmg = *((short *) calcUPGet(unit));
            calcStatReset(unit, 2, 0x4000, 0);
            goto block_27;
          }
          if (calcBackLineChk(unit, 0) != 0)
          {
            goto block_17;
          }
          if (calcStatGet(unit, 3, 0x800) == 0)
          {
            goto block_17;
          }
          dmg = ((short) (*((unsigned short *) calcUPGet(unit)))) / 2;
          calcStatReset(unit, 3, 0x800, 0);
          goto block_27;
          block_17:
          if ((*((int *) unit->work)) & 0x40)
          {
            if (calcStatGet(unit, 1, 1) != 0)
            {
              calcStatReset(unit, 1, 1, 0);
              calcStatSet(unit, 1, 0x100, 0);
              goto block_27;
            }
            if (((*((int *) unit->work)) & 0x40) && (calcStatGet(unit, 7, 4) != 0))
            {
              calcStatReset(unit, 7, 4, 0);
              calcStatSet(unit, 1, 0x100, 0);
              g = (unsigned char *) dataGainGet(calcUPGet(unit)->charaId);
              b18 = g[0xF];
              b17 = g[0x12];
              if (itemGrpChk(b18, b17) == 0)
              {
                b18 = 0;
                b17 = 0;
              }
              printf(battleInitItemChangeText, b18, b17);
              if (b17 != 0)
              {
                name = (char *) itemNameGet(b18, b17);
                if (name != 0)
                {
                  *((Str12 *) msgWork.text) = *((Str12 *) battleInitChangedToText);
                  strcat(msgWork.text, name);
                  strcat(msgWork.text, battleInitJpText2);
                  battleMsg((int) (&msgWork));
                  dataBoxInc(b18, b17);
                  goto block_27;
                }
                goto next;
              }
              goto block_27;
            }
            else
            {
              goto block_27;
            }
          }
          else
          {
            goto block_27;
          }

          block_27:
          if (dmg != 0)
          {
            ret = 1;
            unitCmdClear(unit);
            cmd.selector = CMD_WAKE;
            unitCmdSet(unit, &cmd);
            calcHp(unit, dmg);
            rec.dmg = dmg;
            rec.flags = 0x802;
            rec.unit = unit;
            dmgValPut(0, &rec.unit);
          }

          block_29:
          calcStatReset(unit, 7, 4, 0);

        }
        else
        {
          goto block_29;
        }
      }
      next:
      i += 1;

    }
    while (i < count);
  }
  return ret;
}

/*
 * sndSysSePlay is defined by ov01/tu013 snd.c; other TUs already redeclare it
 * the same way to call it from a different TU.
 */
extern int sndSysSePlay(int soundId);

/* Defined by ov01/tu004 (src/ov01/calc.c). */
extern int calcTuigekiChk(BattleUnit *unit);
void keyBuffClear(void);
int keyBuffSet(BattleUnit *unit, unsigned short key);
int keyBuffChk(BattleUnit *unit, int *apCost);
/* Nonzero when the command cannot be used on the target; 1 also plays the refusal sound. */
extern int keyCmdChk(BattleUnit *unit, BattleUnit *target, int command);

/*
 * One frame of command-key input for unit. Returns 0x1A (KEY_CMD_END) when the
 * unit cannot act (too few action points by dblActKind, or a pursuit it cannot
 * follow) or when cross is pressed. Returns 0 while R1 or R2 is held or when no
 * new key is pressed. Returns 0x1B (KEY_CMD_CANCEL) when the buffered keys form
 * no usable command; the rejected key is dropped again. Otherwise returns the
 * command keyBuffChk matched.
 */
int keyCmdGet(BattleUnit *unit)
{
    Actor *actor;
    int key;
    int command;
    int apCost;
    int refusal;

    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        if (calcUPGet(unit)->dblActKind < 3) {
            return KEY_CMD_END;
        }
    } else {
        if (calcUPGet(unit)->dblActKind < 2) {
            return KEY_CMD_END;
        }
        actor = unit->work;
        if ((actor->flags & 0x1000) && calcTuigekiChk(unit) == 0) {
            return KEY_CMD_END;
        }
    }
    if (dataPadRead(0) & (PAD_R1 | PAD_R2)) {
        return 0;
    }
    key = dataPadRead(1);
    if (key == 0) {
        return key;
    }
    if (key == PAD_CROSS) {
        keyBuffClear();
        command = KEY_CMD_END;
    } else {
        if (keyBuffSet(unit, key) == 0) {
            return 0;
        }
        command = keyBuffChk(unit, &apCost);
        if (command == KEY_CMD_CANCEL) {
            if (keyBuffIdx > 0) {
                keyBuffIdx--;
            }
            return command;
        }
        refusal = keyCmdChk(unit, tgtUnitGet(), command);
        if (refusal != 0) {
            if (keyBuffIdx > 0) {
                keyBuffIdx--;
            }
            if (refusal == 1) {
                sndSysSePlay(5);
            }
            command = KEY_CMD_CANCEL;
        }
    }
    return command;
}

/*
 * One entry of the sentinel-terminated (null sequence) key-command tables that
 * keyBuffChk walks in 0xC-byte steps. The sequence's first halfword is the key
 * count (compared with keyBuffIdx) and the keys follow; apCost (+0x4) is stored
 * for a character, command (+0x8) is returned.
 */
typedef struct KeyCmd {
    const unsigned short *sequence; /* +0x0 */
    int apCost;                     /* +0x4 */
    int command;                    /* +0x8 */
} KeyCmd;
static KeyCmd keyCmdTblCha[16];
static KeyCmd keyCmdTblAgws[13];

/*
 * Halfword tables in the unit's CalcUnitParam block that keyCmd2tid indexes
 * by command number (no single struct member spells them out). Each offset
 * names the table base; the index is added to it at the use.
 */
#define CALC_PARAM_NORMAL_CMD_TECH_OFF 0x74
#define CALC_PARAM_CMD_TECH_OFF 0x6C
#define CALC_PARAM_SLOT_TECH_OFF 0x82
#define CALC_PARAM_HALF(unit, offset) (*(short *)((char *)calcUPGet(unit) + (offset)))

extern int calcSameWpnChk(BattleUnit *unit);

extern int calcDblActChk(BattleUnit *unit);



extern const char battleInitPTGTNULLText[];
extern int calcSpecChk(BattleUnit *unit, int specId);
extern int calcWpnEnableChk(BattleUnit *unit, int slot);

/*
 * The action-point cost of a command: a character's from the command table,
 * an enemy's from the weapon in the command's slot, and fixed values for the
 * commands past the table.
 */
int keyCmdApGet(BattleUnit *unit, int command)
{
    int result = 0;
    int slot;
    KeyCmd *table;
    int n;

    if (command == KEY_CMD_NONE || command == KEY_CMD_CANCEL || command == KEY_CMD_END) {
        return 0;
    }
    if (command < KEY_CMD_ETHER) {
        if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
            if (command != KEY_CMD_ATTACK) {
                slot = keyCmd2wno(unit, command);
                if (slot != -1) {
                    result = dataWpnGet(calcUPGet(unit)->weaponId[slot])->apCost;
                }
            } else {
                result = 6;
            }
        } else {
            table = keyCmdTblCha;
            n = 0;
            for (;;) {
                KeyCmd *entry = (KeyCmd *)((char *)(n * sizeof(KeyCmd)) + (int)table);

                if (entry->sequence == 0) {
                    break;
                }
                if (entry->command == command) {
                    result = entry->apCost;
                    break;
                }
                n++;
            }
        }
    } else {
        switch (command) {
        case KEY_CMD_GUARD:
        case KEY_CMD_MOVE:
            result = 2;
            break;
        case KEY_CMD_ITEM:
            result = 3;
            break;
        case KEY_CMD_ETHER:
        case KEY_CMD_AGWS:
            result = 4;
            break;
        case KEY_CMD_ITEM_WIDE:
            result = 6;
            break;
        }
    }
    return result;
}

/*
 * Whether unit may enter command cmd against tgt: 1 when it can, 2 when the
 * command is refused outright (no target, no technique, unusable slot, or no
 * tuigeki for a counter), and falls through to the action-point and attack
 * checks at the end otherwise. Character and enemy paths differ in which
 * status categories bar which commands.
 */
int keyCmdChk(BattleUnit *unit, BattleUnit *tgt, int cmd)
{
    int tid;
    int apCost;

    switch (cmd) {
    case KEY_CMD_NONE:
    case KEY_CMD_CANCEL:
        return 2;
    default:
        if (tgt == 0) {
            printf(battleInitPTGTNULLText);
            return 2;
        }
    }
    tid = keyCmd2tid(unit, cmd);
    if (tid == 0) {
        return 2;
    }
    if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        if (cmd >= 0x11) {
            if (calcTuigekiChk(unit) == 0) {
                return 2;
            }
            if (calcSpecChk(unit, tid) == 0) {
                return 1;
            }
        } else if (cmd >= 0xB) {
            if (calcStatGet(unit, 0, 1) != 0 && (unsigned int)(cmd - 0xB) < 8) {
                return 1;
            }
            if (calcSpecChk(unit, tid) == 0) {
                return 1;
            }
        } else if (cmd == 7) {
            if (calcStatGet(unit, 0, 1) != 0) {
                return 1;
            }
            if (calcWpnEnableChk(unit, 1) == 0) {
                return 1;
            }
        } else {
            if (calcStatGet(unit, 0, 4) != 0) {
                if (cmd == 1 || cmd == 3 || cmd == 4 || cmd == 6) {
                    return 1;
                }
            }
            if (calcStatGet(unit, 0, 2) != 0) {
                if (cmd == 2 || cmd == 5 || cmd == 6 || cmd == 4) {
                    return 1;
                }
            }
        }
    } else {
        int wno = keyCmd2wno(unit, cmd);
        if (wno == -1) {
            return 2;
        }
        if (calcWpnEnableChk(unit, wno) == 0) {
            return 1;
        }
        if (calcStatGet(unit, 4, 4) != 0) {
            if (cmd == 1 || cmd == 3 || cmd == 4 || cmd == 0xB || cmd == 6 || cmd == 8) {
                return 1;
            }
        }
        if (calcStatGet(unit, 4, 2) != 0) {
            if (cmd == 2 || cmd == 5 || cmd == 6 || cmd == 0xC || cmd == 4 || cmd == 9) {
                return 1;
            }
        }
        if (calcStatGet(unit, 4, 1) != 0) {
            if (cmd == 7 || cmd == 9 || cmd == 8 || cmd == 0xA || cmd == 0xC || cmd == 0xB) {
                return 1;
            }
        }
    }

    apCost = keyCmdApGet(unit, cmd);
    if (((signed char *)calcUPGet(unit))[0x3A] < apCost) {
        return 1;
    }
    return calcAtkChk(unit, tgt, tid) == 0 ? 1 : 0;
}

/*
 * The technique id a command key runs for unit, 0 when none. Characters read
 * the technique tables by command number; enemies read the technique of the
 * weapon in the command's slot.
 */
int keyCmd2tid(BattleUnit *unit, int command)
{
    int tid = 0;
    int slot;
    int weaponId;

    if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        if (command >= 0x11) {
            slot = command - 0x11;
            slot = (slot == 1) ? 4 : 2;
            tid = CALC_PARAM_HALF(unit, CALC_PARAM_SLOT_TECH_OFF + slot * 2);
        } else if (command >= 0xB) {
            tid = CALC_PARAM_HALF(unit, CALC_PARAM_CMD_TECH_OFF + command * 2);
        } else if (command == 7) {
            if (calcUPGet(unit)->flags & 0x10) {
                weaponId = calcUPGet(unit)->weaponId[1];
                if (weaponId != 0) {
                    tid = dataWpnGet(weaponId)->tecId;
                }
            }
        } else if (command != 0) {
            tid = CALC_PARAM_HALF(unit, CALC_PARAM_NORMAL_CMD_TECH_OFF + command * 2);
        }
    } else {
        slot = keyCmd2wno(unit, command);
        if (slot != -1) {
            weaponId = calcUPGet(unit)->weaponId[slot];
            if (weaponId != 0) {
                if (command == KEY_CMD_ATTACK && calcDblActChk(unit)) {
                    tid = dataWpnGet(weaponId)->tecIdDblAct;
                } else if (command == 3 || command == 5 || command == 10) {
                    tid = dataWpnGet(weaponId)->tecIdMultiHit;
                } else {
                    tid = dataWpnGet(weaponId)->tecId;
                }
            }
        }
    }
    return tid;
}

/*
 * The weapon slot (0..2) a command key selects for unit, or -1. Enemies map
 * the keys onto slots directly; characters only ever select slot 1 (key 7).
 * The slot must hold a weapon (its weaponId is nonzero) to be selected.
 */
int keyCmd2wno(BattleUnit *unit, int key)
{
    CalcUnitParam *up;
    int slot = -1;

    up = calcUPGet(unit);
    if (!(up->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        slot = key == 7;
    } else {
        switch (key) {
        case 1:
        case 3:
        case 6:
        case 8:
            slot = 0;
            break;
        case 2:
        case 4:
        case 5:
        case 9:
            slot = 1;
            break;
        case 7:
        case 10:
        case 11:
        case 12:
            slot = 2;
            break;
        case KEY_CMD_ATTACK:
            slot = calcSameWpnChk(unit);
            break;
        }
    }
    if (slot != -1 && calcUPGet(unit)->weaponId[slot] == 0) {
        slot = -1;
    }
    return slot;
}

/* Clears the buffered command-key queue keyBuff and its insertion index keyBuffIdx. */
void keyBuffClear(void)
{
    memset(keyBuff, 0, sizeof(keyBuff));
    keyBuffIdx = 0;
}

int keyBuffIdxGet(void)
{
    return keyBuffIdx;
}

/*
 * Sentinel-terminated (0) key tables for keyBuffSet, one per calcUPGet() flags
 * bit 0x40 (CALC_UNIT_FLAG_IS_AGWS): the first serves units with the bit set,
 * the second the others.
 */
static unsigned short keyItemTblAgws[4];
static unsigned short keyItemTblCha[4];

/* Error text printed by keyBuffSet when the key buffer is full. */
extern const char battleInitKeyBuffSetErrorText[];

/*
 * Looks key up in the unit's own command-key table (enemy or character,
 * chosen by calcUPGet()'s flags bit 0x40) and, if it is a recognised key,
 * appends it to the shared keyBuff queue. Returns whether it both matched a
 * table entry and fit in the queue.
 */
int keyBuffSet(BattleUnit *unit, unsigned short key)
{
    const unsigned short *table;
    int idx;
    unsigned short entry;
    int found;

    found = 0;
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        table = keyItemTblAgws;
    } else {
        table = keyItemTblCha;
    }

    idx = 0;
    entry = table[idx];
    while (entry != 0) {
        if (key == entry) {
            found = 1;
            break;
        }
        idx++;
        entry = table[idx];
    }

    if (!found) {
        return 0;
    }

    if (keyBuffIdx < KEY_BUFF_COUNT) {
        keyBuff[keyBuffIdx] = key;
        keyBuffIdx++;
        return 1;
    }
    printf(battleInitKeyBuffSetErrorText);
    return 0;
}


/*
 * Matches the buffered keys against the unit's key-command table (enemy or
 * character, by calcUPGet()'s flags bit 0x40) and returns the command of
 * the entry whose whole sequence equals the buffer, with its AP cost in
 * *apCost (an enemy's from the weapon in the command's slot). 0 for an
 * empty buffer, 0x1B when nothing matches or an enemy has no weapon slot for
 * the matched command.
 */
int keyBuffChk(BattleUnit *unit, int *apCost)
{
    KeyCmd *table;
    const unsigned short *keys;
    int length;
    int n;
    int i;
    int slot;

    *apCost = 0;
    if (keyBuffIdx == 0) {
        return 0;
    }
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        table = keyCmdTblAgws;
    } else {
        table = keyCmdTblCha;
    }
    for (n = 0; table[n].sequence != 0; n++) {
        keys = table[n].sequence;
        length = *keys++;
        if (keyBuffIdx != length) {
            continue;
        }
        for (i = 0; i < length; i++) {
            if (keyBuff[i] != keys[i]) {
                break;
            }
        }
        if (i == length) {
            if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
                slot = keyCmd2wno(unit, table[n].command);
                if (slot == -1) {
                    return KEY_CMD_CANCEL;
                }
                *apCost = dataWpnGet(calcUPGet(unit)->weaponId[slot])->apCost;
            } else {
                *apCost = table[n].apCost;
            }
            return table[n].command;
        }
    }
    return KEY_CMD_CANCEL;
}

int busyUnitChk(int side)
{
    BattleUnit **units;
    int count = unitTblGet(side, &units);
    int busyCount = 0;
    int i;

    for (i = 0; i < count; i++) {
        BattleUnit *unit = units[i];

        if (unit != 0 && (unit->work->flags & ACTOR_FLAG_CMD_PENDING) != 0) {
            busyCount++;
        }
    }
    return busyCount;
}

/*
 * Collects the units of one side that are still taking part in the battle
 * and returns how many there are. The test is on the flags word of the
 * unit's own actor (the same word transWepIn and camEventTurnStartSet
 * read): bit 0x8 must be set and bit 0x80 clear.
 * A null actUnits only counts them; otherwise the collected units are
 * stored in it in table order.
 */
int actUnitTblMake(int side, BattleUnit **actUnits)
{
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit **out;
    BattleUnit *unit;
    u32 flags;
    int remaining;
    int count;
    int made;

    count = unitTblGet(side, &table);
    made = 0;
    if (count > 0) {
        next = table;
        remaining = count;
        out = actUnits;
        do {
            unit = *next;
            next++;
            if (unit != 0) {
                flags = unit->work->flags;
                if (!(flags & ACTOR_FLAG_DEAD)) {
                    if (flags & 8) {
                        if (actUnits != 0) {
                            *out = unit;
                        }
                        out++;
                        made++;
                    }
                }
            }
            remaining--;
        } while (remaining != 0);
    }
    return made;
}

BattleUnit *tgtUnitPick(BattleUnit **table, int count)
{
    if (count > 0) {
        return table[rnd(count - 1)];
    }
    return 0;
}

extern int unitLiveNumGet2(int side);
u8 cfEventGet(void);

/* Battle end check: records the result in batRetCode and returns 1 while the battle goes on. */
int battleEndChk(void)
{
    int mode;
    int live0;

    batRetCode = 0;
    if (busyUnitChk(2) > 0)
    {
        return 1;
    }
    mode = batCtrl.field04;
    if (mode != 1)
    {
        return 1;
    }
    live0 = unitLiveNumGet2(0);
    if (live0 == 0)
    {
        batRetCode = BATTLE_RESULT_LOSE;
        return 0;
    }
    if (unitLiveNumGet2(1) == 0)
    {
        batRetCode = mode;
        if (cfEventGet() & 0x10)
        {
            batRetCode = BATTLE_RESULT_ESCAPE;
        }
        if (cfEventGet() & 0x20)
        {
            batRetCode = BATTLE_RESULT_ESCAPE;
        }
        return 0;
    }
    if (escapeFlag == mode)
    {
        batRetCode = BATTLE_RESULT_ESCAPE;
        return 0;
    }
    return 1;
}

/* Returns the battle result code (batRetCode) that battleEndChk stores. */
int battleRetCodeGet(void)
{
    return batRetCode;
}

void battleRetCodeSet(int retCode)
{
    batRetCode = retCode;
}

int thinkNoGet(void)
{
    return thinkNo;
}

void thinkNoSet(int no)
{
    thinkNo = no;
}

/*
 * plTbl: three 8-byte entries. plUnitTblGet and plUnitTblSet write only the first two fields of
 * each; nothing in this TU's C touches the remaining four bytes.
 */
typedef struct PlUnitEntry {
    short partyId;         /* +0x0 */
    short attackPosition;  /* +0x2 */
    unsigned char unmodeled_04[4];
} PlUnitEntry;
static PlUnitEntry plTbl[3];

PlUnitEntry *plUnitTblGet(void) {
    PartyAttackPosition *slot;
    PlUnitEntry *entry;
    int i;

    slot = PartyDataGet()->attack_slots;
    entry = plTbl;
    for (i = 0; i < 3; i++) {
        entry->partyId = (short)slot->party_id;
        entry->attackPosition = (short)slot->attack_position;
        slot++;
        entry++;
    }
    return plTbl;
}

/*
 * Assigns a party member and its attack position to slot `index`, both in the
 * party record's attack_slots and in plTbl.
 */
void plUnitTblSet(int index, int partyId, int attackPosition)
{
    PartyDataPrefix *party;

    party = PartyDataGet();
    party->attack_slots[index].party_id = partyId;
    party->attack_slots[index].attack_position = attackPosition;
    plTbl[index].partyId = partyId;
    plTbl[index].attackPosition = attackPosition;
}

/*
 * GameLoopState is a 0x2A030-byte record that several TUs each model only where
 * they touch it (for example src/main/script.h's event byte at +0x29F40). The
 * active monster-set number, read by monsSetNoGet and monsSetNoSet, is a signed
 * halfword at +0x29F90. The rest of the record is not recovered here.
 */
typedef struct GameLoopMonsSetView {
    unsigned char unmodeled_00[0x29F90];
    short monsSetNo; /* +0x29F90 */
} GameLoopMonsSetView;
extern GameLoopMonsSetView GameLoopState;

int monsSetNoGet(void)
{
    return GameLoopState.monsSetNo;
}

void monsSetNoSet(int monsSetNo)
{
    GameLoopState.monsSetNo = monsSetNo;
}

/*
 * GameLoopState also carries the current map number at +0x29F92, directly
 * after the monster-set number above. GameLoopMonsSetView ends at that
 * monster-set number, so this TU models the map number through its own prefix
 * view of the same object.
 */
typedef struct GameLoopMapNoPrefix {
    unsigned char unmodeled_00[0x29F92];
    short mapNo; /* +0x29F92 */
} GameLoopMapNoPrefix;

int mapNoGet(void)
{
    GameLoopMapNoPrefix *mapNoView = (GameLoopMapNoPrefix *)&GameLoopState;
    return mapNoView->mapNo;
}

void mapNoSet(int mapNo)
{
    GameLoopMapNoPrefix *mapNoView = (GameLoopMapNoPrefix *)&GameLoopState;
    mapNoView->mapNo = mapNo;
}

/*
 * GameLoopState also carries the encounter and event configuration bytes at
 * +0x29F96 and +0x29F97. GameLoopMonsSetView ends at the monster-set number, so
 * this TU models these bytes through its own prefix view of the same object.
 * The rest of the record is not recovered here.
 */
typedef struct GameLoopCfConfigPrefix {
    unsigned char unmodeled_00[0x29F96];
    u8 cfEncount; /* +0x29F96 */
    u8 cfEvent;   /* +0x29F97 */
} GameLoopCfConfigPrefix;

u8 cfEncountGet(void)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    return cfConfig->cfEncount;
}

void cfEncountSet(u8 cfEncount)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    cfConfig->cfEncount = cfEncount;
}

u8 cfEventGet(void)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    return cfConfig->cfEvent;
}

void cfEventSet(u8 cfEvent)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    cfConfig->cfEvent = cfEvent;
}

BattleUnit *actUnitGet(void)
{
    return pActUnit;
}

BattleUnit *lastUnitGet(void)
{
    return pLastUnit;
}

static BattleUnit *pTgtUnit;
static BattleUnit *pTgtUnit2;
ManWork *manWorkGet(void);

BattleUnit **tgtUnitAdrGet(void)
{
    switch (manWorkGet()->tgtSelect) {
    case MAN_SELECT_ETH_MENU:
    case MAN_SELECT_ETH_TARGET:
    case MAN_SELECT_ITM_MENU:
    case MAN_SELECT_ITM_TARGET:
    case MAN_SELECT_ITM_TARGET_WIDE:
        return &pTgtUnit2;
    }
    return &pTgtUnit;
}

BattleUnit *tgtUnitGet(void)
{
    return *tgtUnitAdrGet();
}

void tgtUnitSet(BattleUnit *unit)
{
    *tgtUnitAdrGet() = unit;
}

ManWork *manWorkGet(void)
{
    return &manWk;
}

extern int menuOpenChkBat(void);
static int menuFlag;

int menuChk(void)
{
    if (menuOpenChkBat() != 0) {
        return 1;
    }
    return menuFlag == 1;
}

BatCtrl *batCtrlGet(void)
{
    return &batCtrl;
}

void scenarioPtrFuncSet(ScenarioPhaseFunc func)
{
    scenarioPtr.nextPhase = func;
    scenarioPtr.phaseTimer = 0;
}

void scenarioBatExecPhase10(int source, ScenarioDstEntry *dest);
extern void sndInit2(void);
void scenarioBatInit(void)
{
    sndInit2();
    scenarioPtr.source = (ScenarioStep *)scenarioSrc;
    scenarioPtr.count = 0;
    scenarioPtr.index = 0;
    scenarioPtr.dest = scenarioDst;
    scenarioPtrFuncSet(scenarioBatExecPhase10);
}

int scenarioSrcSet(const ScenarioStep *source)
{
    ScenarioStep *destination = scenarioPtr.source;

    *destination = *source;
    return 1;
}

/* Append a copy of `entry` at dest[count]; 0 once the record limit is reached. */
int scenarioDstSet(const ScenarioDstEntry *entry)
{
    if (scenarioPtr.count >= SCENARIO_DST_LIMIT) {
        return 0;
    }
    scenarioPtr.dest[scenarioPtr.count] = *entry;
    scenarioPtr.count++;
    return 1;
}

/* Returns scenarioPtr.source as an int; scenarioBatExec passes it to the phase callback. */
int scenarioSrcGet(int unused)
{
    return (int)scenarioPtr.source;
}

/* The destination record `offset` steps past the current index, or NULL. */
ScenarioDstEntry *scenarioDstGet(int offset)
{
    int index = scenarioPtr.index + offset;

    if (index >= SCENARIO_DST_LIMIT || index >= scenarioPtr.count) {
        return 0;
    }
    return &scenarioPtr.dest[index];
}

/* Advances the source index by its record's stride, clamped to count; returns 1 once clamped. */
int scenarioBatNext(void)
{
    int index;

    index = scenarioPtr.index + scenarioPtr.source->stride;
    scenarioPtr.index = index;
    if (index >= scenarioPtr.count) {
        scenarioPtr.index = scenarioPtr.count;
        return 1;
    }
    return 0;
}


/*
 * Queues the camera event for a scenario-battle action: items (type 3) use
 * event 0x14 with the item's target-selection flags (bit 0x10 added when
 * tecId is set); techniques of kind 2 or 6, or with flag bit 0x10, get
 * none, the rest use event 2 or 3 by their range class. The CalcUnitParam
 * bit tested first is CALC_UNIT_FLAG_IS_AGWS (include/ov01/calc.h).
 */
void scenarioCam(ScenarioBatExec *exec, BattleUnit **targets)
{
    int tecKind;
    int range;

    if (calcUPGet(exec->unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        return;
    }
    if (exec->type == 3) {
        camEv.type = 0x14;
        camEv.unit = exec->unit;
        camEv.target = *targets;
        camEv.param = dataItmGet(exec->slotOrItemId)->trgFlags;
        if (exec->tecId != 0) {
            camEv.param |= 0x10;
        }
        camEventExec(&camEv);
        return;
    }
    tecKind = dataTecGet(exec->tecId)->kind;
    if (tecKind == 2 || tecKind == 6) {
        return;
    }
    if (dataTecGet(exec->tecId)->flags & 0x10) {
        return;
    }
    range = dataTecGet(exec->tecId)->flags & 0x3;
    if (range == 0) {
        camEv.type = 2;
        camEv.unit = exec->unit;
        camEv.param = exec->tecId;
        camEv.target = *targets;
        camEventExec(&camEv);
    } else if (range == 1) {
        camEv.type = 3;
        camEv.unit = exec->unit;
        camEv.param = exec->tecId;
        camEv.target = *targets;
        camEventExec(&camEv);
    }
}

void scenarioBatEnd(void);

/*
 * Run the installed scenario phase on the current destination record; 1 once
 * no record is left (scenarioBatEnd has run), 0 while records remain.
 */
int scenarioBatExec(void)
{
    ScenarioDstEntry *dest = scenarioDstGet(0);
    int source = scenarioSrcGet(0);

    if (dest) {
        scenarioPtr.nextPhase(source, dest);
        if (scenarioDstGet(0)) {
            return 0;
        }
    }
    scenarioBatEnd();
    return 1;
}

int sefCheckLoad(void);
int dataCdSync(void);
void dataSndSeLoad2(int unit);

int scenarioBatDataSync(int *sourceIndex)
{
    if (sefCheckLoad() != 0) {
        return 0;
    }
    if (dataCdSync() > 0) {
        return 0;
    }
    dataSndSeLoad2(*sourceIndex);
    return 1;
}

/*
 * The ether voice for a scenario action: 4 by default, 5 or 6 for the
 * listed techniques of characters 3, 6 and 7.
 */
int etehrCVGet(ScenarioBatExec *exec)
{
    int voice = 4;

    switch (calcUPGet(exec->unit)->charaId) {
    case 3:
        if (exec->tecId >= 0x236 && exec->tecId <= 0x239) {
            voice = 5;
        }
        break;
    case 6:
        if (exec->tecId == 0x254) {
            voice = 5;
        } else if (exec->tecId == 0x257) {
            voice = 6;
        }
        break;
    case 7:
        if (exec->tecId >= 0x262 && exec->tecId <= 0x263) {
            voice = 5;
        }
        break;
    }
    return voice;
}

extern void EXM_SetDirectionalWind(Vector4 *direction);
extern void EXM_SetShakePower(float power);
extern void EXM_SetShakeTime(float time);

/*
 * Blows a fixed directional wind and camera shake, then sets HAIR_WIND_BIT
 * on every other unit's actor (hairWindOff is this bit's pair; unit itself
 * is excluded).
 */
void hairWindOn(BattleUnit *unit)
{
    Vector4 direction;
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit *cur;
    int remaining;
    int count;

    direction.x = 0.0f;
    direction.y = 1.0f;
    direction.z = 0.0f;
    direction.w = 0.03f;
    EXM_SetDirectionalWind(&direction);
    EXM_SetShakePower(1.0f);
    EXM_SetShakeTime(0.75f);

    count = unitTblGet(0, &table);
    if (count > 0) {
        next = table;
        remaining = count;
        do {
            cur = *next;
            next++;
            if (cur != 0 && cur != unit) {
                cur->actor->moveFlags |= HAIR_WIND_BIT;
            }
            remaining--;
        } while (remaining != 0);
    }
}

extern void EXM_StopWind(void);

/*
 * hairWindOn's pair: stops the wind and clears HAIR_WIND_BIT on every other
 * unit's actor again (unit itself is excluded).
 */
void hairWindOff(BattleUnit *unit)
{
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit *cur;
    int remaining;
    int count;

    EXM_StopWind();
    count = unitTblGet(0, &table);
    if (count > 0) {
        next = table;
        remaining = count;
        do {
            cur = *next;
            next++;
            if (cur != 0 && cur != unit) {
                cur->actor->moveFlags &= ~HAIR_WIND_BIT;
            }
            remaining--;
        } while (remaining != 0);
    }
}

void statEffOff(BattleUnit *unit);
/* Defined in src/main/sef.c and src/ov01/snd.c. */
extern void sefLoadEffect(int effect_no, int character_id);
extern int sndSeRegChk(BattleUnit *unit, int seType);
extern void dataSndSeLoad(BattleUnit *unit, int seId);
void scenarioBatExecPhase20(int source, ScenarioDstEntry *dest);

/*
 * First phase of a scenario action. `source` is the record scenarioSrcGet
 * hands out as an int (the action: unit, type, technique id, item id or
 * weapon slot, read at +0x0/+0x4/+0x8/+0xC); `dest` is the current
 * destination record, whose first word scenarioCam reads as the target.
 * On the phase's first frame it suspends the unit's status-effect ticks,
 * queues the action camera, loads the action's effect and its sound effect,
 * then hands over to scenarioBatExecPhase20 within the same frame.
 */
void scenarioBatExecPhase10(int source, ScenarioDstEntry *dest)
{
    ScenarioBatExec *exec = (ScenarioBatExec *)source;
    int effectNo;
    int seType;

    if (scenarioPtr.phaseTimer == 0) {
        statEffOff(exec->unit);
        scenarioCam(exec, (BattleUnit **)dest);
        if (exec->type != 3) {
            effectNo = dataTecGet(exec->tecId)->effectNo;
        } else {
            effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
        }
        sefLoadEffect(effectNo, calcUPGet(exec->unit)->charaId);
        seType = sndConvSpid(exec->unit, exec->type, exec->tecId,
                             exec->slotOrItemId);
        if (!sndSeRegChk(exec->unit, seType)) {
            dataSndSeLoad(exec->unit, seType);
        }
    }
    scenarioPtrFuncSet(scenarioBatExecPhase20);
    scenarioBatExecPhase20(source, dest);
}

extern void unitFaceMotSet(BattleUnit *unit, int face);
extern void dataUnitFileLoadMotSp(BattleUnit *unit, int tecId, int animGroup);
extern void dataFileLoadWepSp(int unit, int tecId);
extern void scenarioBatExecPhase25(int source, ScenarioDstEntry *dest);

/*
 * Second phase of a scenario action: sets the motion, face and sound cues for
 * the action type and waits for the data load, then hands over to
 * scenarioBatExecPhase25. Type 1 is a technique, type 3 an item.
 */
void scenarioBatExecPhase20(int source, ScenarioDstEntry *dest) {
    ScenarioBatExec *arg0 = (ScenarioBatExec *)source;
    int *ap = (int *)dest;
    int temp_17;
    int t;
    int var_5;
    int temp_2_2;
    int temp_4_2;

    int state = arg0->type;
    /* Type 2: start the motion and wind, wait for the data, then end the wind. */
    if (state == 2) {
        switch (scenarioPtr.phaseTimer) {
        case 0:
            unitMotSet(arg0->unit, 0x1F);
            if (!(*(int *)*(int *)((char *)arg0->unit + 0x10) & ACTOR_FLAG_ENEMY) && !(calcUPGet(arg0->unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                if ((calcUPGet(arg0->unit)->charaId < 0xBB) || (calcUPGet(arg0->unit)->charaId >= 0xC3)) {
                    decoEffCall(arg0->unit, 0);
                    unitFaceMotSet(arg0->unit, 0x36);
                    unitWpnMotSet(arg0->unit, dataDefWpnGet(arg0->unit), 0, 0x31);
                    if (calcUPGet(arg0->unit)->charaId == 7) {
                        unitWpnMotSet(arg0->unit, 1, 0, 0x31);
                    }
                    sndSysSePlay(0x1D);
                    sndSeTransPlay(arg0->unit, 0x17, ((int (*)(int *, int))etehrCVGet)((int *)arg0, (int)dest));
                    hairWindOn(arg0->unit);
                }
            }
            scenarioPtr.phaseTimer = 1;
            /* fallthrough */
        case 1:
            scenarioMtd(arg0, (int)ap);
            if (((int (*)(int *, int))scenarioBatDataSync)((int *)arg0, (int)dest) != 0 && (*(int *)((char *)*(int *)((char *)arg0->unit + 0x14) + 0x6F0) & 0x1000)) {
                scenarioPtr.phaseTimer = 2;
            } else {
                return;
            }
            /* fallthrough */
        default:
            unitFaceMotSet(arg0->unit, 0x35);
            hairWindOff(arg0->unit);
            scenarioPtrFuncSet(scenarioBatExecPhase25);
            scenarioBatExecPhase25(source, dest);
            return;
        }
    }
    /* Type 3: play the transition sound once the data is loaded. */
    if (state == 3) {
        switch (scenarioPtr.phaseTimer) {
        case 0:
            if (((int (*)(int *, int))scenarioBatDataSync)((int *)arg0, (int)dest) == 0) {
                return;
            }
            scenarioPtr.phaseTimer = 1;
            /* fallthrough */
        case 1:
            sndSeTransPlay(arg0->unit, 0x17, ((int)arg0->unit ^ *ap) == 0 ? 1 : 2);
            scenarioPtrFuncSet(scenarioBatExecPhase25);
            scenarioBatExecPhase25(source, dest);
            return;
        }
        return;
    }
    /* Type 1 (technique): load the technique's motion and weapon files, then its sound effects. */
    if (state == 1) {
        switch (scenarioPtr.phaseTimer) {
        case 0:
            temp_17 = *(short *)((char *)dataTecGet(arg0->tecId) + 0x14);
            temp_4_2 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0x11);
            if (temp_17 != 0) {
                if (temp_4_2 == 2) {
                    goto case2;
                } else if (temp_4_2 == 6) {
                    goto block_38;
                } else if (temp_4_2 == 5) {
                    goto block_38;
                } else {
                    goto skip;
                }
case2:
                unitMotSet(arg0->unit, 0x28);
                decoEffCall(arg0->unit, 1);
                sndSysSePlay(0x1E);
                sndSeTransPlay(arg0->unit, 0x17, 8);
block_38:
                dataUnitFileLoadMotSp(arg0->unit, (int)temp_17, 4);
                dataFileLoadWepSp((int)arg0->unit, arg0->tecId);
skip:
                ;
            }
            scenarioPtr.phaseTimer = 1;
            /* fallthrough */
        case 1:
            if (((int (*)(int *, int))scenarioBatDataSync)((int *)arg0, (int)dest) != 0) {
                temp_4_2 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0x11);
                if (temp_4_2 == 2) {
                    if (!(*(int *)((char *)*(int *)((char *)arg0->unit + 0x14) + 0x6F0) & 0x1000)) {
                        return;
                    }
                }
                if (!(*(int *)*(int *)((char *)arg0->unit + 0x10) & ACTOR_FLAG_ENEMY) && !(calcUPGet(arg0->unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                    if ((calcUPGet(arg0->unit)->charaId < 0xBB) || (calcUPGet(arg0->unit)->charaId >= 0xC3)) {
                        temp_2_2 = sndConvSpid(arg0->unit, arg0->type, arg0->tecId, arg0->slotOrItemId);
                        var_5 = 5;
                        if (temp_2_2 != 3) {
                            var_5 = (temp_2_2 ^ 4) ? 0 : 7;
                        }
                        if (var_5 != 0) {
                            dataSndSeLoad(arg0->unit, var_5);
                        }
                    }
                }
                scenarioPtrFuncSet(scenarioBatExecPhase25);
                scenarioBatExecPhase25(source, dest);
                return;
            }
            return;
        default:
            return;
        }
    }
    if (scenarioBatDataSync((int *)arg0) != 0) {
        scenarioPtrFuncSet(scenarioBatExecPhase25);
        scenarioBatExecPhase25(source, dest);
        return;
    }
    return;
}

extern void dataMapLoad(int no);
extern int dataCdSync(void);
extern int mapNoGet(void);
extern void mapDispOffAll(void);
extern void unitDispOffAll(void);
extern void mapDispOnAll(void);
extern void unitDispOnAll(void);
extern void fadeIn(int a, int b);
extern int MMv2IsPlaying(void);
extern void MMv2Stop(void);
extern int MMv2Play(const char *filename);
static int loadNum[1];
static const char *mvFname[5];
void scenarioBatExecPhase30(int source, ScenarioDstEntry *dest);

/*
 * Scenario phase for the movie actions (types 0x38, 0x235 and 0x239): hides the
 * map and units, plays the movie, reloads the map and shows it again, then
 * hands over to scenarioBatExecPhase30. Other actions go straight there.
 */
void scenarioBatExecPhase25(int source, ScenarioDstEntry *dest) {
    int type = *(int *)((char *)source + 0x8);
    if (type == 0x38 || type == 0x235 || type == 0x239) {
        switch (scenarioPtr.phaseTimer) {
        case 0:
            mapDispOffAll();
            unitDispOffAll();
            scenarioPtr.phaseTimer++;
            return;
        case 1: {
            int idx = 2;
            if (type != 0x38) {
                idx = (type == 0x235) ? 3 : 4;
            }
            MMv2Play(mvFname[idx]);
            scenarioPtr.phaseTimer++;
            return;
        }
        case 2:
            if (MMv2IsPlaying() == 0) {
                MMv2Stop();
                loadNum[0] = dataCdSync();
                dataMapLoad(mapNoGet());
                scenarioPtr.phaseTimer++;
            }
            return;
        case 3:
            {
                int cd = dataCdSync();
                if (loadNum[0] >= cd) {
                fadeIn(4, 0);
                mapDispOnAll();
                unitDispOnAll();
                scenarioPtr.phaseTimer++;
                }
            }
            return;
        case 4:
            scenarioPtrFuncSet(scenarioBatExecPhase30);
            scenarioBatExecPhase30(source, dest);
            return;
        default:
            return;
        }
    }
phase30:
    scenarioPtrFuncSet(scenarioBatExecPhase30);
    scenarioBatExecPhase30(source, dest);
}

extern int sndConvSe(BattleUnit *unit, int type, int tecId);
extern void dataUnitFileLoadMotSp2(BattleUnit *unit, int a, int b, int c, int d);
extern void dataFileLoadWepSp2(BattleUnit *unit, int tecId);
extern void unitMotSetEx(void *unit, int motion, int flags);
extern const char battleInitMotPackMotActText[];
extern int scenarioEffCall(ScenarioBatExec *arg0p, u8 *arg1p);
void scenarioBatExecPhase40(int source, ScenarioDstEntry *dest);

/*
 * Third phase of a scenario action: sets the actor's motion and weapon motion
 * for the action type (type 1 is a technique), plays the action sound, runs
 * scenarioEffCall on each destination record, then hands over to
 * scenarioBatExecPhase40.
 */
void scenarioBatExecPhase30(int source, ScenarioDstEntry *dest) {
    ScenarioBatExec *arg0 = (ScenarioBatExec *)source;
    s16 temp_20;
    int temp_19;
    int var_18;
    int temp_16_2;
    int temp_2;
    int var_18_3;
    int var_7;
    int i;
    signed char temp_16;
    signed char var_16;
    u8 *temp_2_2;

    if (arg0->type == 2) {
        unitMotSet(arg0->unit, 0x20);
        (*(int *)((char *)arg0->unit->work + 0x70)) = 1;
    } else if (arg0->type == 3) {
        unitMotSet(arg0->unit, 0x27);
        unitWpnMotSet(arg0->unit, dataDefWpnGet(arg0->unit), 0, 0x33);
    } else if (arg0->type == 1) {
        temp_20 = *(s16 *)((char *)dataTecGet(arg0->tecId) + 0x14);
        temp_19 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0x11);
        if (temp_20 == 0) {
            var_18 = 1;
            temp_16 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0xB);
            unitMotSet(arg0->unit, (int) temp_16);
            if (arg0->tecId != 0x27) {
                var_18 = arg0->slotOrItemId;
            }
            unitWpnMotSet(arg0->unit, var_18, 0, temp_16 + 0x22);
        } else {
            if (temp_19 == 2 || temp_19 == 6 || temp_19 == 5) {
                var_18 = 2;
                if (dataTid2WepSp(arg0->tecId) == 0) {
                    var_18 = arg0->slotOrItemId;
                }
                dataUnitFileLoadMotSp2(arg0->unit, (int) temp_20, 4, var_18, (int) temp_19);
                dataFileLoadWepSp2(arg0->unit, arg0->tecId);
                var_16 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0xB);
                unitMotSetEx(arg0->unit, (int) var_16, 4);
                if (temp_19 != 5) {
                    unitWpnMotSet(arg0->unit, var_18, 1, var_16 + 2);
                } else {
                    if (((signed char *)calcUPGet(arg0->unit))[0x5A] < 0x20) {
                        var_7 = 3;
                        var_18_3 = 4;
                    } else {
                        var_7 = 4;
                        var_18_3 = 3;
                    }
                    unitWpnMotSet(arg0->unit, 0, 1, var_7);
                    unitWpnMotSet(arg0->unit, 1, 1, var_18_3);
                }
            } else {
                var_16 = *(signed char *)((char *)dataTecGet(arg0->tecId) + 0xB);
                unitMotSetEx(arg0->unit, (int) var_16, arg0->slotOrItemId + 1);
                unitWpnMotSet(arg0->unit, arg0->slotOrItemId, 0, var_16 + 2);
            }
            printf(battleInitMotPackMotActText, temp_20, var_16, temp_19);
        }
        if (dataMtdRead(arg0->unit, 2) == 0) {
            spWepDispOn(arg0, dataDefWpnGet(arg0->unit));
        }
    }
    temp_16_2 = sndConvSpid(arg0->unit, arg0->type, arg0->tecId, arg0->slotOrItemId);
    temp_2 = sndConvSe(arg0->unit, arg0->type, arg0->tecId);
    if (temp_16_2 > 0) {
        sndSeTransPlay(arg0->unit, temp_16_2, temp_2);
    }
    for (i = 0; i < *(int *)((char *)arg0 + 0x10); i++) {
        temp_2_2 = (u8 *)scenarioDstGet(i);
        scenarioEffCall(arg0, temp_2_2);
        (*(int *)((char *)temp_2_2 + 0x14)) = 0;
    }
    scenarioPtrFuncSet(scenarioBatExecPhase40);
}

#define TEC_PROC_FLOW_SPEC 0x20
/* Actor.moveFlags bit 0x1000: set while the unit is still held in its action motion. */
#define ACTOR_MOTION_HOLD_BIT 0x1000

int scenarioSrcEndChk(ScenarioBatExec *exec, ScenarioDstEntry *dest, int done);
/* Unprototyped on purpose: scenarioBatExecPhase40 calls this with two
 * arguments and specSyoukanProc with three, and the body reads only the
 * first (the extra argument registers are passed through unchanged). */
void scenarioBatExecMain();
void scenarioBatExecPhase50(int source, ScenarioDstEntry *dest);
/* The special-flow technique handlers: nonzero once the flow is done. */
int flowSpecProcInit(ScenarioBatExec *exec, ScenarioDstEntry *dest);
int flowSpecProc(ScenarioBatExec *exec, ScenarioDstEntry *dest);
/* Defined by ov01/tu004 calc.c; takes the unit. */
extern int calcHitMotChk(BattleUnit *unit);
/* Defined by ov01/tu005 data_unit_org_get.c. */
extern void dataFileLoadWepSpEnd(BattleUnit *unit, int tecId);
/*
 * Four-argument call: the target, the action type and the technique id are
 * passed in $5..$7; unit_cmd.c's definition reads only the unit.
 */
extern void unitCmdAtkEnd(BattleUnit *unit, BattleUnit *target, int type, int tecId);

/*
 * Install the next phase handler and run it within the same frame. The
 * do { } while (0) keeps the handler a plain call: the original returns
 * through its own epilogue after `jal scenarioBatExecPhase50` instead of
 * jumping to it.
 */
#define SCENARIO_PHASE_ENTER(phase) \
    do { \
        scenarioPtrFuncSet(phase); \
        phase(source, dest); \
    } while (0)

/*
 * The action's main phase, run on frames 0 and 1 of the phase: an ordinary
 * action runs scenarioBatExecMain; a special-flow technique starts its flow
 * (on frame 0, once the unit is held in motion, the hit motion check passes
 * or the effect reaches the target) and steps it afterwards. Once the
 * action has ended, the special weapon is put away, the unit's weapon
 * motions are reset (5 for an AGWS, 0xFF otherwise), the attack is closed
 * on its target and scenarioBatExecPhase50 takes over.
 */
void scenarioBatExecPhase40(int source, ScenarioDstEntry *dest)
{
    ScenarioBatExec *exec = (ScenarioBatExec *)source;
    int done;
    BattleUnit *target;

    if (scenarioPtr.phaseTimer < 2) {
        if (scenarioPtr.phaseTimer >= 0) {
            done = 0;
            scenarioMtd(exec);
            if (exec->type == 3 || !(dataTecGet(exec->tecId)->procFlags & TEC_PROC_FLOW_SPEC)) {
                scenarioBatExecMain(source, dest);
            } else if (scenarioPtr.phaseTimer == 0) {
                if ((exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT) || calcHitMotChk(exec->unit)
                    || sefIsHitActor(((BattleUnit *)dest->unit)->actor)) {
                    done = flowSpecProcInit(exec, dest);
                    scenarioPtr.phaseTimer = 1;
                }
            } else {
                done = flowSpecProc(exec, dest);
            }
            if (scenarioSrcEndChk(exec, dest, done)) {
                if (exec->type == 1) {
                    if (!dataMtdRead(exec->unit, 3)) {
                        spWepDispOff(exec, dataDefWpnGet(exec->unit));
                    }
                    dataFileLoadWepSpEnd(exec->unit, exec->tecId);
                }
                if (calcUPGet(exec->unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
                    int i;

                    for (i = 0; i < 3; i++) {
                        if (calcUPGet(exec->unit)->weaponId[i] != 0) {
                            unitWpnMotSet(exec->unit, i, 0, 5);
                        }
                    }
                    target = dest->unit;
                } else {
                    /* dest->unit is read at the end of each branch, which keeps the
                     * load in both predecessors of the call's block. */
                    int i;

                    for (i = 0; i < 3; i++) {
                        if (calcUPGet(exec->unit)->weaponId[i] != 0) {
                            unitWpnMotSet(exec->unit, i, 0, 0xFF);
                        }
                    }
                    target = dest->unit;
                }
                unitCmdAtkEnd(exec->unit, target, exec->type, exec->tecId);
                SCENARIO_PHASE_ENTER(scenarioBatExecPhase50);
            }
        }
    }
}

/* Defined in src/main/sef.c; nonzero once the effect has finished. */
extern int sefIsFinishEffect(int effect_no);
void statEffOn(BattleUnit *unit);

/*
 * Wait for the action's effect (the item's for an item action, else the
 * technique's) to finish, then queue camera event 1 for the acting unit,
 * re-enable its status-effect ticks and advance the scenario.
 */
void scenarioBatExecPhase50(int source, ScenarioDstEntry *dest)
{
    ScenarioBatExec *exec = (ScenarioBatExec *)source;
    int effectNo;

    if (exec->type != 3) {
        effectNo = dataTecGet(exec->tecId)->effectNo;
    } else {
        effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
    }
    if (sefIsFinishEffect(effectNo)) {
        camEv.type = 1;
        camEv.unit = exec->unit;
        camEv.param = exec->tecId;
        camEventExec(&camEv);
        statEffOn(exec->unit);
        scenarioBatNext();
    }
}

extern int sefIsSeSignal(int seSignal);
extern int sndConvHitSe(BattleUnit *unit, int mode, int tecId);
extern void sefHitEffect(void);
extern void menuCritDispOn(int mode);
extern int scenarioBatExecDst(int *arg0, int *arg1);

/*
 * Applies the action's hits to each destination record: plays the hit sound,
 * runs the damage record and sets bit 0x4000 on the target's work word.
 */
void scenarioBatExecMain(ScenarioStep *rec, int arg1, int arg2)
{
    int *rw = (int *)rec;
    int i;
    int flag;
    int hit;
    int *d;

    i = 0;
    if (rw[4] > 0) {
        do {
            flag = 0;
            d = (int *)scenarioDstGet(i);
            if (sefIsSeSignal(*(int *)(*d + 0x14)) != 0) {
                sndSePlay(rec->exec.unit, sndConvSpid(rec->exec.unit, rw[1], rw[2], rw[3]),
                          sndConvHitSe(rec->exec.unit, rw[1], rw[2]) + d[5]);
            }
            hit = calcHitMotChk(rec->exec.unit);
            if (hit != 0 || sefIsHitActor((Actor *) *(int *)(*d + 0x14)) != 0) {
                if (d[1] != 0xA || (*(int *)(*(int *)(*d + 0x10)) & 0x4000) == 0) {
                    scenarioBatExecDst((int *)rec, d);
                    if (d[1] != 0xA) {
                        sefHitEffect();
                        if (hit != 0) {
                            sndSePlay(rec->exec.unit,
                                      sndConvSpid(rec->exec.unit, rw[1], rw[2], rw[3]),
                                      sndConvHitSe(rec->exec.unit, rw[1], rw[2]) + d[5]);
                        }
                        flag = 1;
                    }
                    if (rw[1] != 3 && d[5] == *(signed char *)((char *)dataTecGet(rw[2]) + 0x10) - 1) {
                        goto dmg;
                    }
                    if (rw[1] == 3 && d[5] == 0) {
                        goto dmg;
                    }
                    if (d[1] == 0xA) {
dmg:
                        dmgValPut((int)rec, (BattleUnit **)d);
                        if ((*(unsigned long long *)rec->exec.unit->work & 0x2040) == 0x2000) {
                            menuCritDispOn(0);
                        }
                    }
                    if (flag != 0) {
                        d[5]++;
                    }
                    *(int *)(*(int *)(*d + 0x10)) |= 0x4000;
                }
            }
            i++;
        } while (i < rw[4]);
    }
}

/* The stolen-item state that nusumeSet and nusumeGet share. */
static int nusumuFlag;

void nusumeSet(int value)
{
    nusumuFlag = value;
}

int nusumeGet(void)
{
    return nusumuFlag;
}

extern const char battleInitFLOWSPECText[];
static unsigned int jtypeIdx;
extern void specAnalyzeProc(BattleUnit *unit);
extern void specEscapeProc(void);
extern int specNusumuProc(void *exec, BattleUnit **dest);

/*
 * Starts the special-flow technique exec->tecId on the target: runs its one-time
 * effect (analysis, escape, calcBc, status resets or the nusume check). Always
 * returns 0.
 */
int flowSpecProcInit(ScenarioBatExec *exec, ScenarioDstEntry *dest)
{
    jtypeIdx = 0;
    switch (exec->tecId) {
    case 555:
        specAnalyzeProc(dest->unit);
        break;
    case 556:
        specEscapeProc();
        break;
    case 559:
        calcBc((BattleUnit *)dest->unit, 1);
        break;
    case 57:
        calcStatSet(dest->unit, 0, 0x1000, 0);
        calcStatSet(dest->unit, 0, 0x400, 0);
        calcStatSet(dest->unit, 0, 0x80, 0);
        calcStatSet(dest->unit, 4, 0x800, 0);
        calcStatSet(dest->unit, 4, 0x200, 0);
        calcStatSet(dest->unit, 4, 0x400, 0);
        break;
    case 90:
        if (specNusumuProc(exec, &dest->unit)) {
            nusumeSet(1);
        } else {
            nusumeSet(0);
        }
        sefHitEffect();
        break;
    case 596:
        calcStatSet(exec->unit, 7, 2, 0);
        exec->unit->work->movePhaseTimer = 0;
        break;
    case 599:
        calcStatSet(exec->unit, 7, 1, 0);
        exec->unit->work->movePhaseTimer = 0;
        break;
    case 605:
        specNusumuProc(exec, &dest->unit);
        break;
    case 565 ... 568:
    case 580:
    case 593:
    case 600:
    case 606:
    case 608:
    case 610:
    case 612:
        break;
    }
    printf(battleInitFLOWSPECText, exec->tecId);
    return 0;
}

int specSyoukanProc(ScenarioStep *rec, int arg1, unsigned int *state, int arg3);
int specHensinProc(void *arg0, int arg1, unsigned int *arg2);
void specInoriProc(void);
int specNusumuProc(void *arg0, BattleUnit **arg1);

/*
 * Per-frame step of the special-flow technique exec->tecId: waits for its
 * effect, summon or battle message. Returns nonzero once the flow is done.
 */
int flowSpecProc(ScenarioBatExec *exec, ScenarioDstEntry *dest)
{
    int summonId;
    int done = 0;

    switch (exec->tecId) {
    case 555:
        eBattleWinMain3();
        if (dataPadRead(1) & (PAD_CIRCLE | PAD_CROSS)) {
            done = 1;
            eBattleWinClose3();
        }
        break;
    case 565:
        done = sefIsFinishEffect(dataTecGet(565)->effectNo);
        if (done == 1 && calcStatGet(dest->unit, 1, 0x100) == 0) {
            specNusumuProc(exec, &dest->unit);
            battleMsg((int)&msgWork);
        }
        break;
    case 566:
    case 567:
    case 568:
        if (exec->tecId == 566) {
            summonId = 190;
        } else if (exec->tecId == 567) {
            summonId = 191;
        } else {
            summonId = exec->tecId == 568 ? 192 : 193;
        }
        done = specSyoukanProc((ScenarioStep *)exec, (int)dest, &jtypeIdx, summonId);
        break;
    case 57:
        done = exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT;
        break;
    case 556:
        done = exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT;
        break;
    case 559:
        done = exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT;
        break;
    case 576:
        done = specSyoukanProc((ScenarioStep *)exec, (int)dest, &jtypeIdx, 194);
        break;
    case 90:
        if (sefIsFinishEffect(dataTecGet(90)->effectNo)
            && (exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT)) {
            done = 1;
            battleMsg((int)&msgWork);
        }
        break;
    case 593:
        done = sefIsFinishEffect(dataTecGet(593)->effectNo);
        if (done == 1) {
            specInoriProc();
        }
        break;
    case 596:
    case 599:
        done = specHensinProc(exec, (int)dest, &jtypeIdx);
        break;
    case 601:
        done = specSyoukanProc((ScenarioStep *)exec, (int)dest, &jtypeIdx, 189);
        break;
    case 605:
        done = sefIsFinishEffect(dataTecGet(605)->effectNo);
        if (done == 1) {
            battleMsg((int)&msgWork);
        }
        break;
    case 610:
    case 611:
        summonId = exec->tecId == 610 ? 187 : 188;
        done = specSyoukanProc((ScenarioStep *)exec, (int)dest, &jtypeIdx, summonId);
        break;
    case 666:
    case 667:
        done = sefIsFinishEffect(dataTecGet(exec->tecId)->effectNo);
        break;
    }
    return done;
}

/*
 * syoukanFadeOut clears and re-queues a CMD_FADEOUT command on every side-0
 * unit except the acting unit itself.
 */
void syoukanFadeOut(BattleUnit *unit)
{
    BattleUnit **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && table[i] != unit) {
            CmdEntry cmd;

            cmd.selector = CMD_FADEOUT;
            unitCmdClear(table[i]);
            unitCmdSet(table[i], &cmd);
        }
    }
}

/* syoukanFadeIn is syoukanFadeOut's counterpart with the CMD_FADEIN command. */
void syoukanFadeIn(BattleUnit *unit)
{
    BattleUnit **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && table[i] != unit) {
            CmdEntry cmd;

            cmd.selector = CMD_FADEIN;
            unitCmdClear(table[i]);
            unitCmdSet(table[i], &cmd);
        }
    }
}

/*
 * syoukan1Sub's first argument: the two words the body touches
 * (src/ov01/data_unit_org_get.c owns the rest of whatever record this comes
 * from). The second argument is never read by this body; the
 * third is the character id of the unit the summon replaces, carried in the
 * 0x2B command's extra word (syoukan2Sub fills the same word from
 * linkedUnit->charaId).
 */
typedef struct Syoukan1SubArgs {
    BattleUnit *unit; /* +0x00 */
    unsigned char unmodeled_04[4];
    int tecId;         /* +0x08 */
} Syoukan1SubArgs;

void syoukan1Sub(Syoukan1SubArgs *args, int reserved, int replacementCharaId)
{
    CmdEntry cmd;

    unitCmdClear(args->unit);

    cmd.selector = CMD_UNITCHANGE;
    cmd.argument = 8;
    cmd.extra = replacementCharaId;
    unitCmdSet(args->unit, &cmd);

    if (dataTecGet(args->tecId)->flags & 0x10) {
        cmd.selector = CMD_TURNCENTER;
    } else {
        cmd.selector = CMD_TURNCHA;
    }
    unitCmdSet(args->unit, &cmd);

    cmd.selector = CMD_BTST;
    cmd.argument = 0;
    unitCmdSet(args->unit, &cmd);

    cmd.selector = CMD_MOT;
    cmd.argument = 7;
    cmd.extra = 0;
    unitCmdSet(args->unit, &cmd);
}

extern int unitTelOut(BattleUnit *unit);

void syoukan2Sub(BattleUnit **unit)
{
    CmdEntry cmd;

    unitCmdClear(*unit);

    cmd.selector = CMD_UNITCHANGE;
    cmd.argument = calcUPGet(*unit)->linkedUnit->posId;
    cmd.extra = calcUPGet(*unit)->linkedUnit->charaId;
    unitCmdSet(*unit, &cmd);

    cmd.selector = CMD_MOT;
    cmd.argument = 1;
    cmd.extra = 1;
    unitCmdSet(*unit, &cmd);

    cmd.selector = CMD_FADEIN;
    unitCmdSet(*unit, &cmd);
}


/*
 * Summon sequence of a special-flow technique, stepped by *state: fades the
 * other units out, teleports the unit out, changes it, applies the hits,
 * changes it back and fades the others in. Returns 1 once the sequence is
 * finished.
 */
int specSyoukanProc(ScenarioStep *rec, int arg1, unsigned int *state, int arg3)
{
    int ret;
    unsigned int s;
    char *p;

    ret = 0;
    s = *state;
    switch (s) {
    case 0:
        syoukanFadeOut(rec->exec.unit);
        p = *(char **)((char *)calcUPGet(rec->exec.unit) + 0x158);
        if (p != 0) {
            *(int *)(p + 0xA8C) |= 0x100;
        }
        s++;
        statEffOff(rec->exec.unit);
        break;
    case 1:
        if (unitTelOut(rec->exec.unit) != 0) {
            s++;
        }
        break;
    case 2:
        s++;
        syoukan1Sub((Syoukan1SubArgs *)rec, arg1, arg3);
        break;
    case 3:
        scenarioBatExecMain(rec, arg1, arg3);
        if (!(rec->exec.unit->work->flags & ACTOR_FLAG_CMD_PENDING)) {
            s++;
        }
        break;
    case 4:
        s++;
        syoukan2Sub(&rec->exec.unit);
        break;
    case 5:
        if (!(rec->exec.unit->work->flags & ACTOR_FLAG_CMD_PENDING)) {
            s++;
            syoukanFadeIn(rec->exec.unit);
        }
        break;
    case 6:
        p = *(char **)((char *)calcUPGet(rec->exec.unit) + 0x158);
        if (p != 0) {
            *(int *)(p + 0xA8C) &= ~0x100;
        }
        ret = 1;
        statEffOn(rec->exec.unit);
        break;
    }
    *state = s;
    return ret;
}

extern void dataLeaderCidReset(void);
extern void mapDispOffAll(void);
extern void unitDispOffAll(void);
extern void mapDispOnAll(void);
extern void unitDispOnAll(void);
extern void fadeIn(int a, int b);
extern void dataMapLoad(int no);
extern int MMv2IsPlaying(void);
extern void MMv2Stop(void);
extern int MMv2Play(const char *filename);
static int loadNum[1];
static const char *mvFname[5];

/*
 * Transformation sequence of a special-flow technique, stepped by *arg2:
 * teleports the unit out and changes it; for character 6 it also hides the map,
 * plays a movie and reloads the map. Returns 1 once the sequence is finished.
 */
int specHensinProc(void *arg0, int arg1, unsigned int *arg2)
{
    int cmd[8];
    int ret;
    unsigned int s;
    int v;

    s = *arg2;
    ret = 0;
    switch (s) {
    case 0:
        s++;
        /* fallthrough */
    case 1:
        if (unitTelOut(*(BattleUnit **)arg0) != 0) {
            s++;
        }
        break;
    case 2:
        unitCmdClear(*(BattleUnit **)arg0);
        dataLeaderCidReset();
        cmd[0] = CMD_UNITCHANGE;
        if (calcUPGet(*(BattleUnit **)arg0)->posId < 0) {
            v = -calcUPGet(*(BattleUnit **)arg0)->posId;
        } else {
            v = calcUPGet(*(BattleUnit **)arg0)->posId;
        }
        s++;
        cmd[1] = v;
        cmd[2] = calcUPGet(*(BattleUnit **)arg0)->charaId;
        unitCmdSet(*(BattleUnit **)arg0, (CmdEntry *)cmd);
        cmd[0] = CMD_BTST;
        cmd[1] = 0;
        cmdListChange(*(BattleUnit **)arg0, cmd);
        break;
    case 3:
        if (!(*(int *)(*(BattleUnit **)arg0)->work & ACTOR_FLAG_CMD_PENDING)) {
            if (calcUPGet(*(BattleUnit **)arg0)->charaId == 6) {
                s++;
                mapDispOffAll();
                unitDispOffAll();
            } else {
                ret = 1;
            }
        }
        break;
    case 4:
        s++;
        {
            unsigned int x = *(unsigned int *)((char *)arg0 + 0x8);
            unsigned int idx = x != 0x254;
            MMv2Play(mvFname[idx]);
        }
        break;
    case 5:
        if (MMv2IsPlaying() == 0) {
            s++;
            MMv2Stop();
            loadNum[0] = dataCdSync();
            dataMapLoad(mapNoGet());
        }
        break;
    case 6:
        v = dataCdSync();
        if (loadNum[0] >= v) {
            fadeIn(4, 0);
            s++;
            mapDispOnAll();
            unitDispOnAll();
        }
        break;
    case 7:
        ret = 1;
        break;
    }
    *arg2 = s;
    return ret;
}

/* Window record eBattleWinOpen3 reads; field order follows the stack stores. */
typedef struct SpecAnalyzeWin {
    int x;
    int y;
    int style;
    const char *name;
    unsigned short current_hp;
    unsigned short max_hp;
    unsigned short weakness_flags;
    int tech_points;
    int ether_points;
    int skill_points;
    int experience;
    unsigned long long money;
    unsigned short item;        /* item group << 8 | item id */
    unsigned short rare_item;   /* item group << 8 | item id */
} SpecAnalyzeWin;

extern void UmnDataBaseAnalisisSet(int charaId);
extern const char **dataEnemyNameGet(int index);
extern void eBattleWinOpen3(SpecAnalyzeWin *win);

/*
 * Records the unit in the monster database and opens the analysis window with
 * its name and data-record fields.
 */
void specAnalyzeProc(BattleUnit *arg0)
{
    SpecAnalyzeWin w;
    unsigned char *temp;

    temp = dataGainGet(calcUPGet(arg0)->charaId);
    UmnDataBaseAnalisisSet(calcUPGet(arg0)->charaId);
    w.x = 0x10;
    w.y = 0x6C;
    w.style = 0xFFFFFD;
    w.name = *dataEnemyNameGet(calcUPGet(arg0)->charaId);
    w.current_hp = calcUPGet(arg0)->currentHp;
    w.max_hp = ((unsigned short *)calcUPGet(arg0))[0];
    w.weakness_flags = calcUPGet(arg0)->conditions;
    w.tech_points = temp[0x6];
    w.ether_points = temp[0x7];
    w.skill_points = temp[0x8];
    w.money = ((unsigned short *)temp)[2];
    w.experience = *(int *)temp;
    w.item = temp[0x10] + (temp[0xD] << 8);
    w.rare_item = temp[0x11] + (temp[0xE] << 8);
    eBattleWinOpen3(&w);
}

void specEscapeProc(void)
{
    escapeFlag = 1;
}

typedef struct InoriAq {
    signed char s[9];
} InoriAq;

typedef struct InoriEnt {
    int thr;
    int cnt;
    int *tab;
} InoriEnt;

extern const char battleInitInoriItemText[];
extern const char battleInitAquiredText[];
extern const char battleInitJpText2[];
extern char *strcat(char *dst, const char *src);
static InoriEnt inoriTbl[4];

/*
 * Picks a random item from the inori table, shows the acquired-item message and
 * adds the item to the item box.
 */
void specInoriProc(void)
{
    int roll;
    int item;
    int i;

    roll = rnd(0x63);
    item = 0;
    for (i = 0; i < 4; i++)
    {
        if (roll < inoriTbl[i].thr)
        {
            item = inoriTbl[i].tab[rnd(inoriTbl[i].cnt - 1)];
            break;
        }
    }
    printf(battleInitInoriItemText, item);
    *(InoriAq *)msgWork.text = *(InoriAq *)battleInitAquiredText;
    strcat(msgWork.text, *dataItmNameGet(item));
    strcat(msgWork.text, battleInitJpText2);
    battleMsg((int)&msgWork);
    dataBoxInc(0, item);
}

typedef struct { signed char s[7]; } NusumuStole;
typedef struct { signed char s[25]; } NusumuNone;
extern int itemGrpChk(int group, int item);
extern const char *itemNameGet(int group, int index);

/*
 * Steal attempt by a unit: picks one of its two stealable items, subject to a
 * random roll, shows the result message and adds a stolen item to the item box.
 * Returns 1 when an item was stolen, 0 otherwise.
 */
int specNusumuProc(void *arg0, BattleUnit **arg1)
{
    unsigned char *temp;
    int check;
    unsigned char grp;
    unsigned char id;
    const char *name;

    temp = dataGainGet(calcUPGet(*arg1)->charaId);
    check = 1;
    if (*(int *)((char *)arg0 + 0x8) != 0x5A) {
        check = rnd(0x64) >= 0x33;
    }
    if (check != 0 && temp[0x11] != 0 && calcStatGet(*arg1, 7, 8) == 0) {
        grp = temp[0xE];
        id = temp[0x11];
        calcStatSet(*arg1, 7, 8, 0);
    } else if (temp[0x10] != 0 && calcStatGet(*arg1, 7, 0x10) == 0) {
        grp = temp[0xD];
        id = temp[0x10];
        calcStatSet(*arg1, 7, 0x10, 0);
    } else {
        grp = 0;
        id = 0;
    }
    if (itemGrpChk(grp, id) == 0) {
        grp = 0;
        id = 0;
    }
    printf("** Nusumu grp=%d item=%d\n", grp, id);
    if (id != 0) {
        name = itemNameGet(grp, id);
        if (name != 0) {
            *(NusumuStole *)msgWork.text = *(NusumuStole *)"Stole ";
            strcat((char *)msgWork.text, name);
            strcat((char *)msgWork.text, battleInitJpText2);
            dataBoxInc(grp, id);
            return 1;
        }
        return (int)name;
    }
    *(NusumuNone *)msgWork.text = *(NusumuNone *)"Couldn't steal anything!";
    return 0;
}

/*
 * Returns the name of an item in a group: 0 items, 1 weapons, 2 the dataBltNameGet
 * table, 3 accessories; 0 for other groups.
 */
const char *itemNameGet(int group, int index)
{
    const char *name;

    name = 0;
    switch (group) {
    case 0:
        name = *dataItmNameGet(index);
        break;
    case 1:
        name = *dataWepNameGet(index);
        break;
    case 2:
        name = *dataBltNameGet(index);
        break;
    case 3:
        name = *dataAccNameGet(index);
        break;
    default:
        printf("itemNameGet: err=%d %d\n", group, index);
        break;
    }
    return name;
}

void battleMsg(int msgId)
{
    battleMsgPut(0x10, 0x15C, msgId, 0x1E);
}

extern const char battleInitScenarioBatExecDstErrText[];


/*
 * Queues the command for one destination record's type on its unit: damage,
 * guard, evade, recovery, wake or status. Returns 1, or 0 for an unknown type.
 */
int scenarioBatExecDst(int *arg0, int *arg1)
{
    CmdEntry cmd;
    int self;
    int type;

    type = arg1[1];
    cmd.argument = arg0[2];
    self = arg0[0];
    cmd.itmId = arg0[3];
    cmd.extra = self;
    switch (type) {
    case 9:
        cmd.selector = CMD_DMG;
        cmd.argument = arg1[4];
        break;
    case 11:
        cmd.selector = CMD_DEF;
        break;
    case 10:
        cmd.selector = CMD_EVA;
        break;
    case 39:
        cmd.selector = CMD_REC;
        if (self == arg1[0]) {
            cmd.argument = 1;
        } else {
            cmd.argument = 0;
        }
        break;
    case 40:
        cmd.selector = CMD_WAKE;
        break;
    case 41:
        cmd.selector = CMD_STAT;
        if (self == arg1[0]) {
            cmd.argument = 1;
        } else {
            cmd.argument = 0;
        }
        break;
    default:
        printf(battleInitScenarioBatExecDstErrText, type);
        return 0;
    }
    unitCmdClear((BattleUnit *)arg1[0]);
    unitCmdSet((BattleUnit *)arg1[0], &cmd);
    return 1;
}

/* One row of the decoEff table: the character id and two sub-effect ids. */
typedef struct DecoEffEntry {
    int charaId;
    int subId[2];
} DecoEffEntry;
static DecoEffEntry decoEff[8];
void decoEffCallSub(BattleUnit *unit, int subId);

/*
 * Starts the decoration effect of the given type on a unit. Types 0x64 to 0x68
 * select a fixed effect (0x64 by size class); other types look the effect up by
 * the unit's character id in decoEff.
 */
void decoEffCall(BattleUnit *unit, int type)
{
    short charaId;
    int subId;
    int i;

    subId = 0;
    charaId = calcUPGet(unit)->charaId;
    if (type == 0x64)
    {
        switch (calcUPGet(unit)->sizeClass)
        {
        case 0:
        case 1:
            subId = 0xA3D;
            break;
        case 2:
            subId = 0xA3E;
            break;
        case 3:
        case 4:
            subId = 0xA3F;
            break;
        }
    }
    else if (type == 0x65)
    {
        subId = 0xA28;
    }
    else if (type == 0x66)
    {
        subId = 0xA40;
    }
    else if (type == 0x67)
    {
        subId = 0xA41;
    }
    else if (type == 0x68)
    {
        subId = 0x4000;
    }
    else
    {
        i = 0;
        while (decoEff[i].charaId != 0)
        {
            if (charaId == decoEff[i].charaId)
            {
                subId = decoEff[i].subId[type];
                break;
            }
            i++;
        }
    }
    if (subId != 0)
    {
        decoEffCallSub((BattleUnit *)unit, subId);
    }
}

/* decoEffCallSub's own signed byte inside calc.h's unmodeled_10 span. */
#define CALC_PARAM_DECO_EFF_OFF 0x15

/*
 * calc.h's own charaId (+0x38) is signed; decoEffCallSub reads it here with
 * an unsigned halfword load (lhu, not the lh every other charaId read in
 * this TU uses), so it is read through this raw offset instead of the
 * shared ->charaId member.
 */
#define CALC_PARAM_CHARA_ID_OFF 0x38

extern void sefCreateEffect(int *params);

/*
 * The local effect-request block sefCreateEffect reads. Only the fields
 * decoEffCallSub stores are named.
 */
typedef struct DecoEffParams {
    Actor *actor;              /* +0x00 */
    unsigned char unmodeled_04[0xC - 0x4];
    short moveVariant;         /* +0x0C */
    unsigned char unmodeled_0e[0x10 - 0xE];
    short charaId;             /* +0x10 */
    unsigned char unmodeled_12[0x14 - 0x12];
    short subId;               /* +0x14 */
    unsigned char unmodeled_16[0x1A - 0x16];
    short flags;               /* +0x1A */
    unsigned char unmodeled_1c[0x24 - 0x1C];
} DecoEffParams;

void decoEffCallSub(BattleUnit *unit, int subId) {
    DecoEffParams params;
    unsigned short charaId;

    memset(&params, 0, sizeof(params));
    params.actor = unit->actor;
    params.moveVariant = (signed char)((signed char *)calcUPGet(unit))[CALC_PARAM_DECO_EFF_OFF];
    charaId = *(unsigned short *)((unsigned char *)calcUPGet(unit) + CALC_PARAM_CHARA_ID_OFF);
    params.subId = (short)subId;
    params.charaId = charaId;
    params.flags = (short)calcUPGet(unit)->flags;
    sefCreateEffect((int *)&params);
}

extern int calcAtrChg(BattleUnit *unit, int attribute);
typedef struct ScnEffArg {
    BattleUnit *unit;  /* +0x00 */
    int kind;          /* +0x04: 3 = item, otherwise technique */
    int tec;           /* +0x08 */
    int slot;          /* +0x0C: item id when kind == 3, else weapon slot */
} ScnEffArg;
typedef struct ScnEffTgt {
    BattleUnit *unit;            /* +0x00 */
    unsigned char unmodeled_04[0x0C - 0x04];
    unsigned short slot;         /* +0x0C */
} ScnEffTgt;
typedef struct ScnEffParams {
    Actor *actor0;               /* +0x00 */
    Actor *actor1;               /* +0x04 */
    Actor *weapon;               /* +0x08 */
    short var0c;                 /* +0x0C */
    short var0e;                 /* +0x0E */
    unsigned short cid0;         /* +0x10 */
    unsigned short cid1;         /* +0x12 */
    short spid;                  /* +0x14 */
    unsigned short slot16;       /* +0x16 */
    short atr;                   /* +0x18 */
    unsigned short flags1;       /* +0x1A */
    short cfev;                  /* +0x1C */
    short stat;                  /* +0x1E */
    short var20;                 /* +0x20 */
} ScnEffParams;

/*
 * Builds the effect request for one scenario action on one destination record
 * and creates it; kind 5 techniques also create a second effect with the second
 * weapon.
 */
int scenarioEffCall(ScenarioBatExec *arg0p, u8 *arg1p)
{
  ScnEffArg *arg0 = (ScnEffArg *)arg0p;
  ScnEffTgt *arg1 = (ScnEffTgt *)arg1p;
  Actor *new_var;
  ScnEffParams p;
  int idx;
  int sl;
  short spid;
  signed char cat;
  if (arg0->kind == 3)
  {
    cat = 0;
    spid = ((short *)dataItmGet(arg0->slot))[8];
    p.weapon = 0;
    p.atr = 0;
  }
  else
  {
    short *tec;
    tec = (short *)dataTecGet(arg0->tec);
    spid = tec[11];
    tec = (short *)dataTecGet(arg0->tec);
    do
    {
    }
    while (0);
    p.atr = calcAtrChg(arg0->unit, *((unsigned short *) (tec + 9)));
    cat = ((signed char *) dataTecGet(arg0->tec))[0x11];
    if (arg0->slot >= 0)
    {
      idx = 0;
      if (cat != 5)
      {
        if (0 != dataTid2WepSp(arg0->tec))
        {
          idx = 2;
        }
        else
        {
          idx = arg0->slot;
        }
      }
      p.weapon = arg0->unit->weapon[idx & 3];
    }
    else
    {
      p.weapon = 0;
    }
  }
  new_var = arg0->unit->actor;
  p.actor1 = arg1->unit->actor;
  p.actor0 = new_var;
  p.var0c = calcUPGet(arg0->unit)->sizeClass;
  p.var0e = calcUPGet(arg1->unit)->sizeClass;
  p.cid0 = *((unsigned short *) (((char *) calcUPGet(arg0->unit)) + 0x38));
  p.cid1 = *((unsigned short *) (((char *) calcUPGet(arg1->unit)) + 0x38));
  p.slot16 = arg1->slot;
  p.spid = spid;
  p.flags1 = calcUPGet(arg1->unit)->flags;
  p.cfev = 0;
  {
    int upi = (int) calcUPGet(arg0->unit);
    idx = arg0->slot;
    idx += upi;
    p.var20 = (signed char) *(unsigned char *)(idx + 0x5A);
  }
  p.cfev = cfEventGet();
  if (calcStatGet(arg1->unit, 1, 0x100) != 0)
  {
    p.stat = 1;
  }
  else
  {
    p.stat = 0;
  }
  sefCreateEffect((int *) (&p));
  if (cat == 5)
  {
    p.weapon = arg0->unit->weapon[1];
    p.var20 = ((signed char *) calcUPGet(arg0->unit))[0x5B];
    p.spid = spid + 100;
    sefCreateEffect((int *) (&p));
  }
}

/*
 * Applies the motion, weapon, hand and hair-wind changes that dataMtdRead
 * reports for the unit (triggers 2 to 9).
 */
void scenarioMtd(ScenarioBatExec *exec)
{
    if (dataMtdRead(exec->unit, 2) == 1) {
        spWepDispOn(exec, dataDefWpnGet(exec->unit));
    }
    if (dataMtdRead(exec->unit, 3) == 1) {
        spWepDispOff(exec, dataDefWpnGet(exec->unit));
    }
    if (dataMtdRead(exec->unit, 4) == 1) {
        ACT_setHand(exec->unit->actor, 0);
    }
    if (dataMtdRead(exec->unit, 5) == 1) {
        ACT_setHand(exec->unit->actor, 1);
    }
    if (dataMtdRead(exec->unit, 6) == 1) {
        spWepDispOn(exec, 1);
    }
    if (dataMtdRead(exec->unit, 7) == 1) {
        spWepDispOff(exec, 1);
    }
    if (dataMtdRead(exec->unit, 8) == 1) {
        hairWindOn(exec->unit);
    }
    if (dataMtdRead(exec->unit, 9) == 1) {
        hairWindOff(exec->unit);
    }
}

/*
 * Shows the special weapon for a scenario action. The weapon ids 0xD to 0x10
 * and 0xC, the accessory id 0x108 passed to ACT_setArms and the technique ids 8
 * and 0xB are compared as the original does; what each id is, is not
 * established here.
 */
void spWepDispOn(ScenarioBatExec *exec, int weapon)
{
    int tecKind;
    struct WepSpEntry *wepSp;
    int weaponId;
    Actor *weaponActor;

    if (exec->type == 1) {
        tecKind = dataTecGet(exec->tecId)->kind;
    } else {
        spWepDispOnSub(exec->unit, weapon, 0);
        return;
    }

    if (tecKind == 6) {
        spWepDispOnSub(exec->unit, exec->slotOrItemId, 1);
    } else {
        wepSp = dataTid2WepSp(exec->tecId);
        if (wepSp != 0) {
            weaponId = calcUPGet(exec->unit)->weaponId[2];
            if ((unsigned int)(weaponId - 0xD) >= 4) {
                weaponActor = exec->unit->weapon[weapon & 3];
                if (weaponActor != 0) {
                    /* The same bit spWepDispOffSub sets. */
                    weaponActor->flags |= WEAPON_HIDDEN_BIT;
                }
            } else {
                ACT_setArms(exec->unit->weapon[2], exec->unit->actor, 0x108, 0);
            }
            spWepDispOnSub(exec->unit, 2, 0);
        } else if (exec->tecId == 8 || exec->tecId == 0xB) {
            weaponId = calcUPGet(exec->unit)->weaponId[exec->slotOrItemId];
            if (weaponId == 0xC) {
                spWepDispOnSub(exec->unit, exec->slotOrItemId, 0);
            }
        }
    }
}

/*
 * spWepDispOn's worker: with fade clear the weapon actor in slot `slot & 3`
 * is revealed at once, otherwise transWepIn fades it in.
 */
void spWepDispOnSub(BattleUnit *unit, int slot, int fade)
{
    if (fade == 0) {
        unit->weapon[slot & 3]->flags &= ~WEAPON_HIDDEN_BIT;
        return;
    }
    transWepIn(unit, slot);
}

void spWepDispOffSub(BattleUnit *unit, int slot, int fade);

/*
 * spWepDispOn's counterpart at the end of an action: hide the weapon the
 * action showed again (fading for kind-6 techniques and for slot-2 weapons
 * 0x11 and 0xB), or put a special weapon's actor back.
 */
void spWepDispOff(ScenarioBatExec *exec, int weapon)
{
    int tecKind;
    struct WepSpEntry *wepSp;
    int weaponId;
    Actor *weaponActor;

    if (exec->type == 1) {
        tecKind = dataTecGet(exec->tecId)->kind;
    } else {
        spWepDispOffSub(exec->unit, weapon, 0);
        return;
    }

    weaponId = calcUPGet(exec->unit)->weaponId[2];
    if (tecKind == 6) {
        spWepDispOffSub(exec->unit, exec->slotOrItemId, 1);
    } else if (weaponId == 0x11 || weaponId == 0xB) {
        spWepDispOffSub(exec->unit, 2, 1);
    } else {
        wepSp = dataTid2WepSp(exec->tecId);
        if (wepSp != 0) {
            weaponId = calcUPGet(exec->unit)->weaponId[2];
            if ((unsigned int)(weaponId - 0xD) >= 4) {
                weaponActor = exec->unit->weapon[weapon & 3];
                if (weaponActor != 0 && weaponId != 0x11) {
                    weaponActor->flags &= ~WEAPON_HIDDEN_BIT;
                }
            } else {
                ACT_setHand(exec->unit->actor, 0);
            }
            spWepDispOffSub(exec->unit, 2, 0);
        } else if (exec->tecId == 8 || exec->tecId == 0xB) {
            weaponId = calcUPGet(exec->unit)->weaponId[exec->slotOrItemId];
            if (weaponId == 0xC) {
                spWepDispOffSub(exec->unit, exec->slotOrItemId, 0);
            }
        }
    }
}

/*
 * spWepDispOff's worker: with fade clear the weapon actor in slot
 * `slot & 3` is hidden at once, otherwise transWepOut fades it out.
 */
void spWepDispOffSub(BattleUnit *unit, int slot, int fade)
{
    if (fade == 0) {
        unit->weapon[slot & 3]->flags |= WEAPON_HIDDEN_BIT;
        return;
    }
    transWepOut(unit, slot);
}

void transWepIn(BattleUnit *unit, int slot)
{
    Actor *weapon;

    weapon = unit->weapon[slot & 3];
    if (weapon != 0) {
        weapon->flags &= ~WEAPON_HIDDEN_BIT;
        weapon->draw = transWepInDraw;
        weapon->transparency = 0.0f;
    }
}

extern void unitActdraw(void);

void transWepInDraw(Actor *actor) {
    float transparency;

    unitActdraw();
    transparency = actor->transparency + 0.1f;
    actor->transparency = transparency;
    if (transparency >= 1.0f) {
        actor->transparency = 1.0f;
        actor->draw = 0;
    }
}

void transWepOut(BattleUnit *unit, int slot)
{
    Actor *weapon;

    weapon = unit->weapon[slot & 3];
    if (weapon != 0) {
        weapon->draw = transWepOutDraw;
        weapon->transparency = 1.0f;
    }
}

void transWepOutDraw(Actor *actor) {
    float transparency;

    unitActdraw();
    transparency = actor->transparency - 0.1f;
    actor->transparency = transparency;
    if (transparency <= 0.0f) {
        actor->transparency = 0.0f;
        actor->draw = 0;
        actor->flags |= WEAPON_HIDDEN_BIT;
    }
}


extern void unitMotStandSet(BattleUnit *unit);

/*
 * Reports whether a scenario action has finished. Special-flow techniques
 * return `done`. Actions of types 2 and 3 wait for their effect; an item
 * (type 3) also returns its unit to standing motion while held. Others wait
 * for the motion hold.
 */
int scenarioSrcEndChk(ScenarioBatExec *exec, ScenarioDstEntry *dest, int done)
{
    int effectNo;
    unsigned int held;

    if (exec->type != 3 && (dataTecGet(exec->tecId)->procFlags & TEC_PROC_FLOW_SPEC)) {
        return done;
    }
    if (exec->type == 2 || exec->type == 3) {
        if (exec->type != 3) {
            effectNo = dataTecGet(exec->tecId)->effectNo;
        } else {
            effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
            if (exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT) {
                unitMotStandSet(exec->unit);
            }
        }
        return sefIsFinishEffect(effectNo) != 0;
    }
    held = exec->unit->actor->moveFlags & ACTOR_MOTION_HOLD_BIT;
    return held != 0;
}

extern int calcIdol2Chk(BattleUnit *unit);
extern int calcHpDmg(void *attacker, BattleUnit *target, int dmgType, int power);
extern int calcWct(BattleUnit *unit);
extern void calcStatResetAll(BattleUnit *unit);
extern void calcStatResetCha(BattleUnit *unit);
extern void thinkUnitDmgExec(BattleUnit *unit, BattleUnit *src);
extern const char battleInitDmgSAFEText[];

/*
 * Applies the result of a finished scenario action to each target in the
 * destination list: damage or item use, stat changes and resets, and
 * counter attacks.
 */
void scenarioBatEnd(void)
{
    int *src;
    ScenarioDstEntry *dst;
    int i;
    int idolBefore;
    int idolAfter;
    int dmg;
    int stat;
    BattleUnit *unit;
    BattleUnit *unitB;
    CalcUnitParam *up;
    CalcUnitParam *up2;
    int wct;

    scenarioPtr.index = 0;
    if (scenarioDstGet(0) != 0) {
        i = 0;
        src = (int *)scenarioSrcGet(0);
        if (src[4] > 0) {
            do {
                dst = scenarioDstGet(i);
                unit = dst->unit;
                unit->work->flags &= ~0x4000;
                idolBefore = calcIdol2Chk(unit);
                unitB = dst->unit;
                if (unitB == 0 || (unitB->work->flags & ACTOR_FLAG_DEAD)) {
                    idolBefore = 0;
                }
                if (src[1] == 3) {
                    dmg = calcUseItem((BattleUnit *)src[0], unitB, src[3]);
                } else {
                    dmg = calcHpDmg((void *)src[0], unitB, dst->word0c, dst->word08);
                }
                if (src[2] == 0x24C) {
                    up = calcUPGet(dst->unit);
                    up2 = calcUPGet(dst->unit);
                    wct = calcWct(dst->unit);
                    up2->waitCount = wct;
                    up->waitCountBase = (short)wct;
                    calcUPGet(dst->unit)->turnBias = 0;
                }
                if (src[2] == 0x24D) {
                    calcStatResetAll(dst->unit);
                }
                if (calcStatGet(dst->unit, 2, 1) != 0 && dmg == 0) {
                    calcHp(dst->unit, 1);
                    calcStatReset(dst->unit, 2, 1, 0);
                    printf(battleInitDmgSAFEText);
                }
                stat = dst->word18;
                if (stat != 8) {
                    calcStatSet(dst->unit, stat, dst->word1c, 0);
                }
                stat = dst->word20;
                if (stat == 8) {
                    if (dst->word24 != 0) {
                        calcStatResetCha(dst->unit);
                    }
                } else {
                    calcStatReset(dst->unit, stat, dst->word24, 0);
                }
                idolAfter = calcIdol2Chk(dst->unit);
                if (idolBefore == 0) {
                    if (idolAfter == 0) {
                    } else {
                        goto block_30;
                    }
                } else if (idolAfter == 0) {
block_30:
                    unitMotStandSet(dst->unit);
                }
                if (dst->unit->work->flags & 0x20) {
                    thinkUnitDmgExec(dst->unit, (BattleUnit *)src);
                }
                if (dmg != 0 && src[1] == 1) {
                    counterProc(src, (BattleUnit **)dst);
                }
                i++;
            } while (i < src[4]);
        }
    }
}

extern int calcCounter(BattleUnit *unit, int tecId);
extern int unitNoGet(BattleUnit *unit);
extern const char battleInitCounterText[];

/*
 * Runs the counter check of the target *arg1 against the technique arg0[2]
 * of the attacker arg0[0]. Returns 0 when the technique's flag byte has 0x80
 * clear, otherwise 1; pCounterUnit is set to the attacker on a hit.
 */
int counterProc(int *arg0, BattleUnit **arg1)
{
    BattleUnit *unit;
    int hit;

    hit = 0;
    unit = *arg1;
    if (!(*(unsigned char *)dataTecGet(arg0[2]) & 0x80))
    {
        return 0;
    }
    if (!(*(int *)unit->work & ACTOR_FLAG_ENEMY))
    {
        if (calcCounter(unit, arg0[2]) != 0)
        {
            if (calcStatGet(unit, 7, 0x800) != 0 && calcBoostChk(unit, 0) != 0)
            {
                goto boost;
            }
            calcBp(unit, 0x19);
            hit = 1;
        }
    }
    else if (calcCounter(unit, arg0[2]) != 0 && calcBoostChk(unit, 0) != 0)
    {
boost:
        calcBoost(unit, 0);
        hit = 1;
    }
    if (hit == 1)
    {
        pCounterUnit = (BattleUnit *)arg0[0];
        printf(battleInitCounterText, unitNoGet(unit));
    }
    else
    {
        pCounterUnit = 0;
    }
    return 1;
}

void *objDmgNumCreate(void *target, int value, int flags);
extern const char battleInitJpText3[];
extern const char battleInitCRITText[];
extern const char battleInitDEFText[];
extern const char battleInitSTATText[];
extern const char battleInitWAKEText[];
extern const char battleInitMISSText[];
extern const char battleInitHPDMGText[];
extern const char battleInitHPRECText[];
extern const char battleInitEPDMGText[];
extern const char battleInitEPRECText[];
extern const char battleInitSTATSETText[];
extern const char battleInitSTATRECText[];
extern const char battleInitHPDRAINText[];
extern const char battleInitHPDRAIN2Text[];
extern const char battleInitEPDRAINText[];
extern const char battleInitEPDRAIN2Text[];
extern const char battleInitDmgTypeErrText[];

/*
 * Creates the damage-number objects for one hit and prints its result text.
 * arg0 points at a pointer to the target that gets a second number object for
 * some result types; arg1 holds the target at +0x0, the value at +0x8 and the
 * flags word at +0xC (low byte: result kind).
 */
int dmgValPut(int arg0, BattleUnit **arg1) {
    int *d = (int *)arg1;
    int flags;
    int value;
    int type;
    int bits;
    int dst;
    void *first = *(void **)d;
    flags = d[3];
    type = flags & 0xFF;
    value = d[2];
    if (type != 0xB) {
        bits = (flags >> 8) & 0xFF;
        objDmgNumCreate(first, value, flags);
        if ((unsigned)(type - 7) < 2U) {
            dst = 2;
        } else {
            dst = 4;
            if ((unsigned)(type - 9) >= 2U) {
                dst = 0xB;
            }
        }
        if (dst != 0xB) {
            objDmgNumCreate(*(void **)arg0, value, dst);
        }
        printf(battleInitJpText3);
        if (bits & 1) {
            printf(battleInitCRITText);
        } else if (bits & 2) {
            printf(battleInitDEFText);
        } else if (bits & 4) {
            printf(battleInitSTATText);
        } else if (bits & 8) {
            printf(battleInitWAKEText);
        }
        /* Result text for the kind byte at +0xC. */
        switch (*(unsigned char *)((char *)arg1 + 0xC)) {
        case 0:
            printf(battleInitMISSText);
            break;
        case 1:
            printf(battleInitHPDMGText, value);
            break;
        case 2:
            printf(battleInitHPRECText, value);
            break;
        case 3:
            printf(battleInitEPDMGText, value);
            break;
        case 4:
            printf(battleInitEPRECText, value);
            break;
        case 5:
            printf(battleInitSTATSETText);
            break;
        case 6:
            printf(battleInitSTATRECText);
            break;
        case 7:
            printf(battleInitHPDRAINText, value);
            break;
        case 8:
            printf(battleInitHPDRAIN2Text, value);
            break;
        case 9:
            printf(battleInitEPDRAINText, value);
            break;
        case 10:
            printf(battleInitEPDRAIN2Text, value);
            break;
        default:
            printf(battleInitDmgTypeErrText);
            break;
        }
    }
}

/*
 * Battle monster-gain tallies. Only the total size is known (0x58 bytes,
 * cleared by monsGainInit).
 */
typedef struct MonsGain {
    unsigned char unmodeled_00[0x58];
} MonsGain;
static MonsGain monsGain;

MonsGain *monsGainPtrGet(void)
{
    return &monsGain;
}

void monsGainInit(void)
{
    memset(&monsGain, 0, sizeof(MonsGain));
}

extern int calcSkill(BattleUnit *unit, int a, int b);
extern const char battleInitPOINTUpEventTimerBXText[];
extern int menuTimeGet(void);
extern int menuTimeXGet(void);
extern int itemGrpChk(int group, int item);

/*
 * Adds one monster's gain values to monsGain. On a random roll it also
 * records the won item group and id in the list counted at +0x54. Does nothing
 * when the unit's +0x150 pointer is null.
 */
void monsGainSet(BattleUnit *unit)
{
  BattleUnit *arg0 = unit;
  CalcUnitParam *up;
  unsigned char *rec;
  unsigned char *mg;
  void *p150;
  int sk_val;
  int x;
  int var_3;
  int grp;
  int id;
  int var_16;
  int t4;
  int cnt;
  int *cp;
  unsigned char *g;
  unsigned char *ar;
  up = calcUPGet(arg0);
  mg = (char *) up;
  p150 = *((void **) (mg + 0x150));
  if (p150 != 0)
  {
    grp = 0;
    mg = (unsigned char *) (&monsGain);
    rec = (unsigned char *) dataGainGet(up->charaId);
    id = 0;
    *((int *) (mg + 0x10)) = (*((int *) (mg + 0x10))) + (*((unsigned short *) (rec + 4)));
    *((int *) (mg + 0x0)) = (*((int *) (mg + 0x0))) + (*((int *) rec));
    if (menuTimeGet() == 3)
    {
      x = menuTimeXGet();
      *((int *) (mg + 0x4)) = (*((int *) (mg + 0x4))) + (rec[6] * x);
      *((int *) (mg + 0x8)) = (*((int *) (mg + 0x8))) + (rec[7] * x);
      t4 = (*((int *) (mg + 0xC))) + (rec[8] * x);
      *((int *) (mg + 0xC)) = t4;
      printf(battleInitPOINTUpEventTimerBXText, x, *((int *) (mg + 0x4)), *((int *) (mg + 0x8)), t4);
    }
    else
    {
      *((int *) (mg + 0x4)) = (*((int *) (mg + 0x4))) + rec[6];
      *((int *) (mg + 0x8)) = (*((int *) (mg + 0x8))) + rec[7];
      *((int *) (mg + 0xC)) = (*((int *) (mg + 0xC))) + rec[8];
    }
    var_16 = rec[0xB], sk_val = calcSkill(p150, 0x1000, 0x8000);
    var_3 = var_16 < 0x65;
    if (sk_val > 0)
    {
      var_16 += (var_16 * sk_val) / 100;
      ;
    }
    if ((var_16 < 0x65) == 0)
    {
      var_16 = 0x64;
    }
    if ((rnd(0x64) < var_16) && (calcStatGet((BattleUnit *) arg0, 7, 8) == 0))
    {
      grp = rec[0xE];
      id = rec[0x11];
    }
    else
      if ((rnd(0x64) < rec[0xA]) && (calcStatGet((BattleUnit *) arg0, 7, 0x10) == 0))
    {
      grp = rec[0xD];
      id = rec[0x10];
    }
    if ((id != 0) && (itemGrpChk(grp, id) != 0))
    {
      g = (unsigned char *) (&monsGain);
      ar = g + 4;
      *((int *) ((ar + ((*((int *) (g + 0x54))) * 4)) + 0x10)) = grp;
      cp = (int *) (g + 0x54);
      cnt = *cp;
      *((int *) ((ar + (cnt * 4)) + 0x30)) = id;
      *cp = cnt + 1;
    }
  }
}

int itemGrpChk(int group, int item)
{
    switch (group) {
    case 0:
        if (item < 73) break;
        return 0;
    case 1:
        if (item < 116) break;
        return 0;
    case 2:
        if (item < 179) break;
        return 0;
    case 3:
        if (item < 181) break;
        return 0;
    default:
        return 0;
    }
    return 1;
}

extern int calcStatIdx2Bit(int statIdx);
/* Defined by src/main/sef.c. */
extern SchedulerState *sefCreateEffect2(SefEffectRequest *request);
extern void sefDeleteEffect2(SchedulerState *scheduler);

/*
 * The status-effect table statEffProc walks. effTbl[category][statIdx] is
 * the effect slot a status shows in (-1 for none); effId[slot] is that
 * slot's effect number.
 */
static signed char effTbl[8][16];
static int effId[7];

#define STAT_EFF_SLOT_COUNT 8
/* Bit of Actor.moveFlags set while the effect slot 3 is shown. */
#define ACTOR_MOTION_STOPPED_BIT 0x80000

/*
 * Reads BattleUnit.actor as a 32-bit word, so the pointer is loaded as an int.
 */
static __inline__ Actor *unitStatActorGet(BattleUnit *unit)
{
    return (Actor *)*(u32 *)&unit->actor;
}

/*
 * Shows one effect per slot while any status of that slot is active on the
 * unit and removes it once none is: slot 0 also switches the face motion
 * (0x37 on, 0x35 off), slot 3 stops the actor's motion while shown.
 */
void statEffProc(BattleUnit *unit)
{
    SefEffectRequest request;
    int counts[STAT_EFF_SLOT_COUNT];
    int category;
    int statIdx;
    int i;

    memset(counts, 0, sizeof(counts));
    memset(&request, 0, sizeof(request));
    for (category = 0; category < 8; category++) {
        for (statIdx = 0; statIdx < 15; statIdx++) {
            if (calcStatGet(unit, category, calcStatIdx2Bit(statIdx)) != 0
                && effTbl[category][statIdx] != -1) {
                counts[effTbl[category][statIdx]]++;
            }
        }
    }
    request.target = unitStatActorGet(unit);
    request.character_id_copy = calcUPGet(unit)->charaId;
    request.size_copy = calcUPGet(unit)->sizeClass;
    for (i = 0; i < STAT_EFF_SLOT_COUNT; i++) {
        if (counts[i] != 0) {
            if (calcUPGet(unit)->statEffUnits[i] == 0) {
                request.effect_no = effId[i];
                calcUPGet(unit)->statEffUnits[i] = (struct UnitWork *)sefCreateEffect2(&request);
                if (i == 0) {
                    unitFaceMotSet(unit, 0x37);
                }
                if (i == 3) {
                    unitStatActorGet(unit)->moveFlags |= ACTOR_MOTION_STOPPED_BIT;
                    unitStatActorGet(unit)->moveStepRate = 0.0f;
                }
            }
        } else if (calcUPGet(unit)->statEffUnits[i] != 0) {
            sefDeleteEffect2((SchedulerState *)calcUPGet(unit)->statEffUnits[i]);
            calcUPGet(unit)->statEffUnits[i] = 0;
            if (i == 0) {
                unitFaceMotSet(unit, 0x35);
            }
            if (i == 3) {
                unitStatActorGet(unit)->moveFlags &= ~ACTOR_MOTION_STOPPED_BIT;
                unitStatActorGet(unit)->moveStepRate = 1.0f / 30.0f;
            }
        }
    }
}

/* calcUPGet's return type comes from include/ov01/calc.h. */

#define UNIT_WORK_TABLE_COUNT 8
#define UNIT_WORK_STATUS_BLOCKED 0x100u
/* Table base in CalcUnitParam (+0x150) and the +0xC that reaches its first entry. */
#define UNIT_WORK_TABLE_BASE_OFF 0x150
#define UNIT_WORK_PTR_OFF 0xC

/*
 * statEffOn clears and statEffOff sets UNIT_WORK_STATUS_BLOCKED on each of the
 * unit's eight UnitWork entries at CalcUnitParam +0x15C. Each pass calls
 * calcUPGet twice, once to test the slot and once to use it.
 */
void statEffOn(BattleUnit *unit)
{
    int offset;
    int i;

    offset = UNIT_WORK_TABLE_BASE_OFF;
    for (i = UNIT_WORK_TABLE_COUNT - 1; i >= 0; i--) {
        if (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF) != 0) {
            (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF))->flags
                &= ~UNIT_WORK_STATUS_BLOCKED;
        }
        offset += 4;
    }
}

void statEffOff(BattleUnit *unit)
{
    int offset;
    int i;

    offset = UNIT_WORK_TABLE_BASE_OFF;
    for (i = UNIT_WORK_TABLE_COUNT - 1; i >= 0; i--) {
        if (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF) != 0) {
            (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF))->flags
                |= UNIT_WORK_STATUS_BLOCKED;
        }
        offset += 4;
    }
}

/*
 * objEntry2 and objStdInit are defined in ov01/tu001 obj.c. They are declared
 * here because that TU's private header cannot be included.
 */
extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
extern void objStdInit(ObjectTask *task);

/* The enemy cursor's own task callbacks, both defined further down in this TU. */
void objCur(CursorObject *cursor);
void objCurDraw(CursorObject *cursor);

/* curCmdSet selectors: 1 moves the cursor to a position, 2 to a unit; 3 is
 * accepted by objCur alongside 2. */
#define CURSOR_CMD_POSITION 1
#define CURSOR_CMD_UNIT 2
#define CURSOR_CMD_TARGET 3
/* Cursor state flag bits: 0x2 is set by curCmdSet while a command is
 * queued; 0x4 hides the cursor (objCurDraw); 0x8 marks a unit selection;
 * 0x40 is the enemy-side bit objCurDraw tests to pick objDispEnCur. */
#define CURSOR_STATE_FLAG_QUEUED 0x2
#define CURSOR_STATE_FLAG_HIDDEN 0x4
#define CURSOR_STATE_FLAG_UNIT_SELECT 0x8
#define CURSOR_STATE_FLAG_ENEMY 0x40

/*
 * Creates the enemy-side cursor task at (x, y, z). state + 0x04 is cleared
 * here.
 */

ObjectTask *objEnCurCreate(float x, float y, float z)
{
    ObjectTask *task;
    CursorCommandState *state;
    Vector4 *position;

    task = objEntry2(objCur, (void (*)(ObjectTask *))objCurDraw);
    state = task->work;
    objStdInit(task);
    position = &state->transform.fields.position;
    position->x = x;
    position->y = y;
    position->z = z;
    state->flags |= CURSOR_STATE_FLAG_ENEMY;
    state->unmodeled_04 = 0;
    state->targetPosId = 0;
    state->selectActive = 0;
    return task;
}

/*
 * objRemove is defined in ov01/tu001 obj.c. It is declared here because that
 * TU's private header cannot be included.
 */
extern void objRemove(ObjectTask *task);
void objCurRemove(void)
{
    if (pTaskEnCur != 0) {
        objRemove(pTaskEnCur);
        pTaskEnCur = 0;
    }
}

/* Defined in src/ov01/obj.c. */
extern void *objCmdPtrGet(ObjectTask *task);
extern void objHatoVec(ObjectTask *task, int steps);
extern int objStdMove(ObjectTask *task);
/* Defined in src/main/sef.c. */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);

/*
 * The cursor task's per-frame step: run the queued command. A position
 * move steps the cursor until objStdMove reports arrival; a unit command
 * snaps it to the unit's null position unless the cursor is hidden.
 * Either way the queued flag is cleared once the command is done.
 */
void objCur(CursorObject *cursor)
{
    CursorCommandState *state = cursor->state;
    CursorCommand *cmd = objCmdPtrGet((ObjectTask *)cursor);
    Vector4 *position = &state->transform.fields.position;
    BattleUnit *unit;
    unsigned int flags;

    switch (cmd->selector) {
    case CURSOR_CMD_UNIT:
    case CURSOR_CMD_TARGET:
        unit = (BattleUnit *)cmd->argument;
        flags = state->flags;
        if (!(flags & CURSOR_STATE_FLAG_HIDDEN)) {
            sefGetNullPosition(position, unit->actor, 0, 0);
            flags = state->flags;
        }
        state->flags = flags & ~CURSOR_STATE_FLAG_QUEUED;
        break;
    case CURSOR_CMD_POSITION:
        objHatoVec((ObjectTask *)cursor, 2);
        if (objStdMove((ObjectTask *)cursor)) {
            state->flags &= ~CURSOR_STATE_FLAG_QUEUED;
            cmd->selector = 0;
        }
        break;
    }
}

extern void objDispPlCur(void);
extern void objDispEnCur(CursorObject *cursor);

/*
 * Draws the cursor. Returns without drawing when the hidden bit (0x4) is set;
 * otherwise calls objDispEnCur for the enemy side (bit 0x40) or objDispPlCur
 * for the player side.
 */
void objCurDraw(CursorObject *cursor)
{
    CursorCommandState *state;
    unsigned int flags;

    state = cursor->state;
    flags = state->flags;
    if (flags & CURSOR_STATE_FLAG_HIDDEN) {
        return;
    }
    if (!(flags & CURSOR_STATE_FLAG_ENEMY)) {
        objDispPlCur();
        return;
    }
    objDispEnCur(cursor);
}

/*
 * Queues a command on the cursor: clears the state words at +0x70 and +0x74,
 * copies the command into the tail slot, and sets the queued flag when the
 * selector is nonzero.
 */
void curCmdSet(CursorObject *cursor, const CursorCommand *command)
{
    CursorCommandState *state;
    CursorCommand *tail;
    unsigned int flags;

    state = cursor->state;
    tail = objCmdTailGet(cursor);
    state->unmodeled_70 = 0;
    state->unmodeled_74 = 0;
    *tail = *command;
    if (tail->selector != 0) {
        flags = state->flags;
        state->flags = flags | CURSOR_STATE_FLAG_QUEUED;
    }
}

/*
 * Battle cursor selection list. entries[] holds the position id of each
 * selectable unit, indexed by curIdx (word width, stride 4); count bounds
 * curIdx on both ends; prevIdx is a snapshot of curIdx taken on entry and
 * compared again after the target update to decide whether to play the
 * selection sound. units[] and entries[] are each sized to the gap their
 * offsets leave (0x00..0x23 and 0x24..0x47, stride 4 = 9 words).
 */
typedef struct CursorSelectList {
    BattleUnit *units[9]; /* +0x00..+0x23: the selectable units;
                           * curMove2Pos reads units[curIdx]->actor */
    int entries[9]; /* +0x24..+0x47: each unit's CalcUnitParam.posId, stored
                     * by curEnProcInit; curMove2Pos uses it as the
                     * dataPosTblGet index when posMode is set */
    int count;         /* +0x48 */
    int curIdx;         /* +0x4C */
    int prevIdx;         /* +0x50 */
    int posMode;         /* +0x54: nonzero, curMove2Pos targets a field
                          * position from entries[] instead of a unit */
    int groupFlag;       /* +0x58: toggled by curGrpSel, set by curSelGrpSet */
} CursorSelectList;

/*
 * The battle cursor's selection record. curSel and curMove2Pos work on it;
 * curSelGrpSet and curSelGrpGet access its group word.
 */
static CursorSelectList curMove;

/*
 * dataPadRead is defined in src/ov01/data_unit_org_get.c. It is declared
 * here to call it from this TU.
 */
extern int dataPadRead(int padNo);

/*
 * curMove2Pos is defined later in this TU; forward-declared to call it here.
 */
extern void curMove2Pos(CursorObject *cursor, CursorSelectList *list);

/*
 * Steps the selection in curMove by one entry for nextMask or prevMask,
 * wrapping at both ends. When the entry's position id differs from the cursor's
 * target, the cursor moves there and, if the index changed, a sound plays.
 * Returns the new index.
 */
int curSel(CursorObject *cursor, CursorSelectList *list, int nextMask, int prevMask)
{
    CursorCommandState *state;
    int posId;

    list->prevIdx = list->curIdx;
    if (dataPadRead(1) & nextMask) {
        list->curIdx++;
        if (list->curIdx >= list->count) {
            list->curIdx = 0;
        }
    } else if (dataPadRead(1) & prevMask) {
        list->curIdx--;
        if (list->curIdx < 0) {
            list->curIdx = list->count - 1;
        }
    }

    state = cursor->state;
    posId = list->entries[list->curIdx];
    if (state->targetPosId != posId) {
        state->targetPosId = posId;
        curMove2Pos(cursor, list);
        if (list->curIdx != list->prevIdx) {
            sndSysSePlay(0xA);
        }
    }
    return list->curIdx;
}

/*
 * Toggles curMove's group flag when either mask is pressed and plays a sound.
 * Returns 1 when it toggled, 0 otherwise.
 */
int curGrpSel(CursorObject *cursor, CursorSelectList *list, int nextMask, int prevMask)
{
    if (!(dataPadRead(1) & nextMask) && !(dataPadRead(1) & prevMask)) {
        return 0;
    }
    if (list->groupFlag == 0) {
        list->groupFlag = 1;
    } else {
        list->groupFlag = 0;
    }
    sndSysSePlay(0xA);
    return 1;
}

/* Defined in src/ov01/data_unit_org_get.c. */
extern Vector4 *dataPosTblGet(int index);

/*
 * Send the cursor to the selected entry: a unit's null position, raised by
 * the unit's size class (1.0 below class 3, none for class 3, 2.5 above),
 * or a field position from dataPosTblGet (negative ids mirror x). A hidden
 * cursor jumps there at once; the move itself is queued as command 1.
 */
void curMove2Pos(CursorObject *cursor, CursorSelectList *list)
{
    CursorCommand cmd;
    CursorCommandState *state = cursor->state;
    CursorTransform *transform = &state->transform;
    Vector4 *pos;
    int posId;
    int sizeClass;
    float x;

    if (list->posMode == 0) {
        sefGetNullPosition(&transform->fields.goal, list->units[list->curIdx]->actor, 0, 0);
        sizeClass = calcUPGet(list->units[list->curIdx])->sizeClass;
        if (sizeClass < 3) {
            transform->fields.offset.y = 1.0f;
        } else if (sizeClass < 4) {
            transform->fields.offset.y = 0.0f;
        } else {
            transform->fields.offset.y = 2.5f;
        }
    } else {
        posId = list->entries[list->curIdx];
        pos = dataPosTblGet(posId);
        x = pos->x;
        transform->fields.goal.x = x;
        transform->fields.goal.y = pos->y;
        transform->fields.goal.z = pos->z;
        if (posId < 0) {
            transform->fields.goal.x = -x;
        }
        transform->fields.offset.y = 0.0f;
    }
    if (state->flags & CURSOR_STATE_FLAG_HIDDEN) {
        transform->fields.position.x = transform->fields.goal.x;
        transform->fields.position.y = transform->fields.goal.y;
        transform->fields.position.z = transform->fields.goal.z;
    }
    cmd.selector = CURSOR_CMD_POSITION;
    curCmdSet(cursor, &cmd);
}

/*
 * Fills curMove with the living units of one side and points *target at the
 * unit the cursor starts on. Returns 1 when a unit was found.
 */
int curEnProcInit(BattleUnit **target, int side)
{
    ObjectTask *task = pTaskEnCur;
    CursorSelectList *list = &curMove;
    BattleUnit **table[1];
    int count;
    int i = 0;
    int ret;

    list->curIdx = 0;
    list->count = 0;
    count = unitTblGet(side, table);
    for (; i < count; i++)
    {
        BattleUnit *u = table[0][i];
        if (u == 0)
        {
            continue;
        }
        if (u->work->flags & ACTOR_FLAG_DEAD)
        {
            continue;
        }
        if (*target == u)
        {
            list->curIdx = list->count;
        }
        list->units[list->count] = table[0][i];
        list->entries[list->count] = calcUPGet(table[0][i])->posId;
        list->count++;
    }
    if (list->count == 0)
    {
        printf(battleInitCurEnProcNoTgtText);
        ret = 0;
    }
    else
    {
        int idx = list->curIdx;
        ret = 1;
        *(int *)((char *)task->work + 0x78) = 1;
        *target = list->units[idx];
    }
    return ret;
}

/*
 * Reads CursorObject.state as a 32-bit word, so the pointer is loaded as an int.
 */
static __inline__ CursorCommandState *curStateGet(CursorObject *cursor)
{
    return (CursorCommandState *)*(u32 *)&cursor->state;
}

/*
 * One frame of enemy selection with the battle cursor: the pad steps
 * through curMove's units (PAD_RIGHT next, PAD_LEFT previous), *target receives
 * the selection and the cursor is sent to it unless a command is still
 * queued. The result is whether the selection changed.
 */
int curEnProc(BattleUnit **target)
{
    CursorObject *cursor = (CursorObject *)pTaskEnCur;
    CursorSelectList *list = &curMove;
    CursorCommand cmd;
    BattleUnit *unit;
    int changed;

    list->posMode = 0;
    curStateGet(cursor)->flags |= CURSOR_STATE_FLAG_UNIT_SELECT;
    curSel(cursor, list, PAD_RIGHT, PAD_LEFT);
    changed = list->prevIdx != list->curIdx;
    unit = list->units[list->curIdx];
    *target = unit;
    if (!(curStateGet(cursor)->flags & CURSOR_STATE_FLAG_QUEUED)) {
        cmd.selector = CURSOR_CMD_UNIT;
        cmd.argument = (int)unit;
        curCmdSet(cursor, &cmd);
    }
    return changed;
}

static char chaTbl_00A3D7E0[28];
static char agwsTbl_00A3D800[16];
extern const char battleInitCurPosProcNoTgtText[];
extern int calcMoveChk(BattleUnit *unit, int move);

/*
 * Fills curMove with the field-position ids the cursor can pick for arg0: the
 * chaTbl or agwsTbl list, chosen by arg0's AGWS flag and arg1. Ids occupied by
 * a unit are skipped, and so are ids calcMoveChk rejects unless arg1 is 5.
 * Returns 1 when an entry was found.
 */
int curPosProcInit(BattleUnit *arg0, int arg1)
{
    ObjectTask *task = pTaskEnCur;
    CursorSelectList *list = &curMove;
    BattleUnit **table;
    int *var;
    int j;
    int *cnt;
    int count;
    int i;
    int ret;

    if (arg1 == 5)
    {
        var = (calcUPGet(arg0)->flags & CALC_UNIT_FLAG_IS_AGWS) ? (int *)chaTbl_00A3D7E0 : (int *)agwsTbl_00A3D800;
    }
    else
    {
        var = (calcUPGet(arg0)->flags & CALC_UNIT_FLAG_IS_AGWS) ? (int *)agwsTbl_00A3D800 : (int *)chaTbl_00A3D7E0;
    }
    list->curIdx = 0;
    list->count = 0;
    count = unitTblGet(2, &table);
    if (var[0] != 0)
    {
        j = 0;
        cnt = &list->count;
        do
        {
            if (arg1 != 5 && !calcMoveChk(arg0, var[j]))
            {
                goto next;
            }
            for (i = 0; i < count && (table[i] == 0 || var[j] != calcUPGet(table[i])->posId); i++)
            {
            }
            if (i == count)
            {
                int idx;
                if (var[j] == *(int *)((char *)task->work + 0x80))
                {
                    list->curIdx = list->count;
                }
                idx = *cnt;
                list->entries[idx] = var[j];
                *cnt = idx + 1;
            }
next:
            j++;
        } while (var[j] != 0);
    }
    if (list->count == 0)
    {
        printf(battleInitCurPosProcNoTgtText);
        ret = 0;
    }
    else
    {
        ret = 1;
    }
    *(int *)((char *)task->work + 0x78) = 0;
    return ret;
}

/*
 * One frame of field-position selection with the battle cursor: as
 * curEnProc, but over curMove's position ids, which the cursor is sent to
 * (command CURSOR_CMD_POSITION) and *posId receives. unit is not read.
 */
int curPosProc(BattleUnit *unit, int *posId)
{
    CursorObject *cursor = (CursorObject *)pTaskEnCur;
    CursorSelectList *list = &curMove;
    CursorCommand cmd;
    int changed;

    list->posMode = 1;
    curStateGet(cursor)->flags &= ~CURSOR_STATE_FLAG_UNIT_SELECT;
    curSel(cursor, list, PAD_RIGHT, PAD_LEFT);
    changed = list->prevIdx != list->curIdx;
    if (!(curStateGet(cursor)->flags & CURSOR_STATE_FLAG_QUEUED)) {
        cmd.selector = CURSOR_CMD_POSITION;
        cmd.argument = list->entries[list->curIdx];
        curCmdSet(cursor, &cmd);
    }
    *posId = list->entries[list->curIdx];
    return changed;
}

extern int calcTgtUnitTbl(BattleUnit *self, int flags, BattleUnit **table);

/*
 * Fills curMove with the targets calcTgtUnitTbl returns for self, on the player
 * side or the enemy side as curMove's group flag selects, and points *target at
 * the matching unit. Returns 1 when a target was found.
 */
int curTgtProcInit(BattleUnit **target, BattleUnit *self, int flags)
{
    ObjectTask *task = pTaskEnCur;
    CursorSelectList *list = &curMove;
    BattleUnit *table[8];
    int count;
    int n;
    int i;
    int ret;

    list->curIdx = 0;
    if ((flags & 0xC0) == 0xC0)
    {
        curGrpSel((CursorObject *)task, list, PAD_UP, PAD_DOWN);
    }
    count = calcTgtUnitTbl(self, flags, table);
    n = 0;
    for (i = 0; i < count; i++)
    {
        if ((list->groupFlag == 0 && !(table[i]->work->flags & ACTOR_FLAG_ENEMY)) ||
            (list->groupFlag == 1 && (table[i]->work->flags & ACTOR_FLAG_ENEMY)))
        {
            list->units[n++] = table[i];
        }
    }
    list->count = n;
    for (i = 0; i < list->count; i++)
    {
        list->entries[i] = calcUPGet(list->units[i])->posId;
        if (oldCurGrp == list->groupFlag)
        {
            if (*target == list->units[i])
            {
                list->curIdx = i;
            }
        }
        else if (list->groupFlag == 0 && self == list->units[i])
        {
            list->curIdx = i;
        }
    }
    if (list->count == 0)
    {
        printf(battleInitCurEnProcNoTgtText);
        ret = 0;
    }
    else
    {
        int idx = list->curIdx;
        ret = 1;
        *(int *)((char *)task->work + 0x78) = 1;
        *target = list->units[idx];
    }
    oldCurGrp = list->groupFlag;
    return ret;
}

extern int frameCntGet(void);

/*
 * One frame of target selection with the battle cursor. Without bit 0x10
 * of targetFlags the pad steps through curMove's units (PAD_RIGHT next,
 * PAD_LEFT previous), the cursor is sent to the selection unless a command
 * is still queued, and the result is whether the selection changed. With
 * it the cursor stays on its current unit, advancing to the next one
 * every fourth frame, and the result is 0. *target receives the selection
 * only in the stepping mode.
 */
int curTgtProc(BattleUnit **target, int targetFlags)
{
    CursorObject *cursor = (CursorObject *)pTaskEnCur;
    CursorSelectList *list = &curMove;
    CursorTransform *transform = &curStateGet(cursor)->transform;
    CursorCommand cmd;
    BattleUnit *unit;
    int changed;
    int i;
    int sizeClass;

    if (!(targetFlags & 0x10)) {
        list->posMode = 0;
        curStateGet(cursor)->flags &= ~CURSOR_STATE_FLAG_UNIT_SELECT;
        curSel(cursor, list, PAD_RIGHT, PAD_LEFT);
        changed = list->prevIdx != list->curIdx;
        unit = list->units[list->curIdx];
        *target = unit;
        if (!(curStateGet(cursor)->flags & CURSOR_STATE_FLAG_QUEUED)) {
            cmd.selector = CURSOR_CMD_UNIT;
            cmd.argument = (int)unit;
            curCmdSet(cursor, &cmd);
        }
    } else {
        curStateGet(cursor)->flags &= ~CURSOR_STATE_FLAG_UNIT_SELECT;
        if (!(curStateGet(cursor)->flags & CURSOR_STATE_FLAG_QUEUED)) {
            for (i = 0; i < list->count; i++) {
                if (list->units[i] == curStateGet(cursor)->targetUnit) {
                    if ((frameCntGet() & 3) == 0) {
                        i++;
                    }
                    break;
                }
            }
            if (i >= list->count) {
                i = 0;
            }
            /* objCur handles this selector together with CURSOR_CMD_UNIT. */
            cmd.selector = CURSOR_CMD_TARGET;
            cmd.argument = (int)list->units[i];
            curCmdSet(cursor, &cmd);
            curStateGet(cursor)->targetUnit = list->units[i];
            sizeClass = calcUPGet(curStateGet(cursor)->targetUnit)->sizeClass;
            if (sizeClass < 3) {
                transform->fields.offset.y = 1.0f;
            } else if (sizeClass >= 4) {
                transform->fields.offset.y = 2.5f;
            } else {
                transform->fields.offset.y = 0.0f;
            }
        }
        changed = 0;
    }
    return changed;
}

/*
 * Sets the selection group. A group of 0 or 1 is stored as is and the call
 * returns. Otherwise the group is derived from flags (bit 0x8 or 0x40 selects
 * group 0, else group 1) and oldCurGrp is reset to -1.
 */
void curSelGrpSet(u32 group, int flags)
{
    CursorSelectList *state = &curMove;

    if (group < 2U) {
        state->groupFlag = group;
        return;
    }
    if (flags & 8) {
        state->groupFlag = 0;
    } else if (flags & 0x40) {
        state->groupFlag = 0;
    } else {
        state->groupFlag = 1;
    }
    oldCurGrp = -1;
}

/*
 * The current selection group.
 */
int curSelGrpGet(void)
{
    return curMove.groupFlag;
}

void objDispPlCur(void)
{
}

extern void curTexTrans(int unused);
extern void objDispCur(CursorObject *cursor, int isPlayer);
extern void menuStatTexTrans(void);
extern void monsNameDisp(void *packet, BattleUnit *unitTask, int x, int y, int isEnemy);

/*
 * Draw the enemy-side cursor: the cursor itself, the acting unit's marker
 * when the target is an enemy, and the target's name while a selection is
 * active and help mode 2 is off (an enemy target only while it is alive).
 */
void objDispEnCur(CursorObject *cursor)
{
    GrPacket packet; /* grGpInit's record, as in curTexTrans */
    int selectActive;
    unsigned int enemyBit;
    int isEnemy;

    curTexTrans(0);
    objDispCur(cursor, 0);
    menuStatTexTrans();
    if (tgtUnitGet() != 0 && (tgtUnitGet()->work->flags & ACTOR_FLAG_ENEMY)) {
        grGpInit(&packet);
        menuActCurPut(&packet, (ObjectTask *)cursor, actUnitGet(),
                      ((unsigned int)frameCntGet() >> 1) & 7);
    }
    if (tgtUnitGet() == 0) {
        return;
    }
    selectActive = cursor->state->selectActive;
    if (selectActive != 1 || battleCtrlHelpGet() == 2) {
        return;
    }
    enemyBit = tgtUnitGet()->work->flags & ACTOR_FLAG_ENEMY;
    isEnemy = enemyBit != 0;
    if (isEnemy == selectActive) {
        if (tgtUnitGet() == 0 || (tgtUnitGet()->work->flags & ACTOR_FLAG_DEAD)) {
            return;
        }
    }
    grGpInit(&packet);
    monsNameDisp(&packet, tgtUnitGet(), 8, 0x5C, isEnemy);
    grPacketSend(&packet);
}

/* The body is empty, so neither argument is read. */
void objDispCurSubPl(void *packet, CursorObject *cursor);
/* objDispCur's callees, declared ahead of their definitions further down in
 * this TU; without them gcc 2.96 reports "type mismatch with previous implicit
 * declaration". grGpInit, grPacketSend and objCmdPtrGet are declared above. */
extern void curEnvMake(void *packet, int isAlly);
extern void objDispCurSubEn(void *packet, CursorObject *cursor, int outOfLine);
extern void curPutSub(void *packet, CursorObject *cursor);
extern int calcLineChk(BattleUnit *unit);

/*
 * Draw the battle cursor into one GS packet: the player-side cursor, or the
 * enemy-side one, coloured by the target's side and marked when the state's
 * 0x8 flag is set and the target fails calcLineChk.
 */
void objDispCur(CursorObject *cursor, int isPlayer)
{
    GrPacket packet; /* grGpInit's record, as in curTexTrans */
    int outOfLine;
    int isAlly;

    grGpInit(&packet);
    if (!isPlayer) {
        /* The result is ignored. */
        objCmdPtrGet((ObjectTask *)cursor);
        if ((cursor->state->flags & CURSOR_STATE_FLAG_UNIT_SELECT) &&
            tgtUnitGet() != 0 && calcLineChk(tgtUnitGet()) == 0) {
            outOfLine = 1;
            isAlly = 0;
        } else {
            unsigned int isEnemy;

            outOfLine = 0;
            /* No null test is made on the target here. */
            isEnemy = tgtUnitGet()->work->flags & ACTOR_FLAG_ENEMY;
            isAlly = isEnemy == 0;
        }
        curEnvMake(&packet, isAlly);
        objDispCurSubEn(&packet, cursor, outOfLine);
        curPutSub(&packet, cursor);
    } else {
        objDispCurSubPl(&packet, cursor);
    }
    grPacketSend(&packet);
}

void objDispCurSubPl(void *packet, CursorObject *cursor)
{
}

typedef struct CurSubEnRgba {
    unsigned char c[4];
} CurSubEnRgba;

typedef struct CurSubEnSpr {
    float x0, y0, z0, pad0;
    float x1, y1, z1, pad1;
    CurSubEnRgba col;
    short u0, u1, v0, v1;
} CurSubEnSpr;

typedef struct CurSubEnCol {
    CurSubEnRgba c[3];
} CurSubEnCol;

extern const CurSubEnCol curSubEnCol;
extern int grBBCalc(ObjectTask *task, int *x, int *y);
extern void grOpenSpr(void *packet);
extern void grPutSpr(void *packet, CurSubEnSpr *spr, int flag);
extern void grCloseSpr(void);

void objDispCurSubEn(void *packet, CursorObject *cursor, int outOfLine)
{
    CurSubEnSpr spr;
    CurSubEnCol cols;
    int x;
    int y;
    unsigned int frame;
    int w;
    int h;
    int u;
    int v;

    cols = curSubEnCol;
    if (grBBCalc((ObjectTask *)cursor, &x, &y) != 0)
    {
        w = 0x24;
        h = 0x32;
        x -= 0x12;
        y -= 0x32;
        frame = ((unsigned int)frameCntGet() >> 2) % 6;
        u = (int)frame % 3 * w;
        v = (int)frame / 3 * h;
        spr.z0 = 16777212.0f;
        spr.x0 = x;
        spr.y0 = y;
        spr.x1 = x + w;
        spr.y1 = y + h;
        spr.z1 = 16777212.0f;
        spr.u0 = u;
        spr.v0 = v;
        spr.u1 = u + w + 1;
        spr.v1 = v + h + 1;
        __builtin_memcpy(&spr.col, (unsigned char *)&cols + outOfLine * 4, 4);
        grOpenSpr(packet);
        grPutSpr(packet, &spr, 0);
        grCloseSpr();
    }
}

/*
 * Quad record that grPutFT4STQ takes (defined in src/ov01/gr_gp_init.c):
 * four corners, one shared RGBA and per-corner S/T. Restated for the cursor quad.
 */
typedef struct CursorQuad {
    AlignedVector4 points[4]; /* +0x00 */
    unsigned char color[4];  /* +0x40: R, G, B, A */
    float s[4];              /* +0x44 */
    float t[4];              /* +0x54 */
} CursorQuad;
/* The enemy-target cursor quad. */
static CursorQuad cur1;

extern void grCalcMatrix(CursorObject *cursor, Matrix4 matrix);
extern void grGsRegSet(void *packet, int reg, int value);
extern void grOpenFT4STQ(void *packet);
extern void grPutFT4STQ(void *packet, CursorQuad *quad, Matrix4 matrix);
extern void grCloseFT4STQ(void);

/*
 * Draw the spinning cursor quad: the cursor's transform is flattened onto
 * the ground, given a unit scale and a one-turn-per-360-frames Y rotation,
 * turned into a matrix and restored afterwards.
 */
void curPutSub(void *packet, CursorObject *cursor)
{
    CursorTransform saved;
    Matrix4 matrix;
    CursorTransform *transform;
    float degToRad;

    transform = &cursor->state->transform;
    saved = *transform;
    transform->fields.scale.x = 1.0f;
    transform->fields.offset.y = 0.01f;
    transform->fields.scale.y = 1.0f;
    transform->fields.scale.z = 1.0f;
    transform->fields.position.y = 0.0f;
    degToRad = 0.017453292f; /* pi/180 */
    transform->fields.rotation.y = degToRad * (float)((unsigned int)frameCntGet() % 360);
    grCalcMatrix(cursor, matrix);
    grGsRegSet(packet, 0x14, 0x60);
    grGsRegSet(packet, 0x42, 0x44);
    grOpenFT4STQ(packet);
    grPutFT4STQ(packet, &cur1, matrix);
    grCloseFT4STQ();
    *transform = saved;
}

/*
 * Table entry returned by dataXtxFileGet (defined in src/ov01/data_unit_org_get.c).
 * Only textureId, at +0x4, is used here; it is forwarded unchanged to
 * xglPacketTextureTrans.
 */
typedef struct XtxFile {
    unsigned char unmodeled_00[4];
    int textureId; /* +0x4 */
} XtxFile;

extern XtxFile *dataXtxFileGet(int index);
extern void xglPacketTextureTrans(int textureId);

#define CURSOR_XTX_SLOT 2
/* GS register 0x3F (TEXFLUSH): any write flushes the texture cache after a
 * texture upload, matching the dataXtxFileGet(2) transfer just above it. */
#define GS_REG_TEXFLUSH 0x3F

/*
 * Called with 0 by objDispEnCur. The argument is not read.
 */
void curTexTrans(int unused)
{
    GrPacket packet; /* grGpInit's record: a data pointer and an element count. */

    xglPacketTextureTrans(dataXtxFileGet(CURSOR_XTX_SLOT)->textureId);
    grGpInit(&packet);
    grGsRegSet(&packet, GS_REG_TEXFLUSH, 1);
    grPacketSend(&packet);
}

/*
 * Local spelling of GS_REG_SET. The definition in src/ov01/gr_gp_init.c
 * takes a GrPacket *; `void *` is used here because this file passes the
 * `int packet[]` record that grGpInit fills.
 */
extern void GS_REG_SET(void *packet, long long register_id, long long control);
extern unsigned short dataXtxAdrGet(int index);

/*
 * Sets the GS environment for the cursor texture. The second texture base is
 * dataXtxAdrGet entry 0x10 for enemy targets and 0x12 for allies.
 */
void curEnvMake(void *packet, int isAlly)
{
    unsigned short tbp0, tbp1;

    if (!isAlly) {
        tbp0 = dataXtxAdrGet(0xF);
        tbp1 = dataXtxAdrGet(0x10);
    } else {
        tbp0 = dataXtxAdrGet(0xF);
        tbp1 = dataXtxAdrGet(0x12);
    }

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x20000005DD340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x5000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

extern const char battleInitDataYamamotoMvMomoBPssText[32];
extern const char battleInitDataYamamotoMvMomoAPssText[32];
extern const char battleInitDataYamamotoMvGameraPssText[32];
extern const char battleInitDataYamamotoMvKissPssText[32];
extern const char battleInitDataYamamotoMvGrPssText[24];

static const char *mvFname[5] = { battleInitDataYamamotoMvMomoAPssText, battleInitDataYamamotoMvMomoBPssText, battleInitDataYamamotoMvGameraPssText, battleInitDataYamamotoMvKissPssText, battleInitDataYamamotoMvGrPssText };

static unsigned short keyItemTblCha[4] = { 128, 16, 32, 0 };

static unsigned short keyItemTblAgws[4] = { 128, 16, 32, 0 };

static unsigned short keyCmd01[2] = { PAD_L2, PAD_SQUARE };

static unsigned short keyCmd02[2] = { PAD_L2, PAD_TRIANGLE };

static unsigned short keyCmd03[3] = { PAD_R2, PAD_SQUARE, PAD_SQUARE };

static unsigned short keyCmd04[3] = { PAD_R2, PAD_SQUARE, PAD_TRIANGLE };

static unsigned short keyCmd05[3] = { PAD_R2, PAD_TRIANGLE, PAD_TRIANGLE };

static unsigned short keyCmd06[3] = { PAD_R2, PAD_TRIANGLE, PAD_SQUARE };

static unsigned short keyCmd07[3] = { PAD_R2, PAD_SQUARE, PAD_CIRCLE };

static unsigned short keyCmd08[3] = { PAD_R2, PAD_TRIANGLE, PAD_CIRCLE };

static unsigned short keyCmd09[4] = { (PAD_L2 | PAD_R2),PAD_SQUARE, PAD_SQUARE, PAD_CIRCLE };

static unsigned short keyCmd10[4] = { (PAD_L2 | PAD_R2),PAD_SQUARE, PAD_TRIANGLE, PAD_CIRCLE };

static unsigned short keyCmd11[4] = { (PAD_L2 | PAD_R2),PAD_TRIANGLE, PAD_TRIANGLE, PAD_CIRCLE };

static unsigned short keyCmd12[4] = { (PAD_L2 | PAD_R2),PAD_TRIANGLE, PAD_SQUARE, PAD_CIRCLE };

static unsigned short keyCmd13[4] = { (PAD_L2 | PAD_R2),PAD_SQUARE, PAD_CIRCLE, PAD_CIRCLE };

static unsigned short keyCmd14[4] = { (PAD_L2 | PAD_R2),PAD_TRIANGLE, PAD_CIRCLE, PAD_CIRCLE };

static unsigned short keyCmd15[2] = { PAD_L2, PAD_CIRCLE };

static unsigned short keyCmd16[3] = { PAD_R2, PAD_CIRCLE, PAD_SQUARE };

static unsigned short keyCmd17[3] = { PAD_R2, PAD_CIRCLE, PAD_TRIANGLE };

static unsigned short keyCmd18[3] = { PAD_R2, PAD_CIRCLE, PAD_CIRCLE };

static KeyCmd keyCmdTblCha[16] = {
    { keyCmd13, 2, 17 },
    { keyCmd14, 2, 18 },
    { keyCmd07, 2, 11 },
    { keyCmd08, 2, 12 },
    { keyCmd09, 2, 13 },
    { keyCmd10, 2, 14 },
    { keyCmd11, 2, 15 },
    { keyCmd12, 2, 16 },
    { keyCmd03, 2, 3 },
    { keyCmd04, 2, 4 },
    { keyCmd05, 2, 5 },
    { keyCmd06, 2, 6 },
    { keyCmd01, 2, 1 },
    { keyCmd02, 2, 2 },
    { keyCmd15, 6, 7 },
    { 0, 0, 0 }
};

static KeyCmd keyCmdTblAgws[13] = {
    { keyCmd03, 0, 3 },
    { keyCmd04, 0, 4 },
    { keyCmd07, 0, 11 },
    { keyCmd05, 0, 5 },
    { keyCmd06, 0, 6 },
    { keyCmd08, 0, 12 },
    { keyCmd16, 0, 8 },
    { keyCmd17, 0, 9 },
    { keyCmd18, 0, 10 },
    { keyCmd01, 0, 1 },
    { keyCmd02, 0, 2 },
    { keyCmd15, 0, 7 },
    { 0, 0, 0 }
};

static int itemGrp1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

static int itemGrp2[5] = { 10, 12, 13, 14, 16 };

static int itemGrp3[5] = { 15, 17, 20, 24, 25 };

static int itemGrp4[2] = { 9, 11 };

static InoriEnt inoriTbl[4] = {
    { 4, 2, itemGrp4 },
    { 12, 5, itemGrp3 },
    { 50, 5, itemGrp2 },
    { 100, 8, itemGrp1 }
};

static DecoEffEntry decoEff[8] = { { .charaId = 1, .subId = { 2509, 2512 } }, { .charaId = 2, .subId = { 2505, 2508 } }, { .charaId = 3, .subId = { 2501, 2504 } }, { .charaId = 4, .subId = { 2525, 2528 } }, { .charaId = 5, .subId = { 2517, 2520 } }, { .charaId = 6, .subId = { 2513, 2516 } }, { .charaId = 7, .subId = { 2521, 2524 } }, { .charaId = 0, .subId = { 0, 0 } } };

static int effId[7] = { 0xa4c, 0xa4b, 0xa4a, 0xa49, 0xa48, 0xa47, 0xa46 };

static signed char effTbl[8][16] = { { 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 5, 3, -1, -1 }, { -1, -1, 6, 6, 6, -1, 6, 6, -1, -1, 4, 5, 6, 0, 2, -1 }, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, { 6, 6, 6, -1, 6, 6, 6, 6, 5, 5, 5, 6, 5, 3, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 0, 1, -1 }, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

static char chaTbl_00A3D7E0[28] = { 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0 };

static char agwsTbl_00A3D800[16] = { 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0 };

static CursorQuad cur1 = {
    {
        { -0.5f, 0.0f, -0.5f, 1.0f },
        { 0.5f, 0.0f, -0.5f, 1.0f },
        { -0.5f, 0.0f, 0.5f, 1.0f },
        { 0.5f, 0.0f, 0.5f, 1.0f }
    },
    { 0x80, 0x80, 0x80, 0x80 },
    { 0.7795f, 1.0f, 0.7795f, 1.0f },
    { 0.7795f, 0.7795f, 1.0f, 1.0f }
};

int moveTbl[6][5] = { { 2, 4, 5, -1, -1 }, { 1, 3, 4, 5, 6 }, { 2, 5, 6, -1, -1 }, { 1, 2, 5, -1, -1 }, { 1, 2, 3, 4, 6 }, { 2, 3, 5, -1, -1 } };

const char battleInitDataYamamotoMvGrPssText[24] = "data\\yamamoto\\mv\\gr.pss";

const char battleInitDataYamamotoMvKissPssText[32] = "data\\yamamoto\\mv\\kiss.pss";

const char battleInitDataYamamotoMvGameraPssText[32] = "data\\yamamoto\\mv\\gamera.pss";

const char battleInitDataYamamotoMvMomoBPssText[32] = "data\\yamamoto\\mv\\momo_b.pss";

const char battleInitDataYamamotoMvMomoAPssText[32] = "data\\yamamoto\\mv\\momo_a.pss";

const char battleInitPLWinText[16] = "** PL Win!\n";

const char battleInitPLEscText[16] = "** PL Esc!\n";

const char battleInitENWinText[16] = "** EN Win!\n";

const MsgText battleInitUnableToMoveMsg = { .text = "Unable to move due to \"Power Loss\"" };

const char battleInitMakeMaxNumText[32] = "** Make Max=%d Num=%d **\n** ";

const char battleInitUIDText[16] = "%d:U=%X(ID=%d) ";

const char battleInitJpText[8] = "\n";

const char battleInitSP0Text[8] = "** SP0:";

const char battleInitSP1Text[8] = "** SP1:";

const char battleInitPickIDText[32] = "** Pick=%X(ID=%d) %d/%d\n";

const char battleInitBOOSTPLText[24] = "** BOOST PL=%d (%d)\n";

const char battleInitMENUEXITText[16] = "** MENU_EXIT\n";

const char battleInitDEADTIDText[24] = "** DEAD! ->%d /TID=%d\n";

const char battleInitStatDmgSAFEText[24] = "** Stat Dmg: SAFE\n";

const char battleInitItemChangeText[24] = "** item change=%d %d\n";

const char battleInitChangedToText[16] = "Changed to ";

const char battleInitJpText2[16] = "!";

const char battleInitPTGTNULLText[24] = "** pTGT NULL\n";

const char battleInitKeyBuffSetErrorText[32] = "** keyBuffSet: error\n";

const char battleInitMotPackMotActText[32] = "** Mot Pack=%d mot=%d act=%d\n";

const char battleInitFLOWSPECText[24] = "** FLOW_SPEC %d\n";

const char battleInitInoriItemText[24] = "** Inori item=%d\n";

const char battleInitAquiredText[16] = "Aquired ";

const char battleInitScenarioBatExecDstErrText[40] = "** scenarioBatExecDst err: %d\n";

const char battleInitDmgSAFEText[16] = "** Dmg: SAFE\n";

const char battleInitCounterText[16] = "** Counter =%d\n";

const char battleInitJpText3[8] = "** ";

const char battleInitCRITText[8] = "CRIT ";

const char battleInitDEFText[8] = "DEF ";

const char battleInitSTATText[8] = "STAT ";

const char battleInitWAKEText[8] = "WAKE ";

const char battleInitMISSText[8] = "MISS !\n";

const char battleInitHPDMGText[16] = "HP DMG = %d\n";

const char battleInitHPRECText[16] = "HP REC = %d\n";

const char battleInitEPDMGText[16] = "EP DMG = %d\n";

const char battleInitEPRECText[16] = "EP REC = %d\n";

const char battleInitSTATSETText[16] = "STAT SET\n";

const char battleInitSTATRECText[16] = "STAT REC\n";

const char battleInitHPDRAINText[16] = "HP DRAIN = %d\n";

const char battleInitHPDRAIN2Text[16] = "HP DRAIN2 = %d\n";

const char battleInitEPDRAINText[16] = "EP DRAIN = %d\n";

const char battleInitEPDRAIN2Text[16] = "EP DRAIN2 = %d\n";

const char battleInitDmgTypeErrText[24] = "Dmg Type err\n";

const char battleInitPOINTUpEventTimerBXText[40] = "** POINT up: EventTimer B (x%d)\n";

const char battleInitCurEnProcNoTgtText[24] = "curEnProc: no Tgt\n";

const char battleInitCurPosProcNoTgtText[24] = "curPosProc: no Tgt\n";

const CurSubEnCol curSubEnCol = { { { { 0x80, 0x80, 0x80, 0x80 } }, { { 0x30, 0x30, 0x30, 0x80 } }, { { 0x60, 0xd0, 0xb0, 0x80 } } } };

static ConfTgt atkTbl_0[48];

static BattleUnit * pExecUnit;

static BattleUnit * pManUnit;

static BattleUnit * pTgtUnit;

static BattleUnit * pTgtUnit2;

static BattleUnit * pCounterUnit;

static int thinkWk;

static int menuFlag;

static StatUnit statUnit;

static int loadNum[1];

static MsgText msgWork;

static unsigned int jtypeIdx;
