#include "common.h"
#include "jc_face.h"

static inline void copyMatrix(XglQuadword *destination,
                              const XglQuadword *source)
{
    __asm__ __volatile__(
        "lq $2, 0(%0)\n"
        "sq $2, 0(%1)\n"
        "lq $2, 16(%0)\n"
        "sq $2, 16(%1)\n"
        "lq $2, 32(%0)\n"
        "sq $2, 32(%1)\n"
        "lq $2, 48(%0)\n"
        "sq $2, 48(%1)\n"
        : : "r"(source), "r"(destination) : "$2", "memory");
}

JcElement *JC_face(JcWork *work, JcElement *elements, JcChain *chain)
{
    JcElement *element_array;
    JcElement *cursor;
    Matrix *destination;
    Matrix *root_destination;
    void (*filter)(void *, void *, int);
    int chain_length;
    unsigned int flags_after_scan;
    float frame;
    int target_index;
    int previous_index;
    int interpolate_end;
    int bound;
    int slot;
    const Matrix *matrix_table;

    element_array = elements;
    if (work->matrix_buffer != 0) {
        if (chain == 0) {
            return 0;
        }
        matrix_table = *chain->source;
        if (matrix_table != 0) {
            goto matrix_available;
        }
    }
    return 0;
matrix_available:
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &work->matrix_buffer[chain->index];
        {
            __asm__ __volatile__(
                "lq $2, 0(%1)\n"
                "sq $2, 0(%0)\n"
                "lq $2, 16(%1)\n"
                "sq $2, 16(%0)\n"
                "lq $2, 32(%1)\n"
                "sq $2, 32(%0)\n"
                "lq $2, 48(%1)\n"
                "sq $2, 48(%0)\n"
                : : "r"(destination), "r"(chain->static_index * sizeof(*matrix_table) + (unsigned int)matrix_table)
                : "$2", "memory");
        }
        flags_after_copy = work->flags | 8;
        destination->elements[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->elements[7] = 1.0f;
        destination->elements[11] = 1.0f;
        return 0;
    }

    chain_length = 0;
    work->flags |= 0x18;
    cursor = JNT_getElement(element_array, chain->index);
    if (cursor->type == 2) {
        do {
            chain_length++;
            cursor = JNT_nextElement(cursor);
        } while (cursor->type != 4);
    }
    flags_after_scan = work->flags;

    if (flags_after_scan & 0x20) {
        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        target_index = bound;
        if (chain->index == 0) {
            destination = &work->matrix_buffer[chain->index];
            copyMatrix(destination, &matrix_table[chain->static_index]);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            copyMatrix(root_destination, &matrix_table[0]);
            copyMatrix(destination, &matrix_table[chain->static_index]);
        }
        destination->elements[3] = destination->elements[7] =
            destination->elements[11] = 1.0f;

        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal(work, cursor);
        }
        JNT_setInterrupt(work, work->producer);
        previous_index = work->current_index;
        work->current_index = target_index;
        cursor = &element_array[target_index];
        if (target_index < previous_index) {
            do {
                cursor = JNT_constructMatrix(work, cursor);
            } while (work->current_index < previous_index);
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                Matrix *previous_destination;
                matrix_table = *chain->source;
                root_destination = work->matrix_buffer;
                root_destination += chain->index;
                previous_destination = root_destination - 1;
                {
                    __asm__ __volatile__(
                        "lq $2, 0(%1)\n"
                        "sq $2, 0(%0)\n"
                        "lq $2, 16(%1)\n"
                        "sq $2, 16(%0)\n"
                        "lq $2, 32(%1)\n"
                        "sq $2, 32(%0)\n"
                        "lq $2, 48(%1)\n"
                        "sq $2, 48(%0)\n"
                    : : "r"(previous_destination), "r"(chain->static_index * sizeof(*matrix_table) + (unsigned int)matrix_table)
                        : "$2", "memory");
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < previous_index && cursor->type != 4) {
                do {
                    int destination_index = chain->index + slot;
                    int source_index = chain->static_index + slot;
                    unsigned int destination = destination_index * sizeof(Matrix) + (unsigned int)work->matrix_buffer;
                    unsigned int source = source_index * sizeof(*matrix_table) + (unsigned int)matrix_table;
                    {
                        __asm__ __volatile__(
                            "lq $2, 0(%1)\n"
                            "sq $2, 0(%0)\n"
                            "lq $2, 16(%1)\n"
                            "sq $2, 16(%0)\n"
                            "lq $2, 32(%1)\n"
                            "sq $2, 32(%0)\n"
                            "lq $2, 48(%1)\n"
                            "sq $2, 48(%0)\n"
                        : : "r"(destination), "r"(source)
                            : "$2", "memory");
                    }
                    slot++;
                    cursor = JNT_nextElement(cursor);
                    work->current_index++;
                } while (work->current_index < previous_index && cursor->type != 4);
            }
            cursor = &element_array[work->current_index];
            interpolate_end = previous_index;
            while (work->current_index < interpolate_end) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
    } else {
        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getVal2(work, cursor, frame);
        }
        target_index = bound;
        if (chain->index == 0) {
            destination = &work->matrix_buffer[chain->index];
            copyMatrix(destination, &matrix_table[chain->static_index]);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            copyMatrix(root_destination, &matrix_table[0]);
            copyMatrix(destination, &matrix_table[chain->static_index]);
        }
        destination->elements[3] = destination->elements[7] =
            destination->elements[11] = 1.0f;

        bound = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
        if (work->attribute_key == work->expected_attribute_key) {
            while (work->current_index < bound) {
                cursor = JNT_getVal(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
            filter = JNT_getFilter(work, work->producer);
            if (filter != 0) {
                filter(work, element_array, 0);
            }
            JNT_setInterrupt(work, work->producer);
            previous_index = work->current_index;
            work->current_index = target_index;
            cursor = &element_array[target_index];
            if (target_index < previous_index) {
                do {
                    cursor = JNT_constructMatrix(work, cursor);
                } while (work->current_index < previous_index);
            }
        } else {
            while (work->current_index < bound) {
                cursor = JNT_getVal_staticChain(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
            filter = JNT_getFilter(work, work->producer);
            if (filter != 0) {
                filter(work, element_array, 0);
            }
            JNT_setInterrupt(work, work->producer);
            previous_index = work->current_index;
            work->current_index = target_index;
            cursor = &element_array[target_index];
            if (target_index < previous_index) {
                do {
                    cursor = JNT_constructMatrix(work, cursor);
                } while (work->current_index < previous_index);
            }
        }
        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
            if (work->flags & 4) {
                break;
            }
        }
    }

    work->flags &= ~0x40;
    if (chain->index == 0) {
        xglMatrixUnit4s(work->matrix_buffer);
    }
    return cursor;
}

void *JC_setParent(JcWork *work, JcElement *elements, const JcChain *chain)
{
    JcElement *element_array;
    JcElement *cursor;
    const Matrix *records;
    Matrix *destination;
    Matrix *root_destination;
    unsigned int source_byte_offset;
    int chain_length;
    int flags_after_scan;
    float frame;
    int slot;
    int bound;
    int interpolate_end;
    unsigned int dest_offset;
    Matrix *previous_destination;

    element_array = elements;
    if (work->matrix_buffer != 0) {
        if (chain == 0) {
            return 0;
        }
        records = *chain->source;
        if (records != 0) {
            goto matrix_available;
        }
    }
    return 0;
matrix_available:
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &work->matrix_buffer[chain->index];

        {
            __asm__ __volatile__(
                "lq $2, 0(%1)\n"
                "sq $2, 0(%0)\n"
                "lq $2, 16(%1)\n"
                "sq $2, 16(%0)\n"
                "lq $2, 32(%1)\n"
                "sq $2, 32(%0)\n"
                "lq $2, 48(%1)\n"
                "sq $2, 48(%0)\n"
            : : "r"(destination), "r"((const Matrix *)((chain->static_index * sizeof(*records)) + (unsigned int)records))
                : "$2", "memory");
        }
        flags_after_copy = work->flags | 8;
        destination->elements[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->elements[7] = 1.0f;
        destination->elements[11] = 1.0f;
        return 0;
    }

    chain_length = 0;
    work->flags |= 0x18;
    cursor = JNT_getElement(element_array, chain->index);
    if (cursor->type == 2) {
        do {
            chain_length++;
            cursor = JNT_nextElement(cursor);
        } while (cursor->type != 4);
    }
    flags_after_scan = work->flags;

    if (flags_after_scan & 0x20) {

        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (chain->index == 0) {
            source_byte_offset = chain->static_index;
            root_destination = (Matrix *)(chain->index * sizeof(Matrix));
            destination = (Matrix *)((unsigned int)work->matrix_buffer + (unsigned int)root_destination);
            source_byte_offset *= sizeof(*records);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            {
                __asm__ __volatile__(
                    "lq $2, 0(%1)\n"
                    "sq $2, 0(%0)\n"
                    "lq $2, 16(%1)\n"
                    "sq $2, 16(%0)\n"
                    "lq $2, 32(%1)\n"
                    "sq $2, 32(%0)\n"
                    "lq $2, 48(%1)\n"
                    "sq $2, 48(%0)\n"
                : : "r"(root_destination), "r"(&records[0])
                    : "$2", "memory");
            }
            source_byte_offset = chain->static_index * sizeof(*records);
        }

        {
            __asm__ __volatile__(
                "lq $2, 0(%1)\n"
                "sq $2, 0(%0)\n"
                "lq $2, 16(%1)\n"
                "sq $2, 16(%0)\n"
                "lq $2, 32(%1)\n"
                "sq $2, 32(%0)\n"
                "lq $2, 48(%1)\n"
                "sq $2, 48(%0)\n"
            : : "r"(destination), "r"((const Matrix *)(source_byte_offset + (unsigned int)records))
                : "$2", "memory");
        }
        destination->elements[3] = destination->elements[7] =
            destination->elements[11] = 1.0f;

        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                records = *chain->source;
                root_destination = work->matrix_buffer;
                root_destination += chain->index;
                previous_destination = root_destination - 1;
                {
                    __asm__ __volatile__(
                        "lq $2, 0(%1)\n"
                        "sq $2, 0(%0)\n"
                        "lq $2, 16(%1)\n"
                        "sq $2, 16(%0)\n"
                        "lq $2, 32(%1)\n"
                        "sq $2, 32(%0)\n"
                        "lq $2, 48(%1)\n"
                        "sq $2, 48(%0)\n"
                    : : "r"(previous_destination), "r"((const Matrix *)((chain->static_index * sizeof(*records)) + (unsigned int)records))
                        : "$2", "memory");
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    {
                        __asm__ __volatile__(
                            "lq $2, 0(%1)\n"
                            "sq $2, 0(%0)\n"
                            "lq $2, 16(%1)\n"
                            "sq $2, 16(%0)\n"
                            "lq $2, 32(%1)\n"
                            "sq $2, 32(%0)\n"
                            "lq $2, 48(%1)\n"
                            "sq $2, 48(%0)\n"
                        : : "r"((Matrix *)(((chain->index + slot) * sizeof(Matrix)) + (unsigned int)work->matrix_buffer)), "r"((const Matrix *)(((chain->static_index + slot) * sizeof(*records)) + (unsigned int)records))
                            : "$2", "memory");
                    }
                    cursor = JNT_nextElement(cursor);
                    work->current_index++;
                    slot++;
                } while (work->current_index < bound && cursor->type != 4);
            }
            cursor = &element_array[work->current_index];
            interpolate_end = bound;
            while (work->current_index < interpolate_end) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
    } else {

        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getVal2(work, cursor, frame);
        }
        if (chain->index == 0) {
            source_byte_offset = chain->static_index;
            root_destination = (Matrix *)(chain->index * sizeof(Matrix));
            destination = (Matrix *)((unsigned int)work->matrix_buffer + (unsigned int)root_destination);
            source_byte_offset *= sizeof(*records);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            {
                __asm__ __volatile__(
                    "lq $2, 0(%1)\n"
                    "sq $2, 0(%0)\n"
                    "lq $2, 16(%1)\n"
                    "sq $2, 16(%0)\n"
                    "lq $2, 32(%1)\n"
                    "sq $2, 32(%0)\n"
                    "lq $2, 48(%1)\n"
                    "sq $2, 48(%0)\n"
                : : "r"(root_destination), "r"(&records[0])
                    : "$2", "memory");
            }
            source_byte_offset = chain->static_index * sizeof(*records);
        }

        {
            __asm__ __volatile__(
                "lq $2, 0(%1)\n"
                "sq $2, 0(%0)\n"
                "lq $2, 16(%1)\n"
                "sq $2, 16(%0)\n"
                "lq $2, 32(%1)\n"
                "sq $2, 32(%0)\n"
                "lq $2, 48(%1)\n"
                "sq $2, 48(%0)\n"
            : : "r"(destination), "r"((const Matrix *)(source_byte_offset + (unsigned int)records))
                : "$2", "memory");
        }
        destination->elements[3] = destination->elements[7] =
            destination->elements[11] = 1.0f;

        bound = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
        if (work->attribute_key == work->expected_attribute_key) {
            while (work->current_index < bound) {
                cursor = JNT_getVal2(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
        } else {
            while (work->current_index < bound) {
                cursor = JNT_getVal_HOGE2(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
        }
        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
            if (work->flags & 4) {
                break;
            }
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                records = *chain->source;
                root_destination = work->matrix_buffer;
                root_destination += chain->index;
                previous_destination = root_destination - 1;
                {
                    __asm__ __volatile__(
                        "lq $2, 0(%1)\n"
                        "sq $2, 0(%0)\n"
                        "lq $2, 16(%1)\n"
                        "sq $2, 16(%0)\n"
                        "lq $2, 32(%1)\n"
                        "sq $2, 32(%0)\n"
                        "lq $2, 48(%1)\n"
                        "sq $2, 48(%0)\n"
                    : : "r"(previous_destination), "r"((const Matrix *)((chain->static_index * sizeof(*records)) + (unsigned int)records))
                        : "$2", "memory");
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    {
                        __asm__ __volatile__(
                            "lq $2, 0(%1)\n"
                            "sq $2, 0(%0)\n"
                            "lq $2, 16(%1)\n"
                            "sq $2, 16(%0)\n"
                            "lq $2, 32(%1)\n"
                            "sq $2, 32(%0)\n"
                            "lq $2, 48(%1)\n"
                            "sq $2, 48(%0)\n"
                        : : "r"((Matrix *)(((chain->index + slot) * sizeof(Matrix)) + (unsigned int)work->matrix_buffer)), "r"((const Matrix *)(((chain->static_index + slot) * sizeof(*records)) + (unsigned int)records))
                            : "$2", "memory");
                    }
                    slot++;
                    cursor = JNT_nextElement(cursor);
                    work->current_index++;
                } while (work->current_index < bound && cursor->type != 4);
            }
            cursor = &element_array[work->current_index];
            interpolate_end = bound;
            while (work->current_index < interpolate_end) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
    }
    work->flags &= ~0x40;
    if (chain->index == 0) {
        xglMatrixUnit4s(work->matrix_buffer);
    }
    return cursor;
}

JcElement *JC_setParent2(JcWork *work, JcElement *elements, JcChain *chain)
{
    JcElement *element_array;
    JcElement *cursor;
    Matrix *matrix_buffer;
    const Matrix *records;
    Matrix *destination;
    const Matrix *indexed_source;
    unsigned int source_byte_offset;
    int chain_length;
    int slot;
    int bound;
    float frame;

    element_array = elements;
    matrix_buffer = work->matrix_buffer;
    if (matrix_buffer != 0) {
        if (chain == 0) {
            return 0;
        }
        records = *chain->source;
        if (records != 0) {
            goto matrix_available;
        }
    }
    return 0;
matrix_available:
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &matrix_buffer[chain->index];

        copyMatrix(destination->quadwords, records[0].quadwords);
        copyMatrix(destination->quadwords,
                   records[chain->static_index].quadwords);
        flags_after_copy = work->flags | 8;
        destination->elements[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->elements[7] = 1.0f;
        destination->elements[11] = 1.0f;
        return 0;
    }

    chain_length = 0;
    work->flags |= 0x18;
    cursor = JNT_getElement(element_array, chain->index);
    if (cursor->type == 2) {
        do {
            chain_length++;
            cursor = JNT_nextElement(cursor);
        } while (cursor->type != 4);
    }
    if (work->flags & 0x20) {

        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (chain->index == 0) {
            Matrix *row_base = work->matrix_buffer;

            destination = &row_base[chain->index];
            copyMatrix(destination->quadwords,
                       records[chain->static_index].quadwords);
            destination->elements[3] = destination->elements[7] =
                destination->elements[11] = 1.0f;
        } else {
            Matrix *root_matrix = work->matrix_buffer;

            destination = &root_matrix[chain->index];
            copyMatrix(root_matrix->quadwords, records[0].quadwords);
            copyMatrix(destination->quadwords,
                       records[chain->static_index].quadwords);
            destination->elements[3] = destination->elements[7] =
                destination->elements[11] = 1.0f;
        }
        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                Matrix *previous_destination;
                records = *chain->source;
                previous_destination = &work->matrix_buffer[chain->index - 1];
                copyMatrix(previous_destination->quadwords,
                           records[chain->static_index].quadwords);
            }
            cursor = &element_array[work->current_index];
            slot = 0;
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    copyMatrix(work->matrix_buffer[chain->index + slot].quadwords,
                               records[chain->static_index + slot].quadwords);
                    cursor = JNT_nextElement(cursor);
                    slot++;
                    work->current_index++;
                } while (work->current_index < bound && cursor->type != 4);
            }
            cursor = &element_array[work->current_index];
            while (bound > work->current_index) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
    } else {

        bound = chain_length + chain->index + 1;
        cursor = element_array;
        while (work->current_index < bound) {
            cursor = JNT_getVal2(work, cursor, frame);
        }
        if (chain->index == 0) {
            Matrix *row_base = work->matrix_buffer;

            destination = &row_base[chain->index];
            copyMatrix(destination->quadwords,
                       records[chain->static_index].quadwords);
            destination->elements[3] = destination->elements[7] =
                destination->elements[11] = 1.0f;
        } else {
            Matrix *root_matrix = work->matrix_buffer;

            destination = &root_matrix[chain->index];
            copyMatrix(root_matrix->quadwords, records[0].quadwords);
            copyMatrix(destination->quadwords,
                       records[chain->static_index].quadwords);
            destination->elements[3] = destination->elements[7] =
                destination->elements[11] = 1.0f;
        }
        bound = work->hair_id_source->primary_id +
                work->hair_id_source->secondary_id;
        if (work->attribute_key == work->expected_attribute_key) {
            while (work->current_index < bound) {
                cursor = JNT_getVal2(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
        } else {
            while (work->current_index < bound) {
                cursor = JNT_getVal_HOGE2(work, cursor, frame);
                if (work->flags & 4) {
                    break;
                }
            }
        }
        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
            if (work->flags & 4) {
                break;
            }
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                Matrix *previous_destination;
                records = *chain->source;
                previous_destination = &work->matrix_buffer[chain->index - 1];
                copyMatrix(previous_destination->quadwords,
                           records[chain->static_index].quadwords);
            }
            cursor = &element_array[work->current_index];
            slot = 0;
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    copyMatrix(work->matrix_buffer[chain->index + slot].quadwords,
                               records[chain->static_index + slot].quadwords);
                    slot++;
                    cursor = JNT_nextElement(cursor);
                    work->current_index++;
                } while (work->current_index < bound && cursor->type != 4);
            }
            cursor = &element_array[work->current_index];
            while (work->current_index < bound) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
    }
    work->flags &= ~0x40;
    if (chain->index == 0) {
        xglMatrixUnit4s(work->matrix_buffer);
    }
    return cursor;
}

static inline void copyJointMatrix(JntMatrix *destination,
                                   const JntMatrix *source)
{
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        : : "r"(destination), "r"(source)
        : "$2", "memory");
}

static inline void copyChainSlot(JntWork *work, const JcChain *chain,
                                 const JntMatrix *records, unsigned int slot)
{
    copyJointMatrix(&work->matrix_buffer[chain->index + slot],
                    &records[chain->static_index + slot]);
}

void *JC_copyChain(JntWork *work, void *element, JcChain *chain)
{
    const JntMatrix *records = *chain->source;
    void *cursor;
    unsigned int flags_after_element;
    int bound;
    unsigned int slot;

    if (element == 0) {
        copyJointMatrix(&work->matrix_buffer[chain->index],
                        &records[chain->static_index]);
        work->flags |= 8;
        return 0;
    }
    work->flags |= 0x18;
    JNT_getElement(element, chain->index);
    flags_after_element = work->flags;
    cursor = element;

    if (flags_after_element & 0x20) {
        bound = chain->index + 1;
        while (work->current_index < bound)
            cursor = JNT_getStaticVal2(work, cursor);
        bound = work->hair_id_source->channel_limit;
        slot = 0;
        copyJointMatrix(&work->matrix_buffer[0], &records[0]);
        while (work->current_index < bound) {
            copyChainSlot(work, chain, records, slot);
            cursor = JNT_nextElement(cursor);
            slot++;
            work->current_index++;
        }
    } else {
        bound = chain->index + 1;
        while (work->current_index < bound)
            cursor = JNT_getStaticVal2(work, cursor);
        copyJointMatrix(&work->matrix_buffer[0], &records[0]);
        bound = work->hair_id_source->channel_limit;
        slot = 0;
        while (work->current_index < bound) {
            copyChainSlot(work, chain, records, slot);
            cursor = JNT_nextElement(cursor);
            slot++;
            work->current_index++;
        }
    }
    work->flags &= ~0x40;
    return cursor;
}
