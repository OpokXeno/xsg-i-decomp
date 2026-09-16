#include "common.h"
#include "shared.h"

typedef struct VibrationGameLoopState {
    unsigned char unmodeled_00[0x2a000];
    unsigned char strong_start;
    unsigned char strong_end;
    unsigned char unmodeled_2a02[2];
    u32 weak_duration;
    int strong_duration;
    int strong_total_duration;
} VibrationGameLoopState;

typedef struct VibrationPadData {
    unsigned char unmodeled_00[0x50];
    unsigned char weak_active;
    unsigned char strong_output;
} VibrationPadData;

extern VibrationGameLoopState GameLoopState;
extern VibrationPadData PadData;

void Vibration_Stop(void) {
    GameLoopState.strong_duration = 0;
    PadData.weak_active = 0;
    GameLoopState.weak_duration = 0;
    PadData.strong_output = 0;
}

void Vibration_Set_Weak(int duration) {
    GameLoopState.weak_duration = duration;
}

void Vibration_Set_Strong(int start, int end, int duration) {
    GameLoopState.strong_duration = duration;
    GameLoopState.strong_start = (unsigned char)start;
    GameLoopState.strong_end = (unsigned char)end;
    GameLoopState.strong_total_duration = duration;
}

void VibrationEngine(void) {
    if (GameLoopState.weak_duration != 0) {
        int remaining = GameLoopState.weak_duration - 1;
        int clamped = remaining < 0 ? 0 : remaining;
        PadData.weak_active = 1;
        GameLoopState.weak_duration = clamped;
    }

    if (GameLoopState.strong_duration != 0) {
        int remaining = GameLoopState.strong_duration - 1;
        int clamped = remaining < 0 ? 0 : remaining;
        PadData.strong_output = GameLoopState.strong_end +
            (GameLoopState.strong_start - GameLoopState.strong_end) *
            GameLoopState.strong_duration / GameLoopState.strong_total_duration;
        GameLoopState.strong_duration = clamped;
    }
}
