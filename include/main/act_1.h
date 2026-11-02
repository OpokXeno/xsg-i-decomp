#ifndef INCLUDE_MAIN_ACT_1_H
#define INCLUDE_MAIN_ACT_1_H

#include "shared.h"
#include "main/jnt.h"
#include "main/xgl_light.h"
#include "main/enemy_1.h"

/*
 * The engine's actor record (main 0x0043c1e0: 64 entries, 0xa70 stride; the
 * readelf size 0x29c00 is exactly 64 * 0xa70). This is the one definition of
 * every view of it in MAIN: the head +0x00..+0x70 recovered in
 * src/main/near_dir.h, the fields ACT_create, ACT_init and ACT_info touch
 * (+0x80..+0x91, +0x4c8, +0x62c..+0x6e0, +0x6f0..+0x704, +0x824..+0x830,
 * +0x8d0..+0x910), the animation, model and draw-environment sub-records of
 * src/main/act_2.c, and the Chr peer fields of src/main/chr.h.
 *
 *   +0x00 flags         bit 0x8 hides the actor from prevActor, nextActor and the
 *                       cursor's actor selection (src/main/db_light_write.c).
 *   +0x80 number        the slot number ACT_create stores (sb a2,0x80); ACT_info
 *                       prints it for the parent and each child.
 *   +0x82 kind          ACT_create stores bits 16..23 of its id word;
 *                       ACT_allocMatrix compares it with 1 to size the block.
 *   +0x83 humanKind     cleared by ACT_init.
 *   +0x84 state84       cleared by ACT_init.
 *   +0x86 inUseId       the id word ACT_create stores (sh t0,0x86).
 *   +0x88 frame_counter a halfword the Chr setters read as a count.
 *   +0x90 shadow_kind, +0x91 shadowSize: ACT_create stores 4/16 when the id's
 *                       0xf000 bits are clear and 1/80 otherwise.
 *   +0x4c0 java_object  the Java peer's native object word. script.c's
 *                       SCRIPT_execTalkto checks it is a xeno/Chr instance (lw 0x4c0
 *                       at 0x00261540) and DB_evtMonitor finds the actor's VM thread
 *                       through it (lw 0x4c0 at 0x0026443c).
 *   +0x4c8 undulation   the address ACT_init passes to UnduParamInit.
 *   +0x4d0 status_flags Java_xeno_Chr_setID__I ORs in bit 0x1000 here.
 *   +0x4dc translate_y  the position.y mirror Java_xeno_Chr_setTranslate__ writes.
 *   +0x4e0 data_header  the layout header Java_xeno_Chr_setID__I stores.
 *   +0x4e8 attribute_flags a 64-bit attribute word.
 *   +0x510 light        a StudioLight block (include/main/xgl_light.h): the
 *                       seven light quadwords and the two matrices
 *                       xglLightCalcMatrix derives from them.
 *   +0x600 cleared600   two quadwords ACT_init clears.
 *   +0x620 look_point[3], +0x62c look_eye_speed, +0x640 lookState[11],
 *   +0x66c look_speed, +0x670 shadowClipScale, +0x674 look_mode,
 *   +0x678 look_eye_control, +0x67c look_target, +0x680 mapShadowPartIds[8].
 *   +0x676 windowId     the window owner id.
 *   +0x690 mapShadowPartCount, +0x696 pauseCounter, +0x698 attachmentCount,
 *   +0x69c attachmentIndex, +0x6a0 attachments[8], +0x6e0 attachmentUsed[8].
 *   +0x6f0 animation    the embedded animation record ACT_animGetCurrent
 *                       addresses; its slot.flags is the second flags word
 *                       ACT_info prints.
 *   +0x754 transitionFrames, +0x758 holdFrames: the frame counters ACT_setMotion
 *                       decrements.
 *   +0x760 rootTranslation, +0x790 jointProducer (JntProducer, include/main/jnt.h;
 *                       its tail +0x7f4..+0x823 holds the production status, the
 *                       accessory and move-element caches and the shape bindings).
 *   +0x824..+0x830 jointMatrices, previousJointMatrices, staticValueRecords,
 *                       valueRecords: the pointers ACT_allocMatrix stores (the
 *                       block base, then the end of its first part, then the
 *                       extra part and its end when kind is 1).
 *   +0x840 model[1]     ActorModelState: matrixResource at +0x894 and the joint
 *                       matrices at +0x898.
 *   +0x8d0..+0x8fb      the 11 resource pointers, as modelResource, matrixResource,
 *                       move and animPackTables[8] (ACT_info prints each non-null
 *                       one as "RSRC[%d] %p"; ACT_dispose2 and ACT_init clear
 *                       the block).
 *   +0x8fc parent, +0x900 childCount, +0x904 children[7].
 *   +0x920 drawEnv      ActorDrawEnv: flags (+0x9a0), renderLevel (+0x9a4),
 *                       pixelAlpha (+0x9a8), partAlphaCount (+0x9ac),
 *                       lightColor (+0x970), cameraCullDistance (+0x9d0),
 *                       alpha (+0x9d8).
 *   +0x9e0 sortOffset, +0x9e8 body_radius (0.35f from ACT_init).
 *   +0x9f4 talk_message, +0x9f8 touch_message.
 *   +0xa40 moved_hair_mode, +0xa44 moved_hair_delay, +0xa50 moved_hair_sway.
 *   +0xa60 statea60     cleared by ACT_init.
 */

typedef struct ActorAnimSlot {
    unsigned int flags;
    float frame;
    float speed;
    float reverseLimit;
    float forwardLimit;
    unsigned short currentDataId;
    unsigned short nextDataId;
    unsigned short channelMask; /* +0x18 */
    unsigned short bufferIndex; /* +0x1a */
    unsigned char unmodeled_01c[0x22 - 0x1c];
    unsigned short animQueuedFlags;
} ActorAnimSlot;

typedef struct ActorAnimationState {
    ActorAnimSlot slot;
    float blendDuration; /* +0x24 */
    float blendEndFrame; /* +0x28 */
    void *animData;
    void *animUserData;
    union {
        void *animPack;                    /* +0x34: act_2.c clears it */
        struct FpkFcvHeader *fcvHeader;    /* +0x34: the FPK pack hair_test.c counts */
    };
    unsigned char unmodeled_38[0x48 - 0x38]; /* +0x38 */
    float blendWork[4]; /* +0x48 */
    unsigned char unmodeled_58[0x60 - 0x58]; /* +0x58 */
    unsigned short previousDataId; /* +0x60 */
} ActorAnimationState;

typedef struct ActorAccessoryJointTable {
    unsigned char unmodeled_00[0x20];
    int jointId[0x29];
} ActorAccessoryJointTable;

typedef struct ActorDrawEnv {
    float fogDistance[4];
    float fogColor[4];
    float lightPosition[3][4];
    float lightColor[3][4];
    unsigned int flags;
    unsigned int renderLevel;
    int pixelAlpha;
    int partAlphaCount;
    /*
     * Per-part pixel alpha, [n][0] alpha and [n][1] part id: a plain short
     * array, as Java_xeno_Chr_pixelAlphaParts__II's stores show (a struct
     * element moves the field offset out of the store displacements).
     */
    short partAlpha[4][2];
    float transparency;                                /* +0xa0: Chr peers' first filter float */
    float filter_param[3];                             /* +0xa4: the other three filter floats */
    float cameraCullDistance;
    unsigned char unmodeled_b4[4];
    unsigned long long alpha;
} ActorDrawEnv;

typedef struct ActorAttachment {
    short type;
    short joint;
    void **matrix;
} ActorAttachment;

typedef struct ActorModel {
    unsigned char unmodeled_00[0x54];
    void *matrixResource;
} ActorModel;

typedef struct ActorModelState {
    ActorModel model;
    Matrix4 *jointMatrices;
} ActorModelState;

/* The Java argument copy area of a Chr peer (Actor +0xc0..+0x4bf, ACTOR_ARGS):
 * a struct member, not a union, so its fields keep their own alias sets.
 * Comments give the Actor offset; enemy_2.c's Enemy_Command_Action and
 * Start_Enemy_Command keep &actor->args in a base register. */
typedef struct ActorJavaArgs {
    unsigned char unmodeled_00[0x18];
    u16 idle_motion_id;                  /* +0xd8: Enemy_ActionReady reads it as the idle motion */
    short action_frame;                  /* +0xda: Enemy_Command_Action clears it */
    u16 reaction_motion_id;              /* +0xdc: Enemy_ActionReady reads it as the reaction motion */
    short action_duration;               /* +0xde */
    unsigned char unmodeled_20[0x2c - 0x20];
    int command_code;                    /* +0xec: Enemy_Command_Code stores it before Enemy_Init */
    unsigned char unmodeled_30[0x4c - 0x30];
    int methodArg;                       /* +0x10c: DrillHitActor's enemy-broken method argument */
    unsigned char unmodeled_50[0x60 - 0x50];
    int command_count;                   /* +0x120: queued enemy commands */
    u32 locator_flags;                   /* +0x124 */
    unsigned char unmodeled_68[0x80 - 0x68];
    unsigned int command_type[16];       /* +0x140 */
    int command_parameter[5][16];        /* +0x180: five parameter rows */
    unsigned char unmodeled_200[0x400 - 0x200];
} ActorJavaArgs;

/*
 * What the menu's model actors (MenuModelUnitActorSet, ACT_create(-1)) keep
 * in the +0xc0 block instead of Java arguments: the transform the menu
 * controls move, the draw progress and the draw-type hook. The original
 * copies these vectors as two doublewords and keeps the
 * tskMenuModelTaskMain (1, 1, 1, 1) initializer constant only 8-aligned in
 * .rodata (+0x138), so the copy type is the 8-aligned DoublewordAlignedVector4.
 */
struct Actor;

typedef struct ActorMenuControl {
    AlignedVector4 offset;                             /* +0xc0 */
    AlignedVector4 rotation;                           /* +0xd0 */
    AlignedVector4 scale;                              /* +0xe0 */
    int enabled;                                       /* +0xf0 */
    unsigned char unmodeled_34[0xC];
    AlignedVector4 position;                           /* +0x100 */
    unsigned char unmodeled_50[0x10];
    float transparency;                                /* +0x120: menu draw progress */
    void (*drawTypeFunction)(struct Actor *actor);     /* +0x124 */
    int initialMotionId;                               /* +0x128: selected before opening the unit actor */
    unsigned char unmodeled_6C[4];
} ActorMenuControl;

typedef struct Actor {
    u32 flags;                                         /* +0x00 */
    void (*update)(struct Actor *actor);               /* +0x04 */
    void (*draw)(struct Actor *actor);                 /* +0x08 */
    u32 quadword_alignment_gap;                        /* +0x0c */
    AlignedVector4 position;                           /* +0x10 */
    Vector4 previous_position;                         /* +0x20 */
    Vector4 velocity;                                  /* +0x30 */
    Vector4 acceleration;                              /* +0x40 */
    AlignedVector4 rotation;                           /* +0x50 */
    AlignedVector4 scale;                              /* +0x60 */
    AlignedVector4 globalPosition;                     /* +0x70 */
    u8 number;                                         /* +0x80 */
    u8 signal;                                         /* +0x81 */
    u8 kind;                                           /* +0x82 */
    u8 humanKind;                                      /* +0x83 */
    short state84;                                     /* +0x84 */
    short inUseId;                                     /* +0x86 */
    short frame_counter;                              /* +0x88 */
    unsigned char unmodeled_8a[0x90 - 0x8a];
    u8 shadow_kind;                                    /* +0x90 */
    u8 shadowSize;                                     /* +0x91 */
    unsigned char unmodeled_92[0xa0 - 0x92];
    Vector4 shadow_offset;                             /* +0xa0: set_motion's Set_Shadow reads it */
    short last_step_distance;                          /* +0xb0: Sound_FootStep keeps the previous step here */
    unsigned char unmodeled_b2[0xc0 - 0xb2];
    /* The +0xc0 block is kind-specific: Chr peers keep their Java argument
     * copy here, the menu's model actors keep their transform control, and
     * enemies keep their parameter record. */
    union {
        ActorMenuControl menu;                         /* +0xc0 */
        ActorJavaArgs args;                            /* +0xc0 */
        EnemyParams enemy;                             /* +0xc0 */
    };
    SceneObject java_object;                           /* +0x4c0 */
    unsigned char unmodeled_4c4[0x4c8 - 0x4c4];
    /* +0x4c8 is the UnduParam record UnduParamInit fills; its attr_mask word
     * at +0x4d0 is status_flags, so the record is a plain byte array here. */
    unsigned char undulation[8];                       /* +0x4c8 */
    u16 status_flags;                                  /* +0x4d0 */
    unsigned char unmodeled_4d2[0x4dc - 0x4d2];
    float translate_y;                                 /* +0x4dc */
    struct LayoutHeader *data_header;                  /* +0x4e0 */
    unsigned char unmodeled_4e4[0x4e8 - 0x4e4];
    union {
        u64 attribute_flags;                           /* +0x4e8 */
        u32 collision_bits;                            /* +0x4e8: karakama_test.c reads the low word */
    };
    unsigned char unmodeled_4f0[0x510 - 0x4f0];
    StudioLight light;                                 /* +0x510 */
    union {
        Vector4 cleared600[2];                         /* +0x600 */
        u32 look_clear_600[8];                         /* +0x600: the same cleared block as u32s */
    };
    float look_point[3];                               /* +0x620 */
    float look_eye_speed;                              /* +0x62c */
    unsigned char unmodeled_630[0x640 - 0x630];
    union {
        float lookState[11];                           /* +0x640 */
        struct {
            u32 look_clear_640[8];                     /* +0x640 */
            float look_eye_angle[2];                   /* +0x660: lookState[8..9] */
            unsigned char unmodeled_668[4];
        };
    };
    float look_speed;                                  /* +0x66c */
    float shadowClipScale;                             /* +0x670 */
    short look_mode;                                   /* +0x674 */
    u16 windowId;                                      /* +0x676 */
    int look_eye_control;                              /* +0x678 */
    void *look_target;                                 /* +0x67c */
    u16 mapShadowPartIds[8];                           /* +0x680 */
    int mapShadowPartCount;                            /* +0x690 */
    signed char stick_x;                               /* +0x694 */
    signed char stick_y;                               /* +0x695 */
    short pauseCounter;                                /* +0x696 */
    int attachmentCount;                               /* +0x698 */
    int attachmentIndex;                               /* +0x69c */
    ActorAttachment attachments[8];                    /* +0x6a0 */
    signed char attachmentUsed[8];                     /* +0x6e0 */
    unsigned char unmodeled_6e8[0x6f0 - 0x6e8];
    ActorAnimationState animation;                     /* +0x6f0 */
    int transitionFrames;                              /* +0x754 */
    int holdFrames;                                    /* +0x758 */
    unsigned char unmodeled_75c[4];
    Vector4 rootTranslation;                           /* +0x760 */
    unsigned char unmodeled_770[0x790 - 0x770];
    JntProducer jointProducer;                         /* +0x790..+0x823 */
    void *jointMatrices;                               /* +0x824: ACT_updateMPack reads the same word as its linked actor */
    void *previousJointMatrices;                       /* +0x828 */
    void *staticValueRecords;                          /* +0x82c */
    void *valueRecords;                                /* +0x830 */
    unsigned char unmodeled_834[0x840 - 0x834];
    ActorModelState model[1];                          /* +0x840 */
    unsigned char unmodeled_89c[0x8d0 - 0x89c];
    void *modelResource;                               /* +0x8d0 */
    void *matrixResource;                              /* +0x8d4 */
    void *move;                                        /* +0x8d8 */
    void *animPackTables[8];                           /* +0x8dc */
    struct Actor *parent;                              /* +0x8fc */
    int childCount;                                    /* +0x900 */
    struct Actor *children[7];                         /* +0x904 */
    /* children[7..14] (Chr peers keep fifteen, and ACT_setParent does not cap
     * childCount at seven) would be the words that open drawEnv. A union here
     * was measured to reorder the scheduling of drawEnv's stores (chr.c), so the
     * record keeps drawEnv plain and the Chr loops index children as the original
     * does. */
    ActorDrawEnv drawEnv;                              /* +0x920 */
    float sortOffset;                                  /* +0x9e0 */
    float target_angle;                                /* +0x9e4 */
    float body_radius;                                 /* +0x9e8: 0.35f from ACT_init; collision radius and interaction range */
    short look_at_timer;                               /* +0x9ec */
    short look_at_target;                              /* +0x9ee */
    short flash_timer;                                 /* +0x9f0 */
    unsigned char unmodeled_9f2[0x9f4 - 0x9f2];
    /* script.c reads the same words as the JNI method names of the talk and
     * touch scripts (null when the actor has none). */
    union {
        int talk_message;                              /* +0x9f4 */
        const char *talk_method_name;
    };
    union {
        int touch_message;                             /* +0x9f8 */
        const char *touch_method_name;
    };
    unsigned char unmodeled_9fc[0xa00 - 0x9fc];
    AlignedVector4 saved_position;                     /* +0xa00: enemy_1.c's ACT_updateEnemy */
    SchedulerState *effects[8];                       /* +0xa10 */
    short effect_timer;                                /* +0xa30 */
    short effect_slot;                                 /* +0xa32 */
    unsigned char unmodeled_a34[0xa40 - 0xa34];
    u8 moved_hair_mode;                                /* +0xa40 */
    unsigned char unmodeled_a41[3];
    float moved_hair_delay;                            /* +0xa44 */
    unsigned char unmodeled_a48[8];
    Vector4 moved_hair_sway;                           /* +0xa50 */
    union {
        int statea60;                                  /* +0xa60 */
        union Matrix *previous_joint_matrices;         /* +0xa60: EXM reads it as the owner's previous motion matrices */
    };
    unsigned char unmodeled_a64[0xa70 - 0xa64];
} Actor;

void ACT_dispose2(Actor *self, int quick);
int ACT_allocMatrix(Actor *self, int count);
Actor *ACT_create(int slot, int id);
void ACT_draw(void);

/* The 11 consecutive resource pointers at Actor +0x8d0, modelResource first
 * (ACT_init clears them and ACT_info prints each non-null one by index). */
#define ACTOR_RESOURCES(actor) (&(actor)->modelResource)
#define ACTOR_RESOURCE_COUNT 11

/* The Java argument copy area of a Chr peer, +0xc0..+0x4bf (the bytes of the
 * gap above locator_flags and up to java_object). */
#define ACTOR_ARGS(actor) ((unsigned char *)(actor) + 0xc0)

/* Number of entries of the global `actor` array. */
#define ACTOR_COUNT 64

/* Flags every actor slot is reset to. */
#define ACTOR_FLAGS_INITIAL 0x20

/* Actor byte offsets past the recovered head of Actor. */
#define ACTOR_SLOT_NUMBER_OFFSET 0x80 /* byte: the actor's own slot in actor[] */
#define ACTOR_IN_USE_ID_OFFSET 0x86

/* Number of scheduler effects an actor owns (effects[], effect_slot wraps). */
#define ACTOR_EFFECT_SLOT_COUNT 8

/* Actor.flags bits. */
#define ACTOR_FLAG_HIDDEN 0x8 /* skipped by target selection */
#define ACTOR_FLAG_TRANSLUCENT 0x800 /* drawn with the translucent draw environment */
#define ACTOR_FLAG_COLLIDABLE 0x00010000
#define ACTOR_FLAG_FACE_PLAYER 0x00400000
#define ACTOR_FLAG_AILMENT_ACTIVE 0x00800000 /* one of the ailment flags below is set */
#define ACTOR_FLAG_STIFF_ACTIVE 0x01000000 /* explosion damage / stiff */
#define ACTOR_FLAG_ELECTRIC_ACTIVE 0x02000000
#define ACTOR_FLAG_SEAL_ACTIVE 0x08000000
#define ACTOR_FLAG_HOMING 0x80000000 /* the enemy follows the homing graph */

/* Floor attribute bits (attribute word of a map surface) that move whatever
 * stands on it: a conveyor direction index and a speed class index. */
#define ACTOR_ATTR_FLOOR_MASK 0x3f000000
#define ACTOR_ATTR_FLOOR_DIR_MASK 0x0f000000 /* direction index, bits 24..27 */
#define ACTOR_ATTR_FLOOR_DIR_SHIFT 24
#define ACTOR_ATTR_FLOOR_SPEED_SHIFT 28 /* speed class index, bits 28..29 */

/* Motion data ids every character has (animation.slot.currentDataId). */
#define MOTION_ID_IDLE 0
#define MOTION_ID_WALK 1
#define MOTION_ID_RUN 3

/* drawEnv.alpha of a translucent actor (ACTOR_FLAG_TRANSLUCENT). */
#define ACTOR_TRANSLUCENT_ALPHA 0x48

/* Functions and data used by other translation units. */
void ACT_matrixInit(void);
void ACT_update(void);

#endif /* INCLUDE_MAIN_ACT_1_H */
