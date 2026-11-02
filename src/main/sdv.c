#include "common.h"
#include "shared.h"
#include "main/sdv.h"
#include "sdv.h"
#include "main/m_ef_obj.h"
#include "main/m_math.h"
#include "main/sef.h"
#include "main/sef_print.h"

/* A key field halfway between two keys: blend is the 8-bit fraction. */
#define SDV_BLEND(dest, field) \
    (dest) = (key1->field == key0->field ? key0->field : (key0->field + (((key1->field - key0->field) * blend + 128) >> 8)) & 0xffff)

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union SdvQuadColor {
    Quadword quad;
    unsigned int word[4];
} SdvQuadColor;

typedef union SdvColor {
    struct {
        unsigned int red : 8;
        unsigned int green : 8;
        unsigned int blue : 8;
        unsigned int alpha : 8;
    } channel;
    int word;
} SdvColor;

typedef union SdvScreenQuad {
    SdvScreenPoint point;
    Quadword quad;
} SdvScreenQuad;

typedef struct SdvSequence {
    unsigned int event_address;
    int script_base;
    int elapsed;
    int position;
    int stride;
    int last_sound;
} SdvSequence;

typedef struct SdvSpecial {
    SdvSequence sequence;
    unsigned char unmodeled_18[8];
    SdvSpecialSlot slots[7];
} SdvSpecial;

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

/* A key viewed as raw bytes: the char member makes the copy alias every
 * type, so the cached slot fields are reloaded after it. */
typedef union SdvKeyBytes {
    SdvSpecialKey key;
    char byte[sizeof(SdvSpecialKey)];
} SdvKeyBytes;

typedef union SdvTextureCoordinate {
    float stq[4];
    Quadword quad;
} SdvTextureCoordinate;

extern short _hitFlag;
extern short _hitSignal;
extern const char SdvOpcodeNameAconstNull[16];
extern const char SdvOpcodeNameIconstM1[16];
extern const char SdvOpcodeNameIconst0[16];
extern const char SdvOpcodeNameIconst1[16];
extern const char SdvOpcodeNameIconst2[16];
extern const char SdvOpcodeNameIconst3[16];
extern const char SdvOpcodeNameIconst4[16];
extern const char SdvOpcodeNameIconst5[16];
extern const char SdvOpcodeNameLconst0[16];
extern const char SdvOpcodeNameLconst1[16];
extern const char SdvOpcodeNameFconst0[16];
extern const char SdvOpcodeNameFconst1[16];
extern const char SdvOpcodeNameFconst2[16];
extern const char SdvOpcodeNameDconst0[16];
extern const char SdvOpcodeNameDconst1[16];
extern const char SdvOpcodeNameIstore0[16];
extern const char SdvOpcodeNameIstore1[16];
extern const char SdvOpcodeNameIstore2[16];
extern const char SdvOpcodeNameIstore3[16];
extern const char SdvOpcodeNameLstore0[16];
extern const char SdvOpcodeNameLstore1[16];
extern const char SdvOpcodeNameLstore2[16];
extern const char SdvOpcodeNameLstore3[16];
extern const char SdvOpcodeNameFstore0[16];
extern const char SdvOpcodeNameFstore1[16];
extern const char SdvOpcodeNameFstore2[16];
extern const char SdvOpcodeNameFstore3[16];
extern const char SdvOpcodeNameDstore0[16];
extern const char SdvOpcodeNameDstore1[16];
extern const char SdvOpcodeNameDstore2[16];
extern const char SdvOpcodeNameDstore3[16];
extern const char SdvOpcodeNameAstore0[16];
extern const char SdvOpcodeNameAstore1[16];
extern const char SdvOpcodeNameAstore2[16];
extern const char SdvOpcodeNameAstore3[16];
extern const char SdvOpcodeNameInt2byte[16];
extern const char SdvOpcodeNameInt2char[16];
extern const char SdvOpcodeNameInt2short[16];
extern const char SdvOpcodeNameIfIcmpeq[16];
extern const char SdvOpcodeNameIfIcmpne[16];
extern const char SdvOpcodeNameIfIcmplt[16];
extern const char SdvOpcodeNameIfIcmpge[16];
extern const char SdvOpcodeNameIfIcmpgt[16];
extern const char SdvOpcodeNameIfIcmple[16];
extern const char SdvOpcodeNameIfAcmpeq[16];
extern const char SdvOpcodeNameIfAcmpne[16];
extern const char SdvOpcodeNameTableswitch[16];
extern const char SdvOpcodeNameLookupswitch[16];
extern const char SdvOpcodeNameGetstatic[16];
extern const char SdvOpcodeNamePutstatic[16];
extern const char SdvOpcodeNameGetfield[16];
extern const char SdvOpcodeNamePutfield[16];
extern const char SdvOpcodeNameInvokevirtual[16];
extern const char SdvOpcodeNameInvokespecial[16];
extern const char SdvOpcodeNameInvokestatic[16];
extern const char SdvOpcodeNameInvokeinterface[16];
extern const char SdvOpcodeNameNewarray[16];
extern const char SdvOpcodeNameAnewarray[16];
extern const char SdvOpcodeNameArraylength[16];
extern const char SdvOpcodeNameCheckcast[16];
extern const char SdvOpcodeNameInstanceof[16];
extern const char SdvOpcodeNameMonitorenter[16];
extern const char SdvOpcodeNameMonitorexit[16];
extern const char SdvOpcodeNameMultianewarray[16];
extern const char SdvOpcodeNameIfnonnull[16];
extern const char SdvOpcodeNameBreakpoint[16];
extern const char SdvOpcodeNameCheckcastFast[16];
extern const char SdvOpcodeNameInstanceofFast[16];

extern void *memset(void *, int, unsigned int);

extern Vector4 *MMathVectorInterpolation(Vector4 *destination,
                                         const Vector4 *first,
                                         const Vector4 *second,
                                         float parameter);

extern void sefMemZero(void *data, unsigned int size);
extern Vector4 *func_A2C3F8(void);
extern int sefGetDmgNull(void);
extern void sefGetPosition(Vector4 *position, void *actor, int part);
extern void func_A31500(float *params, int kind);
extern void func_A31AE0(int kind, void *params);
extern void func_A31BB8(int kind);
extern int func_A31C30(int kind);

extern void func_A31CA0(int value0, int value1, int value2, int value3,
                        int value4);

extern StudioCamera *xglStudioGetActiveCamera(void);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const void *matrix,
                                 const Vector4 *point);

extern int MMathRotTransPersClip(SdvScreenPoint *destination, const void *screen,
                                 const void *matrix, const void *point);

extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
extern void sefGetWeaponPosition(Vector4 *position, void *weapon, int part);
extern void SGsInitGifPacket(SGsPacket *packet);

extern void SGsOpenGifPacket(SGsPacket *packet, int prim,
                             unsigned long long registers, unsigned int nreg);

extern void SGsCloseGifPacket(SGsPacket *packet);

extern void SGsAddReg(SGsPacket *packet, unsigned long long address,
                      unsigned long long value);

extern void SGsAddGifSTQ(SGsPacket *packet, float texture_s, float texture_t,
                         float texture_q);

extern void SGsAddGifRGBA(SGsPacket *packet, Quadword rgba);
extern void SGsAddGifData(SGsPacket *packet, Quadword data);
extern void SGsSendPacket(SGsPacket *packet);

static float _sdvMapRgb[4] = {1.0f, 1.0f, 1.0f, 1.0f};

static float _sdvAmbient[4] = {0.25f, 0.25f, 0.25f, 1.0f};

static short blksiz_2[9] = { 1, 3, 7, 15, 31, 63, 127, 255, 511 };

static int _sdvAmbFrame = 0;

static int _sdvAmbState = 0;

unsigned char orgData[33][0x180] = {{0}};

GamePlayerRecord plChaData[16] = { { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

unsigned short itmBox[255] = {0};

unsigned short wpnBox[255] = {0};

unsigned short bltBox[255] = {0};

unsigned short accBox[255] = {0};

unsigned short evtBox[255] = {0};

unsigned long long moneyBox = 0;

BattleDataStorage batDatBuf = {0};

unsigned char thinkBuf[0x4000] = {0};

MEfObjCamParams mefCamParams = {0};

/*
 * Partial object-representation view. The original mdstr is a GLOBAL
 * 256-byte OBJECT in initialized .data, with every byte zero. Its original
 * source type and use are unresolved; these unsigned-character units describe
 * storage only, without string, element-format, or pointer-layout semantics.
 */
unsigned char mdstr[256] = {0};

static int serial_3 = 0;

const char *instructionCodeName[240] = {
    sdvOpNameNop, SdvOpcodeNameAconstNull, SdvOpcodeNameIconstM1, SdvOpcodeNameIconst0, SdvOpcodeNameIconst1, SdvOpcodeNameIconst2,
    SdvOpcodeNameIconst3, SdvOpcodeNameIconst4, SdvOpcodeNameIconst5, SdvOpcodeNameLconst0, SdvOpcodeNameLconst1, SdvOpcodeNameFconst0,
    SdvOpcodeNameFconst1, SdvOpcodeNameFconst2, SdvOpcodeNameDconst0, SdvOpcodeNameDconst1, sdvOpNameBipush, sdvOpNameSipush,
    sdvOpNameLdc1, sdvOpNameLdc2, sdvOpNameLdc2w, sdvOpNameIload, sdvOpNameLload, sdvOpNameFload,
    sdvOpNameDload, sdvOpNameAload, sdvOpNameIload0, sdvOpNameIload1, sdvOpNameIload2, sdvOpNameIload3,
    sdvOpNameLload0, sdvOpNameLload1, sdvOpNameLload2, sdvOpNameLload3, sdvOpNameFload0, sdvOpNameFload1,
    sdvOpNameFload2, sdvOpNameFload3, sdvOpNameDload0, sdvOpNameDload1, sdvOpNameDload2, sdvOpNameDload3,
    sdvOpNameAload0, sdvOpNameAload1, sdvOpNameAload2, sdvOpNameAload3, sdvOpNameIaload, sdvOpNameLaload,
    sdvOpNameFaload, sdvOpNameDaload, sdvOpNameAaload, sdvOpNameBaload, sdvOpNameCaload, sdvOpNameSaload,
    sdvOpNameIstore, sdvOpNameLstore, sdvOpNameFstore, sdvOpNameDstore, sdvOpNameAstore, SdvOpcodeNameIstore0,
    SdvOpcodeNameIstore1, SdvOpcodeNameIstore2, SdvOpcodeNameIstore3, SdvOpcodeNameLstore0, SdvOpcodeNameLstore1, SdvOpcodeNameLstore2,
    SdvOpcodeNameLstore3, SdvOpcodeNameFstore0, SdvOpcodeNameFstore1, SdvOpcodeNameFstore2, SdvOpcodeNameFstore3, SdvOpcodeNameDstore0,
    SdvOpcodeNameDstore1, SdvOpcodeNameDstore2, SdvOpcodeNameDstore3, SdvOpcodeNameAstore0, SdvOpcodeNameAstore1, SdvOpcodeNameAstore2,
    SdvOpcodeNameAstore3, sdvOpNameIastore, sdvOpNameLastore, sdvOpNameFastore, sdvOpNameDastore, sdvOpNameAastore,
    sdvOpNameBastore, sdvOpNameCastore, sdvOpNameSastore, sdvOpNamePop, sdvOpNamePop2, sdvOpNameDup,
    sdvOpNameDupX1, sdvOpNameDupX2, sdvOpNameDup2, sdvOpNameDup2X1, sdvOpNameDup2X2, sdvOpNameSwap,
    sdvOpNameIadd, sdvOpNameLadd, sdvOpNameFadd, sdvOpNameDadd, sdvOpNameIsub, sdvOpNameLsub,
    sdvOpNameFsub, sdvOpNameDsub, sdvOpNameImul, sdvOpNameLmul, sdvOpNameFmul, sdvOpNameDmul,
    sdvOpNameIdiv, sdvOpNameLdiv, sdvOpNameFdiv, sdvOpNameDdiv, sdvOpNameIrem, sdvOpNameLrem,
    sdvOpNameFrem, sdvOpNameDrem, sdvOpNameIneg, sdvOpNameLneg, sdvOpNameFneg, sdvOpNameDneg,
    sdvOpNameIshl, sdvOpNameLshl, sdvOpNameIshr, sdvOpNameLshr, sdvOpNameIushr, sdvOpNameLushr,
    sdvOpNameIand, sdvOpNameLand, sdvOpNameIor, sdvOpNameLor, sdvOpNameIxor, sdvOpNameLxor,
    sdvOpNameIinc, sdvOpNameI2l, sdvOpNameI2f, sdvOpNameI2d, sdvOpNameL2i, sdvOpNameL2f,
    sdvOpNameL2d, sdvOpNameF2i, sdvOpNameF2l, sdvOpNameF2d, sdvOpNameD2i, sdvOpNameD2l,
    sdvOpNameD2f, SdvOpcodeNameInt2byte, SdvOpcodeNameInt2char, SdvOpcodeNameInt2short, sdvOpNameLcmp, sdvOpNameFcmpl,
    sdvOpNameFcmpg, sdvOpNameDcmpl, sdvOpNameDcmpg, sdvOpNameIfeq, sdvOpNameIfne, sdvOpNameIflt,
    sdvOpNameIfge, sdvOpNameIfgt, sdvOpNameIfle, SdvOpcodeNameIfIcmpeq, SdvOpcodeNameIfIcmpne, SdvOpcodeNameIfIcmplt,
    SdvOpcodeNameIfIcmpge, SdvOpcodeNameIfIcmpgt, SdvOpcodeNameIfIcmple, SdvOpcodeNameIfAcmpeq, SdvOpcodeNameIfAcmpne, sdvOpNameGoto,
    sdvOpNameJsr, sdvOpNameRet, SdvOpcodeNameTableswitch, SdvOpcodeNameLookupswitch, sdvOpNameIreturn, sdvOpNameLreturn,
    sdvOpNameFreturn, sdvOpNameDreturn, sdvOpNameAreturn, sdvOpNameReturn, SdvOpcodeNameGetstatic, SdvOpcodeNamePutstatic,
    SdvOpcodeNameGetfield, SdvOpcodeNamePutfield, SdvOpcodeNameInvokevirtual, SdvOpcodeNameInvokespecial, SdvOpcodeNameInvokestatic, SdvOpcodeNameInvokeinterface,
    sdvOpNameUnknown, sdvOpNameNew, SdvOpcodeNameNewarray, SdvOpcodeNameAnewarray, SdvOpcodeNameArraylength, sdvOpNameAthrow,
    SdvOpcodeNameCheckcast, SdvOpcodeNameInstanceof, SdvOpcodeNameMonitorenter, SdvOpcodeNameMonitorexit, sdvOpNameWide, SdvOpcodeNameMultianewarray,
    sdvOpNameIfnull, SdvOpcodeNameIfnonnull, sdvOpNameGotoW, sdvOpNameJsrW, SdvOpcodeNameBreakpoint, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown,
    sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, sdvOpNameUnknown, SdvOpcodeNameCheckcastFast, SdvOpcodeNameInstanceofFast
};

static short _sdvSpecialBuf[8];

static SdvAlter _sdvAlter[16];

static int charID_0;

static int eftCate_1;

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const unsigned long long alphaTbl_004CC8D8[4];

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

static int sdvTransOffset(SdvScreenPoint *screen, SdvScreenPoint *offset,
                          SdvSpecialSlot *slot, SdvSpecialKey *key)
{
    Vector4 position;
    Vector4 local_offset;
    Matrix4 local_matrix;
    Matrix4 view_matrix;
    SdvScheduler *scheduler;
    StudioCamera *camera;
    int part = slot->table->part;

    if (part == 3) {
        __asm__ __volatile__(
            "lq $8, 0(%1)\n\t"
            "sq $8, 0(%0)"
            :
            : "r"(screen), "r"(offset)
            : "$8", "memory");
        screen->x += 256;
        screen->y += 224;
        screen->z = key->depth_step << 16;
        return 1;
    }

    scheduler = sefGetNowScheduler();
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "vitof0.xyzw vf1, vf1\n\t"
        "sqc2 vf1, 0(%1)"
        :
        : "r"(offset), "r"(&local_offset)
        : "memory");
    local_offset.w = 1.0f;
    local_offset.z *= 0.01f;
    sefGetPosition(&position, &local_offset, part);
    if (scheduler->flags & 2) {
        MMathMulMatrix(&local_matrix, &slot->matrix, &scheduler->matrix);
        MMathApplyMatrix(&position, local_matrix, &position);
    }
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "vmove.w vf1, vf0\n\t"
        "vmr32.xyzw vf2, vf0\n\t"
        "vmr32.xyzw vf3, vf2\n\t"
        "vmr32.xyzw vf4, vf3\n\t"
        "sqc2 vf1, 48(%1)\n\t"
        "sqc2 vf2, 32(%1)\n\t"
        "sqc2 vf3, 16(%1)\n\t"
        "sqc2 vf4, 0(%1)"
        :
        : "r"(&position), "r"(local_matrix)
        : "memory");
    camera = xglStudioGetActiveCamera();
    MMathMulMatrix(&view_matrix, &camera->viewMatrix, &local_matrix);
    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&position) : "memory");
    if (MMathRotTransPersClip(screen, camera, view_matrix, &position) != 0)
        return 0;
    screen->x = (screen->x >> 4) - 1792;
    screen->y = (screen->y >> 4) - 1824;
    if (key->depth_step != 0) {
        screen->z += key->depth_step;
        if (screen->z > 0xF000)
            screen->z = 0xF000;
    }
    return 1;
}

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

static inline void sdvCopyKey(SdvSpecialKey *destination, const SdvSpecialKey *source)
{
    ((SdvKeyBytes *)destination)->key = ((const SdvKeyBytes *)source)->key;
}

void sdvExecSpecial(SdvSpecial *special)
{
    int index;
    int n;
    SdvScheduler *scheduler;
    SdvSpecialSlot *slot;

    if (special == 0) {
        return;
    }
    if (special->sequence.event_address == 0) {
        return;
    }
    index = sdvExecSeqTbl(&special->sequence);
    if ((unsigned int)index < 1024) {
        scheduler = sefGetNowScheduler();
        for (n = 0; n < 7; n++) {
            slot = &special->slots[n];
            if (slot->table == 0) {
                slot->table = (SdvSpecialTable *)scGetImmAdrImmIdx2(
                    special->sequence.script_base,
                    (unsigned short *)(special->sequence.event_address +
                                       index * special->sequence.stride * sizeof(short)),
                    1);
                slot->frame = 0;
                slot->index = 0;
                __asm__ __volatile__(
                    "lq $8, 0(%1)\n\t"
                    "lq $9, 16(%1)\n\t"
                    "lq $10, 32(%1)\n\t"
                    "lq $11, 48(%1)\n\t"
                    "sq $8, 0(%0)\n\t"
                    "sq $9, 16(%0)\n\t"
                    "sq $10, 32(%0)\n\t"
                    "sq $11, 48(%0)"
                    :
                    : "r"(slot->matrix), "r"(scheduler->base_matrix)
                    : "$8", "$9", "$10", "$11", "memory");
                break;
            }
        }
    }

    for (n = 0; n < special->sequence.position && n < 7; n++) {
        const SdvSpecialKey *keys;
        const SdvSpecialKey *key0;
        const SdvSpecialKey *key1;
        int blend;

        slot = &special->slots[n];
        if (slot->table == 0) {
            continue;
        }
        keys = slot->table->keys;
        key0 = &keys[slot->index];
        key1 = key0 + 1;
        if (key0->time != 1024) {
            if (key0->time < slot->frame && key1->time > 1024) {
                slot->table = 0;
            } else if (key1->time >= 1024) {
                sdvCopyKey(&slot->key, key0);
            } else if (key0->time != key1->time) {
                blend = ((slot->frame - key0->time) << 8) / (key1->time - key0->time);
                SDV_BLEND(slot->key.depth_step, depth_step);
                SDV_BLEND(slot->key.color[0], color[0]);
                SDV_BLEND(slot->key.color[1], color[1]);
                SDV_BLEND(slot->key.color[2], color[2]);
                SDV_BLEND(slot->key.color[3], color[3]);
                SDV_BLEND(slot->key.x, x);
                SDV_BLEND(slot->key.y, y);
                SDV_BLEND(slot->key.width, width);
                SDV_BLEND(slot->key.height, height);
                SDV_BLEND(slot->key.intensity, intensity);
                slot->key.z = key1->z == key0->z
                    ? key0->z
                    : (key0->z + (((key1->intensity - key0->z) * blend + 128) >> 8)) & 0xffff;
            }
        }
        if (slot->frame < 1024) {
            slot->frame++;
            if (slot->frame >= key1->time) {
                slot->index++;
            }
        }
    }
}

void sdvDrawSpecial(SdvSpecial *special)
{
    int params[16];
    SdvScreenPoint offset;
    SdvScreenPoint screen;
    SdvColor color;
    int n;

    for (n = 0; n < special->sequence.position && n < 7; n++) {
        SdvSpecialSlot *slot = &special->slots[n];
        SdvSpecialKey *key;
        int work;
        int size;
        unsigned int i;

        if (slot->table == 0) {
            continue;
        }
        key = &slot->key;
        params[0] = 0;
        switch (slot->table->kind) {
        case 7:
            if (key->color[3] == 0) {
                break;
            }
            offset.x = key->x;
            offset.y = key->y;
            offset.z = key->z != -1 ? key->z : 0;
            if (sdvTransOffset(&screen, &offset, slot, key) == 0) {
                break;
            }
            params[1] = 1;
            params[0] = 0;
            if (sdvAllocSpecialWork(0) >= 0) {
                GameDefocusSet(0, 2, params);
            }
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            params[0] = screen.z;
            params[2] = screen.x;
            params[1] = color.word;
            params[3] = screen.y;
            params[4] = key->width;
            params[5] = key->height;
            params[6] = key->intensity;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 7, params);
            }
            break;
        case 6:
            if (key->color[3] == 0) {
                break;
            }
            params[1] = 1;
            params[0] = 0;
            if (sdvAllocSpecialWork(0) >= 0) {
                GameDefocusSet(0, 2, params);
            }
            i = key->depth_step != 0 ? key->depth_step : 0xfffff8;
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            params[0] = i;
            params[2] = key->x;
            params[3] = key->y;
            params[4] = key->width;
            params[5] = key->height;
            params[1] = color.word;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 6, params);
            }
            break;
        case 5:
            if (key->color[3] == 0) {
                break;
            }
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            size = 1;
            params[0] = key->depth_step;
            params[1] = color.word;
            params[2] = key->x;
            params[3] = key->y;
            params[4] = key->width;
            params[5] = key->height;
            for (i = 0; i < 9; i++) {
                if (blksiz_2[i] >= key->intensity) {
                    size = blksiz_2[i];
                    break;
                }
            }
            params[6] = size;
            params[7] = size;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 5, params);
            }
            break;
        case 3:
            if (key->color[3] == 0) {
                break;
            }
            GameDefocusSet(0, 2, params);
            params[0] = key->intensity;
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            params[2] = key->x;
            params[1] = color.word;
            params[3] = key->y;
            params[4] = key->width;
            params[5] = key->height;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 3, params);
            }
            break;
        case 1:
            if (key->color[3] == 0) {
                break;
            }
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            params[1] = key->intensity;
            params[2] = key->depth_step;
            params[4] = key->x;
            params[5] = key->y;
            params[6] = key->width;
            params[7] = key->height;
            params[0] = 0;
            params[3] = color.word;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 1, params);
            }
            break;
        case 4:
            if (key->color[3] == 0) {
                break;
            }
            {
                unsigned char t1 = (key)->color[1];
                (color).channel.red = (key)->color[0];
                (color).channel.green = t1;
                (color).channel.blue = (key)->color[2];
                (color).channel.alpha = (key)->color[3];
            };
            params[0] = 1;
            params[3] = color.word;
            params[4] = 0;
            params[5] = 0;
            params[6] = 0;
            params[7] = 0;
            params[1] = 1;
            params[2] = key->depth_step;
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 1, params);
            }
            break;
        case 9:
            work = sdvAllocSpecialWork(1);
            if (work > 0) {
                GameDefocusSet(work, 9, params);
            }
            break;
        }
    }
}

void sdvScheduleAlter(SdvSequence *sequence, SdvAlterRequest *request)
{
    struct SdvAlterImagePair {
        short primary_index;
        short secondary_index;
    };
    struct SdvAlterEvent {
        unsigned short unmodeled_00;
        short life;
        unsigned short mode;
        unsigned short blend;
        short rgba[4];
        short selector[2];
        short max_samples;
        unsigned short shrink;
        unsigned short image_frames;
        short length;
        struct SdvAlterImagePair images[2];
    };
    struct SdvAlterImageItem {
        unsigned int texture_word;
        unsigned char unmodeled_04[14];
        unsigned short buffer_pointer;
        unsigned short format;
        unsigned short buffer_width;
        unsigned char unmodeled_18[4];
        unsigned short width_log2;
        unsigned short height_log2;
    };
    extern int sefSearchMapperIndex(int effect_no);
    extern void *svGetImageListItem(int mapper_index, unsigned int image_index);
    struct SdvAlterEvent *event;
    short *image_index;
    struct SdvAlterImageItem *primary;
    struct SdvAlterImageItem *secondary;
    SdvAlterParameters parameters;
    int character_id;
    int event_index;
    int category;
    int mapper_index;
    int psm_default;
    int psm_indexed;
    int psm;
    unsigned long long image_base_bits;
    unsigned long long image_alpha_bits;
    unsigned long long descriptor_format;
    unsigned long long descriptor_image;
    unsigned long long descriptor_dimensions;
    unsigned long long descriptor_base;
    int remaining_pairs;
    short primary_index;
    short secondary_index;

    if (sequence == 0)
        return;
    if (sequence->event_address == 0)
        return;

    event_index = sdvExecSeqTbl(sequence);
    if ((unsigned int)event_index >= 0x400)
        return;

    event = (struct SdvAlterEvent *)(sequence->event_address
        + event_index * sequence->stride * 2);
    memset(&parameters, 0, sizeof(parameters));
    parameters.request = *request;
    parameters.life = event->life;
    parameters.selector[0] = event->selector[0];
    parameters.selector[1] = event->selector[1];
    parameters.max_samples = event->max_samples < 0x81
        ? event->max_samples : 0x80;
    parameters.shrink = event->shrink;
    parameters.mode = event->mode;
    parameters.blend = event->blend;
    parameters.rgba.word[0] = event->rgba[0];
    parameters.rgba.word[1] = event->rgba[1];
    parameters.rgba.word[2] = event->rgba[2];
    parameters.rgba.word[3] = event->rgba[3];
    parameters.image_frames = event->image_frames;
    parameters.length = (float)event->length;
    parameters.category = 0xE;

    srsAnalyzeEftNo(request->effect_no, &character_id, &category);
    if (category == 0)
        category = 0xE;

    psm_default = 20;
    psm_indexed = 19;
    image_base_bits = (unsigned long long)0x8000 << 46;
    image_alpha_bits = (unsigned long long)0x8000 << 19;
    mapper_index = sefSearchMapperIndex(request->effect_no);
    image_index = &event->images[0].secondary_index;
    for (remaining_pairs = 1; remaining_pairs >= 0; remaining_pairs--) {
        primary_index = image_index[-1];
        secondary_index = image_index[0];
        if (primary_index >= 0 && secondary_index >= 0) {
            primary = svGetImageListItem(mapper_index,
                primary_index);
            secondary = svGetImageListItem(mapper_index,
                secondary_index);
            if (primary != 0 && secondary != 0) {
                unsigned short image_category;

                psm = primary->format == 8 ? psm_indexed : psm_default;
                descriptor_dimensions =
                    ((unsigned long long)primary->width_log2 << 26)
                    | image_alpha_bits;
                descriptor_format = ((unsigned long long)psm << 20)
                    | ((unsigned long long)(primary->buffer_width >> 6) << 14);
                descriptor_base = (unsigned long long)primary->buffer_pointer
                    | image_base_bits;
                descriptor_image = ((unsigned long long)primary->height_log2 << 30)
                    | ((unsigned long long)secondary->buffer_pointer << 37);
                image_category = category;
                descriptor_format |= descriptor_image;
                descriptor_base |= descriptor_dimensions;
                parameters.tex0[parameters.image_count] =
                    descriptor_base | descriptor_format;
                parameters.image_count++;
                parameters.category = image_category;
            }
        }
        image_index += 2;
    }

    sdvCreateAlter(&parameters);
}

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

void sdvExecAlter(SdvAlter *alter)
{
    int target_selector;
    int i;

    alter->parameters.life--;
    if (alter->parameters.life > 0) {
        if (alter->cursor < 128) {
            if (alter->cursor < alter->parameters.max_samples) {
                alter->cursor++;
            }
        }
    } else {
        alter->cursor -= alter->parameters.shrink;
    }
    if (alter->cursor > 0) {
        for (i = 0; i < 2; i++) {
            int selector = alter->parameters.selector[i];
            SdvActorPosition *actor;
            Vector4 *slot = &alter->trail[i][alter->samples];

            __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(slot) : "memory");
            if (selector == 529)
                selector = sefGetDmgNull();
            if (selector == 256) {
                actor = alter->parameters.request.source_actor;
                goto copy;
            } else if (selector == 512) {
                actor = alter->parameters.request.target_actor;
            copy:
                if (actor != 0) {
                    __asm__ __volatile__(
                        "lq $8, 0(%1)\n\t"
                        "sq $8, 0(%0)"
                        :
                        : "r"(slot), "r"(&actor->position)
                        : "$8", "memory");
                }
            } else if (selector & 0x100) {
                sefGetNullPosition(slot, alter->parameters.request.source_actor,
                                   selector - 257, 1);
            } else if (selector & 0x200) {
                sefGetNullPosition(slot, alter->parameters.request.target_actor,
                                   selector - 513, 1);
            } else if (selector & 0x400) {
                sefGetWeaponPosition(slot, alter->parameters.request.weapon,
                                     selector - 1024);
            } else {
                sefGetPosition(&alter->trail[i][alter->samples], 0,
                               alter->parameters.selector[i]);
            }
        }
        if (alter->parameters.mode >= 3) {
            float phase = alter->phase;

            alter->phase = phase + alter->parameters.length / alter->parameters.max_samples;
            if (alter->parameters.mode == 3) {
                if (alter->wrapped == 0)
                    alter->level[alter->samples] = phase;
                else
                    alter->level[alter->samples] = 4.0f;
            }
            if (alter->parameters.mode == 4) {
                alter->level[alter->samples] = phase;
                if (alter->phase > 20.0f)
                    alter->phase -= 20.0f;
            }
        }
        alter->samples++;
        if (alter->samples >= 128) {
            alter->wrapped = 1;
            alter->samples &= 0x7f;
        }
    } else if (alter->parameters.life < 0) {
        sdvDestroyAlter(alter);
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const unsigned long long alphaTbl_004CC8D8[4] = {68ULL, 72ULL, 549755813954ULL, 106ULL};

static const SdvTextureCoordinate stqTbl_004CC900[4] = {
    {{0.0f, 0.0f, 1.0f, 0.0f}},
    {{1.0f, 0.0f, 1.0f, 0.0f}},
    {{0.0f, 1.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f, 0.0f}}
};

const Vector4 McMathUnitVector = {0.0f, 0.0f, 0.0f, 1.0f};

const float McMathUnitMatrix[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

void sdvDrawAlter(SdvAlter *alter, SGsPacket *packet)
{
    HermiteVector tangent[2];
    HermiteVector points[2][8];
    SdvScreenQuad screen;
    SdvQuadColor color;
    Matrix4 view;
    int i;
    int j;
    int k;
    int head;
    StudioCamera *camera;
    int count;
    int counter; /* Hermite sample position, then the strand fade countdown */
    int previous;
    int current;
    int next;
    int after;
    int previous_offset;
    int after_offset;
    float alpha;
    float alpha_step = 0.0f;
    float s_step = 0.0f;
    float textureS;

    count = alter->cursor;
    head = alter->samples - 1;
    camera = xglStudioGetActiveCamera();
    textureS = alter->phase;
    SGsInitGifPacket(packet);
    if (alter->parameters.mode == 2) {
        alter->phase -= alter->parameters.length /
                        ((float)alter->parameters.max_samples * 2.0f);
        if (alter->phase < -20.0f) {
            alter->phase += 20.0f;
        }
    }
    color.quad = alter->parameters.rgba.quad;
    alpha = (float)alter->parameters.rgba.word[3];
    if (count >= 2) {
        /* s_step holds the per-segment alpha quotient before its own value. */
        s_step = alpha / (count - 1);
        alpha_step = s_step * 0.25f;
        s_step = alter->parameters.length / (count - 1);
        s_step *= 0.25f;
    }
    if (alter->parameters.image_count != 0) {
        SGsOpenGifPacket(packet, 92, 3822, 3);
        alter->parameters.image_timer++;
        if (alter->parameters.image_timer >= alter->parameters.image_frames) {
            alter->parameters.image_timer = 0;
            alter->parameters.image_index++;
            if (alter->parameters.image_index >= alter->parameters.image_count) {
                alter->parameters.image_index = 0;
            }
        }
        SGsAddReg(packet, 6,
                  alter->parameters.tex0[alter->parameters.image_index]);
        if (alter->parameters.mode == 3) {
            SGsAddReg(packet, 8, 5);
        } else {
            SGsAddReg(packet, 8, 4);
        }
    } else {
        SGsOpenGifPacket(packet, 76, 14, 1);
    }
    SGsAddReg(packet, 66, alphaTbl_004CC8D8[alter->parameters.blend]);
    packet->loopCount = 1;
    SGsCloseGifPacket(packet);
    __asm__ __volatile__(
        "lq $8, 0(%1)\n\t"
        "lq $9, 16(%1)\n\t"
        "lq $10, 32(%1)\n\t"
        "lq $11, 48(%1)\n\t"
        "sq $8, 0(%0)\n\t"
        "sq $9, 16(%0)\n\t"
        "sq $10, 32(%0)\n\t"
        "sq $11, 48(%0)"
        :
        : "r"(view), "r"(camera->viewMatrix)
        : "$8", "$9", "$10", "$11", "memory");
    for (i = 0; i < count - 1; i++) {
        current = (head - i) & 0x7f;
        if (i == 0)
            previous = current;
        else
            previous = (current + 1) & 0x7f;
        if (i >= count - 2) {
            next = (current - 1) & 0x7f;
            after = next;
        } else {
            next = (current - 1) & 0x7f;
            after = (current - 2) & 0x7f;
        }
        previous_offset = previous * sizeof(HermiteVector);
        after_offset = after * sizeof(HermiteVector);
        for (j = 0; j < 2; j++) {
            HermiteVector *trail = (HermiteVector *)alter->trail[j];
            HermiteVector *out;

            MMathCalcHermitePrm(&tangent[0], &tangent[1],
                                (HermiteVector *)((char *)trail + previous_offset), &trail[current],
                                &trail[next], (HermiteVector *)((char *)trail + after_offset));
            /* The row address is built on integers, as in the original. */
            out = (HermiteVector *)(j * sizeof(points[0]) + (unsigned int)points);
            for (counter = 0; counter <= 100; counter += 25) {
                MMathCalcHermite((float *)out, (float)counter * 0.01f, &tangent[0],
                                 &tangent[1], &trail[current], &trail[next]);
                out++;
            }
        }
        if (alter->parameters.image_count != 0) {
            SGsOpenGifPacket(packet, 92, 1298, 3);
        } else {
            SGsOpenGifPacket(packet, 76, 1298, 3);
        }
        if (alter->parameters.mode >= 3) {
            textureS = alter->level[current];
            s_step = (alter->level[next] - textureS) * 0.25f;
        }
        counter = 0;
        for (k = 0; k < 5; k++) {
            color.word[3] = (unsigned int)alpha;
            for (j = 0; j < 2; j++) {
                MMathRotTransPersClip(&screen.point, camera, view, &points[j][k]);
                if (screen.point.w != 0) {
                    counter = 2;
                } else {
                    counter--;
                    if (counter >= 0) {
                        screen.point.w = 0x8000;
                    }
                }
                if (j == 0) {
                    SGsAddGifSTQ(packet, textureS, 0.0f, 1.0f);
                } else {
                    SGsAddGifSTQ(packet, textureS, 1.0f, 1.0f);
                }
                SGsAddGifRGBA(packet, color.quad);
                SGsAddGifData(packet, screen.quad);
            }
            if (k < 4) {
                textureS += s_step;
                alpha -= alpha_step;
            }
        }
        packet->loopCount = 10;
        SGsCloseGifPacket(packet);
        if ((i & 7) == 7) {
            SGsSendPacket(packet);
        }
    }
    SGsSendPacket(packet);
}

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

void sdvExecAlters(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (_sdvAlter[i].active != 0) {
            sdvExecAlter(&_sdvAlter[i]);
        }
    }
}

void sdvDrawAlters(int category)
{
    int i;
    int drawn = 0;

    for (i = 0; i < 16; i++) {
        if (_sdvAlter[i].active != 0 && _sdvAlter[i].parameters.category == category) {
            drawn++;
            if (drawn == 1) {
                SGsInitEnv();
            }
            sdvDrawAlter(&_sdvAlter[i], &_packet);
        }
    }
}
