/*
 * TU-local declarations of main/tu214 (src/main/sc_wait_parse_script.c).
 */

#ifndef SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H
#define SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H

#include "shared.h"

typedef struct EventTask {
    unsigned short active;
} EventTask;

extern EventTask *scGetTaskAdr(int script_index, int task_index);

typedef struct ScriptObject ScriptObject;

typedef struct MissileImagePair {
    short primary_index;
    short secondary_index;
} MissileImagePair;

/* Fields read from the image-list item returned by svGetImageListItem. */
typedef struct MissileImage {
    unsigned int texture_word;
    unsigned char unmodeled_04[14];
    unsigned short buffer_pointer;
    unsigned short format;
    unsigned short buffer_width;
    unsigned char unmodeled_18[4];
    unsigned short width_log2;
    unsigned short height_log2;
} MissileImage;

/* Parameter block passed to the missile-spawn routine. */
typedef struct MissileSpawn {
    int command_value;
    int spawn_context0;
    int spawn_context2;
    int command_flag;
    float position[4];
    unsigned short spawn_parameter;
    unsigned char unmodeled_22[2];
    int spawn_context1;
    int command_type;
    unsigned char unmodeled_2c[4];
    int target[4];
    int primary_image_word;
    int secondary_image_word;
    unsigned long long pair_registers[3];
    unsigned char unmodeled_60[8];
    unsigned short tail_parameters[2];
    unsigned char unmodeled_6c[4];
} MissileSpawn;

typedef struct MissileCommandEntry {
    short frame;
    u16 flags;
    short position_x;
    short position_y;
    short position_z;
    u16 spawn_parameter;
} MissileCommandEntry;

/* The serialized command block stores its variable-sized entries after this header. */
typedef struct MissileCommandData {
    short command_value;               /* +0x00 */
    short primary_image_index;         /* +0x02 */
    short secondary_image_index;       /* +0x04 */
    short command_kind;                /* +0x06 */
    MissileImagePair image_pairs[3];    /* +0x08, the three pairs traversed */
    unsigned char unmodeled_14[4];      /* +0x14, not read by the handler */
    MissileCommandEntry entries[1];     /* +0x18, followed by remaining entries */
} MissileCommandData;

/*
 * The script command record the wait handlers of this unit are dispatched on.
 *
 * The members below are the ones this original object establishes for itself;
 * everything between them is an explicit byte range, not an invented span.
 *
 *   +0x00 flags         scWaitParseScript clears bit 4 of it once the handler
 *                       it dispatched reports the wait finished (lhu/andi
 *                       0xffef/sh at 0x002ebeec..0x002ebef4); scWaitParseMovScript
 *                       tests bit 5 of the same halfword (lhu/srl 5/xori 1/andi
 *                       1 at 0x002ebe88..0x002ebe98); and scWaitParseEftScript
 *                       tests bit 8 of it to pick which ambient-state call it
 *                       makes (0x002ebe18..0x002ebe24).
 *   +0x44 amb_effect_no the ambient-state effect index scWaitParseEftScript
 *                       passes as sdvSetAmbState/sdvSetAmbState2's own
 *                       `effect_no` argument once the awaited scheduler has
 *                       died (lh 0x44 at 0x002ebe1c).
 *   +0x5a wait_kind     the handler selector: scWaitParseScript reads it,
 *                       refuses a value of 6 or more and zeroes it, and
 *                       otherwise calls waitHandlerTbl[wait_kind]
 *                       (0x002ebeb0..0x002ebedc).  scWAITEVEScript
 *                       (src/main/sc_get.c, 0x002eb344) writes 2 here and
 *                       scWAITEFTScript (0x002eb37c) writes its own kind.
 *   +0x5e wait_frames   scWaitParseCntScript counts it down once per visit and
 *                       reports the wait finished when it reaches zero
 *                       (lh/addiu -1/sh/slti 1 at 0x002ebd98..0x002ebda8).
 *   +0x60 wait_operand  the numeric operand of the WAIT command, taken from
 *                       scGetNumScript by both producers (sh 0x60 at
 *                       0x002eb340 and 0x002eb384).  Each handler reads it in
 *                       its own terms: scWaitParseEveScript passes it to
 *                       scGetTaskAdr as the event-task index (lh 0x60 at
 *                       0x002ebdb8) and scWaitParseEftScript passes it to
 *                       sefIsDeadSchduler (lh 0x60 at 0x002ebe08).
 *
 * This is not the complete record.  scGetTaskAdr strides the per-script task
 * array at 128 bytes (sll 7 at 0x002ebc4c), so the object is at least that
 * long, and nothing else of it is recovered here.
 */
struct ScriptObject {
    unsigned short flags;               /* +0x00 */
    unsigned char unmodeled_02[0x2e];   /* +0x02 */
    int missile_spawn_context[3];       /* +0x30, copied into missile parameters */
    short missile_subtype;              /* +0x3c, selects the 527/528 command kind */
    unsigned char unmodeled_3e[6];      /* +0x3e */
    short amb_effect_no;                /* +0x44 */
    unsigned char unmodeled_46[0x14];   /* +0x46 */
    unsigned short wait_kind;           /* +0x5a */
    unsigned short move_kind;           /* +0x5c, scMoveParseScript's handler selector */
    short wait_frames;                  /* +0x5e */
    short wait_operand;                 /* +0x60 */
    unsigned char unmodeled_62[4];
    short missile_wait_frames;           /* +0x66 */
    short missile_record_index;          /* +0x68 */
    unsigned char unmodeled_6a[6];
    MissileCommandData *missile_data;    /* +0x70 */
};

extern int _nowScript;

/*
 * sdvSetAmbState/sdvSetAmbState2 are defined in src/main/sdv.c (main/tu215)
 * with external linkage, but that TU's own src/main/sdv.h does not yet
 * declare them for another TU to call (nothing else needed them before this
 * one).  Declared here with sdv.c's own parameter names/types until a header
 * harvest gives them a canonical home in sdv.h.
 */
extern void sdvSetAmbState(int state, int effect_no);
extern void sdvSetAmbState2(int state, int effect_no);

/*
 * scWaitParseMovieScript polls the movie-playback status routine at
 * 0x00A33248 (ELF symbol MMvIsPlaying, ov01 m_mv.c / ov01/tu019), still
 * INCLUDE_ASM in its own unit.  main is linked without the overlay, so the
 * call resolves through the scaffold's undefined-symbol list
 * (build/main/build/main.undefined.ld: `func_A33248 = 0xA33248`); binding
 * the ELF name instead needs a change to that shared linker input, which
 * this TU does not own, so the call keeps the scaffold's address-derived
 * name (docs/naming.md, "Scaffold-owned data keeps its splat name").
 */
extern int func_A33248(void);

#endif /* SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H */
