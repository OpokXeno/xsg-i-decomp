#ifndef INCLUDE_OV12_RG_ACTOR_MOTID_H
#define INCLUDE_OV12_RG_ACTOR_MOTID_H

/*
 * Motion ids RgActorMotIDGetFromName returns for the names of the motion
 * tables ("STAND", "ADV_F", ...).  "MOT_<n>" names map to id n - 1 and
 * "WMOT_<n>" names to RG_MOT_WEAPON_BASE + n - 1.
 */
#define RG_MOT_STAND 0
#define RG_MOT_ADV_F 1
#define RG_MOT_ADV_FR 2
#define RG_MOT_ADV_R 3
#define RG_MOT_ADV_BR 4
#define RG_MOT_ADV_B 5
#define RG_MOT_ADV_BL 6
#define RG_MOT_ADV_L 7
#define RG_MOT_ADV_FL 8
#define RG_MOT_DASH_F 11
#define RG_MOT_DASH_FR 12
#define RG_MOT_DASH_R 13
#define RG_MOT_DASH_BR 14
#define RG_MOT_DASH_B 15
#define RG_MOT_DASH_BL 16
#define RG_MOT_DASH_L 17
#define RG_MOT_DASH_FL 18
#define RG_MOT_ROLL_R 9
#define RG_MOT_ROLL_L 10
#define RG_MOT_JUMP 19
#define RG_MOT_JUMP_END 75
#define RG_MOT_BREAK 20
#define RG_MOT_SHOT_LEFT_HANDGUN 39
#define RG_MOT_SHOT_RIGHT_HANDGUN 40
#define RG_MOT_DAMAGE_FL 23
#define RG_MOT_DAMAGE_FR 24
#define RG_MOT_DAMAGE_BR 25
#define RG_MOT_DAMAGE_BL 26
#define RG_MOT_ATTACK_LEFT 21
#define RG_MOT_ATTACK_RIGHT 22
#define RG_MOT_DROP_LEFT 31
#define RG_MOT_DROP_RIGHT 32
#define RG_MOT_BRADE_LEFT 43
#define RG_MOT_BRADE_RIGHT 44
#define RG_MOT_POD_LEFT 41
#define RG_MOT_POD_RIGHT 42
#define RG_MOT_GUARD_START 45
#define RG_MOT_GUARD_HIT 46
#define RG_MOT_GUARD_END 47
#define RG_MOT_DRILL_LEFT 48
#define RG_MOT_DRILL_RIGHT 49
#define RG_MOT_HISSATU 78
#define RG_MOT_HISSATU_VX06_HIT 79

/* First id of the weapon motion bank ("WMOT_*"). */
#define RG_MOT_WEAPON_BASE 0x800

/* Id returned for a name that matches no motion ("nul" is accepted silently). */
#define RG_MOT_NONE (-1)

/* Functions and data used by other translation units. */
int RgActorMotIDGetFromName(const char *pszName);

#endif /* INCLUDE_OV12_RG_ACTOR_MOTID_H */
