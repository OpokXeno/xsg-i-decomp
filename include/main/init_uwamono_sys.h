#ifndef INCLUDE_MAIN_INIT_UWAMONO_SYS_H
#define INCLUDE_MAIN_INIT_UWAMONO_SYS_H

#include "shared.h"

int CrossPointUwamono(const Vector4 *position, const Vector4 *previous,
                      float radius, Vector4 *result);

int HitCheckMapUnitPos(const Vector4 *position);

/* Functions and data used by other translation units. */
int HitCheckMapUnitPosSize(const Vector4 *position, float radius);
void UnlockMapUnit(void);
void InitUwamonoSys(void);
void SetHideObject(void *argument);
int HitCheckMapUnit(void *actor);

#endif /* INCLUDE_MAIN_INIT_UWAMONO_SYS_H */
