/*
 * TU-local declarations of main/tu173 (src/main/menu_shop.c).
 */

#ifndef SRC_MAIN_MENU_SHOP_H
#define SRC_MAIN_MENU_SHOP_H

typedef struct MenuShopWorkData MenuShopWorkData;

struct MenuShopWorkData {
    unsigned char state;
    unsigned char unmodeled_01[5];
    unsigned short categoryMask;
    unsigned char unmodeled_08[8];
    unsigned char mode;
    unsigned char category;
    unsigned char unmodeled_12[5];
    unsigned char noCategories;
    unsigned char unmodeled_18[8];
    int listIndex;
};

extern MenuShopWorkData *MenuShopWork;

#endif /* SRC_MAIN_MENU_SHOP_H */
