/*
 * TU-local declarations of main/tu173 (src/main/menu_shop.c).
 */

#ifndef SRC_MAIN_MENU_SHOP_H
#define SRC_MAIN_MENU_SHOP_H

typedef struct MenuShopWorkData MenuShopWorkData;

struct MenuShopWorkData {
    unsigned char state;
};

extern MenuShopWorkData *MenuShopWork;

#endif /* SRC_MAIN_MENU_SHOP_H */
