#ifndef INCLUDE_MAIN_NML_MODEL_SET_H
#define INCLUDE_MAIN_NML_MODEL_SET_H

#include "shared.h"

void nmlModelDirectSend(int mode, const void *data, int count);
void nmlModelSetSortOffset(float offset);
void nmlModelSetGlobalPointLightCol(int index, const Vector4 *color);
void nmlModelSetSpecularOff(int disableSpecular);
void nmlModelSetRenderStatus(int status);
void nmlModelSetClip(int clip);
int nmlModelSetGlobalFogReset(void);
void nmlModelSetFogCancel(void);
void nmlModelSetMapLastEntry(void *map, int blockId);
void nmlModelFlush(void);

#endif /* INCLUDE_MAIN_NML_MODEL_SET_H */
