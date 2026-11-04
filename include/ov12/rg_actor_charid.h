#ifndef INCLUDE_OV12_RG_ACTOR_CHARID_H
#define INCLUDE_OV12_RG_ACTOR_CHARID_H

/*
 * Actor character ids, numbered in the order RgActorNameToCharID tests the
 * model names ("vx01", "ag05", ...).
 */
#define RG_ACTOR_CHAR_VX01 0
#define RG_ACTOR_CHAR_AG05 1
#define RG_ACTOR_CHAR_VX02 2
#define RG_ACTOR_CHAR_AG04 3
#define RG_ACTOR_CHAR_VX06 4
#define RG_ACTOR_CHAR_AG02 5
#define RG_ACTOR_CHAR_HG00 6
#define RG_ACTOR_CHAR_RF01 7
#define RG_ACTOR_CHAR_GLG00 8
#define RG_ACTOR_CHAR_BD00 9
#define RG_ACTOR_CHAR_RF00 10
#define RG_ACTOR_CHAR_MG00 11
#define RG_ACTOR_CHAR_GND00 12
#define RG_ACTOR_CHAR_BD01 13
#define RG_ACTOR_CHAR_HMR00 14
#define RG_ACTOR_CHAR_HMR01 15
#define RG_ACTOR_CHAR_FL00 16
#define RG_ACTOR_CHAR_BDF00 17
#define RG_ACTOR_CHAR_BAM00 18
#define RG_ACTOR_CHAR_MSP00 19
#define RG_ACTOR_CHAR_VSD00 20
#define RG_ACTOR_CHAR_RF02 21
#define RG_ACTOR_CHAR_CB00 22
#define RG_ACTOR_CHAR_BP00 23
#define RG_ACTOR_CHAR_HG01 24

/* Id returned for an unknown character name. */
#define RG_ACTOR_CHAR_NOP (-1)

/* Functions and data used by other translation units. */
int RgActorNameToCharID(const char *pszName);
const char *RgActorCharIDToName(int charID);

#endif /* INCLUDE_OV12_RG_ACTOR_CHARID_H */
