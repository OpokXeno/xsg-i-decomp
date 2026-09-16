/*
 * TU-local declarations of main/tu178 (src/main/menu_skill.c).
 */

#ifndef SRC_MAIN_MENU_SKILL_H
#define SRC_MAIN_MENU_SKILL_H

int SkillCharSkillLvGet(int character_id);

int SkillCharPointPlus(int character_id, int point_delta);

/*
 * The player character's persistent record, recovered head
 *
 * `dataPlChaGet` (ov01 0x00a19210) is the accessor: it multiplies the
 * character id by 0xa8 and indexes `plChaData` (main 0x00425ac0, witnessed
 * size 0xa80 = ten 0xa8-byte entries) from one entry below it, so id 1 is the
 * first entry. `dataPlUnitInit` (0x00a19988) memsets one whole 0xa8-byte entry
 * before filling it in (0x00a19ba8).
 *
 * The decisive witness for the names is the game's own debug page, the
 * character block of `debug_entry` (ov01 0x00a30420..0x00a30560), which reads
 * the entry `dataPlChaGet` just returned and prints each field through
 * `xglFontDebugPrintf` with its own caption in ov01 .rodata:
 *
 *   +0x00 exp            "\x0b EXP      = %8d"  D_00A50DF8, lw  0x00
 *   +0x04 next_exp       "\x0b NEXT EXP = %8d"  D_00A50E08, lw  0x04
 *   +0x08 wait_offset    "\x0b W OFS    = %4d"  D_00A50E18, lh  0x08
 *   +0x0a wait_count     "\x0b W CNT    = %4d"  D_00A50E28, lh  0x0a
 *   +0x0c tech_points    "\x0b TP       = %4d"  D_00A50E38, lw  0x0c
 *   +0x10 ether_points   "\x0b EP       = %4d"  D_00A50E48, lw  0x10
 *   +0x14 skill_points   "\x0b SP       = %4d"  D_00A50E58, lw  0x14
 *   +0x18 growth[0..3]   "\x0b GROW PARA=%4d %4d %4d %4d"  D_00A50E68,
 *   +0x20 growth[4..7]   "\x0b          =%4d %4d %4d %4d"  D_00A50E88,
 *                        lh 0x18/0x1a/0x1c/0x1e and lh 0x20/0x22/0x24/0x26
 *
 * `next_exp` and `growth` are corroborated by dataPlUnitInit, which seeds
 * +0x04 from the experience table at the character's starting level
 * (0x00a19c18) and writes the eight halfwords from +0x18 upwards out of the
 * parameter table in a loop (0x00a19c30..0x00a19c54). `tech_points`,
 * `ether_points` and `skill_points` are the three point pools the item code in
 * ov01 `calc.s` raises and caps at 9999 (0x00a11a28 raises SP, 0x00a11a78 EP,
 * 0x00a11ac8 TP), and `skill_points` is the pool SkillCharPointPlus below
 * spends.
 *
 * The type stops at +0x28. The entry is 0xa8 bytes and the rest of it is not
 * this TU's evidence: +0x28 is the learned-ether bitmap dataEtherLearnGet
 * indexes (0x00a195e0), +0x38 the learned-skill bitmap dataSkillLearnGet
 * indexes (0x00a19700), and dataPlUnitInit writes eight further halfwords
 * 0xc bytes apart from +0x4e to +0xa2 (0x00a19bc8). No member is invented to
 * reach the entry's size.
 */
typedef struct PlayerCharacter {
    int exp;              /* +0x00 */
    int next_exp;         /* +0x04 */
    short wait_offset;    /* +0x08 */
    short wait_count;     /* +0x0a */
    int tech_points;      /* +0x0c */
    int ether_points;     /* +0x10 */
    int skill_points;     /* +0x14 */
    short growth[8];      /* +0x18 */
} PlayerCharacter;

extern void *dataPlChaGet(unsigned int character_id);

/* Populated with the loaded skill table by MenuSkill (INCLUDE_ASM below). */
extern unsigned char *SkillDataBuf;

extern unsigned short SkillNextLvGet(int level);

extern void xglSoundEffectNormalID(int sound_id, int variant);

#endif /* SRC_MAIN_MENU_SKILL_H */
