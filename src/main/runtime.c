#include "common.h"
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

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_execBattle__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getFlags__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setFlags__III);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setLocation__III);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getLocation__);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setRegister__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getRegister__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getEntrance__);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setPlayerControl__Z);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setPlayerMoveParam__FFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_disable__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_enable__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getGameState__);

void Java_xeno_util_Runtime_CaptureEnd__(JThread *thread, void *arguments,
                                         unsigned int *result)
{
    tyaCaptureEnd();
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jump_sub);

INCLUDE_ASM("asm/main/nonmatchings/runtime", scriptReset_jump);

INCLUDE_ASM("asm/main/nonmatchings/runtime", scriptReset_evsExit);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jumpCF__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jumpEvent__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setDefocusQuick__IIII);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setDefocus__IIaI);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setDefocusParam__III);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setMap__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setShootFlag__Z);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setShootHeightCheck__Z);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setShootIDCheck__Z);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setShootUwaCheck__Z);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setShootRange__F);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_addItem__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_addItemWin__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_removeItem__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkItem__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getItemName__II);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_addGold__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_removeGold__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkGold__);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_progressEffect__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_enterShop__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getLeader__);

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

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_resetFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setLockParty__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_resetLockParty__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkLockParty__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setOutFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_resetOutFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkOutFriend__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setTakeAgws__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_resetTakeAgws__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_checkTakeAgws__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_battleChangeParty__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightCol__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightDir__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightVec__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setWindParam__IFFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mpeg2AfterCrossFade__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mailFlag__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mailExec__I);

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

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_evsExit__);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_etherTecSet__I);

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
