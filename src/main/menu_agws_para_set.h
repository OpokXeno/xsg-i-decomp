#include "main/e_battle_win_open.h"

#ifndef MENU_AGWS_PARA_SET_H
#define MENU_AGWS_PARA_SET_H

typedef struct AgwsTextName {
    const char *name;
    const unsigned char *auxiliary;
} AgwsTextName;

typedef struct AgwsInfoContents {
    short textOffset[2]; /* MenuInfoWindow adds these to the window x and y. */
    unsigned char unmodeled_04[8];
    char *text;
    unsigned char unmodeled_10[0x1B0 - 0x10];
} AgwsInfoContents;


typedef struct AgwsInfoWork {
    unsigned char state;
    unsigned char unmodeled_01;
    unsigned char detail;
    unsigned char unmodeled_03;
    int color;
    WindowDX window;
    unsigned char unmodeled_19c[4];
    AgwsInfoContents contents;
    char text[256];
} AgwsInfoWork;

typedef struct AgwsCharPara {
    short maxHp;
    unsigned char unmodeled_02[0x04 - 0x02];
    unsigned short attack;
    unsigned short phyDefense;
    unsigned char unmodeled_08[0x0A - 0x08];
    unsigned short magDefense;
    unsigned char unmodeled_0c[0x0E - 0x0C];
    unsigned char stat8;
    unsigned char unmodeled_0f[0x1E - 0x0F];
    short stat9;
    unsigned char unmodeled_20[0x5E - 0x20];
    short weapon[3];
} AgwsCharPara;

typedef struct AgwsMenuWork {
    unsigned char unmodeled_00[0x03];
    unsigned char state;
    unsigned char unmodeled_04[0x10 - 0x04];
    signed char weaponSlot[12];
    unsigned char unmodeled_1c[0x21 - 0x1C];
    signed char modelBreak;
    signed char weaponChange;
    unsigned char unmodeled_23[0x26 - 0x23];
    short weaponId;
    short ammoId; /* +0x28 */
    short accessoryId; /* +0x2a */
    short pilotId; /* +0x2c */
    short weaponHandKind; /* +0x2e */
    unsigned char flags;
    unsigned char unmodeled_31[3];
    unsigned char commandState;
    unsigned char unmodeled_35[0x40 - 0x35];
    unsigned char transitionTicks;
    signed char selectedAgws;
    signed char menuSelection;
    unsigned char unmodeled_43[0x50 - 0x43];
    signed char listKind;
    signed char reserveCursor;
    signed char listSelect;
    unsigned char unmodeled_53[0x54 - 0x53];
    signed char weaponHand;
    signed char equipSlot;
    signed char selectedIndex;
    signed char selectedWeapon; /* +0x57 */
    signed char selectedAmmo; /* +0x58 */
    signed char selectedAccessory; /* +0x59 */
    signed char nameRow;
    unsigned char unmodeled_5b[0x64 - 0x5B];
    short chrNo;
    short agwsCount;
    unsigned char unmodeled_68[0x70 - 0x68];
    unsigned char pendingState;
    unsigned char unmodeled_71[0x80 - 0x71];
} AgwsMenuWork;

typedef struct CharacterMenuWorkData {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x12 - 0x04];
    unsigned short weaponId;        /* +0x12 */
    unsigned short bulletId;        /* +0x14 */
    unsigned short accessoryId;     /* +0x16 */
    unsigned char bulletFits;       /* +0x18 */
    signed char leaderLocked;       /* +0x19 */
    unsigned char guestDenied;      /* +0x1A */
    unsigned char unmodeled_1b[0x20 - 0x1B];
    unsigned char swapOutId;        /* +0x20 */
    signed char swapInId;           /* +0x21 */
    signed char modelSlot;          /* +0x22 */
    signed char weaponChanged;      /* +0x23 */
    unsigned char unmodeled_24[0x30 - 0x24];
    signed char partyCount;         /* +0x30 */
    signed char partyMode;          /* +0x31 */
    signed char menuCount;          /* +0x32 */
    unsigned char unmodeled_33[0x35 - 0x33];
    signed char equipSlot;          /* +0x35 */
    unsigned char unmodeled_36[0x40 - 0x36];
    signed char partyCursor;        /* +0x40 */
    signed char commandCursor;      /* +0x41 */
    unsigned char unmodeled_42;
    signed char listSelect;         /* +0x43 */
    signed char formationColumn;    /* +0x44 */
    signed char formationRow;       /* +0x45 */
    signed char formationSlot;      /* +0x46 */
    signed char unknown_47;
    unsigned char wait;             /* +0x48 */
    signed char menuCursor;         /* +0x49 */
    signed char equipKind;          /* +0x4A */
    signed char unknown_4b[2];
    unsigned char unmodeled_4d[0x4F - 0x4D];
    signed char accessoryList;      /* +0x4F */
    signed char reserveCount;       /* +0x50 */
    signed char reserveCursor;      /* +0x51 */
    signed char unknown_52[4];
    unsigned char unmodeled_56[0x60 - 0x56];
    short chrNo; /* +0x60: one-based character id */
    unsigned char unmodeled_62[0x79 - 0x62];
    unsigned char flags;            /* +0x79 */
    signed char paraIndex;          /* +0x7A */
    signed char paraUpCount;        /* +0x7B */
    signed char confirmCursor;      /* +0x7C */
    unsigned char unmodeled_7d;
    unsigned short paraCost;        /* +0x7E */
} CharacterMenuWorkData;

typedef struct MenuItemWorkData {
    u8 unmodeled_00[3];
    u8 mode;
    u8 unmodeled_04[0x20 - 0x04];
    u8 flags;
    u8 nextMode;
    u8 busy;
    u8 partyCount;
    signed char selectionMode;
    u8 unmodeled_25[0x2E - 0x25];
    short selectedItem;
    signed char listTab;
    signed char sortChoices[8]; /* +0x31 */
    u8 unmodeled_39[0x40 - 0x39];
    signed char tabChangeDirection;
    signed char scrollDir;
    u8 unmodeled_42[0x50 - 0x42];
    int selectDir;
    u8 unmodeled_54[0x62 - 0x54];
    signed char agwsMode;
    signed char selectedAgws;
    signed char agwsCount;
    u8 unmodeled_65[0x70 - 0x65];
    signed char selectedParty;
    u8 unmodeled_71; /* +0x65 */
    signed char sortSelection; /* +0x72 */
    u8 unmodeled_73;
    u8 segmentSelection; /* +0x74 */
} MenuItemWorkData;

typedef struct MenuEtherWork {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x10 - 0x04];
    signed char charDenied;         /* +0x10 */
    unsigned char unmodeled_11[0x20 - 0x11];
    unsigned char flags;             /* +0x20; bit 0 rebuilds ether windows */
    unsigned char nextState;         /* +0x21 */
    unsigned char unmodeled_22[0x2A - 0x22];
    unsigned char wait;              /* +0x2A */
    unsigned char unmodeled_2b[0x30 - 0x2B];
    signed char commandCursor;       /* +0x30 */
    signed char menuCursor;          /* +0x31 */
    signed char slotCursor;          /* +0x32 */
    signed char slotCount;           /* +0x33 */
    signed char confirmCursor;       /* +0x34 */
    unsigned char unmodeled_35[0x40 - 0x35];
    signed char chrNo;               /* +0x40 */
    signed char partyCount;          /* +0x41 */
    signed char partyCursor;         /* +0x42 */
    signed char targetCursor;        /* +0x43 */
    signed char etherId;             /* +0x44 */
    signed char slotEther;           /* +0x45 */
    signed char targetChrNo;         /* +0x46 */
    signed char useType;             /* +0x47 */
    signed char targetCount;         /* +0x48 */
    unsigned char unmodeled_49[0x50 - 0x49];
    union {
        int listSelect;              /* +0x50 */
        unsigned char listSelectBytes[4];
    };
    unsigned char unmodeled_54[0x60 - 0x54];
    unsigned char targetList[8];     /* +0x60 */
} MenuEtherWork;

/* Mutually exclusive screen views of the original 0x80-byte MenuWork object. */
typedef union MenuWorkData {
    CharacterMenuWorkData character;
    MenuItemWorkData item;
    MenuEtherWork ether;
    AgwsMenuWork agws;
} MenuWorkData;

/* Private diagnostic layout checks; these do not define storage. */
typedef char MenuWorkCharacterExtent[(sizeof(CharacterMenuWorkData) == 0x80) ? 1 : -1];
typedef char MenuWorkItemExtent[(sizeof(MenuItemWorkData) == 0x78) ? 1 : -1];
typedef char MenuWorkEtherExtent[(sizeof(MenuEtherWork) == 0x68) ? 1 : -1];
typedef char MenuWorkAgwsExtent[(sizeof(AgwsMenuWork) == 0x80) ? 1 : -1];
typedef char MenuWorkOwnerExtent[(sizeof(MenuWorkData) == 0x80) ? 1 : -1];
typedef char MenuWorkOwnerAlignment[(__alignof__(MenuWorkData) == 4) ? 1 : -1];

typedef struct WeaponWaglData {
    unsigned char unmodeled_00[0x12];
    short wagl;
    unsigned char unmodeled_14[0x1A - 0x14];
    short defaultAmmoId; /* +0x1a */
} WeaponWaglData;

typedef struct AgwsUnitOrg {
    unsigned char unmodeled_00[0x34];
    short hp;
    unsigned char unmodeled_36[0x54 - 0x36];
    short agwsId;
    unsigned char unmodeled_56[0x5A - 0x56]; /* +0x56 */
    signed char hand[3]; /* +0x5a */
    unsigned char unmodeled_5d; /* +0x5d */
    short weaponId[3]; /* +0x5e */
    short accessoryId[3]; /* +0x64 */
    unsigned short ammoId[3]; /* +0x6a */
} AgwsUnitOrg;

typedef struct AgwsParaDisplay {
    short value[7];
} AgwsParaDisplay;

enum {
    AGWS_PARA_MAX_HP,
    AGWS_PARA_ATTACK,
    AGWS_PARA_PHY_DEFENSE,
    AGWS_PARA_MAG_DEFENSE,
    AGWS_PARA_STAT8,
    AGWS_PARA_WAGL,
    AGWS_PARA_STAT9
};

#define AGWS_STATUS_START 0
#define AGWS_STATUS_SLIDE 2
#define AGWS_STATUS_COLOR 0x00FFFFF0
#define AGWS_STATUS_X_OUT (-512)
#define AGWS_STATUS_X_IN 16

#define AGWS_PAS_START 0
#define AGWS_PAS_SLIDE 2
#define AGWS_PAS_COLOR 0x00FFFFF0
#define AGWS_PAS_WIDTH 0x110
#define AGWS_PAS_X_IN (-0x10)
#define AGWS_PAS_X_OUT (-AGWS_PAS_WIDTH)
#define AGWS_PAS_TAB_PARKED 0x120
#define AGWS_PAS_TAB_LIMIT 0x100
#define AGWS_PAS_TAB_GAP 0x20
#define AGWS_PAS_TAB_SLOT 5
#define AGWS_PAS_TAB_AMMO 11
#define AGWS_PAS_TAB_ACCESSORY 12
#define PAD_L1 0x0004
#define PAD_R1 0x0008

typedef struct AgwsListCursor {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int depth;
    unsigned char unmodeled_0c[0x28 - 0x0C];
} AgwsListCursor;

typedef struct AgwsListWork {
    unsigned char state;
    unsigned char visible;
    unsigned char selectedRow;
    unsigned char unmodeled_03;
    int color;
    ETagFontMainData tag[8];
    AgwsListCursor cursor;
    WindowSPCursor window[4];       /* AgwsWinSP[0..3] */
} AgwsListWork;

typedef union AgwsCameraPreset {
    Vector4 vectors[2];
    u64 doublewords[4];
} AgwsCameraPreset;

typedef struct AgwsSprite {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int work;
    unsigned char unmodeled_0c[0x28 - 0x0C];
} AgwsSprite;


typedef struct AgwsFaceWork {
    unsigned char state;
    unsigned char visible;
    unsigned char unmodeled_02[2];
    int color;
    WindowDX window;
    AgwsSprite face;
} AgwsFaceWork;

typedef struct AgwsSwitchWork {
    unsigned char state;
    unsigned char unmodeled_01;
    signed char slideOffset[2];
    int spriteWork;
    AgwsSprite sprite[2];
} AgwsSwitchWork;

/*
 * Partial view of MenuModelUnit (include/main/menu_model.h): the agws unit.
 * Named unions with MenuModelUnit change this TU's .text (GCC 2.96 union
 * accesses), so the agws view stays separate.
 */
typedef struct MenuModelUnitAgwsView {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char active;
    unsigned char unmodeled_12[1];
    unsigned char state;
    struct AgwsModelOrg *org;
    unsigned char unmodeled_18[0x20 - 0x18];
    struct AgwsModelActor *actor;
} MenuModelUnitAgwsView;

typedef struct AgwsSwitchSlideStep {
    signed char side[2];
} AgwsSwitchSlideStep;

typedef struct AgwsModelTransform {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x60 - 0x30];
    float fade;
} AgwsModelTransform;

typedef struct AgwsModelActor {
    unsigned char unmodeled_00[0xC0];
    AgwsModelTransform transform;
} AgwsModelActor;

typedef struct AgwsModelOrg {
    unsigned char unmodeled_00[0x5A];
    signed char hand[3];
    unsigned char unmodeled_5d[0x5E - 0x5D];
    short weaponId[3];
} AgwsModelOrg;

typedef union AgwsModelPositions {
    Vector4 position[7];
    u64 doublewords[14];
} AgwsModelPositions;

typedef union AgwsCameraTargets {
    float value[2][4];
    u64 doublewords[4];
} AgwsCameraTargets;



typedef struct AgwsStatusWork {
    unsigned char state;
    unsigned char mask;
    signed char selectNo;
    unsigned char unmodeled_03;
    int color;
    WindowDX window;
    ETagFontMainData label[7];
    unsigned char unmodeled_27c[0x29C - 0x27C];
    ENumberMainData number[7];
} AgwsStatusWork;


typedef struct AgwsPasBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} AgwsPasBox;

typedef struct AgwsPasWork {
    unsigned char state;
    unsigned char mask;
    signed char selectNo;
    unsigned char unmodeled_03;
    int color;
    WindowDX window;
    EMessageParam message[17];
    unsigned char unmodeled_620[0x6EC - 0x620];
    AgwsPasBox box;
    unsigned char unmodeled_6f8[0x700 - 0x6F8];
    unsigned char callbackWork[0x770 - 0x700];
} AgwsPasWork;

extern MenuWorkData MenuWork;
extern AgwsParaDisplay MenuAgwsPara;
extern AgwsParaDisplay MenuAgwsPara2;
extern AgwsListWork *AgwsList;
extern unsigned char MenuKeepSelect[];
extern AgwsFaceWork *AgwsFace;
extern AgwsSwitchWork *AgwsSwitch;
extern AgwsStatusWork *AgwsStatus;
extern AgwsPasWork *AgwsPas;
extern PadPrefix PadData;
extern const char pilotListTitleText[];
extern const AgwsCameraPreset cameraSetPreset;
extern const AgwsCameraTargets cameraMoveTargets;
extern const AgwsModelPositions modelPositionTable;
extern const DoublewordAlignedVector4 modelUnitScaleVector;
extern AgwsSwitchSlideStep switchSlideStepTable[];

extern WeaponWaglData *func_A1A3D8(int weaponId);
extern int MenuRWeaponCheck2(int weaponId);
extern AgwsUnitOrg *func_A191C0(int chrNo);
extern AgwsCharPara *func_00A11108(int chrNo, int *attack, int *defense);
int *MenuSortAddrGet(int listIndex);
MenuListEntry *MenuListGet(int listIndex);
void MenuListMake(int listIndex, int sortType);
int MenuSortCheck(int listIndex);
void MenuSortSet(int listIndex, int type, int order);
int MenuBulletCheck(int weaponId, int ammoId);
int MenuAccessoryEquipCheck(int chrNo, int accessoryId, int slot);
int MenuWeaponEquipPosCheck(int chrNo, int weaponId, int position, int hand);


void xglCameraInit(StudioCamera *camera);
void MoveSlide(short *current, short *target, float rate);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);


int MenuSortGet(int listIndex, int index);

int MenuPasLengthGet(const char *text);
void MenuPasWindow(WindowDX *window, void *arg);
void endPrintExtFunc(int color, int id, void *data);
void MenuModelUnitBreak(MenuModelUnitAgwsView *unit);
void MenuModelControl2(MenuModelUnitAgwsView *unit, int mode);
void MenuModelUnitOpen(MenuModelUnitAgwsView *unit, int drawType);
int MenuModelWeaponCreate(MenuModelUnitAgwsView *unit, int drawType, int weaponIndex);
int MenuEquipStealMaskCheck(void);

typedef struct AgwsSelectContents {
    unsigned char unmodeled_00[2];
    unsigned short mode;
    int selectedIndex;
    const char *text;
    const char *annotation;
    unsigned char unmodeled_10[0x5F8 - 0x10];
} AgwsSelectContents;

typedef union AgwsSelectCallbackStorage {
    void (*function)(void *window, void *contents);
    unsigned char bytes[sizeof(void (*)(void *, void *))];
} AgwsSelectCallbackStorage;

typedef union AgwsSelectContextStorage {
    struct AgwsSelectContents *contents;
    unsigned char bytes[sizeof(struct AgwsSelectContents *)];
} AgwsSelectContextStorage;

typedef struct AgwsSelectWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    char *title;
    signed char state;
    unsigned char unmodeled_11[3];
    AgwsSelectCallbackStorage callback;
    AgwsSelectContextStorage callbackArg;
    unsigned char unmodeled_1c[0x194 - 0x1C];
} AgwsSelectWindow;

typedef struct AgwsSelectWork {
    unsigned char state;
    unsigned char visible;
    signed char selectNo;
    unsigned char unmodeled_03;
    int color;
    AgwsSelectWindow window[4];
    AgwsSelectContents contents[4];
} AgwsSelectWork;

extern AgwsSelectWork *AgwsSelect;

#endif
