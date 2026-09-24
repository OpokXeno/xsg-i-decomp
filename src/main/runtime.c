#include "common.h"
#include "shared.h"
#include "main/party.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives of this TU show
 * (a1 read as the argument block, a2 written as the result); these
 * wrappers only need the tag.
 */
typedef struct JThread JThread;

extern void SCRIPT_sceneChangeTimeSet(int value);
extern void CharactorAllRecovery(void);
extern void AgwsAllRecovery(void);
extern void tyaCaptureEnd(void);

/* The call block of a native taking three plain ints (java signature "(III)"). */
typedef struct {
    int first;
    int second;
    int third;
} IntTripleCall;

/*
 * An 8-byte-aligned view of Vector4, so a whole-vector assignment compiles
 * to the aligned ld/sd pair the original uses instead of an unaligned
 * ldl/ldr sequence (config/compiler-patterns.json CP-0237, same technique
 * as src/main/pp_init.c's PpVector4).
 */
typedef union RuntimeVector4 {
    Vector4 vector;
    unsigned long long words[2];
} RuntimeVector4;

/*
 * A partial, TU-local view of the engine's actor record (the same object
 * fully recovered as `Actor` in src/main/chr.h, src/main/near_dir.h and
 * src/main/set_motion.h for main's chr/near_dir/set_motion TUs) for the two
 * members scriptReset_evsExit touches: the position vector at +0x10 and the
 * status flags word at +0x4D0, both documented for the same object in
 * chr.h. Named separately from those TUs' `Actor` tag because it collides
 * with the canonical spelling elsewhere (config/header-canon.json).
 */
typedef struct RuntimeActor {
    unsigned char unmodeled_00[0x10];
    RuntimeVector4 position;              /* +0x10, ld/sd at
                                              0x002f76bc..0x002f76d8
                                              (scriptReset_evsExit) */
    unsigned char unmodeled_20[0x4D0 - 0x20];
    unsigned short status_flags;          /* +0x4D0, lhu/sh at
                                              0x002f76c0..0x002f76e0
                                              (scriptReset_evsExit) */
} RuntimeActor;

/*
 * GameLoopState offsets this TU touches. GameLoopState is a TU-local
 * divergent view (config/header-canon.json "GameLoopState"): every unit
 * models only the offsets its own bytes evidence, and no shared header
 * declares it.
 */
typedef struct {
    unsigned char unmodeled_00[4];
    RuntimeActor *player;                  /* +0x04, lw at 0x002f76b0
                                              (scriptReset_evsExit) */
    XglTaskScheduler *task_scheduler;     /* +0x08, lw/beq at 0x002f7350
                                              (setLocation) */
    unsigned char unmodeled_0c[0xE - 0xC];
    unsigned short deferred_jump_pending; /* +0x0E, sh $0 at 0x002f767c */
    unsigned int flags;                   /* +0x10, lw/nor/and/sw at
                                              0x002f74a4..0x002f74bc (disable),
                                              lw/or/sw at 0x002f74c4..0x002f74d8
                                              (enable), lw/sw at
                                              0x002f74e4..0x002f74ec
                                              (getGameState) */
    unsigned char unmodeled_14[0x20 - 0x14];
    unsigned int shoot_control;           /* +0x20, lw/ori/sw and lw/and/sw at
                                              0x002f78a0..0x002f78c8
                                              (setShootFlag, bit 0),
                                              0x002f78dc..0x002f7904
                                              (setShootHeightCheck, bit 1),
                                              0x002f791c..0x002f7944
                                              (setShootIDCheck, bit 2),
                                              0x002f795c..0x002f7984
                                              (setShootUwaCheck, bit 3) */
    unsigned char unmodeled_24[0x52 - 0x24];
    short entrance;                       /* +0x52, lh at 0x002f7424 */
    unsigned char unmodeled_54[0x29F44 - 0x54];
    float shoot_range;                    /* +0x29F44, swc1 at 0x002f799c */
    unsigned char unmodeled_29f48[0x2A018 - 0x29F48];
    int active_event_id;                  /* +0x2A018, lw at 0x002f8730 */
    unsigned char unmodeled_2a01c[0x2A020 - 0x2A01C];
    RuntimeVector4 saved_position;        /* +0x2A020, ld/sd at
                                              0x002f76bc..0x002f76d8
                                              (scriptReset_evsExit) */
} GameLoopStateRuntimeView;

extern GameLoopStateRuntimeView GameLoopState;

/*
 * A partial view of the script VM thread (JThread, fully recovered in
 * src/main/chr.h for main/tu248) for the three words evsExit touches: the
 * state word at +0x24 and the resume/frame counters at +0x3C/+0x3E chr.h
 * documents for the same object. Modeled separately here because chr.h is
 * TU-local to tu248.
 */
typedef struct {
    unsigned char unmodeled_00[0x24];
    unsigned int flags;              /* +0x24 */
    unsigned char unmodeled_28[0x3C - 0x24 - 4];
    unsigned short resume_frames;    /* +0x3C */
    unsigned short frame_depth;      /* +0x3E */
} RuntimeThreadState;

/* setLocation below touches the same three RuntimeThreadState words as
   evsExit: resume_frames takes frame_depth and flags gains bit 0x8
   (native-call block) and 0x1 (generic wait) (0x002f7388..0x002f739c). */

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_execBattle__II);

/* The call block of a native taking two plain ints (java signature "(II)"). */
typedef struct {
    int first;
    int second;
} IntPairCall;

extern int xglFlagsGet(int group, int bit);

void Java_xeno_util_Runtime_getFlags__II(JThread *thread, IntPairCall *arguments,
                                         int *result)
{
    *result = xglFlagsGet(arguments->first, arguments->second);
}

/* The call block of Java_xeno_util_Runtime_setFlags__III, the xglFlagsSet
   operands the native passes through unchanged and in order. */
typedef struct {
    int bit_offset;
    int bit_count;
    int value;
} FlagsSetCall;

extern int xglFlagsSet(int bit_offset, int bit_count, int value);

void Java_xeno_util_Runtime_setFlags__III(JThread *thread, FlagsSetCall *arguments,
                                          unsigned int *result)
{
    xglFlagsSet(arguments->bit_offset, arguments->bit_count, arguments->value);
}

extern void LoadMapOnly(int map_id);
extern void MapChange2(int map_id);

void Java_xeno_util_Runtime_setLocation__III(RuntimeThreadState *thread,
                                             IntTripleCall *arguments,
                                             unsigned int *result)
{
    int destination = arguments->first;

    if (arguments->third == 0 || GameLoopState.task_scheduler == 0) {
        MapChange2(destination);
        return;
    }
    LoadMapOnly(destination);
    thread->resume_frames = thread->frame_depth;
    thread->flags |= 9;
}

extern int MapGetNo(void);

void Java_xeno_util_Runtime_getLocation__(JThread *thread, void *arguments, int *result)
{
    *result = MapGetNo();
}

/* The script VM's 32-entry general register file, indexed with the low 5
   bits of the requested register number (VMRegister[index & 0x1F]). */
extern int VMRegister[32];

void Java_xeno_util_Runtime_setRegister__II(JThread *thread, IntPairCall *arguments,
                                            unsigned int *result)
{
    VMRegister[arguments->first & 0x1F] = arguments->second;
}

void Java_xeno_util_Runtime_getRegister__I(JThread *thread, int *arguments, int *result)
{
    *result = VMRegister[*arguments & 0x1F];
}

void Java_xeno_util_Runtime_getEntrance__(JThread *thread, void *arguments, int *result)
{
    *result = GameLoopState.entrance;
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setPlayerControl__Z);

/* The call block of a native taking three plain floats (java signature "(FFF)"). */
typedef struct {
    float walk_threshold;
    float run_threshold;
    float vector_rate;
} PlayerMoveParamCall;

extern void GameCfPlayerMoveParamSet(float walk_threshold, float run_threshold,
                                      float vector_rate);

void Java_xeno_util_Runtime_setPlayerMoveParam__FFF(JThread *thread,
                                                     PlayerMoveParamCall *arguments,
                                                     unsigned int *result)
{
    GameCfPlayerMoveParamSet(arguments->walk_threshold, arguments->run_threshold,
                             arguments->vector_rate);
}

void Java_xeno_util_Runtime_disable__I(JThread *thread, unsigned int *arguments,
                                       unsigned int *result)
{
    GameLoopState.flags &= ~*arguments;
}

void Java_xeno_util_Runtime_enable__I(JThread *thread, unsigned int *arguments,
                                      unsigned int *result)
{
    GameLoopState.flags |= *arguments;
}

void Java_xeno_util_Runtime_getGameState__(JThread *thread, void *arguments, int *result)
{
    *result = GameLoopState.flags;
}

void Java_xeno_util_Runtime_CaptureEnd__(JThread *thread, void *arguments,
                                         unsigned int *result)
{
    tyaCaptureEnd();
}

/*
 * A java.lang.String argument, as CaptureStart/setEventTimer below read it:
 * offset +0x00 (the class word every object reference begins with, per
 * shared.h) is untouched by this TU, and +0x04 holds the interned string
 * record the String was loaded from (lw v1,4(v0) at 0x002f7514/0x002f78ac).
 */
typedef struct {
    void *unmodeled_00;
    struct InternedString *value;
} StringRef;

/*
 * The interned constant-string record `StringRef.value` points to, modeled
 * here only for the `bytes` pointer both natives below read at +0x08 (lw
 * v0,8(v0) at 0x002f7518/0x002f78b0). Same object as `ConstString` of
 * src/main/string_utf_get_hash.h (main/tu224): next-in-bucket link and
 * packed hash/length occupy the same leading 8 bytes there.
 */
struct InternedString {
    unsigned char unmodeled_00[8];
    char *bytes;
};

/* The call block of a native taking (java.lang.String, int). */
typedef struct {
    StringRef *string;
    int value;
} StringArgCall;

extern void tyaCaptureStart(const char *name, int count);

void Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I(JThread *thread,
                                                               StringArgCall *arguments,
                                                               unsigned int *result)
{
    tyaCaptureStart(arguments->string->value->bytes, arguments->value);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jump_sub);

extern void Java_xeno_util_Runtime_jump_sub(int destination, int argument);

/* The deferred script jump scriptReset_jump below executes once scheduled. */
typedef struct {
    int destination;
    int argument;
} DeferredJumpCall;

extern DeferredJumpCall args_jump;

static void scriptReset_jump(void)
{
    GameLoopState.deferred_jump_pending = 0;
    Java_xeno_util_Runtime_jump_sub(args_jump.destination, args_jump.argument);
}

static void scriptReset_evsExit(void)
{
    GameLoopState.deferred_jump_pending = 0;
    Java_xeno_util_Runtime_jump_sub(args_jump.destination, args_jump.argument);
    GameLoopState.player->position = GameLoopState.saved_position;
    GameLoopState.player->status_flags |= 0x1000;
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jumpCF__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jumpEvent__I);

/* The call block of a native taking four plain ints (java signature "(IIII)"). */
typedef struct {
    int first;
    int second;
    int third;
    int fourth;
} IntQuadCall;

extern void GameDefocusQuickSet(int first, int second, int third, int fourth);

void Java_xeno_util_Runtime_setDefocusQuick__IIII(JThread *thread, IntQuadCall *arguments,
                                                  unsigned int *result)
{
    GameDefocusQuickSet(arguments->first, arguments->second, arguments->third,
                        arguments->fourth);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setDefocus__IIaI);

/* A defocus parameter table of seventeen-entry rows, one word ahead of the
   symbol (index 0 is untouched by this TU). */
extern int GameDefocusParam[];

void Java_xeno_util_Runtime_setDefocusParam__III(JThread *thread, IntTripleCall *arguments,
                                                 unsigned int *result)
{
    GameDefocusParam[1 + arguments->first * 17 + arguments->second] = arguments->third;
}

extern void setEventTimerTaskEntry(const char *method_reference, int countdown);

void Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I(JThread *thread,
                                                                StringArgCall *arguments,
                                                                unsigned int *result)
{
    setEventTimerTaskEntry(arguments->string->value->bytes, arguments->value);
}

extern void MapChangeResource(int map_id, int resource_id);

void Java_xeno_util_Runtime_setMap__II(JThread *thread, IntPairCall *arguments,
                                       unsigned int *result)
{
    MapChangeResource(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_setShootFlag__Z(JThread *thread, unsigned char *arguments,
                                            unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 1;
    } else {
        GameLoopState.shoot_control &= ~1;
    }
}

void Java_xeno_util_Runtime_setShootHeightCheck__Z(JThread *thread, unsigned char *arguments,
                                                    unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 2;
    } else {
        GameLoopState.shoot_control &= ~2;
    }
}

void Java_xeno_util_Runtime_setShootIDCheck__Z(JThread *thread, unsigned char *arguments,
                                               unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 4;
    } else {
        GameLoopState.shoot_control &= ~4;
    }
}

void Java_xeno_util_Runtime_setShootUwaCheck__Z(JThread *thread, unsigned char *arguments,
                                                unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 8;
    } else {
        GameLoopState.shoot_control &= ~8;
    }
}

void Java_xeno_util_Runtime_setShootRange__F(JThread *thread, float *arguments,
                                             unsigned int *result)
{
    GameLoopState.shoot_range = *arguments;
}

extern int dataBoxInc(int item, int count);

void Java_xeno_util_Runtime_addItem__II(JThread *thread, IntPairCall *arguments,
                                        signed char *result)
{
    *result = (signed char) dataBoxInc(arguments->first, arguments->second);
}

extern int CreateEvtItemGetTask(int item, int count);

void Java_xeno_util_Runtime_addItemWin__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) CreateEvtItemGetTask(arguments->first, arguments->second);
}

extern int dataBoxDec(int item, int count);

void Java_xeno_util_Runtime_removeItem__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) dataBoxDec(arguments->first, arguments->second);
}

extern int dataBoxChk(int item, int count);

void Java_xeno_util_Runtime_checkItem__II(JThread *thread, IntPairCall *arguments,
                                          int *result)
{
    *result = dataBoxChk(arguments->first, arguments->second);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getItemName__II);

extern int dataMoneyBoxInc(int amount);

void Java_xeno_util_Runtime_addGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxInc(*arguments);
}

extern int dataMoneyBoxDec(int amount);

void Java_xeno_util_Runtime_removeGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxDec(*arguments);
}

extern int dataMoneyBoxChk(void);

void Java_xeno_util_Runtime_checkGold__(JThread *thread, void *arguments, int *result)
{
    *result = dataMoneyBoxChk();
}

extern void sefProgressEffect(int frame_count);

void Java_xeno_util_Runtime_progressEffect__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    sefProgressEffect(*arguments);
}

extern void GameStateRestoreCameraLight(void);
extern void GameStateSaveCameraLight(void);
extern void MenuShopMain(int shop);

void Java_xeno_util_Runtime_enterShop__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    GameStateSaveCameraLight();
    MenuShopMain(*arguments);
    GameStateRestoreCameraLight();
}

/*
 * The leader's battle id: read directly out of SaveData (lui/lhu at
 * 0x002f7bd0/0x002f7bd4, no call) rather than through PartyDataGet(), which
 * getPartyDataOfs below shows returns SaveData + 0x10078, so this offset is
 * that region's own +0x2C.
 */
#define SAVE_LEADER_BATTLE_ID 0x100A4

typedef struct {
    unsigned short leader_battle_id;
} SaveDataLeaderView;

void Java_xeno_util_Runtime_getLeader__(JThread *thread, void *arguments, int *result)
{
    *result = ((SaveDataLeaderView *) (SaveData + SAVE_LEADER_BATTLE_ID))->leader_battle_id;
}

static char *getPartyDataOfs(unsigned int selector)
{
    int type = (selector >> 16) & 0xff;
    unsigned int offset = selector & 0xffff;
    char *party_data = (char *)PartyDataGet();

    char *selected;

    if (type == 1) {
        selected = party_data + 0x30;
    } else if (type < 2) {
        goto default_party;
    } else if (type == 2) {
        selected = party_data + 0x3c;
    } else if (type == 3) {
        selected = party_data + 0x140;
    } else {
default_party:
        selected = party_data;
    }
    return selected + offset;
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getPartyData__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setPartyData__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", cid2bic);

/* Converts a Java character id to its battle-member index, as every
   Party*Check/On/Off callee below expects (still asm in this TU). */
extern int cid2bic(int character_id);

extern void PartyFriendOn(int battle_index);

void Java_xeno_util_Runtime_setFriend__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    PartyFriendOn(cid2bic(*arguments));
}

extern void PartyFriendOff(int battle_index);

void Java_xeno_util_Runtime_resetFriend__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyFriendOff(cid2bic(*arguments));
}

extern int PartyFriendCheck(int battle_index);

void Java_xeno_util_Runtime_checkFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyFriendCheck(cid2bic(*arguments));
}

extern void PartyLockPartyOn(int battle_index);

void Java_xeno_util_Runtime_setLockParty__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyLockPartyOn(cid2bic(*arguments));
}

extern void PartyLockPartyOff(int battle_index);

void Java_xeno_util_Runtime_resetLockParty__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyLockPartyOff(cid2bic(*arguments));
}

extern int PartyLockPartyCheck(int battle_index);

void Java_xeno_util_Runtime_checkLockParty__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyLockPartyCheck(cid2bic(*arguments));
}

extern void PartyOutFriendOn(int battle_index);

void Java_xeno_util_Runtime_setOutFriend__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyOutFriendOn(cid2bic(*arguments));
}

extern void PartyOutFriendOff(int battle_index);

void Java_xeno_util_Runtime_resetOutFriend__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyOutFriendOff(cid2bic(*arguments));
}

extern int PartyOutFriendCheck(int battle_index);

void Java_xeno_util_Runtime_checkOutFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyOutFriendCheck(cid2bic(*arguments));
}

extern void PartyTakeAgwsOn(int battle_index);

void Java_xeno_util_Runtime_setTakeAgws__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyTakeAgwsOn(cid2bic(*arguments));
}

extern void PartyTakeAgwsOff(int battle_index);

void Java_xeno_util_Runtime_resetTakeAgws__I(JThread *thread, int *arguments,
                                             unsigned int *result)
{
    PartyTakeAgwsOff(cid2bic(*arguments));
}

extern int PartyTakeAgwsCheck(int battle_index);

void Java_xeno_util_Runtime_checkTakeAgws__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyTakeAgwsCheck(cid2bic(*arguments));
}

extern void PartyBattleChange(int battle_index);

void Java_xeno_util_Runtime_battleChangeParty__I(JThread *thread, int *arguments,
                                                  unsigned int *result)
{
    PartyBattleChange(cid2bic(*arguments));
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightCol__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightDir__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightVec__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setWindParam__IFFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mpeg2AfterCrossFade__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mailFlag__I);

extern void UmnMailMain(int mail);
extern void xglCullingIgnore(void);
extern void xglCullingIgnoreOff(void);

void Java_xeno_util_Runtime_mailExec__I(JThread *thread, int *arguments,
                                        unsigned int *result)
{
    GameStateSaveCameraLight();
    xglCullingIgnore();
    UmnMailMain(*arguments);
    xglCullingIgnoreOff();
    GameStateRestoreCameraLight();
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_minigameExec__I);

void Java_xeno_util_Runtime_setMenuLock__(JThread *thread, void *arguments,
                                          unsigned int *result)
{
    /*
     * Arms the script scene-change timer, which SCRIPT_sceneChangeTimeDec
     * counts down once per game frame, with 62 frames.
     */
    SCRIPT_sceneChangeTimeSet(62);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_evsSetRetPoint__);

extern void (*jthreadResetFunc)(void);
extern void scriptReset_evsExit(void);

void Java_xeno_util_Runtime_evsExit__(RuntimeThreadState *thread, void *arguments,
                                      unsigned int *result)
{
    /* Blocks the calling thread the same way CHR_sclX/System_waitFor do
       (chr.h): resume_frames takes the current frame_depth and flags gains
       0x8 (native-call block) and 0x1 (generic wait). The updated flags word
       is staged before the resume_frames store and written back after it,
       matching the original instruction order (0x002f873c..0x002f8750). */
    unsigned int flags;

    args_jump.destination = GameLoopState.active_event_id;
    args_jump.argument = -1;
    jthreadResetFunc = scriptReset_evsExit;
    flags = thread->flags | 9;
    thread->resume_frames = thread->frame_depth;
    thread->flags = flags;
}

extern void dataEtherTecSet(int value);

void Java_xeno_util_Runtime_etherTecSet__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    dataEtherTecSet(*arguments);
}

void Java_xeno_util_Runtime_charAllRecovery__(JThread *thread, void *arguments,
                                              unsigned int *result)
{
    CharactorAllRecovery();
}

void Java_xeno_util_Runtime_AGWSAllRecovery__(JThread *thread, void *arguments,
                                              unsigned int *result)
{
    AgwsAllRecovery();
}
