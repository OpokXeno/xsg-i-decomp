#ifndef INCLUDE_MAIN_XGL_HDD_H
#define INCLUDE_MAIN_XGL_HDD_H

int xglHddMount(void);
extern int xglHddActivate(int state);
extern int xglHddMcUmount(void);
extern int xglHddMcLoadMount(void);
extern int xglHddMcLoadCore(void *save);

#endif /* INCLUDE_MAIN_XGL_HDD_H */
