#include "common.h"
#include "shared.h"

extern float D_004D7F88;
extern float D_004D7F8C;

float D_004D7F88 = 0.3926990926f;
float D_004D7F8C = 0.3926990926f;
extern float xglSin(float angle);
extern float xglCos(float angle);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_uwamono", PauseMenuPageUwamono);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_uwamono", DebDispEvtItem);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_uwamono", DebDispItemBox);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_uwamono", DispDebugUwamono);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_uwamono", DispCollUwamono);

void DispPillar(Vector4 *base, Vector4 *step)
{
    union PillarVertex {
        Vector4 vector;
        u64 words[2];
    } corners[4];
    int segment = 0;
    float angle;
    float angle_increment;

    base->w = 1.0f;
    do {
        /* Compute four local corners for each angular segment. */
        corners[0].vector = *base;
        corners[1].vector = *base;
        angle_increment = D_004D7F88;
        angle = (float)segment * angle_increment;
        corners[0].vector.x += xglSin(angle) * step->x;
        angle_increment = D_004D7F8C;
        corners[0].vector.z += xglCos(angle) * step->x;
        angle = (float)(segment + 1) * angle_increment;
        corners[1].vector.x += xglSin(angle) * step->x;
        corners[1].vector.z += xglCos(angle) * step->x;
        corners[2] = corners[0];
        corners[3] = corners[1];
        corners[2].vector.y += step->y;
        corners[3].vector.y += step->y;
        segment++;
    } while (segment < 17);
}
