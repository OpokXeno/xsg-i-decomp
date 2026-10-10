#include "common.h"

#include "shared.h"

#include "main/xgl_task.h"

#include "main/xgl_2.h"

#include "create_evt_item_get_task.h"

/* Event-item IDs index a 0x80-byte formatted name slot. */

unsigned char EvtItemTbl[255 * 0x80] = { 0 };

const char evt_item_format_special[24] =
    "Obtained \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80.";

const char evt_item_format_normal[16] = "Obtained %s.";

extern int dataBoxInc(int category, int id);

extern char *GetItemName(int category, int id);

extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *),
                                       XglTaskPrefix *entry);

/* One MapUnit record view shared by the initialization and item-update
 * functions below.  Its stride and all named members are supported by their
 * combined accesses; gaps remain unmodelled. */

typedef struct ItemUnitInfo {
    unsigned char unmodeled_00[2];
    unsigned short itemNo;
    signed char signal;
    signed char unitNo;
    unsigned char unmodeled_06[0x27];
    signed char rsrcIndex;
    unsigned char unmodeled_2e[2];
    short item_get_method;
    unsigned char unmodeled_32[2];
    int seId;
} ItemUnitInfo;

typedef union ItemMapVectorBlock {
    Vector4 vector;
    unsigned long long words[2];
} ItemMapVectorBlock;

typedef struct ItemMapUnit {
    unsigned int flags;
    void (*update)(struct ItemMapUnit *);
    void (*draw)(struct ItemMapUnit *);
    unsigned char unmodeled_0c[4];
    ItemMapVectorBlock position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x10];
    Matrix4 matrix;
    Matrix4 *matrixPtr;
    unsigned char unmodeled_84[0x1c];
    unsigned char seChannel;
    unsigned char actionNo;
    unsigned char actionSub;
    unsigned char unmodeled_a3;
    unsigned short serial;
    unsigned char unmodeled_a6[2];
    short sequenceNo;
    unsigned char unmodeled_aa[0xe0 - 0xaa];
    void *model;
    const char *texture;
    int effectCf[3];
    int ownerNo;
    unsigned char unmodeled_f8[0xa8];
    ItemUnitInfo info;
} ItemMapUnit;

typedef struct MapDisplayRecord {
    unsigned char unmodeled_00[4];
    void *model;
} MapDisplayRecord;

extern void MAP_updateUnitSpecialSymbol(ItemMapUnit *unit);

extern void MAP_updateUnitItemSymbol(ItemMapUnit *unit);

extern void SetItemSymbolRsrc(ItemMapUnit *unit);

extern int xglFlagsGet1(int bitOffset);

extern int printf(const char *format, ...);

extern void sefDeleteEffectCf(int effectId);

static const char D_004CA5D0[32] = "ItemSymbol serial error!! %d\n";

typedef struct ItemOwnerUnit {
    unsigned char unmodeled_00[0x10];
    ItemMapVectorBlock position;
    unsigned char unmodeled_20[0x82];
    unsigned char actionSub;
    unsigned char unmodeled_a3;
    short serial;
    unsigned char unmodeled_a6[0xfe];
    unsigned char item_box_state;
    unsigned char unmodeled_1a5[0x15b];
} ItemOwnerUnit;

extern ItemOwnerUnit MapUnit[64];

extern void nmlModelSetPartsVisible(void *model, short partsNo, int visible);

extern void DrawActiveCursol(ItemMapUnit *unit);

float D_004D7F7C = 0.05f;

extern signed char printflg;

static const char D_004CA650[48] = "unit koware delete id=0x%x serial=%d\n";

extern void nmlModelEntry(int entry);

extern void nmlModelSetClip(int clip);

extern void nmlModelSetPlace(Matrix4 matrix);

extern void nmlModelSetTexture(const char *texture);

/*
 * Only the fields MAP_drawUnitItemBox reads/writes are evidenced: the
 * embedded placement matrix at +0x40 (addiu $4,$16,64 into nmlModelSetPlace),
 * the rotation-X angle at +0xA8 (lh 0xA8, divided by 9.0 before
 * xglMatrixStackRotX), the embedded translation vector at +0xC0 (addiu
 * $17,$16,192, passed to xglMatrixStackTrans), the model entry index at
 * +0xE0 (lw 0xE0 into nmlModelEntry), the texture pointer at +0xE4 (lw 0xE4
 * into nmlModelSetTexture) and the saved-matrix pointer at +0x230 (lw 0x230
 * into xglMatrixStackSave). The spans between them are left unmodelled.
 */

typedef struct UnitItemBox {
    unsigned char unmodeled_00[0x40];  /* +0x00 */
    Matrix4 place;                      /* +0x40 */
    unsigned char unmodeled_80[0x28];  /* +0x80 */
    short angle;                        /* +0xA8 */
    unsigned char unmodeled_aa[0x16];  /* +0xAA */
    Vector4 translation;                /* +0xC0 */
    unsigned char unmodeled_d0[0x10];  /* +0xD0 */
    int model_entry;                    /* +0xE0 */
    const char *texture;                /* +0xE4 */
    unsigned char unmodeled_e8[0x148]; /* +0xE8 */
    Matrix4 *saved_matrix;              /* +0x230 */
} UnitItemBox;

/* Definitions follow their callers so references remain address-based. */

/* Event-item IDs index a 0x80-byte formatted name slot. */

static void taskItemGet(XglTaskPrefix *entry);

/* One MapUnit record view shared by the initialization and item-update
 * functions below.  Its stride and all named members are supported by their
 * combined accesses; gaps remain unmodelled. */

/* Position records are copied by two aligned doubleword loads and stores. */


typedef struct ItemBoxRecord {
    unsigned char category;
    unsigned char item;
    unsigned char count;
    unsigned char option;
    int money;
} ItemBoxRecord;

typedef struct ItemNameRecord {
    char *name;
} ItemNameRecord;

extern ItemNameRecord *func_A2C5F8(int item);

extern ItemNameRecord *func_A2C6E8(int item);

extern ItemNameRecord *func_A2C738(int item);

extern ItemNameRecord *func_A2C698(int item);

extern void UwamonoCommonFunc(void *unit);

extern float CheckDist3D(const void *first, const void *second);

extern void xglSoundEffectNormalID(int effectId, int channel);

extern void ClearUwamonoEffect(void *unit);

extern void *uwares_tbl[14];

extern const char *xtxres_tbl[14];

extern int xglFlagsSet1(int flag, int value);

extern int dataMoneyBoxInc(int money);

extern int CallMethod_I(const char *method, int value);

extern char *strcat(char *destination, const char *source);

extern const char D_004DB730[];

extern const char D_004DB738[];

extern const char D_004DB740[];

extern const char D_004DB748[];

extern const char D_004DB750[];

extern const char D_004DB758[];

extern const char D_004DB760[];

extern const char D_004DB768[];

extern const char D_004DB770[];

extern const char D_004DB778[];

static char deb_0[5] = "NULL";

static const char *strno[10] = {
    D_004DB778, D_004DB770, D_004DB768, D_004DB760, D_004DB758,
    D_004DB750, D_004DB748, D_004DB740, D_004DB738, D_004DB730
};

ItemBoxRecord ItemBoxTbl[601] = {{0}};

const char D_004CA4D0[16] = "Obtained %d G.";

const char D_004CA508[24] = "Obtained %s \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80.";

const char D_004CA520[16] = "Obtained %s %s.";

const char D_004CA530[32] = "Obtained %s%s \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80";

const char D_004CA550[24] = "Obtained %s%s %s";

const char D_004CA568[16] = "Illegal grp %d\n";

float D_004D7F60 = 0.05f;

float D_004D7F64 = 0.8f;

/* Definitions follow their callers so references remain address-based. */

static void taskItemGet(XglTaskPrefix *entry)
{
    EventItemTask *task = (EventItemTask *)entry;
    ItemBoxRecord *table;
    ItemBoxRecord *item;
    ItemMapUnit *unit;
    ItemUnitInfo *info;
    char message[64];
    int item_id;
    int item_index;
    int count;
    GameLoopState[7] = 0;
    if (task->window_created == 0) {
        memset(message, 0, sizeof(message));
        table = ItemBoxTbl;
        item_index = (short)task->item_no;
        item = &table[item_index];
        task->category = item->category;
        item_id = item->item;
        task->count = item->count;
        task->option = item->option;
        task->money = ItemBoxTbl[item_index].money;
        xglFlagsSet1(item_index + 0x79EC7, 1);
        task->window_created = 1;
        if (task->money != 0) {
            sprintf(message, D_004CA4D0, task->money);
            dataMoneyBoxInc(task->money);
        } else if (task->count == 1) {
            task->item_name = GetItemName(task->category, item_id);
            if (task->category == 10) {
                sprintf(message, evt_item_format_special, task->item_name);
            } else {
                sprintf(message, evt_item_format_normal, task->item_name);
            }
            dataBoxInc(task->category, item_id);
        } else {
            count = task->count;
            if (count > 99) count = 99;
            task->item_name = GetItemName(task->category, item_id);
            if (task->count < 10) {
                if (task->category == 10) {
                    sprintf(message, D_004CA508, strno[count], task->item_name);
                } else {
                    sprintf(message, D_004CA520, strno[count], task->item_name);
                }
            } else if (task->category == 10) {
                sprintf(message, D_004CA530, strno[count / 10], strno[count % 10], task->item_name);
            } else {
                sprintf(message, D_004CA550, strno[count / 10], strno[count % 10], task->item_name);
            }
            if (count > 0) {
                do {
                    count--;
                    dataBoxInc(task->category, item_id);
                } while (count != 0);
            }
        }
        task->window = createItemGetWin(message);
        GameLoopState[4] |= 0x20000;
    }
    if (task->window->state == 2) {
        unit = task->owner_unit;
        info = &unit->info;
        GameLoopState[4] &= ~0x20000;
        if (info->item_get_method != -1) {
            CallMethod_I("itemget", info->item_get_method);
        }
        if (info->unitNo != -1) {
            MapUnit[info->unitNo].actionSub = 5;
        }
        xglTaskWaitRemove(entry);
    }
}

static void taskEvtItemGet(EventItemTask *task)
{
    char message[64];
    GameLoopState[0x1c / sizeof(unsigned int)] = 0;
    if (task->window_created == 0) {
        memset(message, 0, 64);
        task->window_created = 1;
        if (task->category == 10) {
            sprintf(message, evt_item_format_special, task->item_name);
        } else {
            sprintf(message, evt_item_format_normal, task->item_name);
        }
        task->window = createItemGetWin(message);
    }
    if (task->window->state == 2) {
        GameLoopState[0x10 / sizeof(unsigned int)] &= 0xfffdffffu;
        xglTaskWaitRemove(&task->task);
    }
}

int CreateEvtItemGetTask(int item, int count)
{
    XglTaskScheduler *scheduler;
    EventItemTask *task;

    if (dataBoxInc(item, count) == 0) {
        return 0;
    }
    scheduler = (XglTaskScheduler *) GameLoopState[2];
    task = (EventItemTask *) xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *)) taskEvtItemGet,
        scheduler != 0 ? scheduler->active_tail : 0);
    task->zero_word_10 = 0;
    task->zero_word_14 = 0;
    task->category = item;
    task->window_created = 0;
    task->item_name = GetItemName(item, count);
    GameLoopState[4] |= 0x20000;
    return 1;
}

char *GetItemName(int category, int item)
{
    ItemNameRecord *record;
    char *name;
    if (item == 0) {
        return deb_0;
    }
    switch (category) {
    case 0:
        record = func_A2C5F8(item);
        name = deb_0;
        if (record != 0) return record->name;
        return name;
    case 1:
        record = func_A2C6E8(item);
        name = deb_0;
        if (record != 0) return record->name;
        return name;
    case 2:
        record = func_A2C738(item);
        name = deb_0;
        if (record != 0) return record->name;
        return name;
    case 3:
        record = func_A2C698(item);
        name = deb_0;
        if (record != 0) return record->name;
        return name;
    case 10:
        name = dataEvtItmNameGet(item);
        return name != 0 ? name : deb_0;
    default:
        printf(D_004CA568, category);
        return 0;
    }
}

char *dataEvtItmNameGet(int index)
{
    return (char *)EvtItemTbl + index * 0x80 - 0x80;
}

static int Pow(int base, int exponent)
{
    int count = 1;
    int result = base;

    if (count < exponent) {
        count = exponent - 1;
        do {
            count--;
            result *= base;
        } while (count != 0);
    }

    return result;
}

int HexToStr(int value, char *result)
{
    int digits = 1;

    if (value < 0) {
        return 1;
    }
    {
        int exponent = 1;

        while (exponent < 16 && value >= Pow(10, exponent)) {
            digits++;
            exponent++;
        }
        if (digits == 1) {
            strcat(result, strno[value % 10]);
            return 1;
        }
        exponent = digits;
        while (exponent > 1) {
            int power = Pow(10, exponent - 1);
            strcat(result, strno[(value / power) % 10]);
            exponent--;
        }
        strcat(result, strno[value % 10]);
    }
    return digits;
}

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", InitItemBox);

void InitItemSymbol(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;
    int i;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.vector.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixPtr = &unit->matrix;
    if (info->rsrcIndex == -1) {
        info->rsrcIndex = 0;
    }
    unit->update = MAP_updateUnitItemSymbol;
    unit->flags |= 0x10000;
    unit->flags |= 0x10000000;
    if ((unsigned int) unit->serial - 0x7033 < 5) {
        unit->update = MAP_updateUnitSpecialSymbol;
        return;
    }
    if (info->itemNo >= 602) {
        unit->draw = 0;
        unit->update = 0;
        unit->serial = (unsigned short) -1;
        printf(D_004CA5D0, (short) info->itemNo);
        return;
    }
    if (xglFlagsGet1(0x79EC7 + (short) info->itemNo) == 1) {
        unit->serial = (unsigned short) -1;
        unit->update = 0;
        for (i = 0; i < 3; i++) {
            if (unit->effectCf[i] != 0) {
                sefDeleteEffectCf(unit->effectCf[i]);
            }
        }
        return;
    }
    SetItemSymbolRsrc(unit);
}

void InitSpecialSymbol(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.vector.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixPtr = &unit->matrix;
    if (info->rsrcIndex == -1) {
        info->rsrcIndex = 0;
    }
    unit->update = MAP_updateUnitSpecialSymbol;
    unit->flags |= 0x10004;
    unit->flags |= 0x10000000;
    SetItemSymbolRsrc(unit);
}

void SetItemSymbolRsrc(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;
    int category;
    int count;
    ItemBoxRecord *item;

    if ((short)unit->serial != 0x7000) {
        switch ((short)unit->serial) {
        case 0x7009:
            unit->model = uwares_tbl[8];
            unit->texture = xtxres_tbl[8];
            return;
        case 0x700a:
            unit->model = uwares_tbl[9];
            unit->texture = xtxres_tbl[9];
            return;
        case 0x700b:
            unit->model = uwares_tbl[10];
            unit->texture = xtxres_tbl[10];
            return;
        case 0x700c:
            unit->model = uwares_tbl[11];
            unit->texture = xtxres_tbl[11];
            return;
        case 0x700d:
            unit->model = uwares_tbl[12];
            unit->texture = xtxres_tbl[12];
            return;
        case 0x700e:
            unit->model = uwares_tbl[13];
            unit->texture = xtxres_tbl[13];
            return;
        default:
            return;
        }
    } else {
        int money = ItemBoxTbl[(short)info->itemNo].money;
        item = &ItemBoxTbl[(short)info->itemNo];
        category = (signed char)ItemBoxTbl[(short)info->itemNo].category;
        count = (signed char)item->count;
        if (money != 0) {
            void *model = uwares_tbl[8];
            unit->serial = 0x7009;
            unit->model = model;
            unit->texture = xtxres_tbl[8];
            return;
        }
        switch (category) {
        case 0:
            if (count < 15) {
                unit->model = uwares_tbl[9]; unit->texture = xtxres_tbl[9];
            } else {
                unit->model = uwares_tbl[10]; unit->texture = xtxres_tbl[10];
            }
            return;
        case 1:
            if (count < 70) {
                unit->model = uwares_tbl[11]; unit->texture = xtxres_tbl[11];
            } else {
                unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            }
            return;
        case 2:
            unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            return;
        case 3:
            if (count < 128) {
                unit->model = uwares_tbl[11]; unit->texture = xtxres_tbl[11];
            } else {
                unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            }
            return;
        case 10:
            unit->model = uwares_tbl[13]; unit->texture = xtxres_tbl[13];
            return;
        default:
            return;
        }
    }
}

void MAP_updateUnitItemBox(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;
    int (*callback)(XglTaskPrefix *);
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    EventItemTask *task;
    ItemOwnerUnit *owner;
    int itemNo;

    unit->flags |= 4;
    UwamonoCommonFunc(unit);
    switch (unit->actionSub) {
    case 0:
        if (info->signal == 1) {
            unit->actionSub = 1;
            xglSoundEffectNormalID(info->seId, 0);
            ClearUwamonoEffect(unit);
        }
        return;
    case 1:
        unit->sequenceNo++;
        callback = (int (*)(XglTaskPrefix *))taskItemGet;
        scheduler = (XglTaskScheduler *)GameLoopState[2];
        entry = scheduler != 0 ? scheduler->active_tail : 0;
        task = (EventItemTask *)xglTaskEntryNext(
            scheduler, callback, entry);
        itemNo = info->itemNo;
        task->zero_word_10 = 0;
        task->zero_word_14 = 0;
        task->owner_unit = unit;
        task->item_no = itemNo;
        task->window_created = 0;
        unit->actionSub = 2;
        if (info->unitNo == -1) {
            break;
        }
        owner = &MapUnit[info->unitNo];
        owner->item_box_state = 1;
        owner->position = unit->position;
        return;
    case 2:
        if (++unit->sequenceNo >= 17) {
            unit->actionSub = 3;
            return;
        }
        break;
    case 3:
        ClearUwamonoEffect(unit);
        break;
    }
}

void MAP_drawUnitItemBox(UnitItemBox *box)
{
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackRotX((float) box->angle / 9.0f);
    xglMatrixStackSave(*box->saved_matrix);
    nmlModelSetTexture(box->texture);
    nmlModelSetPlace(box->place);
    nmlModelSetClip(1);
    nmlModelEntry(box->model_entry);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackSave(*box->saved_matrix);
}

void MAP_updateUnitItemSymbol(ItemMapUnit *unit)
{
    ItemUnitInfo *info;
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    EventItemTask *task;
    int (*callback)(XglTaskPrefix *);

    info = &unit->info;
    unit->rotation.y += D_004D7F60;
    UwamonoCommonFunc(unit);
    switch (unit->actionSub) {
    case 0:
        if (CheckDist3D(&((ItemMapUnit *) GameLoopState[1])->position,
                        &unit->position) < D_004D7F64 &&
            (GameLoopState[4] & 0x20000) == 0) {
            scheduler = (XglTaskScheduler *) GameLoopState[2];
            callback = (int (*)(XglTaskPrefix *)) taskItemGet;
            entry = scheduler != 0 ? scheduler->active_tail : 0;
            task = (EventItemTask *) xglTaskEntryNext(
                scheduler, callback, entry);
            task->item_no = info->itemNo;
            task->zero_word_10 = 0;
            task->window_created = 0;
            task->zero_word_14 = 0;
            task->owner_unit = unit;
            unit->actionSub = 1;
            unit->flags |= 4;
            xglSoundEffectNormalID(6, 0);
            GameLoopState[4] |= 0x20000;
        }
        break;
    case 1:
        unit->update = 0;
        unit->serial = (unsigned short) -1;
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitSpecialSymbol);

void MAP_updateUnitItem(ItemMapUnit *unit)
{
    unsigned short nextSequenceNo;

    if (MapUnit[unit->ownerNo].serial != -1) {
        return;
    }
    unit->flags &= ~4;
    DrawActiveCursol(unit);
    unit->rotation.y = unit->rotation.y + D_004D7F7C;
    switch (unit->actionSub) {
    case 0:
        if (unit->actionNo == 1) {
            unit->actionSub = 1;
        }
        break;
    case 1:
        unit->sequenceNo++;
        nextSequenceNo = unit->sequenceNo;
        if ((nextSequenceNo & 1) == 0) {
            nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 0);
        } else {
            nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 1);
        }
        if (unit->sequenceNo < 4) {
            return;
        }
        if (unit->sequenceNo < 17) {
            return;
        }
        nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 0);
        if (printflg != 0) {
            printf(D_004CA650, (short) unit->serial, unit->seChannel);
        }
        unit->update = 0;
        unit->flags = 0;
        unit->serial = (unsigned short) -1;
        break;
    }
}
