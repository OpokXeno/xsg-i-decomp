#ifndef SRC_MAIN_GAME_SNAP_SHOT_H
#define SRC_MAIN_GAME_SNAP_SHOT_H

#include "main/xgl_2.h"

typedef union SnapshotDmaPacket {
    u64 words[24];
    XglQuadword quadwords[12];
} SnapshotDmaPacket;

int GameSnapShotSaveFile(int number, const void *data, int size);

#endif /* SRC_MAIN_GAME_SNAP_SHOT_H */
