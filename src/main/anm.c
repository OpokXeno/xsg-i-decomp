#include "common.h"

#include "shared.h"

#include "anm.h"

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

#define ANM_WAITING 2

#define ANM_LOOP 8

#define ANM_FINISHED 0x1000

#define ANM_RESTART 0x1010

#define ANM_RESET_BOUNDS 0x40000

typedef struct AnmDefaultState {
    u32 flags;
    float frame;
    float speed;
    float reverse_limit;
    float forward_limit;
    u16 entry_index;
} AnmDefaultState;

#define D_004D852C 0.033333335f

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

void FCV2_readAttribute(FcvAttributeState *attributes, void *data);

#define D_004D8530 0.033333335f

#define D_004D8534 0.033333335f

#define D_004D8538 0.033333335f

#define D_004D853C 0.033333335f

#define D_004D8540 0.033333335f

#define D_004D8544 0.033333335f

#define D_004D8548 0.033333335f

#define D_004D854C 0.033333335f

#define ANM_ADVANCE_ENTRY 0x20000

#define ANM_PAUSED 0x80000

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

/* The shared FPK/FCV prefix used by the pack accessors. */

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

    data_index = clock->current_data_id & 0xFF;
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

FpkAnimationEntry *ANM_resetDefault(AnmDefaultState *state, FpkFcvPack *pack)
{
    float frame;
    float frame_duration;
    FpkAnimationEntry *entry;

    entry = PACK_getEntry(pack, state->entry_index);
    frame_duration = D_004D852C;
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

FpkAnimationEntry *ANM_resetTime(AnmClock *clock)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    entry = clock->entry;
    clock->flags &= ~ANM_RESTART;
    if (entry != 0) {
        frame_duration = D_004D8530;
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
            speed = D_004D8534;
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
        frame_duration = D_004D8538;
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
            clock->speed = D_004D853C;
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

FpkAnimationEntry *ANM_resetTimeInterp(AnmClock *clock)
{
    FcvAttributeState attributes;
    FpkAnimationEntry *entry;
    float frame_duration;
    float speed;

    entry = clock->entry;
    clock->flags &= ~ANM_RESTART;
    if (entry != 0) {
        frame_duration = D_004D8540;
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
            clock->speed = D_004D8544;
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
        frame_duration = D_004D8548;
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
            clock->speed = D_004D854C;
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

void ANM_update(AnmClock *clock)
{
    u32 flags = clock->flags;
    float speed = clock->speed;

    if (!(flags & ANM_PAUSED)) {
        clock->frame += speed;
    }
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
            if (clock->flags & ANM_ADVANCE_ENTRY) {
                int entry_count = PACK_getSize(clock->pack);
                u16 current_data_id = clock->current_data_id;
                int data_index = (current_data_id & 0xFF) + 1;
                int data_bank = current_data_id & 0x700;
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
            int data_index = (current_data_id & 0xFF) + 1;
            int data_bank = current_data_id & 0x700;
            if (data_index >= entry_count) {
                data_index = 0;
            }
            clock->current_data_id = data_index | data_bank;
            ANM_reset(clock, clock->pack);
                return;
        }
    }
}

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
