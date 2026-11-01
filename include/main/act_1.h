#ifndef INCLUDE_MAIN_ACT_1_H
#define INCLUDE_MAIN_ACT_1_H

#include "shared.h"

/*
 * Fields this TU's functions read or write beyond the head above, one
 * witness each (ACT_init and ACT_create clear or seed them; ACT_info prints
 * them):
 *   +0x80 number        ACT_create stores the slot (sb a2,0x80); ACT_info
 *                       prints it for the parent and each child.
 *   +0x82 kind          ACT_create stores bits 16..23 of its id word;
 *                       ACT_allocMatrix compares it with 1 to size the block.
 *   +0x83, +0x84        cleared by ACT_init (sb/sh zero).
 *   +0x90, +0x91        ACT_create stores 4/16 when the id's 0xf000 bits are
 *                       clear and 1/80 otherwise.
 *   +0x4c8 undulation   the address ACT_init passes to UnduParamInit.
 *   +0x640..+0x66b      three quadwords (with +0x66c) cleared by ACT_init.
 *   +0x62c, +0x66c, +0x670, +0x674, +0x678, +0x67c, +0x690, +0x754, +0x758,
 *   +0x9a0..+0x9ac, +0x9e0, +0x9f4, +0x9f8
 *                       seeded by ACT_init/ACT_create (names as in
 *                       src/main/chr.h, which stores to the same offsets).
 *   +0x676, +0x696, +0x698, +0x69c, +0x6e0..+0x6e7, +0x970..+0x99f, +0x9d0,
 *   +0x9e8, +0xa60      only ever cleared or seeded by ACT_init/ACT_create;
 *                       +0x6e0 is cleared as eight bytes, +0x970 as three
 *                       quadwords, +0x9e8 receives 0.35f.
 *   +0x6f0, +0x6f8, +0x704  printed by ACT_info as the second flags word, the
 *                       "SPEED" value (divided by 1/30) and "NO" (lhu).
 *   +0x824..+0x830      the matrix block ACT_allocMatrix stores (base, end of
 *                       the first part, then the extra part and its end when
 *                       kind is 1).
 *   +0x8d0..+0x8f8      eleven resource pointers: ACT_info prints each
 *                       non-null one as "RSRC[%d] %p", ACT_dispose2/ACT_init
 *                       clear the block.
 *   +0x8fc..+0x910      parent, child count and the four child pointers.
 */
typedef struct ActRecord {
    unsigned int flags;                      /* +0x00 */
    void (*update)(struct ActRecord *self);  /* +0x04 */
    void (*draw)(struct ActRecord *self);    /* +0x08 */
    unsigned int quadword_alignment_gap;     /* +0x0c */
    Vector4 position;                        /* +0x10 */
    Vector4 previous_position;               /* +0x20 */
    Vector4 velocity;                        /* +0x30 */
    Vector4 acceleration;                    /* +0x40 */
    Vector4 rotation;                        /* +0x50 */
    Vector4 scale;                           /* +0x60 */
    Vector4 global_position;                 /* +0x70 */
    unsigned char number;                    /* +0x80 */
    unsigned char unmodeled_81;
    unsigned char kind;                      /* +0x82 */
    unsigned char state83;                   /* +0x83 */
    short state84;                           /* +0x84 */
    short inUseId;                           /* +0x86 */
    unsigned char unmodeled_88[0x90 - 0x88];
    unsigned char shadow_kind;               /* +0x90 */
    unsigned char shadow_size;               /* +0x91 */
    unsigned char unmodeled_92[0x4c8 - 0x92];
    unsigned int undulation;                 /* +0x4c8 */
    unsigned char unmodeled_4cc[0x62c - 0x4cc];
    float look_eye_speed;                    /* +0x62c */
    unsigned char unmodeled_630[0x640 - 0x630];
    float lookState[11];                     /* +0x640 */
    float look_speed;                        /* +0x66c */
    float shadow_clip_scale;                 /* +0x670 */
    short look_mode;                         /* +0x674 */
    short state676;                          /* +0x676 */
    int look_eye_control;                    /* +0x678 */
    void *look_target;                       /* +0x67c */
    unsigned char unmodeled_680[0x690 - 0x680];
    int shadow_map;                          /* +0x690 */
    unsigned char unmodeled_694[0x696 - 0x694];
    short state696;                          /* +0x696 */
    int state698;                            /* +0x698 */
    int state69c;                            /* +0x69c */
    unsigned char unmodeled_6a0[0x6e0 - 0x6a0];
    unsigned char state6e0[8];               /* +0x6e0 */
    unsigned char unmodeled_6e8[0x6f0 - 0x6e8];
    unsigned int runtimeFlags;               /* +0x6f0 */
    unsigned char unmodeled_6f4[0x6f8 - 0x6f4];
    float speed;                             /* +0x6f8 */
    unsigned char unmodeled_6fc[0x704 - 0x6fc];
    unsigned short motionNumber;             /* +0x704 */
    unsigned char unmodeled_706[0x754 - 0x706];
    int hair_stop_a;                         /* +0x754 */
    int hair_stop_b;                         /* +0x758 */
    unsigned char unmodeled_75c[0x824 - 0x75c];
    void *matrixBlock;                       /* +0x824 */
    void *matrixBlockEnd;                    /* +0x828 */
    void *matrixExtra;                       /* +0x82c */
    void *matrixExtraEnd;                    /* +0x830 */
    unsigned char unmodeled_834[0x8d0 - 0x834];
    void *resource[11];                      /* +0x8d0 */
    struct ActRecord *parent;                /* +0x8fc */
    int childCount;                          /* +0x900 */
    struct ActRecord *children[4];           /* +0x904 */
    unsigned char unmodeled_914[0x970 - 0x914];
    float state970[12];                      /* +0x970 */
    int render_flags;                        /* +0x9a0 */
    int render_command;                      /* +0x9a4 */
    int pixel_alpha;                         /* +0x9a8 */
    int pixel_alpha_parts;                   /* +0x9ac */
    unsigned char unmodeled_9b0[0x9d0 - 0x9b0];
    int state9d0;                            /* +0x9d0 */
    unsigned char unmodeled_9d4[0x9e0 - 0x9d4];
    float sort_offset;                       /* +0x9e0 */
    unsigned char unmodeled_9e4[0x9e8 - 0x9e4];
    float state9e8;                          /* +0x9e8 */
    unsigned char unmodeled_9ec[0x9f4 - 0x9ec];
    int talk_message;                        /* +0x9f4 */
    int touch_message;                       /* +0x9f8 */
    unsigned char unmodeled_9fc[0xa60 - 0x9fc];
    int statea60;                            /* +0xa60 */
    unsigned char unmodeled_a64[0xa70 - 0xa64];
} ActRecord;

void ACT_dispose2(ActRecord *self, int quick);
int ACT_allocMatrix(ActRecord *self, int count);
ActRecord *ACT_create(int slot, int id);
void ACT_draw(void);

#endif /* INCLUDE_MAIN_ACT_1_H */
