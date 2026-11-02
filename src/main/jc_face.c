#include "common.h"
#include "main/jc_face.h"
#include "jc_face.h"
#include "quadword.h"

static inline void copyMatrix(void *destination, const void *source)
{
    QuadwordCopyMatrix(destination, source);
}

/*
 * The joint table is JntMatrix, a union of the float and quadword views
 * (alias set 0), which fixes the original store/load order of the w-column
 * stores against the quadword copies.
 */
JntStaticChannel *JC_face(JntWork *work, JntStaticChannel *elements, JcChain *chain)
{
    JntStaticChannel *element_array;
    JntStaticChannel *cursor;
    JntMatrix *destination;
    JntMatrix *root_destination;
    JntProductionConsumer filter;
    int chain_length;
    unsigned int flags_after_scan;
    float frame;
    int target_index;
    int previous_index;
    int interpolate_end;
    int bound;
    int slot;
    const JntMatrix *matrix_table;

    element_array = elements;
    if (work->matrix_buffer == 0 || chain == 0 || (matrix_table = *chain->source) == 0) {
        return 0;
    }
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &work->matrix_buffer[chain->index];
        {
            QuadwordCopyMatrix(destination, chain->static_index * sizeof(*matrix_table) + (unsigned int)matrix_table);
        }
        flags_after_copy = work->flags | 8;
        destination->flat[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->flat[7] = 1.0f;
        destination->flat[11] = 1.0f;
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
        destination->flat[3] = destination->flat[7] =
            destination->flat[11] = 1.0f;

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
                JntMatrix *previous_destination;
                matrix_table = *chain->source;
                root_destination = work->matrix_buffer;
                root_destination += chain->index;
                previous_destination = root_destination - 1;
                {
                    QuadwordCopyMatrix(previous_destination, chain->static_index * sizeof(*matrix_table) + (unsigned int)matrix_table);
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < previous_index && cursor->type != 4) {
                do {
                    int destination_index = chain->index + slot;
                    int source_index = chain->static_index + slot;
                    unsigned int destination = destination_index * sizeof(JntMatrix) + (unsigned int)work->matrix_buffer;
                    unsigned int source = source_index * sizeof(*matrix_table) + (unsigned int)matrix_table;
                    {
                        QuadwordCopyMatrix(destination, source);
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
        destination->flat[3] = destination->flat[7] =
            destination->flat[11] = 1.0f;

        bound = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
        if (work->attribute_key == work->requested_attribute_key) {
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
        xglMatrixUnit4s(work->matrix_buffer->elements);
    }
    return cursor;
}

void *JC_setParent(JntWork *work, JntStaticChannel *elements, const JcChain *chain)
{
    JntStaticChannel *element_array;
    JntStaticChannel *cursor;
    const JntMatrix *records;
    JntMatrix *destination;
    JntMatrix *root_destination;
    unsigned int source_byte_offset;
    int chain_length;
    int flags_after_scan;
    float frame;
    int slot;
    int bound;
    int interpolate_end;
    unsigned int dest_offset;
    JntMatrix *previous_destination;

    element_array = elements;
    if (work->matrix_buffer == 0 || chain == 0 || (records = *chain->source) == 0) {
        return 0;
    }
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &work->matrix_buffer[chain->index];

        {
            QuadwordCopyMatrix(destination, (chain->static_index * sizeof(*records)) + (unsigned int)records);
        }
        flags_after_copy = work->flags | 8;
        destination->flat[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->flat[7] = 1.0f;
        destination->flat[11] = 1.0f;
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
            root_destination = (JntMatrix *)(chain->index * sizeof(JntMatrix));
            destination = (JntMatrix *)((unsigned int)work->matrix_buffer + (unsigned int)root_destination);
            source_byte_offset *= sizeof(*records);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            {
                QuadwordCopyMatrix(root_destination, &records[0]);
            }
            source_byte_offset = chain->static_index * sizeof(*records);
        }

        {
            QuadwordCopyMatrix(destination, source_byte_offset + (unsigned int)records);
        }
        destination->flat[3] = destination->flat[7] =
            destination->flat[11] = 1.0f;

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
                    QuadwordCopyMatrix(previous_destination, (chain->static_index * sizeof(*records)) + (unsigned int)records);
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    {
                        QuadwordCopyMatrix(((chain->index + slot) * sizeof(JntMatrix)) + (unsigned int)work->matrix_buffer, ((chain->static_index + slot) * sizeof(*records)) + (unsigned int)records);
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
            root_destination = (JntMatrix *)(chain->index * sizeof(JntMatrix));
            destination = (JntMatrix *)((unsigned int)work->matrix_buffer + (unsigned int)root_destination);
            source_byte_offset *= sizeof(*records);
        } else {
            root_destination = &work->matrix_buffer[0];
            destination = &work->matrix_buffer[chain->index];
            {
                QuadwordCopyMatrix(root_destination, &records[0]);
            }
            source_byte_offset = chain->static_index * sizeof(*records);
        }

        {
            QuadwordCopyMatrix(destination, source_byte_offset + (unsigned int)records);
        }
        destination->flat[3] = destination->flat[7] =
            destination->flat[11] = 1.0f;

        bound = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
        if (work->attribute_key == work->requested_attribute_key) {
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
                    QuadwordCopyMatrix(previous_destination, (chain->static_index * sizeof(*records)) + (unsigned int)records);
                }
            }
            slot = 0;
            cursor = &element_array[work->current_index];
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    {
                        QuadwordCopyMatrix(((chain->index + slot) * sizeof(JntMatrix)) + (unsigned int)work->matrix_buffer, ((chain->static_index + slot) * sizeof(*records)) + (unsigned int)records);
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
        xglMatrixUnit4s(work->matrix_buffer->elements);
    }
    return cursor;
}

JntStaticChannel *JC_setParent2(JntWork *work, JntStaticChannel *elements, JcChain *chain)
{
    JntStaticChannel *element_array;
    JntStaticChannel *cursor;
    JntMatrix *matrix_buffer;
    const JntMatrix *records;
    JntMatrix *destination;
    const JntMatrix *indexed_source;
    unsigned int source_byte_offset;
    int chain_length;
    int slot;
    int bound;
    float frame;

    element_array = elements;
    matrix_buffer = work->matrix_buffer;
    if (matrix_buffer == 0 || chain == 0 || (records = *chain->source) == 0) {
        return 0;
    }
    frame = work->current_frame;

    if (element_array == 0) {
        unsigned int flags_after_copy;
        destination = &matrix_buffer[chain->index];

        copyMatrix(destination, &records[0]);
        copyMatrix(destination, &records[chain->static_index]);
        flags_after_copy = work->flags | 8;
        destination->flat[3] = 1.0f;
        work->flags = flags_after_copy;
        destination->flat[7] = 1.0f;
        destination->flat[11] = 1.0f;
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
            JntMatrix *row_base = work->matrix_buffer;

            destination = &row_base[chain->index];
            copyMatrix(destination, &records[chain->static_index]);
            destination->flat[3] = destination->flat[7] =
                destination->flat[11] = 1.0f;
        } else {
            JntMatrix *root_matrix = work->matrix_buffer;

            destination = &root_matrix[chain->index];
            copyMatrix(root_matrix, &records[0]);
            copyMatrix(destination, &records[chain->static_index]);
            destination->flat[3] = destination->flat[7] =
                destination->flat[11] = 1.0f;
        }
        bound = work->hair_id_source->channel_limit;
        while (work->current_index < bound) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (work->flags & 2) {
            work->current_index = chain->index;
            if (element_array[chain->index].type == 2) {
                JntMatrix *previous_destination;
                records = *chain->source;
                previous_destination = &work->matrix_buffer[chain->index - 1];
                copyMatrix(previous_destination,
                           &records[chain->static_index]);
            }
            cursor = &element_array[work->current_index];
            slot = 0;
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    copyMatrix(&work->matrix_buffer[chain->index + slot],
                               &records[chain->static_index + slot]);
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
            JntMatrix *row_base = work->matrix_buffer;

            destination = &row_base[chain->index];
            copyMatrix(destination, &records[chain->static_index]);
            destination->flat[3] = destination->flat[7] =
                destination->flat[11] = 1.0f;
        } else {
            JntMatrix *root_matrix = work->matrix_buffer;

            destination = &root_matrix[chain->index];
            copyMatrix(root_matrix, &records[0]);
            copyMatrix(destination, &records[chain->static_index]);
            destination->flat[3] = destination->flat[7] =
                destination->flat[11] = 1.0f;
        }
        bound = work->hair_id_source->primary_id +
                work->hair_id_source->secondary_id;
        if (work->attribute_key == work->requested_attribute_key) {
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
                JntMatrix *previous_destination;
                records = *chain->source;
                previous_destination = &work->matrix_buffer[chain->index - 1];
                copyMatrix(previous_destination,
                           &records[chain->static_index]);
            }
            cursor = &element_array[work->current_index];
            slot = 0;
            if (work->current_index < bound && cursor->type != 4) {
                do {
                    copyMatrix(&work->matrix_buffer[chain->index + slot],
                               &records[chain->static_index + slot]);
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
        xglMatrixUnit4s(work->matrix_buffer->elements);
    }
    return cursor;
}

static inline void copyJointMatrix(JntMatrix *destination,
                                   const JntMatrix *source)
{
    QuadwordCopyMatrix(destination, source);
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
