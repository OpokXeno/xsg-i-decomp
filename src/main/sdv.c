#include "common.h"

#include "shared.h"

#include "sdv.h"

#include "m_ef_obj.h"

static float _sdvMapRgb[4] = {1.0f, 1.0f, 1.0f, 1.0f};

static float _sdvAmbient[4] = {0.25f, 0.25f, 0.25f, 1.0f};

const float McMathUnitMatrix[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

static short _sdvSpecialBuf[8];

static SdvAlter _sdvAlter[16];

static int _sdvAmbFrame = 0;

static int _sdvAmbState = 0;

static unsigned char charID_0;

static int eftCate_1;

unsigned short itmBox[255] = {0};

unsigned short wpnBox[255] = {0};

unsigned short bltBox[255] = {0};

unsigned short accBox[255] = {0};

unsigned short evtBox[255] = {0};

long long moneyBox = 0;

unsigned char orgData[33][0x180] = {{0}};

unsigned char batDatBuf[0x10000] = {0};

unsigned char thinkBuf[0x4000] = {0};

MEfObjCamParams mefCamParams = {0};

extern void *memset(void *, int, unsigned int);

extern Vector4 *MMathVectorInterpolation(Vector4 *destination,
                                         const Vector4 *first,
                                         const Vector4 *second,
                                         float parameter);

extern unsigned char *sefGetBattleData(void);

extern short _hitFlag;

extern short _hitSignal;

typedef struct SdvSequence {
    unsigned int event_address;
    unsigned char unmodeled_04[4];
    int elapsed;
    int position;
    int stride;
    int last_sound;
} SdvSequence;

extern void sefMemZero(void *data, unsigned int size);

const Vector4 McMathUnitVector = {0.0f, 0.0f, 0.0f, 1.0f};

static int serial_3 = 0;

extern Vector4 *func_A2C3F8(void);

extern int sefGetDmgNull(void);

extern void sefGetPosition(Vector4 *position, void *actor, int part);

extern int _nowEftNo;

extern void func_A31500(float *params, int kind);

extern void func_A31AE0(int kind, void *params);

extern void func_A31BB8(int kind);

extern int func_A31C30(int kind);

extern void func_A31CA0(int value0, int value1, int value2, int value3,
                        int value4);



typedef struct SdvCameraParam {
    int kind;
    unsigned char unmodeled_04[12];
    Vector4 position;
    int actor;
    int actor_number;
    int part;
    int source_actor;
    int source_number;
    int source_part;
    int target_actor;
    int target_number;
    int target_part;
    unsigned char unmodeled_44[40];
    float weight;
} SdvCameraParam;

typedef struct SdvBattleCameraActors {
    int source_actor;
    int target_actor;
    unsigned char unmodeled_08[8];
    short source_number;
    short target_number;
} SdvBattleCameraActors;

void sdvInitAmbient(void)
{
    _sdvAmbFrame = 0;
    _sdvAmbState = 0;
}

void sdvSaveAmbient(void)
{
    void *ambient = xglStudioGetLight2();
    void *map_rgb = func_A2C3F8();

    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r" (_sdvAmbient), "r" (ambient)
        : "$8", "memory");
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r" (_sdvMapRgb), "r" (map_rgb)
        : "$8", "memory");
    _sdvAmbFrame = 0;
    _sdvAmbState = 0;
}

static void sdvSetAmbStateSub(int state, int effect_no, int force)
{
    if (force != 0) {
        _sdvAmbState = state;
        return;
    }

    srsAnalyzeEftNo(effect_no, &charID_0, &eftCate_1);
    if (eftCate_1 == 0xE || effect_no == 0xB25) {
        if ((unsigned int)(effect_no - 0x8FC) >= 0xB6) {
            _sdvAmbState = state;
        }
    }
}

void sdvSetAmbState(int state, int effect_no)
{
    sdvSetAmbStateSub(state, effect_no, 0);
}

void sdvSetAmbState2(int state, int effect_no)
{
    sdvSetAmbStateSub(state, effect_no, 1);
}

void sdvSetAmbient(void *map_rgb, void *ambient)
{
    xglLightIntensityAmbient(xglStudioGetLight2(), ambient);
    func_A2C3D8(map_rgb);
}

void sdvRestoreAmbient(void)
{
    sdvSetAmbient(_sdvMapRgb, _sdvAmbient);
}

void sdvExecAmbient(void)
{
    Vector4 map_rgb;
    Vector4 ambient;
    float parameter;
    int state = _sdvAmbState;

    if (state == 0) {
        return;
    }

    if (state == 1) {
        if (_sdvAmbFrame >= 20) {
            _sdvAmbState = 0;
            _sdvAmbFrame = 20;
            state = 0;
        } else {
            _sdvAmbFrame++;
        }
    } else if (state == 2) {
        if (_sdvAmbFrame <= 0) {
            _sdvAmbFrame = 0;
            state = 0;
            _sdvAmbState = 0;
        } else {
            _sdvAmbFrame--;
        }
    }

    if (state != 0) {
        float half = 0.5f;
        float divisor = 20.0f;

        parameter = (float)_sdvAmbFrame / divisor;
        /* These constrained operations implement the eight evidenced COP2 instructions. */
        __asm__ __volatile__(
            "lqc2 vf1, 0(%1)\n\t"
            "mfc1 $8, %0\n\t"
            "qmtc2 $8, vf2\n\t"
            "vmulx.xyz vf1, vf1, vf2x\n\t"
            "sqc2 vf1, 0(%2)"
            :
            : "f"(half), "r"(&_sdvMapRgb[0]), "r"(&map_rgb)
            : "$8", "memory");
        __asm__ __volatile__(
            "lqc2 vf1, 0(%1)\n\t"
            "mfc1 $8, %0\n\t"
            "qmtc2 $8, vf2\n\t"
            "vmulx.xyz vf1, vf1, vf2x\n\t"
            "sqc2 vf1, 0(%2)"
            :
            : "f"(half), "r"(&_sdvAmbient[0]), "r"(&ambient)
            : "$8", "memory");
        MMathVectorInterpolation(&map_rgb, (const Vector4 *)_sdvMapRgb,
                                 &map_rgb, parameter);
        MMathVectorInterpolation(&ambient, (const Vector4 *)_sdvAmbient,
                                 &ambient, parameter);
        sdvSetAmbient(&map_rgb, &ambient);
    }
}

int sdvExecSeqTbl(SdvSequence *sequence)
{
    int result = -1;
    const short *events = (const short *)sequence->event_address;
    short opcode = events[sequence->position * sequence->stride];
    unsigned char *battle = sefGetBattleData();

    if (opcode == 1024) {
        return 1024;
    }
    if (opcode == 4096) {
        if (battle[22] == 0) {
            sequence->position++;
        } else {
            int *position = &sequence->position;
            if (_hitFlag != 0 || _hitSignal != 0) {
                result = *position;
                *position = result + 1;
            }
        }
    } else if (sequence->elapsed >= opcode) {
        int *position = &sequence->position;
        result = *position;
        *position = result + 1;
    }

    sequence->elapsed++;
    if (sequence->elapsed >= 1024) {
        sequence->elapsed = 1023;
    }
    return result;
}

int sdvProgressKey(const short *sequence, SdvKeyCursor *cursor, int stride)
{
    int result = -1;
    short opcode = sequence[cursor->position * stride];
    unsigned char *battle = sefGetBattleData();

    if (opcode == 1024) {
        return 1024;
    }
    if (opcode == 4096) {
        if (battle[22] == 0) {
            cursor->position++;
        } else if (_hitFlag != 0 || _hitSignal != 0) {
            result = cursor->position;
            cursor->position++;
        }
    } else if (cursor->elapsed >= opcode) {
        result = cursor->position;
        cursor->position++;
    }

    cursor->elapsed++;
    if (cursor->elapsed >= 1024) {
        cursor->elapsed = 1023;
    }
    return result;
}

void sdvPlaySound(int sound_id, int unused, int flags)
{
    xglSoundEffectNormalID(sound_id, 0);
}

void sdvScheduleSound(SdvSequence *sequence)
{
    struct SdvSoundEvent {
        short command;
        short latch;
        int sound_id;
    };
    short latch;
    int sound_id;
    const struct SdvSoundEvent *event;
    int event_index;
    int word_index;
    unsigned int event_address;

    if (sequence == 0) {
        return;
    }
    if (sequence->event_address == 0) {
        return;
    }
    event_index = sdvExecSeqTbl(sequence);
    if ((unsigned int)event_index >= 1024) {
        return;
    }

    word_index = event_index * sequence->stride;
    event_address = word_index * sizeof(short) + sequence->event_address;
    event = (const struct SdvSoundEvent *)event_address;
    sound_id = event->sound_id;
    latch = event->latch;
    if (sound_id > 0) {
        sdvPlaySound(sound_id, 0, 0);
        if (latch > 0) {
            sequence->last_sound = sound_id;
        }
    }
}

int sdvSelectCamera(SdvCameraChoice *choice, int script_base,
                    unsigned short *table)
{
    int weights[8];
    int candidates[8];
    int chosen = 0;
    int roll = rand() % 100;
    int i;
    int weight_sum;

    if (table == 0 || script_base == 0) {
        return 0;
    }
    for (i = 0; i < 5; i++) {
        candidates[i] = scGetImmAdrImmIdx2(script_base, table, i * 2);
        weights[i] = scGetImmNumIdx((short *)table, i * 2 + 1);
    }

    if (sefIsEntryBoss() != 0 && candidates[4] != 0) {
        chosen = candidates[4];
    } else {
        weight_sum = 0;
        for (i = 0; i < 4; i++) {
            weight_sum += weights[i];
            if (roll < weight_sum && candidates[i] != 0) {
                chosen = candidates[i];
                break;
            }
        }
    }

    if (chosen != 0) {
        choice->camera_commands[0][0] =
            scGetImmAdrImmIdx2(script_base, (unsigned short *)chosen, 0);
        choice->camera_commands[1][0] =
            scGetImmAdrImmIdx2(script_base, (unsigned short *)chosen, 1);
        choice->camera_commands[2][0] =
            scGetImmAdrImmIdx2(script_base, (unsigned short *)chosen, 2);
        choice->camera_commands[3][0] =
            scGetImmAdrImmIdx2(script_base, (unsigned short *)chosen, 3);
    }
    choice->script_base = script_base;
    choice->selected = chosen;
    choice->animation = 0;
    return chosen;
}

static void sdvSetCameraParam(void *camera_data, int selection, const short *position)
{
    SdvCameraParam *camera = camera_data;
    Vector4 origin;
    int part = -1;
    int source = 0;
    int kind;
    void *battle_data = sefGetBattleData();
    SdvBattleCameraActors *battle = battle_data;

    if (selection == 256 || selection == 512) {
        kind = 1;
        part = 0;
        if (selection == 256)
            source = 1;
    } else if ((unsigned int)(selection - 257) < 16) {
        kind = 1;
        part = selection - 256;
        source = 1;
    } else if (selection == 529) {
        kind = 1;
        part = sefGetDmgNull() - 512;
    } else if ((unsigned int)(selection - 513) < 16) {
        kind = 1;
        part = selection - 512;
    } else {
        kind = 2;
        if (selection != 32)
            kind = selection == 33 ? 3 : 0;
    }
    memset(camera, 0, 112);
    camera->kind = kind;
    switch (kind) {
    case 0: {
        float x = position[0] * 0.01f;
        float y = position[1] * 0.01f;
        float z = position[2] * 0.01f;
        camera->position.z = z;
        camera->position.x = x;
        camera->position.y = y;
        if (selection == 5) {
            sefGetPosition(&origin, 0, 5);
            __asm__ __volatile__(
                "lqc2 vf1, 0(%0)\n\t"
                "lqc2 vf2, 0(%1)\n\t"
                "vadd.xyz vf1, vf1, vf2\n\t"
                "sqc2 vf1, 0(%0)"
                : : "r"(&camera->position), "r"(&origin) : "memory");
        }
        return;
    }
    case 1:
        camera->actor = source ? battle->source_actor : battle->target_actor;
        {
            short number = source ? battle->source_number : battle->target_number;
            camera->actor_number = number;
            camera->part = part;
        }
        return;
    case 2:
    case 3: {
        int actor = battle->source_actor;
        short target_number = battle->target_number;
        camera->source_actor = actor;
        {
            short source_number = battle->source_number;
            int target_actor = battle->target_actor;
            camera->source_part = 15;
            camera->target_actor = target_actor;
            camera->source_number = source_number;
            camera->target_number = target_number;
            camera->target_part = 15;
        }
        break;
    }
    }
}

static void sdvSetCameraOffset(SdvCamOffset *cam, int kind,
                               const int16_t *vec)
{
    int cam_kind = kind;
    const int16_t *cam_vec = vec;

    memset(cam, 0, sizeof(*cam));
    cam->kind = cam_kind;

    if (cam_kind == 1) {
        cam->pos[0] = (float)cam_vec[0] * 0.01f;
        cam->pos[1] = (float)cam_vec[1] * 0.01f;
        cam->pos[2] = (float)cam_vec[2] * 0.01f;
    } else if (cam_kind == 2) {
        cam->ang[0] = (float)cam_vec[0];
        cam->ang[1] = (float)cam_vec[1];
        cam->ang[2] = (float)cam_vec[2];
    }
}

int sdvProgressPrm(int kind, void *data, int size)
{
    typedef struct SdvCameraParameters {
        float before_offset[28];
        SdvCamOffset offset;
        float after_offset[4];
    } SdvCameraParameters;

    typedef struct SdvCameraInterpolation {
        SdvCameraParameters params;
        int source_byte_3;
        unsigned int source_byte_2;
        int frame_delta;
        short transition_type;
        short unmodeled_be;
    } SdvCameraInterpolation;

    SdvCameraPhase *phase = data;
    const short *keyframes = phase->active;
    const short *current;
    const short *next;
    SdvCameraParameters params;
    SdvCameraInterpolation transition;
    int frame;
    int time_delta;
    int now_effect_no;
    int has_camera_params;
    int in_reset_window;

    frame = sdvProgressKey(keyframes, &phase->cursor, size);
    if ((unsigned int)frame < 0x400U) {
        current = keyframes + frame * size;
        next = current + size;
        time_delta = next[0] - current[0];
        now_effect_no = _nowEftNo;
        in_reset_window =
            (unsigned int)now_effect_no - 2000U < 100U;
        if (now_effect_no == 0x9A1 || now_effect_no == 0x99D) {
            in_reset_window = 1;
        }

        if (kind == 0x41) {
            if (current[1] >= 0) {
                func_A31CA0(current[1], current[2], current[3], current[4],
                            current[5]);
            }
            return frame;
        }

        if (time_delta <= 0 || frame == 0 || current[0] == 0x1000 ||
            next[0] == 0x400) {
            has_camera_params = 0;
            if (func_A31C30(kind) != 0) {
                func_A31BB8(kind);
            }
            memset(&params, 0, 0xB0);
            if ((unsigned int)kind < 2U) {
                has_camera_params = 1;
                sdvSetCameraParam(params.before_offset, current[7], current + 4);
                sdvSetCameraOffset(&params.offset, current[11], current + 8);
                params.before_offset[27] = 0.5f;
            } else if (kind == 3) {
                params.after_offset[1] = (float)current[2];
                params.after_offset[0] = (float)current[3] * 0.017453292f;
                func_A31920(2, params.before_offset);
            }
            func_A31920(kind, params.before_offset);
            if (in_reset_window && has_camera_params) {
                memset(&params, 0, 0xB0);
                func_A31500(&params.before_offset[4], kind);
                params.before_offset[0] = 0.0f;
                params.offset.kind = 0;
                func_A31920(kind, params.before_offset);
            }
        }

        if (time_delta > 0 && next[0] < 0x400) {
            memset(&transition, 0, 0xC0);
            transition.source_byte_3 = (signed char)((unsigned short)current[1] >> 8);
            transition.source_byte_2 = (unsigned short)current[1] & 0xFFU;
            transition.frame_delta = time_delta;
            if ((unsigned int)kind < 2U) {
                sdvSetCameraParam(transition.params.before_offset, next[7], next + 4);
                sdvSetCameraOffset(&transition.params.offset, next[11], next + 8);
                transition.params.before_offset[27] = 0.5f;
                transition.transition_type = 1;
            } else if (kind == 3) {
                if (next[3] != current[3]) {
                    transition.params.after_offset[0] =
                        (float)next[3] * 0.017453292f;
                    func_A31AE0(2, &transition);
                }
                if (next[2] == current[2]) {
                    return frame;
                }
                transition.params.after_offset[1] = (float)next[2];
            }
            func_A31AE0(kind, &transition);
            if (next[size] == 0x400) {
                phase->cursor.position++;
            }
        }
    }

    return frame;
}

void sdvScheduleCamera(SdvCameraTask *task)
{
    float params[44];

    if (_sefBattleMode != 0 && task != 0 && task->active != 0) {
        if (task->pos.active != 0) {
            sdvProgressPrm(0, &task->pos, 0xC);
        }
        if (task->angle.active != 0) {
            sdvProgressPrm(1, &task->angle, 0xC);
        }
        if (task->scale.active != 0) {
            sdvProgressPrm(3, &task->scale, 4);
        } else {
            memset(params, 0, sizeof(params));
            func_A31920(2, params);
            params[41] = 40.0f;
            func_A31920(3, params);
        }
        if (task->offset.active != 0) {
            sdvProgressPrm(0x41, &task->offset, 6);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvTransOffset);

void sdvInitSpecialWork(void)
{
    memset(_sdvSpecialBuf, 0, sizeof(_sdvSpecialBuf));
}

void sdvClearSpecialWork(void)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (_sdvSpecialBuf[i] != 0) {
            GameDefocusSet(i, 0, 0);
            _sdvSpecialBuf[i] = 0;
        }
    }
}

static int sdvAllocSpecialWork(int kind)
{
    int slot;

    if (kind == 0) {
        if (_sdvSpecialBuf[0] == 0) {
            _sdvSpecialBuf[0] = 1;
            return 0;
        }
        return -1;
    }

    for (slot = 1; slot < 8; slot++) {
        if (_sdvSpecialBuf[slot] == 0) {
            _sdvSpecialBuf[slot] = 1;
            return slot;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvScheduleAlter);

void sdvInitAlter(SdvAlter *alter)
{
    sefMemZero(alter, sizeof(*alter));
}

int sdvCreateAlter(SdvAlterParameters *parameters)
{
    int index;
    int unavailable = -1;

    index = 0;
    do {
        if (_sdvAlter[index].active == 0) {
            int serial = (serial_3 + 1) & 255;
            serial_3 = serial;
            _sdvAlter[index].parameters = *parameters;
            _sdvAlter[index].serial = serial_3;
            _sdvAlter[index].active = 1;
            _sdvAlter[index].samples = 0;
            _sdvAlter[index].phase = 0.0f;
            _sdvAlter[index].cursor = 0;
            return (serial << 16) | index;
        }
        ++index;
    } while (index < 16);
    return unavailable;
}

void sdvDestroyAlter(SdvAlter *alter)
{
    sefMemZero(alter, sizeof(*alter));
}

void sdvKillAlter(int alter_id)
{
    int index;
    int serial;

    if (alter_id < 0)
        return;
    index = alter_id & 0xFFFF;
    serial = alter_id >> 16;
    if (_sdvAlter[index].active == 0)
        return;
    if (_sdvAlter[index].serial != serial)
        return;
    sdvDestroyAlter(&_sdvAlter[index]);
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlter);

void sdvInitAlters(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        sdvInitAlter(&_sdvAlter[i]);
    }
}

void sdvDestroyAlters(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        sdvDestroyAlter(&_sdvAlter[i]);
    }
}

unsigned short sdvExecAlters(void)
{
    int i;
    unsigned short status;

    for (i = 0; i < 16; i++) {
        status = _sdvAlter[i].active;
        if (status != 0) {
            status = sdvExecAlter(&_sdvAlter[i]);
        }
    }
    return status;
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlters);
