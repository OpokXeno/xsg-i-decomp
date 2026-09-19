/*
 * TU-local declarations of ov12/tu008 (src/ov12/rg_robot_spec.c).
 */

#ifndef SRC_OV12_RG_ROBOT_SPEC_H
#define SRC_OV12_RG_ROBOT_SPEC_H

#include "shared.h"

/*
 * Parser index for the module's auto-homing text field (_AnaAutoHoming,
 * 0x00a0d2e8, still INCLUDE_ASM); -1 means no auto-homing entry is active.
 * .data is still scaffold-owned.
 */
extern int s_nAnaAutoHomingID;

#endif /* SRC_OV12_RG_ROBOT_SPEC_H */
