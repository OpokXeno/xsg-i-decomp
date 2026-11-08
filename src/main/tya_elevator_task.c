#include "common.h"
#include "tya_elevator_task.h"
#include "main/tya_elevator_task.h"

/*
 * Advances one elevator peer's state machine and copies its height into the
 * shared height table, its map entry and the Java height field.
 * field_offset is not used.
 */
void tyaElevatorTask(void *peer_context, int field_offset)
{
    ElevatorPeer *peer = peer_context;
    ElevatorMapScene *scene;
    ElevatorMapEntry *elevator_entries;
    float current_height;
    int highest_slot;
    int i;
    ElevatorPeer *other;
    float remaining;
    unsigned char initial_state;
    float target_height;

    initial_state = peer->elevator_state;
    /* State 0 assigns the slot and map entry, 1 picks the direction, 2 moves toward the target. */
    switch (initial_state) {
    case 0:
        highest_slot = 0;
        /* Scan every MapUnit record for other active elevators and take the slot after the highest one. */
        for (i = 0, other = MapUnit; i < ELEVATOR_UNIT_COUNT; i++, other++) {
            if (other->serial >= 0 && other->type_update == tyaElevatorTask &&
                other != peer && other->elevator_state != 0) {
                if (highest_slot < other->elevator_slot) {
                    highest_slot = other->elevator_slot + 1;
                }
            }
        }

        peer->elevator_slot = highest_slot;
        scene = GameLoopState.map_resource->scene;
        /* The entry-table displacement is stored relative to the scene resource. */
        elevator_entries = (ElevatorMapEntry *)((unsigned char *)scene +
                                                 scene->elevator_entries_offset);
        peer->elevator_state = 1;
        peer->map_entry = &elevator_entries[peer->map_data_index];
        /* Fall through: a newly initialized elevator checks its target now. */
    case 1:
        target_height = peer->target_height;
        if (target_height == peer->height) {
            break;
        }
        if (target_height < peer->height) {
            peer->height_step = -peer->movement_speed;
        } else {
            peer->height_step = peer->movement_speed;
        }
        peer->elevator_state = 2;
        /* Fall through: start moving after choosing the direction. */
    case 2:
        target_height = peer->target_height;
        current_height = peer->height;
        current_height += peer->height_step;
        remaining = target_height - current_height;
        peer->height = current_height;

        if ((remaining >= 0.0f && peer->height_step < 0.0f) ||
            (remaining <= 0.0f && peer->height_step > 0.0f)) {
            peer->height = target_height;
            peer->elevator_state = 1;
        }
        break;
    default:
        break;
    }

    GameLoopState.elevator_heights[peer->elevator_slot] =
        peer->map_entry->height = peer->height;
    *peer->elevator_field.height = peer->height;
}
