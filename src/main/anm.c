#include "common.h"
#include "shared.h"
#include "anm.h"
#include "main/anm.h"

#define FPK_RELOCATED_MASK 0xFF000000
#define FPK_RELOCATED 0x01000000
#define FCV_MAGIC 0x00564346
#define ANM_WAITING 2
#define ANM_LOOP 8
#define ANM_FINISHED 0x1000
#define ANM_RESTART 0x1010
#define ANM_RESET_BOUNDS 0x40000
#define ANM_DEFAULT_FRAME_DURATION 0.033333335f
#define ANM_TIME_FRAME_DURATION 0.033333335f
#define ANM_TIME_SPEED 0.033333335f
#define ANM_PACK_FRAME_DURATION 0.033333335f
#define ANM_PACK_CLOCK_SPEED 0.033333335f
#define ANM_TIME_INTERP_FRAME_DURATION 0.033333335f
#define ANM_TIME_INTERP_CLOCK_SPEED 0.033333335f
#define ANM_RESET_FRAME_DURATION 0.033333335f
#define ANM_RESET_CLOCK_SPEED 0.033333335f
#define ANM_ADVANCE_ENTRY 0x20000
#define ANM_PAUSED 0x80000
#define ANM_DATA_INDEX_MASK 0xFF   /* clock->current_data_id: entry index in the pack */
#define ANM_DATA_BANK_MASK 0x700   /* clock->current_data_id: bank bits kept when the index advances */

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

typedef struct AnmClock {
    u32 flags;
    float frame;
    float speed;
    float reverse_limit;
    float forward_limit;
    u16 current_data_id;
    u8 unmodeled_16[0xA];
    u16 attribute_index;
    u16 unmodeled_22;
    float wait_duration;
    float loop_limit;
    struct FpkAnimationEntry *entry;
    void *attribute_data;
    FpkFcvPack *pack;
    float reset_low;
    float reset_high;
} AnmClock;

typedef struct AnmDefaultState {
    u32 flags;
    float frame;
    float speed;
    float reverse_limit;
    float forward_limit;
    u16 entry_index;
} AnmDefaultState;

typedef struct FcvPlaybackRange {
    u8 selector;
    signed char speed;
    u16 first_frame;
    u16 last_frame;
} FcvPlaybackRange;

typedef struct FcvAttributeState {
    u8 unmodeled_00[12];
    u32 frame_count;
    u16 attribute_index;
    u16 unmodeled_12;
    void *attribute_data;
    FcvPlaybackRange *playback_range;
    FcvPlaybackRange *reset_range;
    u8 unmodeled_20[16];
} FcvAttributeState;

typedef struct FpkAnimationEntry {
    u32 magic;
    u16 format;
    u16 unmodeled_06;
    u16 first_frame;
    u16 last_frame;
    u8 unmodeled_0c[4];
    u8 attributes[1];
} FpkAnimationEntry;

typedef struct FpkAnimationAttributeSlot {
    u16 attribute_index;
    u16 unmodeled_02;
} FpkAnimationAttributeSlot;

void FCV2_readAttribute(FcvAttributeState *attributes, void *data);

/*
 * Returns the entry at clock->current_data_id's index in an FPK pack and stores
 * that entry's attribute index in clock->attribute_index. An FCV pack is
 * returned as the entry itself. The pack's offsets are relocated to pointers on
 * first access.
 */
FpkAnimationEntry *ANM_getEntry(AnmClock *clock, FpkFcvPack *pack)
{
    u32 signature;
    int entry_count;
    int entry_index;
    int data_index;
    u32 *entries;
    u32 *entry_pointer;
    u32 *format2_pointer;
    FpkAnimationAttributeSlot *attribute_slots;
    u16 attribute_index;

    data_index = clock->current_data_id & ANM_DATA_INDEX_MASK;
    if (pack != 0) {
        signature = pack->magic;
        if ((signature & FPK_MAGIC_MASK) == FPK_MAGIC) {
            entries = pack->entries;
            entry_count = pack->entry_count;
            if ((signature & FPK_RELOCATED_MASK) == 0) {
                if (pack->format == 2) {
                    entry_index = 0;
                    if (entry_count > 0) {
                        format2_pointer = entries;
                        do {
                            if (*format2_pointer != 0) {
                                *format2_pointer = *format2_pointer +
                                    pack->data_offset + (u32)pack;
                            }
                            entry_index++;
                            format2_pointer++;
                        } while (entry_index < entry_count);
                    }
                } else if (entry_count > 0) {
                    entry_pointer = entries;
                    entry_index = entry_count;
                    do {
                        if (*entry_pointer != 0) {
                            *entry_pointer += (u32)pack;
                        }
                        entry_index--;
                        entry_pointer++;
                    } while (entry_index != 0);
                }
                pack->magic |= FPK_RELOCATED;
            }

            if (data_index >= 0 && data_index < entry_count) {
                attribute_slots = (FpkAnimationAttributeSlot *)&pack->entries[entry_count];
                attribute_index =
                    attribute_slots[data_index].attribute_index;
                clock->attribute_index = attribute_index;
                return (FpkAnimationEntry *)entries[data_index];
            }
        } else if ((signature & FPK_MAGIC_MASK) == FCV_MAGIC) {
            clock->attribute_index = 0;
            return (FpkAnimationEntry *)pack;
        }
    }
    return 0;
}

/*
 * Returns entry 'index' of an FPK pack, relocating the pack's offsets on first
 * access. An FCV pack is returned as is; a null pack, an unknown magic or an
 * out-of-range index gives 0.
 */
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

/*
 * Returns attribute 'index' of an FPK pack, from the table that follows its
 * entries; relocates the pack's offsets on first access. An FCV pack gives 0.
 */
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

/*
 * Undoes the pointer relocation of a relocated FPK pack: entries become offsets
 * from base_address again (format 2 entries are left as they are) and the
 * relocated flag is cleared. Returns the pack for an FCV pack, otherwise 0.
 */
void *PACK_unlink(FpkFcvPack *pack, int base_address)
{
    u32 signature;
    int entry_count;
    u32 *entries;
    u32 *entry_pointer;

    if (pack != 0) {
        signature = pack->magic;
        if ((signature & FPK_MAGIC_MASK) == FPK_MAGIC) {
            entries = pack->entries;
            entry_count = pack->entry_count;
            if ((signature & FPK_RELOCATED_MASK) != 0) {
                if (pack->format != 2 && entry_count > 0) {
                    entry_pointer = entries;
                    do {
                        if (*entry_pointer != 0) {
                            *entry_pointer -= base_address;
                        }
                        entry_count--;
                        entry_pointer++;
                    } while (entry_count != 0);
                    signature = pack->magic;
                }
                pack->magic = signature & FPK_MAGIC_MASK;
            }
        } else if ((signature & FPK_MAGIC_MASK) == FCV_MAGIC) {
            return pack;
        }
    }
    return 0;
}

/*
 * Returns the entry count of an FPK pack, relocating its offsets on first
 * access (format 2 and later use the data_offset form). An FCV pack counts as
 * 1; a null pack or an unknown magic gives 0.
 */
u32 PACK_getSize(FpkFcvPack *pack)
{
    u32 signature;
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
                if (pack->format >= 2) {
                    entry_index = 0;
                    if (entry_count > 0) {
                        format2_pointer = entries;
                        do {
                            if (*format2_pointer != 0) {
                                *format2_pointer = *format2_pointer +
                                    pack->data_offset + (u32)pack;
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
                            if (*entry_pointer != 0) {
                                *entry_pointer += (u32)pack;
                            }
                            entry_index--;
                            entry_pointer++;
                        } while (entry_index != 0);
                    }
                }
                pack->magic |= FPK_RELOCATED;
            }
            return (u32)entry_count;
        }
        if (signature == FCV_MAGIC) {
            return 1;
        }
    }
    return 0;
}

/*
 * Starts default playback on the entry at state->entry_index of the pack and
 * clears ANM_FINISHED. Sets the speed to the default frame duration, the limits
 * to the entry's first and last frames and the frame to its first frame.
 * Returns the entry, or 0 when the pack has none at that index.
 */
FpkAnimationEntry *ANM_resetDefault(AnmDefaultState *state, FpkFcvPack *pack)
{
    float frame;
    float frame_duration;
    FpkAnimationEntry *entry;

    entry = PACK_getEntry(pack, state->entry_index);
    frame_duration = ANM_DEFAULT_FRAME_DURATION;
    state->flags &= ~ANM_FINISHED;
    if (entry != 0) {
        u16 first_frame = entry->first_frame;
        u16 last_frame = entry->last_frame;
        state->speed = frame_duration;
        frame = first_frame * frame_duration;
        state->forward_limit = last_frame * frame_duration;
        state->reverse_limit = frame;
        state->frame = frame;
    }
    return entry;
}

/*
 * Restarts a time-driven clock on its current entry and clears ANM_RESTART. The
 * limits come from the entry, or from its FCV2 playback range when there is one,
 * and the reset bounds come from the reset range or the limits. The frame goes
 * to the start of the playback direction. Returns the entry, or 0.
 */
FpkAnimationEntry *ANM_resetTime(AnmClock *clock)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    entry = clock->entry;
    clock->flags &= ~ANM_RESTART;
    if (entry != 0) {
        frame_duration = ANM_TIME_FRAME_DURATION;
        clock->reverse_limit = entry->first_frame * frame_duration;
        clock->forward_limit = entry->last_frame * frame_duration;
        if (entry->format >= 2) {
            attributes.attribute_index = clock->attribute_index;
            FCV2_readAttribute(&attributes, entry->attributes);
            if (attributes.playback_range != 0) {
                clock->reverse_limit = attributes.playback_range->first_frame * frame_duration;
                clock->forward_limit = attributes.playback_range->last_frame * frame_duration;
                clock->speed = attributes.playback_range->speed * 0.03125f * frame_duration;
            }
            if (attributes.reset_range != 0) {
                u16 first_frame = attributes.reset_range->first_frame;
                u16 last_frame = attributes.reset_range->last_frame;
                clock->reset_high = last_frame * frame_duration;
                clock->reset_low = first_frame * frame_duration;
            } else {
                clock->reset_low = clock->reverse_limit;
                clock->reset_high = clock->forward_limit;
            }
            clock->attribute_data = attributes.attribute_data;
        }
        speed = clock->speed;
        if (speed == 0.0f) {
            speed = ANM_TIME_SPEED;
            clock->speed = speed;
        }
        if (speed >= 0.0f) {
            clock->frame = clock->reverse_limit;
        } else {
            clock->frame = clock->forward_limit;
        }
    }
    return entry;
}

/*
 * Fetches the pack entry at clock->current_data_id into clock->entry and starts
 * playback on it with the pack frame duration. When flags bit 0 is set, the
 * clock also waits (ANM_WAITING) until its frame passes loop_limit, which is
 * wait_duration past the start frame. Returns the entry, or 0.
 */
FpkAnimationEntry *ANM_resetPack(AnmClock *clock)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    entry = ANM_getEntry(clock, clock->pack);
    clock->entry = entry;
    clock->flags &= ~ANM_FINISHED;
    if (entry != 0) {
        frame_duration = ANM_PACK_FRAME_DURATION;
        clock->reverse_limit = entry->first_frame * frame_duration;
        clock->forward_limit = entry->last_frame * frame_duration;
        if (entry->format >= 2) {
            attributes.attribute_index = clock->attribute_index;
            FCV2_readAttribute(&attributes, entry->attributes);
            if (attributes.playback_range != 0) {
                clock->reverse_limit = attributes.playback_range->first_frame * frame_duration;
                clock->forward_limit = attributes.playback_range->last_frame * frame_duration;
                clock->speed = attributes.playback_range->speed * 0.03125f * frame_duration;
            }
            if (attributes.reset_range != 0) {
                u16 first_frame = attributes.reset_range->first_frame;
                u16 last_frame = attributes.reset_range->last_frame;
                clock->reset_high = last_frame * frame_duration;
                clock->reset_low = first_frame * frame_duration;
            } else {
                clock->reset_low = clock->reverse_limit;
                clock->reset_high = clock->forward_limit;
            }
            clock->attribute_data = attributes.attribute_data;
        }
        speed = clock->speed;
        if (speed == 0.0f) {
            clock->speed = ANM_PACK_CLOCK_SPEED;
            speed = clock->speed;
        }
        if (speed >= 0.0f) {
            clock->frame = clock->reverse_limit;
        } else {
            clock->frame = clock->forward_limit;
        }
        if (clock->flags & 1) {
            clock->flags |= ANM_WAITING;
            if (speed >= 0.0f) {
                clock->loop_limit = clock->reverse_limit + clock->wait_duration;
            } else {
                clock->loop_limit = clock->reverse_limit - clock->wait_duration;
            }
        }
    }
    return entry;
}

/*
 * Restarts the clock's current entry for interpolated time playback. It sets up
 * the limits, reset bounds, speed and frame as ANM_resetPack does, with the
 * interpolated time frame duration, and clears ANM_RESTART. Returns the entry,
 * or 0.
 */
FpkAnimationEntry *ANM_resetTimeInterp(AnmClock *clock)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    entry = clock->entry;
    clock->flags &= ~ANM_RESTART;
    if (entry != 0) {
        frame_duration = ANM_TIME_INTERP_FRAME_DURATION;
        clock->reverse_limit = entry->first_frame * frame_duration;
        clock->forward_limit = entry->last_frame * frame_duration;
        if (entry->format >= 2) {
            attributes.attribute_index = clock->attribute_index;
            FCV2_readAttribute(&attributes, entry->attributes);
            if (attributes.playback_range != 0) {
                clock->reverse_limit = attributes.playback_range->first_frame * frame_duration;
                clock->forward_limit = attributes.playback_range->last_frame * frame_duration;
                clock->speed = attributes.playback_range->speed * 0.03125f * frame_duration;
            }
            if (attributes.reset_range != 0) {
                u16 first_frame = attributes.reset_range->first_frame;
                u16 last_frame = attributes.reset_range->last_frame;
                clock->reset_high = last_frame * frame_duration;
                clock->reset_low = first_frame * frame_duration;
            } else {
                clock->reset_low = clock->reverse_limit;
                clock->reset_high = clock->forward_limit;
            }
            clock->attribute_data = attributes.attribute_data;
        }
        speed = clock->speed;
        if (speed == 0.0f) {
            clock->speed = ANM_TIME_INTERP_CLOCK_SPEED;
            speed = clock->speed;
        }
        if (speed >= 0.0f) {
            clock->frame = clock->reverse_limit;
        } else {
            clock->frame = clock->forward_limit;
        }
        if (clock->flags & 1) {
            clock->flags |= ANM_WAITING;
            if (speed >= 0.0f) {
                clock->loop_limit = clock->reverse_limit + clock->wait_duration;
            } else {
                clock->loop_limit = clock->reverse_limit - clock->wait_duration;
            }
        }
    }
    return entry;
}

/*
 * Sets clock->pack to pack and starts playback on the entry at
 * clock->current_data_id, set up as ANM_resetPack does but with the reset frame
 * duration. ANM_update calls it to move on to the next entry. Returns the entry,
 * or 0.
 */
FpkAnimationEntry *ANM_reset(AnmClock *clock, FpkFcvPack *pack)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    clock->pack = pack;
    entry = ANM_getEntry(clock, pack);
    clock->entry = entry;
    clock->flags &= ~ANM_FINISHED;
    if (entry != 0) {
        frame_duration = ANM_RESET_FRAME_DURATION;
        clock->reverse_limit = entry->first_frame * frame_duration;
        clock->forward_limit = entry->last_frame * frame_duration;
        if (entry->format >= 2) {
            attributes.attribute_index = clock->attribute_index;
            FCV2_readAttribute(&attributes, entry->attributes);
            if (attributes.playback_range != 0) {
                clock->reverse_limit = attributes.playback_range->first_frame * frame_duration;
                clock->forward_limit = attributes.playback_range->last_frame * frame_duration;
                clock->speed = attributes.playback_range->speed * 0.03125f * frame_duration;
            }
            if (attributes.reset_range != 0) {
                u16 first_frame = attributes.reset_range->first_frame;
                u16 last_frame = attributes.reset_range->last_frame;
                clock->reset_high = last_frame * frame_duration;
                clock->reset_low = first_frame * frame_duration;
            } else {
                clock->reset_low = clock->reverse_limit;
                clock->reset_high = clock->forward_limit;
            }
            clock->attribute_data = attributes.attribute_data;
        }
        speed = clock->speed;
        if (speed == 0.0f) {
            clock->speed = ANM_RESET_CLOCK_SPEED;
            speed = clock->speed;
        }
        if (speed >= 0.0f) {
            clock->frame = clock->reverse_limit;
        } else {
            clock->frame = clock->forward_limit;
        }
        if (clock->flags & 1) {
            clock->flags |= ANM_WAITING;
            if (speed >= 0.0f) {
                clock->loop_limit = clock->reverse_limit + clock->wait_duration;
            } else {
                clock->loop_limit = clock->reverse_limit - clock->wait_duration;
            }
        }
    }
    return entry;
}

/*
 * Advances a clock by its speed unless it is paused, then applies its limits.
 * A waiting clock only checks whether the wait is over. At the end of the range
 * the clock loops (ANM_LOOP) or finishes, and with ANM_ADVANCE_ENTRY it moves to
 * the next entry of the pack.
 */
void ANM_update(AnmClock *clock)
{
    u32 flags = clock->flags;
    float speed = clock->speed;

    if (!(flags & ANM_PAUSED)) {
        clock->frame += speed;
    }
    /* While waiting, the clock only checks whether the frame has passed loop_limit. */
    if (flags & ANM_WAITING) {
        if (speed >= 0.0f) {
            float frame = clock->frame;
            if (clock->loop_limit < frame) {
                clock->flags = flags & ~ANM_WAITING;
            }
        } else {
            float frame = clock->frame;
            if (frame < clock->loop_limit) {
                clock->flags = flags & ~ANM_WAITING;
            }
        }
        return;
    }
    /* Forward playback ends when the frame passes forward_limit. */
    if (speed >= 0.0f) {
        if (clock->forward_limit < clock->frame) {
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
            /* Move to the next entry of the pack, keeping the bank bits, and restart on it. */
            if (clock->flags & ANM_ADVANCE_ENTRY) {
                int entry_count = PACK_getSize(clock->pack);
                u16 current_data_id = clock->current_data_id;
                int data_index = (current_data_id & ANM_DATA_INDEX_MASK) + 1;
                int data_bank = current_data_id & ANM_DATA_BANK_MASK;
                if (data_index >= entry_count) {
                    data_index = 0;
                }
                clock->current_data_id = data_index | data_bank;
                ANM_reset(clock, clock->pack);
                return;
            }
        }
    } else if (clock->frame < clock->reverse_limit) {
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
        if (clock->flags & ANM_ADVANCE_ENTRY) {
            int entry_count = PACK_getSize(clock->pack);
            u16 current_data_id = clock->current_data_id;
            int data_index = (current_data_id & ANM_DATA_INDEX_MASK) + 1;
            int data_bank = current_data_id & ANM_DATA_BANK_MASK;
            if (data_index >= entry_count) {
                data_index = 0;
            }
            clock->current_data_id = data_index | data_bank;
            ANM_reset(clock, clock->pack);
                return;
        }
    }
}

/*
 * Moves a clock's frame forward by elapsed time, in place of its speed, and
 * applies the same wait, loop and finish limits as ANM_update. It does not move
 * to the next pack entry.
 */
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
