#ifndef INCLUDE_OV12_RG_SELECT_AGWS_H
#define INCLUDE_OV12_RG_SELECT_AGWS_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_select_robot.h"
#include "ov12/rg_select_subcon.h"

/* Number of characters the selection offers, and equip types per character. */
#define RG_SELECT_AGWS_CHAR_NUM RG_SELECT_CHAR_NUM

/* Size of one weapon name slot of the selection data. */
#define RG_SELECT_AGWS_WEAPON_NAME_SIZE 0x20

/* What the selection reads and writes back: the character, the characters
 * that may be chosen, and the name of the weapon chosen per character and
 * equip type (empty when none). 0x248 bytes. */
struct RgSelectAGWSData {
    int selectedCharacter;
    unsigned int availableCharacters;
    char weaponName[RG_SELECT_AGWS_CHAR_NUM][RG_SELECT_WEAPON_SLOTS]
                   [RG_SELECT_AGWS_WEAPON_NAME_SIZE];
};

/* The selection screen's working data, defined by rg_select_agws.c. */
typedef struct SelDat SelDat;

/* The weapon list display object, defined by rg_select_agws.c. */
typedef struct WepListDisp WepListDisp;

typedef struct RgSelectAGWS RgSelectAGWS;

/* The character and weapon selection screen. */
struct RgSelectAGWS {
    int state;
    SelDat *pSelDat;
    RgBxx *pBxx;
    struct RgPicList *pAgwsSelTop;
    struct RgPicList *pAgwsSel;
    struct RgPicList *pWepSelTop;
    struct RgPicList *pWepSel;
    int ofsX;
    int ofsY;
    WepListDisp *pWepListDisp;
    struct XrgPaint2D *paint;
    RgDrawStudio *pStudio;
    RgSelectRobot *pRobot;
};

/* Values returned by RgSelectAGWSIsEnd. */
enum {
    RG_SELECT_AGWS_END_NONE,
    RG_SELECT_AGWS_END_DONE,
    RG_SELECT_AGWS_END_CANCELLED
};

/* Functions and data used by other translation units. */
extern void InitRgSelectAGWSData(struct RgSelectAGWSData *pData);
extern void RgSelectAGWSSetSelectData(RgSelectAGWS *pSel,
                                      struct RgSelectAGWSData *pData);
extern void RgSelectAGWSGetSelectData(RgSelectAGWS *pSel,
                                      struct RgSelectAGWSData *pData);
extern int RgSelectAGWSGetCollectData(RgSelectAGWS *pSel,
                                      RgWeaponEssence **apCollects);
extern RgSelectAGWS *CreateRgSelectAGWS(void);
extern void DisposeRgSelectAGWS(RgSelectAGWS *pSel);
extern void RgSelectAGWSSetMode(RgSelectAGWS *pSel, int mode);
extern int RgSelectAGWSIsEnd(RgSelectAGWS *pSel);
extern int RgSelectAGWSGetCharacter(RgSelectAGWS *pSel);
extern void RgSelectAGWSGetWeapon(
    RgSelectAGWS *pSel, RgWeaponEssence **apCollects);
extern void RgSelectAGWSPassTime(RgSelectAGWS *pSel, float deltaTime);
extern void RgSelectAGWSDisp(RgSelectAGWS *pSel);

#endif /* INCLUDE_OV12_RG_SELECT_AGWS_H */
