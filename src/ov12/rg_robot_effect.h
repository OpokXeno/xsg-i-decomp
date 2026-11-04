/*
 * TU-local declarations of ov12/tu010 (src/ov12/rg_robot_effect.c). The
 * shared records and functions live in include/ov12/rg_robot_effect.h.
 */

#ifndef SRC_OV12_RG_ROBOT_EFFECT_H
#define SRC_OV12_RG_ROBOT_EFFECT_H

#include "ov12/rg_robot_effect.h"

/*
 * _StopEffect (0x00a0e2a0) has local binding in the original symbol table.
 * RgRobotEffectTermJet calls it on the jet item (&effect->jet).
 */
static void _StopEffect(RgRobotEffectItem *item);

#endif /* SRC_OV12_RG_ROBOT_EFFECT_H */
