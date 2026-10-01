#include "common.h"
#include "shared.h"

/* The shared FPK/FCV prefix used by the pack accessors. */
typedef struct FpkFcvPack {
    u32 magic;
    u16 format;
    u16 unmodeled_06;
    int entry_count;
    u16 unmodeled_0c;
    u16 data_offset;
    u32 entries[1];
} FpkFcvPack;

#define FPK_MAGIC 0x004B5046
#define FPK_MAGIC_MASK 0x00FFFFFF
#define FPK_RELOCATED_MASK 0xFF000000
#define FPK_RELOCATED 0x01000000
#define FCV_MAGIC 0x00564346

typedef struct AnmClock {
    u32 flags;
    float frame;
    float speed;
    float reverse_limit;
    float forward_limit;
    u32 unmodeled_14[5];
    float loop_limit;
    u32 unmodeled_2c[3];
    float reset_low;
    float reset_high;
} AnmClock;

#define ANM_WAITING 2
#define ANM_LOOP 8
#define ANM_FINISHED 0x1000
#define ANM_RESTART 0x1010
#define ANM_RESET_BOUNDS 0x40000

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_getEntry);

void *PACK_getEntry(FpkFcvPack *pack, int index)
{
    int signature;
    int entry_count;
    int entry_index;
    u32 *entries;
    u32 *entry_pointer;
    u32 *format2_pointer;

    if (pack != 0) {
        signature = pack->magic & FPK_MAGIC_MASK;
        if (signature == FPK_MAGIC) {
            entries = pack->entries;
            entry_count = pack->entry_count;
            if ((pack->magic & FPK_RELOCATED_MASK) == 0) {
                if (pack->format == 2) {
                    entry_index = 0;
                    if (entry_count > 0) {
                        format2_pointer = entries;
                        do {
                            u32 entry = *format2_pointer;
                            if (entry != 0) {
                                *format2_pointer = entry + pack->data_offset + (u32)pack;
                            }
                            entry_index++;
                            format2_pointer++;
                        } while (entry_index < entry_count);
                    }
                } else if (entry_count > 0) {
                    entry_pointer = entries;
                    entry_index = entry_count;
                    do {
                        u32 entry = *entry_pointer;
                        if (entry != 0) {
                            *entry_pointer = entry + (u32)pack;
                        }
                        entry_index--;
                        entry_pointer++;
                    } while (entry_index != 0);
                }
                pack->magic |= FPK_RELOCATED;
            }

            if (index >= 0 && index < entry_count) {
                return (void *)entries[index];
            }
        } else if (signature == FCV_MAGIC) {
            return pack;
        }
    }
    return 0;
}

void *PACK_getAttr(FpkFcvPack *pack, int index)
{
    int signature;
    int entry_count;
    int entry_index;
    u32 *entries;
    u32 *entry_pointer;
    u32 *format2_pointer;

    if (pack != 0) {
        signature = pack->magic & FPK_MAGIC_MASK;
        if (signature == FPK_MAGIC) {
            entries = pack->entries;
            entry_count = pack->entry_count;
            if ((pack->magic & FPK_RELOCATED_MASK) == 0) {
                if (pack->format == 2) {
                    entry_index = 0;
                    if (entry_count > 0) {
                        format2_pointer = entries;
                        do {
                            u32 entry = *format2_pointer;
                            if (entry != 0) {
                                *format2_pointer = entry + pack->data_offset + (u32)pack;
                            }
                            entry_index++;
                            format2_pointer++;
                        } while (entry_index < entry_count);
                    }
                } else {
                    if (entry_count > 0) {
                        entry_pointer = entries;
                        entry_index = entry_count;
                        do {
                            u32 entry = *entry_pointer;
                            if (entry != 0) {
                                *entry_pointer = entry + (u32)pack;
                            }
                            entry_index--;
                            entry_pointer++;
                        } while (entry_index != 0);
                    }
                }
                pack->magic |= FPK_RELOCATED;
            }

            if (index >= 0 && index < entry_count) {
                u32 *attribute_entry = &pack->entries[entry_count];
                return (void *)attribute_entry[index];
            }
        } else if (signature == FCV_MAGIC) {
            return 0;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/anm", PACK_unlink);

INCLUDE_ASM("asm/main/nonmatchings/anm", PACK_getSize);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_resetDefault);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_resetTime);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_resetPack);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_resetTimeInterp);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_reset);

INCLUDE_ASM("asm/main/nonmatchings/anm", ANM_update);

void ANM_updateTime(AnmClock *clock, float elapsed)
{
    float frame = clock->frame + elapsed;
    u32 flags = clock->flags;

    clock->frame = frame;
    if (flags & ANM_WAITING) {
        if (clock->speed >= 0.0f) {
            if (clock->loop_limit < frame) {
                clock->flags = flags & ~ANM_WAITING;
            }
        } else if (frame < clock->loop_limit) {
            clock->flags = flags & ~ANM_WAITING;
        }
        return;
    }

    if (clock->speed >= 0.0f) {
        if (clock->forward_limit < frame) {
            if (flags & ANM_LOOP) {
                clock->flags = flags | ANM_RESTART;
                clock->frame = clock->reverse_limit;
                if (clock->flags & ANM_RESET_BOUNDS) {
                    clock->reverse_limit = clock->reset_low;
                    clock->forward_limit = clock->reset_high;
                    clock->frame = clock->reset_low;
                }
            } else {
                clock->frame = clock->forward_limit;
                clock->flags = flags | ANM_FINISHED;
            }
        }
    } else if (frame < clock->reverse_limit) {
        if (flags & ANM_LOOP) {
            clock->flags = flags | ANM_RESTART;
            clock->frame = clock->forward_limit;
            if (clock->flags & ANM_RESET_BOUNDS) {
                clock->forward_limit = clock->reset_high;
                clock->reverse_limit = clock->reset_low;
                clock->frame = clock->reset_high;
            }
        } else {
            clock->frame = clock->reverse_limit;
            clock->flags = flags | ANM_FINISHED;
        }
    }
}
