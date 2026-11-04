#ifndef INCLUDE_OV12_RG_SELECT_ROBOT_H
#define INCLUDE_OV12_RG_SELECT_ROBOT_H

#include "shared.h"

#include "ov12/rg_dispmodel.h"
#include "ov12/rg_select_subcon.h"

struct Actor;
struct RgDrawStudio;

/* Number of resource files one preview actor loads. */
#define RG_SELECT_ROBOT_FILE_MAX 4

/* Longest resource file name, with its terminator. */
#define RG_SELECT_ROBOT_NAME_SIZE 64

/* Weapons shown on one robot. */
#define RG_SELECT_ROBOT_WEAPON_NUM RG_SELECT_WEAPON_SLOTS

/* Values of RgSelectRobot.state. */
enum {
    RG_SELECT_ROBOT_STATE_IDLE,
    RG_SELECT_ROBOT_STATE_LOADING,
    RG_SELECT_ROBOT_STATE_ACTIVE,
    RG_SELECT_ROBOT_STATE_DISPOSING
};

/* What RgSelectRobotSet takes: the character and its three weapons, and
 * whether each weapon is shown. */
struct RgSelectRobotChoice {
    int actorID;
    RgWeaponEssence *weapons[RG_SELECT_ROBOT_WEAPON_NUM];
    int weaponActive[RG_SELECT_ROBOT_WEAPON_NUM];
};

/* The preview of a robot: its actor (or the weapon child of one), the files
 * it loads and the display model of its weapon. */
struct RgSelectRobot {
    int state;
    struct Actor *actor;
    int disposeCounter;
    char names[RG_SELECT_ROBOT_FILE_MAX][RG_SELECT_ROBOT_NAME_SIZE];
    unsigned int nameCount;
    void *buffers[RG_SELECT_ROBOT_FILE_MAX];
    int actorID;
    struct RgSelectRobot *parent;
    int hasWeapons;
    struct RgSelectRobot *weapons[RG_SELECT_ROBOT_WEAPON_NUM];
    float screenPos;
    RgFileSysData *fileData;
    RgDispModel *displayModel;
    int weaponActive[RG_SELECT_ROBOT_WEAPON_NUM];
    unsigned int drawCount;
    float displayYaw;
    float displayPosition;
    RgWeaponEssence *weapon;
    int weaponSlot;
    void *heapBuffer;
};

typedef struct RgSelectRobot RgSelectRobot;

/* Functions and data used by other translation units. */
extern RgSelectRobot *CreateRgSelectRobot(struct RgDrawStudio *pStudio);
extern void DisposeRgSelectRobot(RgSelectRobot *pCont);
extern void RgSelectRobotScreenPos(RgSelectRobot *pCont, float screenPos);
extern void RgSelectRobotSetMode(RgSelectRobot *pCont, int mode);
extern void RgSelectRobotSet(RgSelectRobot *pCont,
                             struct RgSelectRobotChoice *selection);
extern void RgSelectRobotPassTime(RgSelectRobot *pCont, float deltaTime);
extern void RgSelectRobotDisp(RgSelectRobot *pCont);

#endif /* INCLUDE_OV12_RG_SELECT_ROBOT_H */
