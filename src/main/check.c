#include "common.h"
#include "check.h"
#include "main/check.h"
#include "main/act_1.h"

/*
 * Returns the index of the first actor within reach of `position` (the sum of
 * its body radius and actor `number`'s radius), or -1 if there is none.
 */
int Check_Actor(const Vector4 *position, short number)
{
    short i;

    for (i = 0; i < ACTOR_COUNT; ++i) {
        float distance;

        if (actor[i].sound_effect_id <= 0)
            continue;
        if (actor[i].kind == 1)
            continue;
        if ((enepc[number].state_flags & 1) &&
            &actor[i] == GameLoopState.player)
            continue;
        if ((enepc[i].state_flags & 1) &&
            &actor[number] == GameLoopState.player)
            continue;
        if (actor[i].body.radius == 0.0f)
            continue;
        if (actor[i].flags & 8)
            continue;
        if (number == i)
            continue;
        if (__builtin_fabsf(actor[i].position.y - position->y) >= 1.0f)
            continue;
        distance = Get_Distance(position, &actor[i].position);
        if (distance <= actor[number].body.radius + actor[i].body.radius)
            break;
    }

    return i != ACTOR_COUNT ? i : -1;
}
