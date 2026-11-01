#include "common.h"
#include "shared.h"
#include "spl.h"

float minCardinalInterval = 9.9999999747524271e-07f;

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

void SPL_init2(SplineState *spline, int interpolation_mode, float *samples,
               int sample_count, unsigned int component_count)
{
    if ((spline->first_key & 0x0f) != 8) {
        unsigned short weight_scheme = spline->first_key & 0xf0;
        interpolation_mode += 2;
        switch (weight_scheme) {
        case 0:
            setWeightLen((SplinePoint *)samples, sample_count);
            break;
        case 0x20:
            setWeightLen2((SplinePoint *)samples, sample_count);
            break;
        case 0x40:
            setWeightTime((SplinePoint *)samples, sample_count,
                          (float)(short)spline->last_key);
            break;
        case 0x30:
            setWeightIndex((SplinePoint *)samples, sample_count);
            break;
        }
    }
    spline->first_key = spline->first_key & 0x0f;
    spline->samples = samples;
    spline->sample_count = sample_count;
    spline->component_count = component_count;
    spline->weight_mode = interpolation_mode;
    switch (interpolation_mode) {
    case 3:
    case 1:
        SPL_cardinalInit(spline);
        break;
    case 2:
    case 0:
        return;
    }
}

static void SPL_cardinalInit(SplineState *spline)
{
    struct SplineWorkspace *workspace = (struct SplineWorkspace *)0x70000000;
    float *next_curvature_cell;
    int component = 0;
    float *linear = spline->coefficients[SPL_LINEAR_TERM];
    float *quadratic = spline->coefficients[SPL_QUADRATIC_TERM];
    float *cubic = spline->coefficients[SPL_CUBIC_TERM];
    unsigned short sample_count = spline->sample_count;
    int components = spline->component_count;
    const float *samples = spline->samples;
    int last = (short)sample_count - 1;
    int penultimate;
    int final_interval;

    if (components <= 0)
        return;
    penultimate = (short)sample_count - 2;
    final_interval = last - 1;
    do {
        int segment;
        float *last_curvature = &workspace->second_derivative[last];

        {
            segment = 0;
            if (segment < last) {
                unsigned int stride = spline->component_count + 1;
                do {
                    int current = segment * stride;
                    int next = (segment + 1) * stride;
                    float interval = samples[next] - samples[current];

                    workspace->interval[segment] = interval;
                    if (interval == 0.0f)
                        workspace->interval[segment] = minCardinalInterval;
                    workspace->slope[segment] =
                        (samples[next + component + 1] - samples[current + component + 1]) /
                        workspace->interval[segment];
                } while (++segment < last);
            }

            workspace->diagonal[0] =
                (workspace->interval[0] + workspace->interval[1]) * 2.0f;
            workspace->right_hand_side[0] =
                (workspace->slope[1] - workspace->slope[0]) * 6.0f;
            for (segment = 1; segment < last; ++segment) {
                float previous_interval = workspace->interval[segment - 1];
                float previous_diagonal = workspace->diagonal[segment - 1];

                workspace->diagonal[segment] =
                    (workspace->interval[segment] + previous_interval) * 2.0f -
                    previous_interval * previous_interval / previous_diagonal;
                workspace->right_hand_side[segment] =
                    (workspace->slope[segment] - workspace->slope[segment - 1]) * 6.0f -
                    previous_interval * workspace->right_hand_side[segment - 1] /
                    previous_diagonal;
            }
        }

        workspace->second_derivative[0] = 0.0f;
        *last_curvature = 0.0f;
        for (segment = penultimate; segment > 0; --segment) {
            workspace->second_derivative[segment] =
                (workspace->right_hand_side[segment] -
                 workspace->interval[segment] * workspace->second_derivative[segment + 1]) /
                workspace->diagonal[segment];
        }

        segment = 0;
        if (segment <= final_interval) {
            float (*curvatures)[32] = &workspace->second_derivative;
            do {
                unsigned int stride = (unsigned int)components + 1;
                int index = segment * stride + component;
                float *second_derivative = &(*curvatures)[segment];
                float interval;
                float difference;

                quadratic[index] = second_derivative[0] * 0.5f;
                next_curvature_cell = &second_derivative[1];
                interval = workspace->interval[segment];
                difference = samples[(segment + 1) * stride + component + 1] -
                    samples[index + 1];
                linear[index] =
                    (-interval / 6.0f) * *next_curvature_cell -
                    (interval / 3.0f) * second_derivative[0] +
                    (1.0f / interval) * difference;
                {
                    float next_curvature = *next_curvature_cell;
                    float current_curvature = (*curvatures)[segment];
                    float curvature_change = next_curvature - current_curvature;

                    cubic[index] =
                        (1.0f / (workspace->interval[segment] * 6.0f)) *
                        curvature_change;
                }
            } while (++segment <= penultimate);
        }
        ++component;
    } while (component < components);
}

float SPL_getValue(void *spline, int index, float frame)
{
    SplineState *state = spline;
    const float *samples = state->samples;
    const float *time;
    const float *row_base;
    unsigned int components = state->component_count;
    const float *linear = state->coefficients[SPL_LINEAR_TERM];
    const float *quadratic = state->coefficients[SPL_QUADRATIC_TERM];
    const float *cubic = state->coefficients[SPL_CUBIC_TERM];
    int last = (short)state->sample_count - 1;
    int selection;
    int sample_offset = last * (components + 1);
    float distance = 0.0f;

    ++index;
    if (frame <= samples[0])
        return samples[index];
    if (samples[sample_offset] <= frame)
        return samples[sample_offset + index];
    row_base = samples;
    selection = (short)state->weight_mode;
    switch (selection) {
    case 0: {
        int upper_offset;
        unsigned int first_row_address;
        float value;
        float difference;

        first_row_address = components * sizeof(float) + (unsigned int)row_base;
        time = (const float *)first_row_address;
        selection = 1;
        if (time[1] <= frame) {
            do {
                if (frame == row_base[selection * (components + 1)])
                    return row_base[selection * (components + 1) + index];
                ++selection;
            } while (row_base[selection * (components + 1)] <= frame);
        }
        upper_offset = selection * (components + 1);
        sample_offset = (selection - 1) * (components + 1);
        distance = (frame - row_base[sample_offset]) /
                   (row_base[upper_offset] - row_base[sample_offset]);
        value = row_base[sample_offset + index];
        difference = row_base[upper_offset + index] - value;
        return value + distance * difference;
    }
    case 1: {
        int component_offset;
        unsigned int previous_values_address;
        unsigned int component_bytes;
        int reverse_bytes;
        float value;
        float difference;
        float sample_time;

        if ((selection = last) >= 0) {
            component_bytes = components * sizeof(float);
            time = row_base + selection * (components + 1) + 1;
            while (selection >= 0) {
                sample_time = *--time;
                distance = frame - sample_time;
                if (distance >= 0.0f)
                    break;
                reverse_bytes = -component_bytes;
                previous_values_address = reverse_bytes + (unsigned int)time;
                time = (const float *)previous_values_address;
                --selection;
            }
        }
        sample_offset = selection * (components + 1);
        component_offset = sample_offset + index;
        value = row_base[component_offset];
        difference = linear[component_offset - 1] + distance *
            (quadratic[component_offset - 1] + distance * cubic[component_offset - 1]);
        return value + distance * difference;
    }
    default:
        return 0.0f;
    }
}

void SPL_getValueXYZ(float *destination, void *state, float frame)
{
    const SplineState *spline = state;
    int components;
    unsigned int stride;
    int last;
    int end;
    float elapsed;
    float weight;
    const float *samples;
    const float (*linear)[96];
    const float (*quadratic)[96];
    const float (*cubic)[96];
    unsigned int sample_count;
    int segment;
    int row_index;
    short easing_mode;
    short weight_mode;

    components = spline->component_count;
    stride = spline->component_count + 1;
    last = (short)spline->sample_count - 1;
    end = last * stride;
    elapsed = 0.0f;
    samples = spline->samples;
    linear = &spline->coefficients[SPL_LINEAR_TERM];
    quadratic = &spline->coefficients[SPL_QUADRATIC_TERM];
    cubic = &spline->coefficients[SPL_CUBIC_TERM];
    sample_count = spline->sample_count;

    easing_mode = (short)spline->first_key;
    switch (easing_mode) {
    case 0:
        weight = frame * (1.0f / (float)(short)spline->last_key);
        frame = weight * weight * samples[end];
        break;
    case 1: {
        float duration = samples[end];
        weight = 1.0f - frame * (1.0f / (float)(short)spline->last_key);
        frame = duration - weight * weight * duration;
        break;
    }
    case 2:
        weight = frame * (1.0f / (float)(short)spline->last_key);
        frame = weight * samples[end];
        break;
    case 3: {
        float square = frame * frame;
        float inverse = 1.0f / (float)(short)spline->last_key;
        float inverse_square = inverse * inverse;
        frame = samples[end] *
            (square * 3.0f * inverse_square -
             2.0f * (square * frame * inverse_square) * inverse);
        break;
    }
    }

    if (frame <= samples[0]) {
        destination[0] = samples[1];
        destination[1] = samples[2];
        destination[2] = samples[3];
        return;
    }
    if (samples[end] <= frame) {
        destination[0] = samples[end + 1];
        destination[1] = samples[end + 2];
        destination[2] = samples[end + 3];
        return;
    }

    weight_mode = (short)spline->weight_mode;
    switch (weight_mode) {
    case 0:
    case 2: {
        segment = 1;
        while (samples[segment * (components + 1)] <= frame) {
            if (frame == samples[segment * (components + 1)]) {
                destination[0] = samples[segment * (components + 1) + 1];
                destination[1] = samples[segment * (components + 1) + 2];
                destination[2] = samples[segment * (components + 1) + 3];
                return;
            }
            ++segment;
        }
        row_index = segment * stride;
        end = (segment - 1) * stride;
        elapsed = (frame - samples[end]) / (samples[row_index] - samples[end]);
        for (segment = 0; segment < components; ++segment) {
            float difference =
                samples[row_index + segment + 1] - samples[end + segment + 1];
            destination[segment] =
                samples[end + segment + 1] + difference * elapsed;
        }
        return;
    }
    case 1:
    case 3:
        segment = (short)sample_count - 1;
        if (segment >= 0) {
            elapsed = frame - samples[segment * ((unsigned int)components + 1)];
            if (!(elapsed >= 0.0f)) {
                do {
                    --segment;
                    if (segment < 0)
                        break;
                    elapsed = frame - samples[segment * ((unsigned int)components + 1)];
                } while (!(elapsed >= 0.0f));
            }
        }
        row_index = segment * stride;
        for (segment = 0; segment < components; ++segment) {
            int index = segment + row_index;
            destination[segment] = samples[index + 1] + elapsed *
                ((*linear)[index] + elapsed *
                 ((*quadratic)[index] + elapsed * (*cubic)[index]));
        }
        return;
    }
}
