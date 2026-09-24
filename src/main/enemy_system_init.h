#ifndef ENEMY_SYSTEM_INIT_H
#define ENEMY_SYSTEM_INIT_H

#include "shared.h"

/* GameLoopState is opaque here; only this scalar offset is evidenced
 * (sw zero at 0x002d3e44, absolute-addressed through a lui/sw pair rather
 * than gp-relative). */
typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;

/* Buffer addresses Enemy_SystemInit fills with xglCdReadFile, one per
 * enemy resource file (data\matumoto\enemy.dat, spline.dat, bikkuri.lex,
 * hatena.lex, maru.lex and sikaku.lex in that order). res_get_path.c reads
 * AdrsEnemyPreset back as the buffer Enemy_LoadPreset fills; this TU only
 * ever writes the six, so their pointee type stays unmodelled. */
extern void *AdrsEnemyPreset;
extern void *AdrsEnemySpline;
extern void *AdrsEnemyExclamation;
extern void *AdrsEnemyQuestion;
extern void *AdrsEnemySphere;
extern void *AdrsEnemySquare;

#endif
