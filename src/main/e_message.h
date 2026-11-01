#ifndef MAIN_E_MESSAGE_H
#define MAIN_E_MESSAGE_H

#include "main/open_close_main.h"

struct EMessageTextParam;

/* Cursor prefix initialized by eCursolSet in open_close_main.c. */
void eMessageTextChange(struct EMessageTextParam *message, char *text);

#endif
