#ifndef SRC_MAIN_GAME_SNAP_SHOT_H
#define SRC_MAIN_GAME_SNAP_SHOT_H

#include "main/xgl_2.h"
#include "main/game_snap_shot.h"

typedef union SnapshotDmaPacket {
    u64 words[24];
    XglQuadword quadwords[12];
} SnapshotDmaPacket;

#endif /* SRC_MAIN_GAME_SNAP_SHOT_H */
