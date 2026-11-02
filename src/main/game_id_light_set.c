#include "common.h"
#include "game_id_light_set.h"
#include "main/game_id_light_set.h"
#include "main/Undulate.h"
#include "quadword.h"

#define ID_LIGHT_COUNT 16

/* UnduDataGetHeader group holding the light-id floor attributes, and the unit
 * asked for (the map default). */
#define UNDU_HEADER_ID_LIGHT 0x306
#define UNDU_UNIT_DEFAULT 0x8000

/* Scratchpad layout of the terrain query: the block, then its scratch area. */
#define ID_LIGHT_QUERY_SCRATCH_OFFSET 0x40

/* Ground attribute word: bits 13-15 are the attribute type (3 = light), bits
 * 8-12 the light id. */
#define ATTR_TYPE_MASK 0xE000
#define ATTR_TYPE_LIGHT 0x6000
#define ATTR_ID_MASK 0x1F00
#define ATTR_ID_SHIFT 8

/* IdLightActor.flags bit set whenever the actor's light changes. */
#define ID_LIGHT_FLAG_CHANGED 0x8000

/* Frames an actor's light blends towards a new light id. */
#define ID_LIGHT_BLEND_STEPS 4

/* The background parameter block ends in the light preset table's first slot. */
typedef struct GameBgDrawType2 {
    unsigned char unmodeled_00[0x7F0];
    IdLightSet lights[ID_LIGHT_COUNT];
} GameBgDrawType2;

extern GameBgDrawType2 GameBgDrawType2Param;
extern IdLightSet GameIdLight[];

extern void UnduParamInit(void *param);
extern void *xglStudioGetLight2(void);
extern void UnduCheck(void *position, void *exclude, void *param);
extern float I2F(int value);

extern void xglVectorInter(IdLightQuadword *destination, IdLightQuadword *start,
                           const IdLightQuadword *end, float fraction);

void GameIdLightSet(IdLightActor *actor, IdLightQuery *query) {
    int id;
    const IdLightSet *target;
    IdLightSet *studio;
    float fraction;

    if (query == 0) {
        query = (IdLightQuery *)SCRATCHPAD_BASE;
        UnduParamInit(query);
        query->header = UnduDataGetHeader(UNDU_HEADER_ID_LIGHT, UNDU_UNIT_DEFAULT);
        query->scratch = (void *)(SCRATCHPAD_BASE + ID_LIGHT_QUERY_SCRATCH_OFFSET);
        query->attrMask = 0x810;
        UnduCheck(&actor->position, 0, (void *)SCRATCHPAD_BASE);
    }

    if ((query->attribute & ATTR_TYPE_MASK) == ATTR_TYPE_LIGHT) {
        id = (query->attribute & ATTR_ID_MASK) >> ATTR_ID_SHIFT;
    } else {
        id = 0;
    }

    if (actor->lightId != id) {
        actor->blendSteps = ID_LIGHT_BLEND_STEPS;
        if (actor->lightId == 0) {
            studio = xglStudioGetLight2();
            actor->light = *studio;
        }
        actor->lightId = id;
    }

    if (actor->blendSteps == 0) {
        if (id != 0) {
            actor->flags |= ID_LIGHT_FLAG_CHANGED;
            actor->light = GameBgDrawType2Param.lights[id];
        }
    } else {
        actor->flags |= ID_LIGHT_FLAG_CHANGED;
        if (id == 0) {
            target = xglStudioGetLight2();
        } else {
            target = &GameIdLight[id] - 1;
        }
        actor->blendSteps--;
        fraction = 1.0f / I2F(actor->blendSteps + 1);
        xglVectorInter(&actor->light.ambientColor, &actor->light.ambientColor,
                       &target->ambientColor, fraction);
        xglVectorInter(&actor->light.lights[0].color, &actor->light.lights[0].color,
                       &target->lights[0].color, fraction);
        xglVectorInter(&actor->light.lights[1].color, &actor->light.lights[1].color,
                       &target->lights[1].color, fraction);
        xglVectorInter(&actor->light.lights[2].color, &actor->light.lights[2].color,
                       &target->lights[2].color, fraction);
        QuadwordCopy(&actor->light.lights[0].direction, &target->lights[0].direction);
        QuadwordCopy(&actor->light.lights[1].direction, &target->lights[1].direction);
        QuadwordCopy(&actor->light.lights[2].direction, &target->lights[2].direction);
    }
}
