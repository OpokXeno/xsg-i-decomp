#ifndef INCLUDE_MAIN_MENU_MODEL_H
#define INCLUDE_MAIN_MENU_MODEL_H

void MenuModelMemorySet(int state);
void MenuModelMemoryInit(void *base, int size);
void MenuModelMenuMotionLoad(void);
void MenuModelMain(void);
void MenuModelAllBreak(void);
void *MenuModelInit(void *pool);

#endif /* INCLUDE_MAIN_MENU_MODEL_H */
