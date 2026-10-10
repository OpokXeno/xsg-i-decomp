#include "common.h"
#include "shared.h"
#include "call_java_method.h"

void CallMethod_II(const char *method_name, int event_type, int event_id);
typedef struct EventLocatorFunctionData {
    unsigned char unmodeled_00[0x64];
    u32 locator_flags;
} EventLocatorFunctionData;

typedef struct EventLocatorFunctionActor {
    u32 flags;
    void (*update)(struct EventLocatorFunctionActor *actor);
    void (*draw)(struct EventLocatorFunctionActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    Vector4 global_position;
    u8 number;
    u8 unmodeled_81[0xc0 - 0x81];
    EventLocatorFunctionData event_data;
    u8 unmodeled_128[0x9e8 - 0x128];
    float interaction_radius;
} EventLocatorFunctionActor;

void Call_JavaMethod(void *actor, int method_id)
{
    CallJavaActorPrefix *actor_record;
    CallJavaEnemyWork *enemy_work;
    CallJavaGameLoopStatePrefix *game_loop_state;

    (void)method_id;
    actor_record = actor;
    game_loop_state = (CallJavaGameLoopStatePrefix *)GameLoopState;
    enemy_work = &((CallJavaEnemyWork *)enepc)[actor_record->number];
    if (game_loop_state->active_actor == actor_record) {
        CallMethod_II("KickEvent", 100, EventID);
    } else {
        CallMethod_II("KickEvent",
                      enemy_work->kick_event_type,
                      EventID);
    }
    FlagExEvent = 1;
}

/* Call_JavaMethod (defined above in this TU, still assembler) takes actor
 * plus the method id, the same two arguments its own body proves for
 * EventCheck_Line_Touch below (it dereferences actor at +0x80 for the
 * enepc-table index and takes the method id in $a1). */
extern void Call_JavaMethod(void *actor, int method_id);

/* Check_InsideID (0x002d67e8, still assembler in this TU) takes actor plus
 * the two UnduDataGetHeader results, as Check_Locater passes them; this
 * handler forwards those same three arguments untouched. */
extern int Check_InsideID(void *actor, LayoutHeader *first, LayoutHeader *second);

/* PadData's pressed-button halfword (include/shared.h PadPrefix.half_2a);
 * bit 0x20 gates every EventCheck_*_Button handler below, the same bit
 * src/ov01/debug_entry.c tests as PadData.half_2a & 0x20. */
extern PadPrefix PadData;

void EventCheck_Line_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 1);
    }
}

/* Call_JavaMethod is defined above in this TU (still assembler); both
 * arguments are proven by its own body: it dereferences actor at +0x80 for
 * the enepc-table index and takes the method id in $a1 ($a2, reused there as
 * a local constant, is never supplied by any caller in this TU). */


/* Check_InsideID (0x002d67e8, still assembler in this TU) takes actor plus
 * the two UnduDataGetHeader results, as Check_Locater passes them. The touch
 * handlers receive those same three arguments and forward them untouched
 * (whole-program argument liveness: all three are read on entry). */


void EventCheck_Line_Touch(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if (Check_InsideID(actor, first, second) != 0) {
        Call_JavaMethod(actor, 2);
    }
}

void EventCheck_Circle_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 3);
    }
}

/*
 * This TU's own reading of the engine's actor record: only the +0x00..+0x70
 * common head (the six Vector4 members position/previous_position/velocity/
 * acceleration/rotation/scale after flags/update/draw/quadword_alignment_gap)
 * plus the one field EventCheck_Square_Button_Center below reads past it.
 * src/main/chr.h, src/main/act_2.h, src/main/enemy_2.h, src/main/near_dir.h
 * and src/main/set_motion.h each restate the same common head independently
 * as their own TU-local view, under their own tag.
 */
typedef struct EventLocatorActor {
    u32 flags;
    void (*update)(struct EventLocatorActor *actor);
    void (*draw)(struct EventLocatorActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x9e8 - 0x70];
    float interaction_radius; /* +0x9e8: EventCheck_Square_Button_Center's
                                  Check_InsideFan radius argument, alongside
                                  rotation.y as the facing angle and the
                                  literal 50.0/60.0 degree bounds. */
} EventLocatorActor;

/* Check_InsideFan (0x002d8240, defined in a different TU, still assembler)
 * is called here with &actor->position, the incoming first locator header
 * (passed through unmodified in $a1), -1, and four floats: actor's
 * rotation.y, the literal 50.0/60.0 degree bounds and actor's +0x9e8 field. */
extern int Check_InsideFan(Vector4 *origin, LayoutHeader *target, int flag,
                            float facing, float minDegrees, float maxDegrees,
                            float radius);

void EventCheck_Square_Button_Center(EventLocatorActor *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20)
        && (Check_InsideFan(&actor->position, first, -1, actor->rotation.y,
                             50.0f, 60.0f, actor->interaction_radius) != 0)
        && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 4);
    }
}

void EventCheck_Square_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 5);
    }
}

void EventCheck_Square_Touch(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if (Check_InsideID(actor, first, second) != 0) {
        Call_JavaMethod(actor, 6);
    }
}

void Check_Locater(EventLocatorFunctionActor *actor)
{
    EventLocatorFunctionActor *actor_record;
    CallJavaEnemyWork *enemy_work;
    EventLocatorFunctionData *event_data;
    LayoutHeader *first_header;
    LayoutHeader *second_header;
    short pair_index;
    int locator_type;
    CallJavaGameLoopStatePrefix *game_loop_state;

    first_header = 0;
    second_header = 0;
    actor_record = actor;
    game_loop_state = (CallJavaGameLoopStatePrefix *)GameLoopState;
    enemy_work = &((CallJavaEnemyWork *)enepc)[actor_record->number];
    event_data = &actor_record->event_data;

    if ((game_loop_state->flags & 0x400) != 0) {
        return;
    }
    if ((FLAG_FRAME_60 != 0) && ((PadData.half_28 & 0x200) != 0)) {
        return;
    }
    if ((event_data->locator_flags & 0x40000) == 0) {
        return;
    }

    pair_index = 0;
    while (pair_index < 100) {
        first_header = UnduDataGetHeader(519, pair_index);
        second_header = UnduDataGetHeader(519, pair_index + 1);
        if ((first_header == 0) || (second_header == 0)) {
            break;
        }
        if (Check_InsideID(actor_record, first_header, second_header) == 1) {
            break;
        }
        pair_index += 2;
    }

    if ((pair_index == 100) || (first_header == 0) || (second_header == 0)) {
        enemy_work->active_locator_index = -1;
        return;
    }

    FlagExEvent = 0;
    pair_index /= 2;
    EventID = (short)pair_index;
    locator_type = Get_LocaterType((short)pair_index);

    switch (locator_type) {
    case 1:
        EventCheck_Line_Button(actor_record, first_header, second_header);
        break;
    case 2:
        if (enemy_work->active_locator_index != EventID) {
            EventCheck_Line_Touch(actor_record, first_header, second_header);
        }
        break;
    case 3:
        EventCheck_Circle_Button(actor_record, first_header, second_header);
        break;
    case 4:
        EventCheck_Square_Button_Center((EventLocatorActor *)actor_record, first_header, second_header);
        break;
    case 5:
        EventCheck_Square_Button(actor_record, first_header, second_header);
        break;
    case 6:
        if (enemy_work->active_locator_index != EventID) {
            EventCheck_Square_Touch(actor_record, first_header, second_header);
        }
        break;
    }
    enemy_work->active_locator_index = (short)pair_index;
}

char Get_LocaterType(short locator_index)
{
    int offset;
    LayoutHeader *locator_header;
    float angle;
    short type;

    offset = locator_index * 2;
    locator_header = UnduDataGetHeader(0x207, offset);
    UnduDataGetHeader(0x207, offset + 1);
    angle = locator_header->components[3];
    type = 0;
    while (type < 8) {
        if (LocaterAngle[type * 2] < angle) {
            if (angle < LocaterAngle[type * 2 + 1]) {
                break;
            }
        }
        type += 1;
    }
    return type == 8 ? -1 : (char)type;
}

char Get_LocaterType_Angle(float angle)
{
    short type;

    type = 0;
    while (type < 8) {
        if (LocaterAngle[type * 2] < angle) {
            if (angle < LocaterAngle[type * 2 + 1]) {
                break;
            }
        }
        type += 1;
    }
    return type == 8 ? -1 : (char)type;
}

typedef union InsidePosition {
    Vector4 coordinates;
    u64 copy_alignment;
} InsidePosition;
typedef struct InsideActor {
    u32 flags;
    void (*update)(struct InsideActor *);
    void (*draw)(struct InsideActor *);
    u32 quadword_alignment_gap;
    InsidePosition position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x80 - 0x70];
    unsigned char number;
    unsigned char unmodeled_81[0x9e8 - 0x81];
    float interaction_radius;
} InsideActor;
typedef struct InsidePillarSource {
    float x, y, z, w;
} InsidePillarSource;
typedef struct InsidePillarTarget {
    float x, y, z, w;
} InsidePillarTarget;
extern signed char FLAG_FRAME_60;
extern void Get_Point_By_AngleLength(const Vector4 *source, Vector4 *destination,
                                      float angle, float length);
extern float Get_Distance(const Vector4 *first, const Vector4 *second);
extern int Check_CrossingOver(const LayoutHeader *first, const LayoutHeader *second,
                               const Vector4 *point, const Vector4 *origin);
extern void DispPillar(const void *first, const void *second, const int *color);

int Check_InsideID(void *actor_ptr, LayoutHeader *first, LayoutHeader *second)
{
    InsideActor *actor = actor_ptr;
    Vector4 bounds[3];
    InsidePillarSource first_point;
    InsidePillarTarget second_point;
    int color[4];
    InsidePosition origin[1];
    InsidePosition adjusted;
    CallJavaEnemyWork *enemy;
    float radius;
    float first_x;
    float first_z;
    float first_point_x;
    float first_point_z;
    float pillar_far_x;
    float origin_x;
    float origin_z;
    const float *coordinates;
    int locator_type;

    locator_type = Get_LocaterType_Angle(first->components[3]);
    enemy = &((CallJavaEnemyWork *)enepc)[actor->number];
    __builtin_memcpy(&origin[0], &actor->position, sizeof(origin[0]));
    if (enemy->locator_mode == 10) {
        float start = actor->interaction_radius;
        float interpolated = start + (enemy->locator_distance - start)
            * (float)(enemy->locator_current_frame - enemy->locator_start_frame)
            / (float)(enemy->locator_end_frame - enemy->locator_start_frame);
        Get_Point_By_AngleLength(&actor->position.coordinates, &adjusted.coordinates,
                                  actor->rotation.y, interpolated);
        __builtin_memcpy(&origin[0], &adjusted, sizeof(origin[0]));
    }
    color[0] = 255;
    color[1] = 127;
    color[3] = 128;
    color[2] = 0;
    first_point.x = first->components[0];
    first_point.y = first->components[1];
    first_point.z = first->components[2];
    first_point.w = 1.0f;
    second_point.x = second->components[0];
    second_point.y = second->components[1];
    second_point.z = second->components[2];
    second_point.w = 1.0f;

    switch (locator_type - 1) {
    case 0: {
        Vector4 *position = &origin[0].coordinates;
        Get_Point_By_AngleLength(position, &bounds[0], actor->rotation.y, 1.0f);
        return Check_CrossingOver(first, second, &bounds[0], position);
    }
    case 1:
        return Check_CrossingOver(first, second, &actor->previous_position, &origin[0].coordinates);
    case 2:
        radius = Get_Distance((Vector4 *)first, (Vector4 *)second);
        second_point.y = 0.0f;
        second_point.x = radius;
        if (FLAG_FRAME_60 != 0) {
            DispPillar(&first_point, &second_point, color);
        }
        if (radius < Get_Distance((Vector4 *)first, &origin[0].coordinates)) return 0;
        return 1;
    case 3:
    case 4:
    case 5:
        radius = Get_Distance((Vector4 *)first, (Vector4 *)second);
        coordinates = first->components;
        origin_x = origin[0].coordinates.x;
        first_x = coordinates[0];
        first_point_x = first_point.x;
        pillar_far_x = first_point_x + radius;
        first_point_z = first_point.z;
        second_point.z = first_point_z + radius;
        first_point.z -= radius;
        first_z = coordinates[2];
        bounds[1].x = first_x - radius;
        first_point.x -= radius;
        bounds[2].x = first_x + radius;
        bounds[1].z = first_z - radius;
        bounds[2].z = first_z + radius;
        second_point.x = pillar_far_x;
        if (origin_x < bounds[1].x || bounds[2].x < origin_x) return 0;
        origin_z = origin[0].coordinates.z;
        if (origin_z < bounds[1].z) return 0;
        if (bounds[2].z < origin_z) return 0;
        return 1;
    default:
        return 0;
    }
}

/* Font-script labels for the locator debug modes. */
const char D_004CBB18[16] = "\013\xCB\xA7\xC0\xEE\xA5\xC6\xA5\xB9\xA5\xC8";
const char D_004CBB28[16] = "\013HairTest";
const char D_004CBB38[16] = "\013WindTest";
const char D_004CBB48[16] = "\013ColliTest";
