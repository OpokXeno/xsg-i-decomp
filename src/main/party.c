#include "common.h"
#include "shared.h"
#include "main/party.h"
#include "main/xgl_cd.h"
#include "main/xgl_hdd.h"
#include "party.h"

/*
 * Party state: the original party source file, main 0x002b56c0..0x002b6494
 * (the file begins at the gcc2_compiled. marker 0x002b56c0; this candidate
 * stops after PartyAllPartyGet2, before PartyAgwsGet).
 *
 * Every function except PartyAllPartyGet and PartyReserveGet is an accepted
 * unit included here unchanged, in original address order, so that the two
 * new functions are compiled in the same translation unit as the callees
 * that precede them (PartyFriendCheck, PartyFriendLockCheck). GCC 2.96 only
 * lets the loop exits of PartyAllPartyGet/PartyReserveGet carry the return
 * value in a plain beq delay slot when those callees are defined earlier in
 * the same file; compiled alone the same source produces beql.
 */

/* The party state is the 0x188-byte region embedded at SaveData + 0x10078. */
unsigned char *PartyDataGet(void)
{
    return SaveData + 0x10078;
}

/*
 * PartyDataInit resets the party state, loads the base technique records
 * (tecinit.bin) and the skill table (skilldat.bin) through the menu work
 * buffer, resets sound output, HDD, pad, radar and UMN mail state, clears the
 * item/weapon/bullet/accessory/event inventories, gives the starting money and
 * weapon 19, clears the simulation number and restarts the play clock.
 */
void PartyDataInit(void)
{
    PartyDataPrefix *party_state = (PartyDataPrefix *)(SaveData + SAVE_PARTY_STATE);
    unsigned char *party;
    TecInitEntry *tec_init;
    unsigned char *skill_party;
    unsigned char *skill_data;
    PartySkillPoints *next_level_points;
    unsigned char *skill_level;
    unsigned char *umn_work;
    unsigned short *mail_data;
    unsigned char *block0;
    unsigned char *block1;
    unsigned char *block2;
    unsigned char *pad;
    unsigned char initial_skill_level;
    int record;
    int chr;
    int i;

    memset(party_state, 0, PARTY_STATE_SIZE);

    party = PartyDataGet();
    tec_init = MenuWorkEndGet();
    xglCdReadFile("data\\endou\\tec\\tecinit.bin", tec_init, 0, 1);
    for (record = 0; record < 8; record++) {
        ((TecInitEntry *)(party + PARTY_TEC_RECORDS))[record] = tec_init[record];
    }

    /* Every character starts at skill level 1 with the level-1 entry of the
     * next-level table (the one SkillNextLvGet() reads) as its points to the
     * next level. Both arrays are walked by pointer: the original reverses
     * this loop's counter, which GCC only does when the counter indexes
     * nothing. */
    skill_party = PartyDataGet();
    skill_data = MenuWorkEndGet();
    xglCdReadFile("data\\endou\\skill\\skilldat.bin", skill_data, 0, 1);
    initial_skill_level = 1;
    next_level_points = (PartySkillPoints *)(skill_party + PARTY_SKILL_POINTS);
    skill_level = ((PartySkillLevelArray *)(skill_party + PARTY_SKILL_LEVELS))->level_by_character;
    for (chr = 0; chr < 8; chr++) {
        *skill_level++ = initial_skill_level;
        *next_level_points++ = ((unsigned short *)(skill_data + SKILL_NEXT_LEVEL_TABLE))[1];
    }

    SsdSetOutputMode(1);
    SaveData[40] = 1;
    if (xglHddMount() == 1) {
        xglHddActivate(1);
        SaveData[64] = 0;
    }
    /* The same PadData byte pair MenuSystemInitSet() and subMenuSystemMain()
     * write; their meaning is not recovered, so the object is viewed as bytes. */
    pad = (unsigned char *)&PadData;
    pad[0x56] = 0;
    pad[0xBE] = 0;
    PartyRadarDispSet(1);

    /* The same member PartyDataInit2() clears. */
    party_state->leader_battle_id = 1;

    memset(SaveData + SAVE_MENU_SAVE_DATA, 0, 8);
    umn_work = SaveData + SAVE_UMN_WORK;
    memset(umn_work, 0, UMN_WORK_SIZE);
    for (i = 0; i < UMN_MAIL_BOX_SLOTS; i++) {
        SaveData[SAVE_UMN_WORK + i] = 0xFF;     /* empty mail-box slot */
    }

    UmnMailBoxSet(78);
    *UmnMailDataGet(78) |= 1;
    UmnMailBoxSet(79);
    *UmnMailDataGet(79) |= 1;
    UmnMailBoxSet(80);
    *UmnMailDataGet(80) |= 1;

    *(unsigned short *)(umn_work + UMN_WORK_FLAGS) |= 1;

    /* One memset per mail entry; the original walks the pointer. */
    mail_data = (unsigned short *)(SaveData + SAVE_UMN_MAIL_DATA);
    for (i = 0; i < UMN_MAIL_DATA_COUNT; i++) {
        memset(mail_data, 0, sizeof(*mail_data));
        mail_data++;
    }

    memset(itmBox, 0, sizeof(itmBox));
    memset(wpnBox, 0, sizeof(wpnBox));
    memset(bltBox, 0, sizeof(bltBox));
    memset(accBox, 0, sizeof(accBox));
    memset(evtBox, 0, sizeof(evtBox));

    moneyBox = 1000;
    dataWpnBoxInc(19);

    block0 = SaveData + SAVE_INIT_BLOCKS;
    block1 = block0 + 256;
    block2 = block0 + 512;
    memset(block0, 0, 256);
    memset(block1, 0, 256);
    memset(block2, 0, 256);

    UmnSimulationNo = 0;
    PartyTimeInit();
}

void PartyDataInit2(void)
{
    PartyDataPrefix *party_data;

    PartyFriendOn(3);
    PartyFriendOn(1);
    PartyFriendOn(2);
    PartyAttackerSet(0, 3, 1);
    PartyAttackerSet(1, 1, 2);
    PartyAttackerSet(2, 2, 3);
    party_data = (PartyDataPrefix *)PartyDataGet();
    party_data->leader_battle_id = 0;
}

void PartyFriendOn(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 7 && id != 4) {
        data->friend_mask |= (unsigned short)(1 << index);
    }
}

void PartyFriendOff(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 7 && id != 4) {
        data->friend_mask &= (unsigned short)~(1 << index);
    }
}

int PartyFriendCheck(int id)
{
    unsigned int index = (unsigned int)(id - 1);
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int mask;
    if (index >= 7u || id == 4) {
        return 0;
    }
    mask = data->friend_mask;
    return ((mask & (1 << index)) != 0);
}

void PartyLockPartyOn(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 12 && id != 4) {
        data->lock_party_mask |= (unsigned short)(1 << index);
    }
}

void PartyLockPartyOff(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 12 && id != 4) {
        data->lock_party_mask &= (unsigned short)~(1 << index);
    }
}

int PartyLockPartyCheck(int id)
{
    unsigned int index = (unsigned int)(id - 1);
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int mask;
    if (index >= 12u || id == 4) {
        return 0;
    }
    mask = data->lock_party_mask;
    return ((mask & (1 << index)) != 0);
}

void PartyOutFriendOn(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 12 && id != 4) {
        data->out_friend_mask |= (unsigned short)(1 << index);
    }
}

void PartyOutFriendOff(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 1;
    if (index < 12 && id != 4) {
        data->out_friend_mask &= (unsigned short)~(1 << index);
    }
}

int PartyOutFriendCheck(int id)
{
    unsigned int index = (unsigned int)(id - 1);
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int mask;
    if (index >= 12u || id == 4) {
        return 0;
    }
    mask = data->out_friend_mask;
    return ((mask & (1 << index)) != 0);
}

void PartyTakeAgwsOn(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 17;
    if (index < 16) {
        data->take_agws_mask |= (unsigned short)(1 << index);
    }
}

void PartyTakeAgwsOff(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = id - 17;
    if (index < 16) {
        data->take_agws_mask &= (unsigned short)~(1 << index);
    }
}

int PartyTakeAgwsCheck(int id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = (unsigned int)(id - 17);
    unsigned int bit;

    if (index >= 16u) {
        return 0;
    }
    bit = 1u << index;
    return ((data->take_agws_mask & bit) != 0);
}

/*
 * Puts `party_id` into attack slot `slot_index`, with `attack_position` when it
 * is positive. A zero party id empties the slot. Returns -1 when the member is
 * not a recruited friend, 0 otherwise.
 */
int PartyAttackerSet(int slot_index, int party_id, int attack_position)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();

    if (party_id != 0) {
        if (PartyFriendCheck(party_id) != 0) {
            data->attack_slots[slot_index].party_id = party_id;
            if (attack_position > 0) {
                data->attack_slots[slot_index].attack_position = attack_position;
            }
        } else {
            return -1;
        }
    } else {
        data->attack_slots[slot_index].party_id = 0;
        data->attack_slots[slot_index].attack_position = 0;
    }
    return 0;
}

/*
 * Swaps an attacking member out of battle: the attack slot that holds
 * `party_id` is given to the first reserve member, who also becomes the
 * party leader when the member leaving was the leader.
 */
void PartyBattleChange(int party_id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    PartyAttackPosition *slot;
    int reserve_ids[6];
    int slot_index;
    int new_id;

    if (party_id == 0) {
        return;
    }
    slot = data->attack_slots;
    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->party_id == party_id) {
            if (PartyReserveGet(reserve_ids) != 0) {
                new_id = reserve_ids[0];
                slot->party_id = new_id;
                if (PartyLeaderCheck(party_id) != 0) {
                    PartyLeaderSet(new_id);
                }
            }
            break;
        }
    }
}

/*
 * Makes an attacking party member the party leader. The low halfword of
 * `leader` is the member's party id, the high halfword an optional costume
 * number (1 or 2). Returns 0 when the member is not in the attack slots.
 */
int PartyLeaderSet(int leader)
{
    /* Costume battle ids, indexed [battle id - 1][costume - 1]; an entry of 0
     * keeps the member's own battle id. The original object is the
     * function-local static CostumData.0 at 0x0036dd38. */
    static u16 CostumData[8][2] = {
        { 11, 9 },
        { 0, 0 },
        { 18, 0 },
        { 22, 23 },
        { 25, 24 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
    };
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    int party_id = leader & 0xffff;
    int costume;
    int battle_id;
    int costume_battle_id;

    if (PartyAttackerCheck(party_id) == 0) {
        return 0;
    }
    battle_id = dataCidGet(party_id);
    costume = (unsigned int)leader >> 16;
    if (costume != 0) {
        costume_battle_id = CostumData[battle_id - 1][costume - 1];
        if (costume_battle_id != 0) {
            battle_id = costume_battle_id;
        }
    }
    data->leader_battle_id = battle_id;
    return 1;
}

int PartyLeaderCheck(int chara_id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();

    return data->leader_battle_id == dataCidGet(chara_id);
}

void PartyRadarDispSet(int value)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    data->radar_disp = value;
}

int PartyFriendLockCheck(int id, int flag)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    unsigned int index = (unsigned int)(id - 1);
    unsigned int bits = data->friend_mask;
    if (index >= 12u || id == 4) {
        return 0;
    }
    if (flag & 1) {
        bits &= ~data->out_friend_mask;
    }
    if (flag & 2) {
        bits &= ~data->lock_party_mask;
    }
    return (bits & (1 << index)) != 0;
}

/* Collects party member ids into out[] and returns how many were stored.
 * mode 0: the attack line followed by the reserve line,
 * mode 1: every friend that is not locked out (at most six),
 * mode 2: every friend that has joined (at most six). */
int PartyAllPartyGet(int *out, int mode)
{
    int count = 0;
    int id;

    switch (mode) {
    case 0:
        count = PartyAttackerGet(out);
        count += PartyReserveGet(out + count);
        break;
    case 1:
        for (id = 1; id < 8; id++) {
            if (PartyFriendLockCheck(id, 1)) {
                *out++ = id;
                count++;
                if (count == 6) {
                    break;
                }
            }
        }
        break;
    case 2:
        for (id = 1; id < 8; id++) {
            if (PartyFriendCheck(id)) {
                *out++ = id;
                count++;
                if (count == 6) {
                    break;
                }
            }
        }
        break;
    }
    return count;
}

/*
 * Stores the menu ids of the occupied attack slots, stopping at the first
 * empty slot, and returns how many were stored.
 *
 * The original loop is not rotated: both exits test at the top of every
 * iteration, including the first one. GCC 2.96 rotates for(;;)/while loops
 * whose body breaks out (and then folds the first slot_index < 3 test); the
 * do { } while (1) spelling keeps the tests in place. The slot's party id is
 * read again for the call, as in the original, which keeps slot++ after the
 * empty-slot test instead of in the halfword load's delay.
 */
int PartyAttackerGet(int *out)
{
    PartyAttackPosition *slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
    int count = 0;
    int slot_index = 0;

    do {
        if (slot_index >= 3) {
            break;
        }
        slot_index++;
        if (slot->party_id == 0) {
            break;
        }
        *out++ = MenuMaryIdChange(slot->party_id);
        count++;
        slot++;
    } while (1);
    return count;
}

/* Stores the ids of available friends that are not in the attack line, up
 * to a combined party of six, and returns how many were stored. */
int PartyReserveGet(int *reserve_ids)
{
    int attackers[16];
    int attacker_count = PartyAttackerGet(attackers);
    int count = 0;
    int id;

    for (id = 1; id < 13; id++) {
        if (!PartyFriendLockCheck(id, 1)) {
            continue;
        }
        if (PartyAttackerCheck(id)) {
            continue;
        }
        *reserve_ids++ = id;
        count++;
        if (attacker_count + count == 6) {
            break;
        }
    }
    return count;
}

/*
 * The complete 0x188-byte party state that PartyDataGet() points into
 * (SaveData + 0x10078), handled here only as a whole block. Its members are
 * not recovered in this unit: the image records just the evidenced size and
 * the 8-byte alignment implied by the original's doubleword ld/sd block copy.
 */
typedef struct PartyStateImage {
    unsigned long long words[0x188 / 8];
} PartyStateImage;

/* Runs PartyAllPartyGet(out, 0) against a caller-supplied party state and
 * restores the live state afterwards; returns the number of ids stored. */
int PartyAllPartyGet2(int *out, PartyStateImage *state)
{
    PartyStateImage *party = (PartyStateImage *)PartyDataGet();
    PartyStateImage saved;
    int count;

    saved = *party;
    *party = *state;
    count = PartyAllPartyGet(out, 0);
    *party = saved;
    return count;
}

int PartyAgwsGet(int *party_ids)
{
    int owned_agws;
    int agws_index;
    int party_count;

    owned_agws = ((PartyDataPrefix *)PartyDataGet())->take_agws_mask;
    party_count = 0;
    agws_index = 0;

    while (agws_index < 16) {
        if ((owned_agws >> agws_index) & 1) {
            *party_ids++ = agws_index + 17;
            party_count++;
            if (party_count == 6) {
                break;
            }
        }
        agws_index++;
    }
    return party_count;
}

/* Count every set bit in the first two bytes of the party-state prefix. */

int PartyCharNumGet(void)
{
    unsigned char *party_prefix = PartyDataGet();
    int bit_index;
    int set_bit_count;

    bit_index = 0;
    set_bit_count = 0;

    while (bit_index < 16) {
        set_bit_count += (party_prefix[bit_index / 8]
                          >> (bit_index % 8)) & 1;
        bit_index++;
    }
    return set_bit_count;
}

/* Count the low-nibble bits of the first four party-state prefix bytes. */

int PartyAgwsNumGet(void)
{
    unsigned char *party_prefix = PartyDataGet();
    int nibble_bit_index;
    int set_bit_count;

    nibble_bit_index = 0;
    set_bit_count = 0;

    while (nibble_bit_index < 16) {
        set_bit_count += (party_prefix[nibble_bit_index / 4]
                          >> (nibble_bit_index % 4)) & 1;
        nibble_bit_index++;
    }
    return set_bit_count;
}

int PartyAttackerCheck(int party_id)
{
    PartyDataPrefix *data = (PartyDataPrefix *)PartyDataGet();
    PartyAttackPosition *slot;
    int slot_index;

    if (party_id < 1 || party_id > 16) {
        return 0;
    }

    switch (party_id) {
    case 11:
        party_id = 20;
        break;
    case 12:
        party_id = 19;
        break;
    }

    slot = data->attack_slots;
    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->party_id == party_id) {
            return 1;
        }
    }
    return 0;
}

int PartyAttackPosCheck(int attack_position)
{
    PartyAttackPosition *slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
    int slot_index;

    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->attack_position == attack_position) {
            return slot->party_id;
        }
    }
    return 0;
}

int PartyAttackPosGet(int party_id)
{
    PartyAttackPosition *slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
    int slot_index;

    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->party_id == party_id) {
            return slot->attack_position;
        }
    }
    return 0;
}

void PartyAttackPosSet(int party_id, int attack_position)
{
    PartyAttackPosition *slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
    int slot_index;

    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->party_id == party_id) {
            slot->attack_position = attack_position;
            return;
        }
    }
}

void PartyAttackPosChange(int first_party_id, int second_party_id)
{
    PartyAttackPosition *slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
    int slot_index;
    int first_attack_position = 0;
    int second_attack_position = 0;

    for (slot_index = 0; slot_index < 3; slot_index++, slot++) {
        if (slot->party_id == first_party_id) {
            first_attack_position = slot->attack_position;
        } else if (slot->party_id == second_party_id) {
            second_attack_position = slot->attack_position;
        }
    }
    PartyAttackPosSet(first_party_id, second_attack_position);
    PartyAttackPosSet(second_party_id, first_attack_position);
}

/*
 * PartyTimeLimitCheck (main 0x002b6868, 88 B): clamps an accumulated
 * party-time record shaped like XglClock (the same day/hour/minute/second
 * breakdown xglClockUInt2DayTime writes) to a maximum of 5 days, 3 hours,
 * 59 minutes and 59 seconds.
 *
 * Evidence: PartyTimeUpDate (0x002b6954) refreshes the static party-time
 * record at 0x00491818 with xglClockUInt2DayTime/xglClockDayTime2UInt (an
 * 8-byte ldl/ldr -> sdl/sdr copy) and then calls this function; SeisanTimeEx
 * (0x0029fc44) builds a matching stack record with
 * xglClockUInt2DayTime(sp, quotient) and passes the same record on to
 * PartyTimeDispChange. Both callers discard the returned flag.
 *
 * Only day/hour/minute/second are read or written; status, month and year
 * are never accessed by this function.
 */
int PartyTimeLimitCheck(XglClock *elapsed)
{
    unsigned int day = elapsed->day;

    if (day >= 6u || (day >= 5u && elapsed->hour >= 4u)) {
        elapsed->minute = 59;
        elapsed->day = 5;
        elapsed->hour = 3;
        elapsed->second = 59;
        return 1;
    }
    return 0;
}

void PartyTimeInit(void)
{
    xglClockUInt2DayTime((XglClock *)(SaveData + 0x68), 0);
    xglClockRead(&_CountTime);
}

/*
 * Refreshes the persistent play-time record (SaveData+0x68, the same
 * offset PartyTimeInit uses to seed it) with the elapsed clock ticks
 * since the last call, mirrors the fresh reading into _CountTime for the
 * next call, clamps the record at the 99-day limit via
 * PartyTimeLimitCheck, and returns the record for the caller
 * (PartyTimePauseStart forwards it unchanged).
 */
XglClock *PartyTimeUpDate(void)
{
    XglClock *play_time = (XglClock *)(SaveData + 0x68);
    XglClock now;
    unsigned int base;
    unsigned int prev;
    unsigned int cur;

    xglClockRead(&now);
    base = xglClockDayTime2UInt(play_time);
    prev = xglClockDayTime2UInt(&_CountTime);
    cur = xglClockDayTime2UInt(&now);
    xglClockUInt2DayTime(play_time, base + (cur - prev));
    _CountTime = now;
    PartyTimeLimitCheck(play_time);
    return play_time;
}

INCLUDE_ASM("asm/main/nonmatchings/party", PartyTimeDispChange);

XglClock *PartyTimePauseStart(void)
{
    return PartyTimeUpDate();
}

void PartyTimePauseEnd(void)
{
    xglClockRead(&_CountTime);
}
