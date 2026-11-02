#ifndef INCLUDE_MAIN_TYA_H
#define INCLUDE_MAIN_TYA_H

#include "shared.h"

/* Functions and data used by other translation units. */
void tyaCaptureEnd(void);
void tyaCaptureStart(const char *name, int mode);

#endif /* INCLUDE_MAIN_TYA_H */
