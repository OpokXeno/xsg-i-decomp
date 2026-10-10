/*
 * TU-local declarations of main/tu173 (src/main/menu_shop.c).
 */

#ifndef SRC_MAIN_MENU_SHOP_H
#define SRC_MAIN_MENU_SHOP_H

typedef struct MenuShopWorkData MenuShopWorkData;

struct MenuShopWorkData {
    unsigned char state;
    unsigned char stateFlags;
    unsigned char nextState;
    unsigned char waitFrames;
    unsigned char unmodeled_04[2];
    unsigned short categoryMask;
    unsigned char unmodeled_08[8];
    unsigned char mode;
    unsigned char category;
    signed char quantity;
    unsigned char quantityAction;
    unsigned char quantityLimit;
    unsigned char canEquip;
    unsigned char sortOption;
    unsigned char noCategories;
    unsigned char unmodeled_18[8];
    int listIndex;
    int itemId;
    int modelId;
    unsigned int modelPending;
    union {
        long long availableMoney;
    };
    unsigned char unmodeled_38[8];
    short unitId;
    short equipmentId;
    signed char equipmentStatus;
    signed char unitCount;
    unsigned char unmodeled_46;
    signed char unitSelection;
    signed char sort_mode_selector;
    unsigned char unmodeled_49[7];
    /* PartyAgwsGet emits at most six IDs; the core stores their low halfwords. */
    short unitIds[6];
};

extern MenuShopWorkData *MenuShopWork;

#endif /* SRC_MAIN_MENU_SHOP_H */
