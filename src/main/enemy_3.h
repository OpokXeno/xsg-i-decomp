#ifndef SRC_MAIN_ENEMY_3_H
#define SRC_MAIN_ENEMY_3_H

/* One enemy-work record is 0x38b0 bytes; the fields below are used by the
 * action-ready transition. */
typedef struct EnemyReadyWork {
    unsigned char unmodeled_00[4];
    u16 initial_motion_id;
    unsigned char unmodeled_06[0x1a];
    float random_turn_x;
    unsigned char unmodeled_24[4];
    float random_turn_z;
    unsigned char unmodeled_2c[0x18];
    u16 motion_mode;
    u16 motion_id;
    signed char action;
    unsigned char unmodeled_49[3];
    u16 action_motion_id;
    u16 action_timer_a;
    u16 action_timer_b;
    u16 action_timer_c;
    unsigned char unmodeled_54[0x16];
    u8 effect_state;
    unsigned char unmodeled_6b[3];
    u16 action_counter;
    unsigned char unmodeled_70[0x2020];
    u16 action_motion_table[256][4];
    u16 motion_id_table[256][4];
    short motion_table_row;
    short motion_table_column;
    unsigned char unmodeled_3094[0x37e0 - 0x3094];
    u32 reaction_flags;
    unsigned char unmodeled_37e4[0x70];
    float saved_actor_speed;
    unsigned char unmodeled_3858[0x48];
    EnemyActionEffect *effect;
    unsigned char unmodeled_38a4[0x0c];
} EnemyReadyWork;

extern EnemyReadyWork enepc[16];

#endif
