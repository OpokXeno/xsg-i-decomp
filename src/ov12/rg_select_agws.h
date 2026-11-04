/*
 * TU-local declarations of ov12/tu069 (src/ov12/rg_select_agws.c).
 */

#ifndef SRC_OV12_RG_SELECT_AGWS_H
#define SRC_OV12_RG_SELECT_AGWS_H

#include "ov12/rg_font.h"
#include "ov12/rg_select_agws.h"

/* Number of character positions on the character-select cursor
 * (s_aeCharTbl in rg_select_agws.c). */
#define RG_ACTOR_CHAR_NUM RG_SELECT_AGWS_CHAR_NUM

/* One weapon name slot. */
#define WEP_NAME_SIZE RG_SELECT_AGWS_WEAPON_NAME_SIZE

typedef struct FloatArrow FloatArrow;

/* Two arrow pictures with their positions, and the mask of the visible ones. */
struct FloatArrow {
    struct RgBxxPic *m_apPic[2];
    int m_aiX[2];
    int m_aiY[2];
    unsigned int visibility_mask;
};

typedef struct WinInfo WinInfo;

/* The determination (yes/no) window's picture set; m_nValue0/m_nValue1 are
 * the window position it is drawn at. */
struct WinInfo {
    struct RgBxxPic *m_pTop[2];
    struct RgBxxPic *m_pBtm[2];
    struct RgBxxPic *m_pSide[2];
    struct RgBxxPic *m_pInner;
    struct RgBxxPic *m_pLeft;
    struct RgBxxPic *m_pRight;
    struct RgBxxPic *m_pMaru;
    struct RgBxxPic *m_pBatu;
    int m_nValue0;
    int m_nValue1;
};

/* Character-select cursor state: the cursor position and the mask of the
 * cursor positions that can be chosen. */
typedef struct SelChar {
    int cursor;
    unsigned int selectableMask;
} SelChar;

/* Weapon-selection list for one equip type: the cursor, the weapons offered
 * and how many there are. */
#define WEP_MAX RG_SELECT_WEAPON_MAX

typedef struct SelWep {
    int cursor;
    RgWeaponEssence *list[WEP_MAX];
    unsigned int m_uSize;
} SelWep;

/* Equip-type selection state: the cursor (equip ID), the cursor before the
 * last change, one weapon list per equip type, the character the lists were
 * built for and, per equip type, the mask of the types it conflicts with. */
typedef struct SelType {
    int cursor;
    int oldCursor;
    unsigned char unmodeled_008[8];
    SelWep wepList[RG_EQUIP_TYPE_NUM];
    unsigned char unmodeled_628[8];
    int charID;
    int confrict[RG_EQUIP_TYPE_NUM];
} SelType;

/* The selection screen's working data: mode (character, weapon or leaving),
 * the character and weapon selectors, the saved selection and the player. */
struct SelDat {
    int mode;
    unsigned char unmodeled_004[12];
    SelChar selChar;
    unsigned char unmodeled_018[8];
    SelType selType;
    struct RgSelectAGWSData base;
    int playerMode;
    unsigned char unmodeled_8ac[4];
};

/* Weapon-list display object. */
struct WepListDisp {
    SelType *pSelType;
    SelWep *pSelWep;
    RgFont *font;
    struct RgBxxPic *pFrame;
    struct RgBxxPic *pUpArrow;
    struct RgBxxPic *pDownArrow;
    int originX;
    int originY;
    int topIndex;
};

#endif /* SRC_OV12_RG_SELECT_AGWS_H */
