/*
 * TU-local declarations of main/tu116 (src/main/game.c).
 */

#ifndef SRC_MAIN_GAME_H
#define SRC_MAIN_GAME_H

#include "shared.h"

typedef void (*GameModeCameraCallback)(void);

/*
 * PARTIAL ACCESSED PREFIX of PadData (0xd0-byte object at 0x490d90).
 * GameModeDebugMenu reads only the halfword at +0x2e (masked with 0x100),
 * which the shared PadPrefix/PadDataLayout views (evidenced to +0x2c) do
 * not reach. held/pressed keep the accepted names at +0x28/+0x2a; the
 * +0x2c halfword is not accessed here. Neutral debug_buttons name: only the
 * 0x100 debug-menu exit test is evidenced.
 *
 * This TU uses the partial PadDataDebugLayout view through +0x2e.
 */
typedef struct PadDataDebugLayout {
    u8 _unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 _unmodeled_2c;
    u16 debug_buttons;
} PadDataDebugLayout;

int getScriptFlag(SceneObject object);

int xglSoundSendSwd(void *swd, int bank);
/* The original leaves the sequence result word in v0, including 0xffff
 * when no sequence data is supplied. */
int xglSoundSendSmd2(void *smd, int bank);
int SsdResetSegmentAllocMode(int segment);
int SsdSetSegmentAllocMode(int segment, int mode);
void xglSoundLoadEffect(const char *name, void *segment, int bank);
extern const char D_004BE2B0[9];
extern struct SoundWork SoundWork;
int SsdGetSeqPlayStatus(int sequence);
int SsdGetResultValue(int *value);
void xglSoundSequenceStop2(int channel);
void EnemySound_StopAll(int mode);
extern unsigned char GameCFSoundMenuPurgeFlag;
void *GameResourceGetFreeAddr(void);
const char *RES_GetEnemySeName(int index);
void RES_GetMapEnvSeName(char *name);
void xglSoundSequenceNormal2(int channel, int volume);
extern int UmnSimulationNo;
extern int MenuDrillCall;

#endif /* SRC_MAIN_GAME_H */
