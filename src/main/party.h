#ifndef MAIN_GAME_177_PARTY_H
#define MAIN_GAME_177_PARTY_H

#include "shared.h"
#include "main/party.h"
#include "main/data.h"
#include "main/menu_1.h"
#include "main/menu_system.h"
#include "main/sdv.h"
#include "main/ssd_set.h"
#include "main/window_tex_load.h"
#include "main/xgl_cd.h"
#include "main/xgl_dma.h"

/*
 * TU-local declarations of game/177_party (main#177): the private header of
 * the accepted unit core-main-002b56c0 (src/core/main-002b56c0/private.h),
 * copied verbatim below. core-main-002b68e8 (PartyTimeUpDate) has a private.h
 * that only includes xeno/core/types.h, which the TU prelude includes.
 */
/* TU-local declarations for the merged party translation unit
 * (main 0x002b56c0..0x002b6494, one original file: gcc2_compiled. marks
 * 0x002b56c0 and the next file starts at 0x002b69e8).
 *
 * This header is the union of the accepted units' private headers, copied
 * unchanged:
 *   - src/core/main-002b56d0/private.h (PartyDataInit): SaveData region
 *     offsets, party-state offsets and TecInitEntry;
 *   - src/core/main-002b5e58/private.h (PartyAttackerSet, PartyBattleChange,
 *     PartyLeaderSet): the PartyDataPrefix view.
 * Shared prototypes and types come from xeno/core/{types,functions}.h.
 */
/* SaveData regions touched here. Each offset is the one its accessor uses:
 *   PARTY_STATE      0x10078  PartyDataGet(), a 0x188-byte party state
 *   MENU_SAVE_DATA   0x10254  MenuSaveDataGet()
 *   UMN_WORK         0x1025c  UmnMailBoxSet() scans its first 128 signed
 *                             bytes for an empty (-1) mail-box slot
 *   UMN_MAIL_DATA    0x10304  UmnMailDataGet(id) returns entry id (2 bytes)
 *   INIT_BLOCKS      0x15090  three 256-byte blocks only cleared here
 */
#define SAVE_PARTY_STATE      0x10078

#define SAVE_UMN_WORK         0x1025C
#define SAVE_INIT_BLOCKS      0x15090
#define UMN_MAIL_BOX_SLOTS    128
#define UMN_MAIL_DATA_COUNT   128
#define UMN_WORK_SIZE         168

/* Halfword flag inside the UMN work (tyaUmlDispLoad reads SaveData+0x102e2). */
#define UMN_WORK_FLAGS        0x86

/* Party state offsets, as used by the accepted readers:
 * +0x3c  eight technique records (getPartyDataOfs() type 2),
 * +0x140 skill level per character, +0x148 points to the next skill level
 *        per character (SkillCharSkillLvGet(), SkillCharPointPlus()).
 * Skill data +0x17e is the u16 next-level table SkillNextLvGet(level)
 * indexes (lhu 382(level * 2 + table)). */
#define PARTY_STATE_SIZE      0x188

#define PARTY_TEC_RECORDS     0x3C
#define PARTY_SKILL_LEVELS    0x140
#define PARTY_SKILL_POINTS    0x148
#define SKILL_NEXT_LEVEL_TABLE 0x17E

/* Inventory quantity tables: 255 u16 counters each (nm size 0x1fe),
 * the tables dataBoxPtrGet() hands to dataBoxInc/dataBoxDec. */
extern unsigned short itmBox[255];

extern unsigned short wpnBox[255];
extern unsigned short bltBox[255];
extern unsigned short accBox[255];
extern unsigned short evtBox[255];

/* UmnSimulationNo: 4-byte .sdata value at 0x004da814 (sw -21340(gp)). */
extern int UmnSimulationNo;

/*
 * Puts `party_id` into attack slot `slot_index`, with `attack_position` when it
 * is positive. A zero party id empties the slot. Returns -1 when the member is
 * not a recruited friend, 0 otherwise.
 */
int PartyAttackerSet(int slot_index, int party_id, int attack_position);

/* Stores the ids of available friends that are not in the attack line, up
 * to a combined party of six, and returns how many were stored. */
int PartyReserveGet(int *reserve_ids);

int dataCidGet(int chara_id);
int PartyCharNumGet(void);
int PartyAgwsNumGet(void);
int PartyAttackerCheck(int party_id);
void PartyTimeInit(void);

/*
 * Refreshes the persistent play-time record (SaveData+0x68, the same
 * offset PartyTimeInit uses to seed it) with the elapsed clock ticks
 * since the last call, mirrors the fresh reading into _CountTime for the
 * next call, clamps the record at the 99-day limit via
 * PartyTimeLimitCheck, and returns the record for the caller
 * (PartyTimePauseStart forwards it unchanged).
 */
XglClock *PartyTimeUpDate(void);

XglClock *PartyTimePauseStart(void);

#endif /* MAIN_GAME_177_PARTY_H */
