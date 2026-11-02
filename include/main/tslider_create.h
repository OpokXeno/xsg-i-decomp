#ifndef INCLUDE_MAIN_TSLIDER_CREATE_H
#define INCLUDE_MAIN_TSLIDER_CREATE_H

#include "shared.h"

/* Functions and data used by other translation units. */
void *createItemGetWin(const char *text);
void TMENU_addQuery2(MenuNativeQueryView *menu, char **texts, int count);
void TWIN_initScene(TComponent *window);
void TWIN_initCF(void *window);
void TSLIDER_drawDefault(TComponent *window);

#endif /* INCLUDE_MAIN_TSLIDER_CREATE_H */
