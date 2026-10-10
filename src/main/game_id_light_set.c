#include "common.h"

#include "game_id_light_set.h"

/* The background parameter block ends in the light preset table's first slot. */

typedef struct GameBgDrawType2 {
    unsigned char unmodeled_00[0x7F0];
    IdLightSet lights[16];
} GameBgDrawType2;

extern GameBgDrawType2 GameBgDrawType2Param;

extern IdLightSet GameIdLight[];

extern void UnduParamInit(void *param);

extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

extern void *xglStudioGetLight2(void);

extern void UnduCheck(void *position, void *exclude, void *param);

extern float I2F(int value);

extern void xglVectorInter(IdLightQuadword *destination, IdLightQuadword *start,
                           const IdLightQuadword *end, float fraction);

/* The background parameter block ends in the light preset table's first slot. */

void GameIdLightSet(IdLightActor *actor, IdLightQuery *query) {
    int id;
    const IdLightSet *target;
    IdLightSet *studio;
    float fraction;

    if (query == 0) {
        query = (IdLightQuery *)0x70000000;
        UnduParamInit(query);
        query->header = UnduDataGetHeader(0x306, 0x8000);
        query->scratch = (void *)0x70000040;
        query->attrMask = 0x810;
        UnduCheck(&actor->position, 0, (void *)0x70000000);
    }

    if ((query->attribute & 0xE000) == 0x6000) {
        id = (query->attribute & 0x1F00) >> 8;
    } else {
        id = 0;
    }

    if (actor->lightId != id) {
        actor->blendSteps = 4;
        if (actor->lightId == 0) {
            studio = xglStudioGetLight2();
            actor->light = *studio;
        }
        actor->lightId = id;
    }

    if (actor->blendSteps == 0) {
        if (id != 0) {
            actor->flags |= 0x8000;
            actor->light = GameBgDrawType2Param.lights[id];
        }
    } else {
        actor->flags |= 0x8000;
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
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r"(&actor->light.lights[0].direction),
                               "r"(&target->lights[0].direction)
                             : "$2", "memory");
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r"(&actor->light.lights[1].direction),
                               "r"(&target->lights[1].direction)
                             : "$2", "memory");
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r"(&actor->light.lights[2].direction),
                               "r"(&target->lights[2].direction)
                             : "$2", "memory");
    }
}
