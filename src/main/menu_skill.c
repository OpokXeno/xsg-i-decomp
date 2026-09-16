#include "common.h"
#include "shared.h"
#include "main/party.h"
#include "menu_skill.h"

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", SkillSetLvGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", SkillGetPtGet);

/*
 * Offset of the per-level "points required for next skill level" table inside
 * the loaded skill data block (SkillDataBuf). Evidenced by the original
 * SkillNextLvGet (`lhu $2, 382($4)`, 382 = 0x17e);
 */
#define SKILL_NEXT_LV_TABLE_OFFSET 0x17e

unsigned short SkillNextLvGet(int level)
{
    return ((unsigned short *)(SkillDataBuf + SKILL_NEXT_LV_TABLE_OFFSET))[level];
}

int SkillCharSkillLvGet(int character_id)
{
    unsigned char *party = PartyDataGet();
    PartySkillLevelArray *skill_levels;
    int character_index = (unsigned short)character_id;

    skill_levels = (PartySkillLevelArray *)(party + 0x140);
    if (character_index == 0)
        return 0;
    if (character_index >= 9)
        return 0;
    return skill_levels->level_by_character[character_id - 1];
}

int SkillCharPointPlus(int character_id, int point_delta)
{
    int character_index = (unsigned short)character_id;
    PlayerCharacter *character;
    unsigned char *party_bytes;
    unsigned char *level_by_character;
    PartySkillPoints *points_to_next_level;
    int level;
    int character_points;
    int remaining_points;
    PartySkillPoints next_level_points;

    if (character_index == 0 || character_index >= 9)
        return 0;

    character = (PlayerCharacter *)dataPlChaGet(character_index);
    party_bytes = PartyDataGet();
    character_points = character->skill_points - point_delta;
    if (character_points < 0)
        character_points = 0;
    character->skill_points = character_points;

    level_by_character = party_bytes + character_id + 0x13f;
    if (*level_by_character >= 5)
        return 1;

    points_to_next_level = (PartySkillPoints *)(character_id * 8 + (unsigned int)party_bytes + 0x140);
    remaining_points = *points_to_next_level;
    remaining_points -= point_delta;
    if (remaining_points <= 0) {
        xglSoundEffectNormalID(1, 0);
        level = *level_by_character + 1;
        *level_by_character = level;
        if ((unsigned char)level < 5) {
            next_level_points = SkillNextLvGet(*level_by_character);
            *points_to_next_level = next_level_points;
            if (remaining_points < 0)
                *points_to_next_level = next_level_points - -remaining_points;
        } else {
            *points_to_next_level = 0;
        }
    } else {
        *points_to_next_level = remaining_points;
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillMenuMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillExMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillL1R1Main);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillSetListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillCategoryMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillListChange00);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillListChange01);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkillListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_skill", MenuSkill);
