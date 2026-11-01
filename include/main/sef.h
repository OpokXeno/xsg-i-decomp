#ifndef INCLUDE_MAIN_SEF_H
#define INCLUDE_MAIN_SEF_H

#include "shared.h"

int sefIsDeadSchduler(unsigned int scheduler_index);
void sefKillEffect(int effect_no);
int sefSearchMapperIndex(int effect_id);
void sefInitEffectCf(void);
void sefExecEffect(void);
int sefLoadEffectCf(int cf_id, int effect_no);

SchedulerState *sefCreateEffectCf(int effectNo, const Vector4 *position,
                                const Vector4 *orientation);

int sefIsFinishEffect2(int effect_no);
void sefSetLoadQue(int effect_no);
int *sefGetLoadQue(void);
unsigned char *sevGetPtAllocator2(int allocator_index);

#endif /* INCLUDE_MAIN_SEF_H */
