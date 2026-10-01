/*
 * TU-local declarations of main/tu198 (src/main/call_java_method.c).
 */

#ifndef SRC_MAIN_CALL_JAVA_METHOD_H
#define SRC_MAIN_CALL_JAVA_METHOD_H

typedef struct EnemyWork EnemyWork;
extern EnemyWork enepc[16];

typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;

/* from src/math/main/review09-002f6c50/private.h (unit math-main-002f6c50-review09, function LAYOUT_mapID_setUnit @ 0x002f6c50) */
/* UnduDataGetHeader returns this bounded four-component result. */
typedef struct LayoutHeader {
    float components[4];
} LayoutHeader;

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType @ 0x002d6668) */
char Get_LocaterType(short locator_index);

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType_Angle @ 0x002d6748) */
char Get_LocaterType_Angle(float angle);

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType @ 0x002d6668) */
extern float LocaterAngle[16];

/* canon: config/header-canon.json chose src/math/main/review09-002f6c50/private.h over 1 other accepted spelling */
/* from src/math/main/review09-002f6c50/private.h (unit math-main-002f6c50-review09, function LAYOUT_mapID_setUnit @ 0x002f6c50) */
extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

typedef struct CallJavaActorPrefix {
    u32 flags;
    void (*update)(struct CallJavaActorPrefix *actor);
    void (*draw)(struct CallJavaActorPrefix *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    Vector4 global_position;
    u8 number;
    u8 unmodeled_81[0x124 - 0x81];
    u32 locator_flags;
} CallJavaActorPrefix;

typedef struct CallJavaGameLoopStatePrefix {
    u32 unmodeled_00;
    CallJavaActorPrefix *active_actor;
    u8 unmodeled_08[0x10 - 8];
    u32 flags;
    u8 unmodeled_14[0x2a030 - 0x14];
} CallJavaGameLoopStatePrefix;

typedef struct CallJavaEnemyWork {
    float locator_distance;
    u8 unmodeled_04[2];
    short locator_start_frame;
    short locator_end_frame;
    u8 unmodeled_0a[0x44 - 0x0a];
    short locator_current_frame;
    u8 unmodeled_46[2];
    signed char locator_mode;
    u8 unmodeled_49[0x37b8 - 0x49];
    int kick_event_type;
    u8 unmodeled_37bc[0x37e4 - 0x37bc];
    short active_locator_index;
    u8 unmodeled_37e6[0x38b0 - 0x37e6];
} CallJavaEnemyWork;

extern short EventID;
extern signed char FlagExEvent;
extern signed char FLAG_FRAME_60;
extern const char D_004CBAD0[];

#endif /* SRC_MAIN_CALL_JAVA_METHOD_H */
