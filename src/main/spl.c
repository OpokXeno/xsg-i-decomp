#include "common.h"
#include "shared.h"
#include "spl.h"

void SPL_init(SplineState *spline, unsigned int weight_mode,
              const float *samples, unsigned int sample_count,
              unsigned int component_count)
{
    spline->component_count = component_count;
    spline->weight_mode = (unsigned short)weight_mode;
    spline->samples = samples;
    spline->sample_count = (unsigned short)sample_count;

    switch ((int)weight_mode) {
    case 0:
        return;
    case 1:
        SPL_cardinalInit(spline);
        return;
    case 2:
        setWeightLen((SplinePoint *)samples, (int)sample_count);
        return;
    case 3:
        setWeightLen((SplinePoint *)samples, (int)sample_count);
        SPL_cardinalInit(spline);
        break;
    default:
        break;
    }
}

static void setWeightLen2(SplinePoint *points, int count)
{
    int index = 4;
    int limit = count << 2;
    float weight = 0.0f;
    SplinePoint *current;

    points->weight = weight;
    if (index < limit) {
        current = points + 1;
        while (index < limit) {
            Vector4 difference;
            float length;

            difference.x = current->x - current[-1].x;
            difference.y = current->y - current[-1].y;
            difference.z = current->z - current[-1].z;
            current->weight = weight;
            xglVectorLength(&length, &difference);
            weight += length;
            index += 4;
            ++current;
        }
    }
}

static void setWeightLen(SplinePoint *points, int count)
{
    int index = 0;
    int limit = count << 2;
    float weight = 0.0f;
    SplinePoint *current;

    if (index < limit) {
        current = points;

        while (index < limit) {
        Vector4 point;
        float length;

        current->weight = weight;
        point.x = current->x;
        point.y = current->y;
        point.z = current->z;
        xglVectorLength(&length, &point);
        weight += length;
        index += 4;
        ++current;
        }
    }
}

static void setWeightTime(SplinePoint *points, int count, float duration)
{
    int index = 0;
    int limit = count << 2;
    float weight = 0.0f;
    float step = duration / (float)(count - 1);

    while (index < limit) {
        index += 4;
        points->weight = weight;
        weight += step;
        ++points;
    }
}

static void setWeightIndex(SplinePoint *points, int count)
{
    int index = 0;
    int limit = count << 2;
    float weight = 0.0f;

    while (index < limit) {
        index += 4;
        points->weight = weight;
        weight += 1.0f;
        ++points;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/spl", SPL_init2);

INCLUDE_ASM("asm/main/nonmatchings/spl", SPL_cardinalInit);

INCLUDE_ASM("asm/main/nonmatchings/spl", SPL_getValue);

INCLUDE_ASM("asm/main/nonmatchings/spl", SPL_getValueXYZ);
