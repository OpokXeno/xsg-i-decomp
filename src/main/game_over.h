/*
 * TU-local declarations of main/tu130 (src/main/game_over.c).
 */

#ifndef SRC_MAIN_GAME_OVER_H
#define SRC_MAIN_GAME_OVER_H

extern int copyframe(void);

#include "shared.h"

/* GameLoopState is the global game-loop record. Only the two fields this
 * TU touches are modeled: scene_id at +0x18 (read by GameOver, compared
 * against GAME_END_SCENE_ID) and the frame-status halfword at +0x50
 * (written by GameEnd). The bytes in between and before are real,
 * unmodeled state, not invented fields: byte-backed accepted users name
 * some of it at other offsets under a different shape (e.g. the task
 * scheduler pointer at +0x08 in the GameLoopStateAddressView users), so
 * this TU does not repeat or contradict those names for spans it never
 * touches  */
typedef struct {
    u8 unmodeled_00[0x18];
    int scene_id;
    u8 unmodeled_1c[0x50 - 0x1c];
    u16 frame_status;
} GameLoopStatePrefix;

/* The JPEG decoder's request block: a 32-byte argument of which only the
 * source and destination words are initialized here (the rest is cleared
 * by memset before the call). The remaining 24 bytes are live decoder
 * parameters, not padding: xglJpegDecode (0x00225840) reads the u16s at
 * +0x14 and +0x16 as limits and writes the decoded size back into them at
 * exit, reads the mode byte at +0x1a and the scale bytes at +0x18/+0x19
 * and +0x1c..+0x1e; StartOfScan (0x00225310), called with this request as
 * a1, reads the s32 at +0x04, the s16 at +0x0c/+0x0e, the u16 at
 * +0x10/+0x12 and the byte at +0x1b. */
typedef struct {
    u32 source;
    u32 destination;
    u8 unmodeled_08[0x18];
} JpegDecodeRequest;

extern void xglSoundEffectNormalDirect(int effect_id);

extern PadPrefix PadData;

/* The game-over screen: show the game-over image with its stream, then
 * fade picture and stream volume out together. */
void GameOver(void);

extern const char GameEndImagePath[];

extern const char GameEndQuestion[];

extern const char GameEndSaveQuestion[];

extern const char GameOverImagePath[];

extern const char GameOverStreamPath[];

/* Function-local VIF fade-command scratch packets. GCC's assembler names
 * them TestEnv.4 (GameEnd) and TestEnv.5 (GameOver); only element 4 (the
 * fade command written at +0x20) is touched by either function, so the
 * remaining template bytes are supplied by the reference image, not
 * generated here. */
extern u64 TestEnv4[12];

extern u64 TestEnv5[12];

extern int xglJpegDecode(JpegDecodeRequest *request);

extern void xglRenderDrawFlipPk(XglPacket *packet);

extern void DrawImage(void *framebuffer);

extern void DrawBack(int alpha);

extern void redraw_frame(void *framebuffer);

extern void xglStudioMainCameraInit(void);

extern int MenuFileMain(int mode);

extern void FlushCache(int mode);

extern int xglSoundStreamOpenVagStereoParam(int channel, const char *path, int size, int volume);

extern int xglSoundStreamMain(void);

extern void SsdSetVagStreamVolume(int channel, int volume, int pan);

extern void xglSoundStreamStop(int channel);

#endif /* SRC_MAIN_GAME_OVER_H */
