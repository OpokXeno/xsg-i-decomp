#include "common.h"

#include "shared.h"

#include "sc_wait_parse_script.h"

#include "main/sef.h"

static int scWaitParseNopScript(ScriptObject *script);
static int scWaitParseCntScript(ScriptObject *script);
static int scWaitParseEveScript(ScriptObject *script);
static int scWaitParseEftScript(ScriptObject *script);
static int scWaitParseMovieScript(ScriptObject *script);
static int scWaitParseMovScript(ScriptObject *script);
static int scWaitMissileScript(ScriptObject *script);

static int (*moveHandlerTbl_1[])(ScriptObject *) = {
    scWaitParseNopScript, scWaitMissileScript
};

/* WAITNOP: no condition to wait on; the command finishes on its first visit. */

/*
 * WAITCNT: the wait is over once `wait_frames`, decremented once per visit,
 * has run out.
 */

/*
 * WAITEVE: the wait is over once the event task the command names has gone
 * away or stopped running.  `wait_operand` is the task index scWAITEVEScript
 * stored when it parsed the command, and scGetTaskAdr returns 0 for a negative
 * one (bltz at 0x002ebc58), which counts as finished.
 */

/*
 * WAITEFT: the wait is over once the effect scheduler scWAITEFTScript named
 * (`wait_operand`) has died.  Once it has, this also restores the ambient
 * state the effect's own SETAMB command replaced: bit 8 of `flags` picks
 * sdvSetAmbState2 over sdvSetAmbState, both given `amb_effect_no` as the
 * effect index.
 */

/* WAITMOVIE: the wait is over once the movie player reports it stopped. */

/* WAITMOV: the wait is over while `flags` bit 5 is clear. */


static int (*waitHandlerTbl_0[6])(ScriptObject *) = {
    scWaitParseNopScript,
    scWaitParseCntScript,
    scWaitParseEveScript,
    scWaitParseEftScript,
    scWaitParseMovieScript,
    scWaitParseMovScript
};

extern int sefSearchMapperIndex(int effect_no);

extern MissileImage *svGetImageListItem(int mapper_index, int image_index);

extern void sefCaclAllTarget(int *target);

/* OV01 MEfCreate at 0x00a33648 returns the allocation success status.
 * The main linker retains its address alias; this caller discards the status. */
extern int func_A33648(MissileSpawn *spawn);

static int scWaitParseNopScript(ScriptObject *script)
{
    return 1;
}

static int scWaitParseCntScript(ScriptObject *script)
{
    int remaining_frames = script->wait_frames - 1;

    script->wait_frames = remaining_frames;
    return remaining_frames < 1;
}

static int scWaitParseEveScript(ScriptObject *script)
{
    EventTask *event_task = scGetTaskAdr(_nowScript, script->wait_operand);

    return event_task == 0 || event_task->active == 0;
}

static int scWaitParseEftScript(ScriptObject *script)
{
    int scheduler_dead = sefIsDeadSchduler(script->wait_operand);

    if (scheduler_dead)
    {
        short amb_effect_no = script->amb_effect_no;

        if (script->flags & 0x100)
            sdvSetAmbState2(2, amb_effect_no);
        else
            sdvSetAmbState(2, amb_effect_no);
    }

    return (short)scheduler_dead;
}

static int scWaitParseMovieScript(ScriptObject *script)
{
    return func_A33248() == 0;
}

static int scWaitParseMovScript(ScriptObject *script)
{
    return ((script->flags >> 5) ^ 1) & 1;
}

int scWaitParseScript(ScriptObject *script)
{
    unsigned short wait_kind = script->wait_kind;
    int result;

    if (wait_kind >= 6) {
        script->wait_kind = 0;
        return 1;
    }

    result = waitHandlerTbl_0[(short)wait_kind](script);
    if (result != 0)
        script->flags &= ~0x10;

    return (short)result;
}

/* Capture the common command fields and the first record's z and tail parameter. */
#define MISSILE_CAPTURE_COMMON_HEADER(spawn_value, data_value, first_entry_value, parameter_value) do { \
    (spawn_value).command_value = (data_value)->command_value; \
    (spawn_value).tail_parameters[0] = (first_entry_value)->position_z; \
    (parameter_value) = (first_entry_value)->spawn_parameter; \
} while (0)

static int scWaitMissileScript(ScriptObject *script)
{
    const float position_scale = 0.01f;
    MissileCommandData *data;
    int spawned;
    int frame;

    frame = script->missile_wait_frames;
    data = script->missile_data;

    do {
        int record_index = script->missile_record_index;
        MissileCommandEntry *entry = &data->entries[record_index];

        spawned = 0;
        if (entry->frame >= 1024)
            return 1;

        if (frame >= entry->frame) {
            MissileSpawn spawn;
            MissileImage *primary;
            MissileImage *secondary;
            int mapper_index;
            int primary_word = 0;
            int secondary_word = 0;
            unsigned short first_entry_parameter;
            MissileImagePair *pair;
            int i;
            int psm;
            short primary_index;
            int command_type;
            int context2;
            int context0;

            mapper_index = sefSearchMapperIndex(script->amb_effect_no);
            primary = data->primary_image_index > 0
                ? svGetImageListItem(mapper_index, data->primary_image_index) : 0;
            secondary = 0;
            if (data->secondary_image_index > 0)
                secondary = svGetImageListItem(mapper_index, data->secondary_image_index);

            memset(&spawn, 0, sizeof(MissileSpawn));
            if (primary != 0)
                primary_word = primary->texture_word;
            if (secondary != 0)
                secondary_word = secondary->texture_word;

            for (i = 0, pair = data->image_pairs; i < 3; i++, pair++) {
                primary_index = pair->primary_index;
                primary = primary_index > 0
                    ? svGetImageListItem(mapper_index, primary_index) : 0;
                secondary = pair->secondary_index > 0
                    ? svGetImageListItem(mapper_index, pair->secondary_index) : 0;
                if (primary != 0 && secondary != 0) {
                    psm = primary->format == 8 ? 19 : 20;
                    spawn.pair_registers[i] =
                        (unsigned long long)primary->buffer_pointer
                        | (unsigned long long)(primary->buffer_width >> 6) << 14
                        | (unsigned long long)psm << 20
                        | (unsigned long long)primary->width_log2 << 26
                        | (unsigned long long)primary->height_log2 << 30
                        | (unsigned long long)0x8000 << 19
                        | (unsigned long long)secondary->buffer_pointer << 37
                        | (unsigned long long)0x8000 << 46;
                }
            }

            /* The pair cursor has stopped on the reserved slot at +0x14; the
             * first entry follows it. */
            MISSILE_CAPTURE_COMMON_HEADER(spawn, data, (MissileCommandEntry *)(pair + 1),
                                          first_entry_parameter);
            spawn.primary_image_word = primary_word;
            spawn.tail_parameters[1] = first_entry_parameter;
            context0 = script->missile_spawn_context[0];
            spawn.spawn_parameter = entry->spawn_parameter;
            context2 = script->missile_spawn_context[2];
            spawn.secondary_image_word = secondary_word;
            spawn.spawn_context2 = context2;
            spawn.spawn_context0 = context0;

            if (entry->flags & 0x400) {
                spawn.spawn_context2 = context2;
                spawn.command_flag = (short)entry->flags - 1024;
            } else if (entry->flags & 0x100) {
                spawn.spawn_context2 = context0;
                spawn.command_flag = (short)entry->flags - 249;
            }

            spawn.spawn_context1 = script->missile_spawn_context[1];
            command_type = data->command_kind;
            if (command_type == 529)
                command_type = script->missile_subtype >= 2 ? 527 : 528;
            if (command_type & 0x200)
                spawn.command_type = command_type - 505;
            else if (command_type & 0x100)
                spawn.command_type = command_type - 249;
            else if (command_type & 0x400)
                spawn.command_type = command_type - 1024;
            else if (command_type == 5)
                sefCaclAllTarget(spawn.target);
            else
                spawn.command_type = 22;

            spawn.position[0] = entry->position_x * position_scale;
            spawn.position[3] = 1.0f;
            spawn.position[1] = entry->position_y * position_scale;
            spawn.position[2] = entry->position_z * position_scale;
            func_A33648(&spawn);
            script->missile_record_index = ++record_index;
            spawned = 1;
        }
    } while (spawned);

    script->missile_wait_frames = ++frame;
    return 0;
}

int scMoveParseScript(ScriptObject *script)
{
    unsigned short move_kind = script->move_kind;
    int result;

    if (move_kind >= 2) {
        script->move_kind = 0;
        return 1;
    }

    result = moveHandlerTbl_1[(short)move_kind](script);
    if (result != 0)
        script->flags &= ~0x20;

    return (short)result;
}
