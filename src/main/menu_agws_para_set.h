#ifndef MENU_AGWS_PARA_SET_H
#define MENU_AGWS_PARA_SET_H

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
    unsigned char unmodeled_28[0x2E - 0x28];
    unsigned char weaponHandKind;
    unsigned char unmodeled_2f[0x30 - 0x2F];
    unsigned char flags;
    unsigned char unmodeled_31[0x51 - 0x31];
    signed char reserveCursor;
    signed char listSelect;
    unsigned char unmodeled_53[0x54 - 0x53];
    signed char weaponHand;
    signed char equipSlot;
    signed char selectedIndex;
    unsigned char unmodeled_57[0x64 - 0x57];
    short chrNo;
    unsigned char unmodeled_66[0x80 - 0x66];
} AgwsMenuWork;

typedef struct WeaponWaglData {
    unsigned char unmodeled_00[0x12];
    short wagl;
} WeaponWaglData;

typedef struct AgwsUnitOrg {
    unsigned char unmodeled_00[0x34];
    short hp;
    unsigned char unmodeled_36[0x54 - 0x36];
    short agwsId;
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

typedef struct AgwsListRow {
    unsigned char unmodeled_00[0x08];
    unsigned char flag;
    unsigned char equipped;
    unsigned char unmodeled_0a[0x0C - 0x0A];
} AgwsListRow;

typedef struct AgwsListSPWindow {
    unsigned char state;
    unsigned char rowCount;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    short width;
    short height;
    int flags;
    unsigned char columns;
    unsigned char rows;
    unsigned char unmodeled_16[0x1C - 0x16];
    AgwsListRow *items;
} AgwsListSPWindow;

typedef struct AgwsListTag {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int depth;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char unmodeled_0f[0x20 - 0x0F];
} AgwsListTag;

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
    AgwsListTag tag[8];
    AgwsListCursor cursor;
    AgwsListSPWindow window;
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

typedef struct AgwsWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0x10 - 0x0C];
    unsigned char state;
    unsigned char unmodeled_11[0x14 - 0x11];
} AgwsWindow;

typedef struct AgwsFaceWork {
    unsigned char state;
    unsigned char visible;
    unsigned char unmodeled_02[2];
    int color;
    AgwsWindow window;
    unsigned char unmodeled_1c[0x19C - 0x1C];
    AgwsSprite face;
} AgwsFaceWork;

typedef struct AgwsSwitchWork {
    unsigned char state;
    unsigned char unmodeled_01;
    signed char slideOffset[2];
    int spriteWork;
    AgwsSprite sprite[2];
} AgwsSwitchWork;

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

typedef struct AgwsModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char active;
    unsigned char unmodeled_12[1];
    unsigned char state;
    AgwsModelOrg *org;
    unsigned char unmodeled_18[0x20 - 0x18];
    AgwsModelActor *actor;
} AgwsModelUnit;

typedef union AgwsModelPositions {
    Vector4 position[7];
    u64 doublewords[14];
} AgwsModelPositions;

typedef union AgwsModelVector {
    Vector4 vector;
    u64 doublewords[2];
} AgwsModelVector;

typedef union AgwsCameraTargets {
    float value[2][4];
    u64 doublewords[4];
} AgwsCameraTargets;

typedef struct AgwsStatusWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state;
    unsigned char unmodeled_11[0x194 - 0x11];
} AgwsStatusWindow;

typedef struct AgwsStatusLabel {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    signed char rgba[4];
    unsigned char unmodeled_10[0x20 - 0x10];
} AgwsStatusLabel;

typedef struct AgwsStatusNumber {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[0x0E - 0x08];
    unsigned char format;
    unsigned char digits;
    unsigned char unmodeled_10[1];
    signed char arrow;
    unsigned char unmodeled_12[1];
    signed char arrowHidden;
    int value;
    unsigned char unmodeled_18[0x90 - 0x18];
} AgwsStatusNumber;

typedef struct AgwsStatusWork {
    unsigned char state;
    unsigned char mask;
    signed char selectNo;
    unsigned char unmodeled_03;
    int color;
    AgwsStatusWindow window;
    AgwsStatusLabel label[7];
    unsigned char unmodeled_27c[0x29C - 0x27C];
    AgwsStatusNumber number[7];
} AgwsStatusWork;

typedef struct AgwsPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state;
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(void);
    void *callbackArg;
    unsigned char unmodeled_1c[0x194 - 0x1C];
} AgwsPasWindow;

typedef struct AgwsPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x44 - 0x0C];
} AgwsPasMessage;

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
    AgwsPasWindow window;
    AgwsPasMessage message[17];
    unsigned char unmodeled_620[0x6EC - 0x620];
    AgwsPasBox box;
    unsigned char unmodeled_6f8[0x700 - 0x6F8];
    unsigned char callbackWork[0x770 - 0x700];
} AgwsPasWork;

extern AgwsMenuWork MenuWork;
extern AgwsParaDisplay MenuAgwsPara;
extern AgwsParaDisplay MenuAgwsPara2;
extern AgwsListWork *AgwsList;
extern unsigned char MenuKeepSelect[];
extern AgwsFaceWork *AgwsFace;
extern AgwsSwitchWork *AgwsSwitch;
extern AgwsStatusWork *AgwsStatus;
extern AgwsPasWork *AgwsPas;
extern PadPrefix PadData;
extern const unsigned char D_004C6F38[];
extern const AgwsCameraPreset D_004C7190;
extern const AgwsCameraTargets D_004C71B0;
extern const AgwsModelPositions D_004C71D0;
extern const AgwsModelVector D_004C7240;
extern AgwsSwitchSlideStep D_004DAFE8[];

extern WeaponWaglData *func_A1A3D8(int weaponId);
extern int MenuRWeaponCheck2(int weaponId);
extern AgwsUnitOrg *func_A191C0(int chrNo);
extern AgwsCharPara *func_00A11108(int chrNo, int *attack, int *defense);
int *MenuSortAddrGet(int listIndex);
AgwsListRow *MenuListGet(int listIndex);
void MenuListMake(int listIndex, int sortType);
int MenuSortCheck(int listIndex);
void MenuSortSet(int listIndex, int type, int order);
int MenuBulletCheck(int weaponId, int ammoId);
int MenuAccessoryEquipCheck(int chrNo, int accessoryId, int slot);
int MenuWeaponEquipPosCheck(int chrNo, int weaponId, int position, int hand);
void WindowSPItemChange(AgwsListSPWindow *window);
void WindowSPSetSelect(AgwsListSPWindow *window, unsigned char *savedSelection);
void xglCameraInit(StudioCamera *camera);
void MoveSlide(short *current, short *target, float rate);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
void WindowDXSet(void *window);
void WindowDXMain(void *window);
int MenuSortGet(int listIndex, int index);
int MenuFaceEpidGet(short sortEntry, int kind);
void eNumberSet(void *number, int mode);
void eNumberMain(void *number);
void eTagFontSet(void *tag, const char *text);
void eTagFontMain(void *tag);
int MenuPasLengthGet(const char *text);
void MenuPasWindow(void);
void eMessageSet(void *message, const char *text);
void eMessageMain(void *message);
void endPrintExtFunc(int color, int id, void *data);
void MenuModelUnitBreak(AgwsModelUnit *unit);
void MenuModelControl2(AgwsModelUnit *unit, int mode);
void MenuModelUnitOpen(AgwsModelUnit *unit, int drawType);
int MenuModelWeaponCreate(AgwsModelUnit *unit, int drawType, int weaponIndex);
int MenuEquipStealMaskCheck(void);

#endif
