#include "common.h"

#include "shared.h"

#include "main/xgl_cd.h"

typedef struct RadarActor {
    u32 flags;
    void (*update)(struct RadarActor *actor);
    void (*draw)(struct RadarActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    u8 unmodeled_70[0x10];
    u8 number;
    u8 unmodeled_81[5];
    short inUseId;
    u8 unmodeled_88[0x454];
    float radarVisibility;
    u8 unmodeled_4e0[0x41C];
    struct RadarActor *parent;
    u8 unmodeled_900[0x170];
} RadarActor;

#define image_004DC554 radarImage

#define rate radarRate

static u8 *radarImage;

static u8 radarRate;

static const char D_004C0618[32] = "data\\yajima\\cf_rader00.xtx";

extern u8 *image_004DC554;

extern u8 rate;

extern u8 ping;

extern u32 cnt_2;

static u32 sPoint_3[28] = {
    0, 0, 0, 0x51000006,
    0x00008001, 0x50624000, 0x0005551e, 0,
    0x44, 0, 0x42, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

extern float xglSin(float);

extern float xglCos(float);

extern const float D_004D7C7C;

extern const float D_004D7C80;

extern const float D_004D7C84;

extern void nmlModelDirectSend(int, u8 *, int);

typedef unsigned int GameLoopStateWords[];

extern GameLoopStateWords GameLoopState;

typedef struct EnemyWork {
    u8 unmodeled_00[0x48];
    signed char type;
    u8 unmodeled_49[0x3797];
    u32 flags;
    u8 unmodeled_37e4[0xCC];
} EnemyWork;

extern EnemyWork enepc[16];

extern RadarActor actor[64];

int CheckActorExist(RadarActor *actor);

static void DrawActorPoint(float angle, int *position, u32 *color);

typedef struct RadarPoint {
    int x;
    int y;
    int depth;
    int fog;
} RadarPoint;

typedef struct RadarMarkerColor {
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
} RadarMarkerColor;

typedef struct RadarOrigin { int x, y, depth; u8 red, green, blue, alpha; } RadarOrigin;

typedef struct RadarTextureUV {
    int u, v;
    u32 unmodeled_08[2];
} RadarTextureUV;

typedef struct RadarSpritePacket {
    u32 dma[4], giftag[4];
    u64 primitive;
    u32 primitiveRegister;
    u32 unmodeled_2c;
    u64 alpha;
    u32 alphaRegister;
    u32 unmodeled_3c;
    RadarMarkerColor color;
    RadarTextureUV uv0;
    RadarPoint xyz0;
    RadarTextureUV uv1;
    RadarPoint xyz1;
} RadarSpritePacket;

typedef struct RadarVertex { u32 color[4]; int position[4]; } RadarVertex;

typedef struct RadarPingHeader {
    u32 dma[4], scissorTag[4];
    u64 scissor;
    u32 scissorRegister;
    u32 unmodeled_2c;
    u32 vertexTag[4];
} RadarPingHeader;

typedef struct RadarPingEnd {
    u32 tag[4];
    u64 scissor;
    u32 scissorRegister;
    u32 unmodeled_1c;
} RadarPingEnd;

int WIN_checkActiveWindow(void);

void nmlModelDirectSendXtx(int, u8 *);

float I2F(int);

int F2I(float);

static void DrawRadarPing(int *);

static void DrawRadarInfo(int *);

static inline void RadarSpritePacketInit(RadarSpritePacket **packet)
{
    *packet = (RadarSpritePacket *)0x70000000;
}

void GameRadarDraw(RadarOrigin *origin)
{
    extern u32 TestEnv_0_00369BA0[28];
    RadarSpritePacket *packet;
    u64 secondSpritePrimitive;
    if (!SaveData[0x100a6] || !(GameLoopState[4] & 0x80000) || WIN_checkActiveWindow()) {
        if (!rate) return;
        rate >>= 1;
    } else {
        int difference = 128 - rate;
        if (difference >= 8) difference >>= 1;
        rate += difference;
    }
    RadarSpritePacketInit(&packet);
    nmlModelDirectSendXtx(5, image_004DC554);
    nmlModelDirectSend(6, (u8 *)TestEnv_0_00369BA0, 7);
    packet->dma[0] = 0;
    packet->dma[1] = 0;
    packet->dma[2] = 0;
    packet->dma[3] = 0x51000008;
    packet->giftag[0] = 0x8001;
    packet->giftag[1] = 0x70ab4000;
    packet->giftag[2] = 0x053531ee;
    packet->giftag[3] = 0;
    packet->primitiveRegister = 6;
    packet->alpha = 0x44;
    packet->alphaRegister = 0x42;
    packet->color.red = origin->red;
    packet->color.green = origin->green;
    packet->color.blue = origin->blue;
    packet->color.alpha = origin->alpha * rate >> 7;
    packet->primitive = 0x2007670621343800ULL;
    {
        RadarPoint * const lastPosition = &packet->xyz1;
        int depth = origin->depth;
        lastPosition->depth = depth;
        packet->xyz0.depth = depth;
        lastPosition->fog = 0;
        packet->xyz0.fog = 0;
    }
    {
    const int xOrigin = origin->x * 16;
    const int yOrigin = origin->y * 16;
    int innerLeft;
    int bottom;
    int top;
    int right;
    int outerLeft;
    int outerTop;
    int outerBottom;
    int outerRight;
    packet->uv0.u = 0;
    outerLeft = xOrigin + 0x6ff8;
    outerTop = yOrigin + 0x71f7;
    right = xOrigin + 0x7878;
    top = yOrigin + 0x7277;
    outerRight = right + 128;
    outerBottom = yOrigin + 0x7af7;
    bottom = yOrigin + 0x7a77;
    packet->xyz0.y = top; packet->uv0.v = 0;
    innerLeft = xOrigin + 0x7078;
    packet->xyz0.x = innerLeft;
    packet->uv1.u = 0x800; packet->uv1.v = 0x800;
    packet->xyz1.y = bottom; packet->xyz1.x = right;
    secondSpritePrimitive = 0x2007678621343800ULL;
    nmlModelDirectSend(5, (u8 *)packet, 9);
    packet->uv0.u = 0x800;
    packet->primitive = secondSpritePrimitive;
    packet->uv1.u = 0x880; packet->uv1.v = 0x900;
    packet->xyz0.x = outerLeft; packet->xyz0.y = outerTop;
    packet->xyz1.x = innerLeft; packet->xyz1.y = outerBottom;
    nmlModelDirectSend(5, (u8 *)packet, 9);
    packet->uv0.u = 0x880; packet->uv1.u = 0x900; packet->xyz0.x = right;
    packet->xyz1.x = outerRight;
    nmlModelDirectSend(5, (u8 *)packet, 9);
    }
    DrawRadarPing(&origin->x);
    DrawRadarInfo(&origin->x);
}

static void DrawRadarPing(int *origin)
{
    extern u32 TestEnv_1_00369C10[16];
    RadarVertex *vertex = (RadarVertex *)0x70000040;
    RadarPingEnd *end;
    int segment = 0;
    int xOrigin;
    int yOrigin;
    nmlModelDirectSend(6, (u8 *)TestEnv_1_00369C10, 4);
    {
    RadarPingHeader *header = (RadarPingHeader *)0x70000000;
    u64 scissor;
    header->dma[0] = 0; header->dma[1] = 0; header->dma[2] = 0;
    header->dma[3] = 0x51000069;
    header->scissorTag[0] = 1;
    header->scissorTag[1] = 0x10000000; header->scissorTag[2] = 14;
    header->scissorTag[3] = 0;
    scissor = (u64)(origin[0] + 16) | ((u64)(origin[0] + 128) << 16)
        | ((u64)(origin[1] + 16) << 32) | ((u64)(origin[1] + 128) << 48);
    header->scissorRegister = 0x40;
    header->vertexTag[0] = 25; header->vertexTag[1] = 0x40264000;
    header->vertexTag[2] = 0x5151; header->vertexTag[3] = 0;
    header->scissor = scissor;
    xOrigin = origin[0] * 16 + 0x7478;
    yOrigin = origin[1] * 16 + 0x7677;
    }
    do {
        float innerRadius = I2F(ping) * 0.5f;
        float outerRadius = I2F(ping + 16) * 0.5f;
        float angle = I2F(segment) * D_004D7C7C / 24.0f;
        float sine;
        float cosine;
        innerRadius *= 16.0f; outerRadius *= 16.0f; segment++;
        sine = xglSin(angle); cosine = xglCos(angle);
        vertex[0].color[0] = 96; vertex[0].color[1] = 96;
        vertex[0].color[2] = 128; vertex[0].color[3] = 0;
        vertex[0].position[0] = xOrigin + F2I(sine * innerRadius);
        vertex[0].position[1] = yOrigin + F2I(cosine * innerRadius);
        {
            u32 alpha = rate >> 1;
            int depth = origin[2] + 1;
            vertex[1].color[3] = alpha;
            vertex[0].position[3] = 0;
            vertex[0].position[2] = depth;
        }
        vertex[1].color[0] = 96; vertex[1].color[1] = 96; vertex[1].color[2] = 128;
        vertex[1].position[0] = xOrigin + F2I(sine * outerRadius);
        vertex[1].position[1] = yOrigin + F2I(cosine * outerRadius);
        vertex[1].position[2] = origin[2] + 1; vertex[1].position[3] = 0;
        vertex += 2;
    } while (segment < 25);
    end = (RadarPingEnd *)vertex;
    end->tag[0] = 0x8001;
    end->tag[1] = 0x10000000; end->tag[2] = 14;
    end->scissor = 0x01bf000001ff0000ULL; end->tag[3] = 0; end->scissorRegister = 0x40;
    nmlModelDirectSend(5, (u8 *)0x70000000, 106);
    if (!(GameLoopState[4] & 1)) ping += 8;
}

static void DrawRadarInfo(int *origin)
{

    RadarMarkerColor color;
    RadarPoint point;
    RadarActor *player = (RadarActor *)GameLoopState[1];
    RadarActor *current;
    int xOrigin;
    int yOrigin;
    int actorIndex;
    int xDistance;
    int yDistance;
    int absX;
    int absY;
    signed char actorType;

    xOrigin = origin[0] * 16 + 0x7498;
    yOrigin = origin[1] * 16 + 0x7680;
    for (actorIndex = 0; actorIndex < 64; actorIndex++) {
        if (CheckActorExist(&actor[actorIndex]) != 0 && actorIndex != player->number) {
            current = &actor[actorIndex];
            xDistance = (int)((current->position.x - player->position.x) * 64.0f);
            yDistance = (int)((current->position.z - player->position.z) * 64.0f);
            if (!(current->flags & 0x80)) {
                absX = xDistance < 0 ? -xDistance : xDistance;
                if (absX < 0x381) {
                    absY = yDistance < 0 ? -yDistance : yDistance;
                    if (absY < 0x301) {
                        if (enepc[current->number].flags & 1) {
                            if (current->radarVisibility == -1000.0f) {
                                continue;
                            }
                            actorType = enepc[actorIndex].type;
                            if (actorType == 6 || actorType == 10) {
                                color.green = 0;
                                color.red = cnt_2;
                                color.blue = 0;
                            } else {
                                color.red = 255;
                                color.green = 255;
                                color.blue = 0;
                            }
                        } else {
                            color.red = 0;
                            color.green = 255;
                            color.blue = 0;
                        }
                        color.alpha = 0x80;
                        point.x = xOrigin + xDistance;
                        point.y = yOrigin + yDistance;
                        point.depth = origin[2] + 2;
                        point.fog = 0;
                        DrawActorPoint(current->rotation.y, &point.x, &color.red);
                    }
                }
            }
        }
    }

    color.red = 0xFF;
    color.green = 0;
    color.blue = 0;
    color.alpha = 0x80;
    point.x = xOrigin;
    point.y = yOrigin;
    point.depth = origin[2] + 3;
    point.fog = 0;
    DrawActorPoint(player->rotation.y, &point.x, &color.red);
    cnt_2 = (cnt_2 + 0x10) & 0xFF;
}

static void DrawActorPoint(float angle, int *position, u32 *color)
{



    if (angle > D_004D7C80 || angle < D_004D7C84) {
        return;
    }

    sPoint_3[12] = color[0];
    sPoint_3[13] = color[1];
    sPoint_3[14] = color[2];
    sPoint_3[15] = color[3] * rate >> 7;

    position[0] -= xglSin(angle) * 48.0f;
    position[1] -= xglCos(angle) * 48.0f;

    sPoint_3[16] = position[0] + xglSin(angle) * 128.0f;
    sPoint_3[17] = position[1] + xglCos(angle) * 128.0f;
    sPoint_3[18] = position[2];
    sPoint_3[19] = position[3];

    sPoint_3[20] = position[0] + xglCos(angle) * 48.0f;
    sPoint_3[21] = position[1] - xglSin(angle) * 48.0f;
    sPoint_3[22] = position[2];
    sPoint_3[23] = position[3];

    sPoint_3[24] = position[0] - xglCos(angle) * 48.0f;
    sPoint_3[25] = position[1] + xglSin(angle) * 48.0f;
    sPoint_3[26] = position[2];
    sPoint_3[27] = position[3];

    nmlModelDirectSend(5, (u8 *)sPoint_3, 7);
}

int CheckActorExist(RadarActor *actor)
{
    if (actor->inUseId <= 0) {
        return 0;
    }
    if (actor->flags & 8) {
        return 0;
    }
    return actor->parent == 0;
}

void GameRadarInit(void)
{
    image_004DC554 = (u8 *) (((int) WorkEnd + 0xF) & ~0xF);
    WorkEnd = image_004DC554 + xglCdReadFile(D_004C0618, image_004DC554, 0, 0);
    rate = 0;
}
