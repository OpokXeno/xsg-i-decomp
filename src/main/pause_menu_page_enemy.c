#include "common.h"
#include "pause_menu_page_enemy.h"
#include "main/pause_menu_page_enemy.h"
#include "quadword.h"

/* Partial view of UnduParam; the full type changes this object (the extern
 * UnduTemp of this TU is recorded at its 0x28-byte view size). */
typedef struct UnduParamPauseView {
    int queryFlags;
    unsigned char unmodeled_04[4];
    short attrMask;
    unsigned char unmodeled_0a[0x18 - 0x0a];
    struct LayoutHeader *header;
    unsigned char unmodeled_1c[4];
    long long attribute;
} UnduParamPauseView;

extern UnduParamPauseView UnduTemp;

short DebugActor = 0;

static short Page_0 = 0;

/* JP: 発見
 * EN: Discovered */
const char discoveredText[5] = "\310\257\270\253";

/* JP: 追跡中
 * EN: Chasing */
const char chasingText[7] = "\304\311\300\327\303\346";

/* JP: 硬直
 * EN: Stiffen */
const char stiffenText[5] = "\271\305\304\276";

/* JP: 帰巣
 * EN: Return home */
const char returnHomeText[5] = "\265\242\301\343";

/* JP: 触手中
 * EN: Tentacle active */
const char tentacleActiveText[7] = "\277\250\274\352\303\346";

/* JP: ぬる
 * EN: Null */
const char pauseMenuPageEnemyNullText[5] = "\244\314\244\353";

/* JP: 触手
 * EN: Tentacle */
const char tentacleText[5] = "\277\250\274\352";

/* JP: 警備員
 * EN: Guard */
const char guardText[7] = "\267\331\310\367\260\367";

/* JP: 門番
 * EN: Gatekeeper */
const char gatekeeperText[5] = "\314\347\310\326";

/* JP: ルート
 * EN: Route */
const char routeText[7] = "\245\353\241\274\245\310";

/* JP: 往復
 * EN: Round trip */
const char roundTripText[5] = "\261\375\311\374";

/* JP: 壁
 * EN: Wall */
const char wallText[3] = "\312\311";

/* JP: 浮遊
 * EN: Float */
const char floatText[5] = "\311\342\315\267";

/* JP: 逃亡
 * EN: Flee */
const char fleeText[5] = "\306\250\313\264";

/* JP: ぬる
 * EN: Null */
const char locatorNullText[7] = "\244\314\244\353\241\241";

/* JP: ／Ｂ
 * EN: / B */
const char locatorSlashBText[7] = "\241\277\243\302\241\241";

/* JP: ／Ｔ
 * EN: / T */
const char locatorSlashTText[7] = "\241\277\243\324\241\241";

/* JP: ●Ｂ
 * EN: (B) */
const char locatorParenBText[7] = "\241\374\243\302\241\241";

/* JP: ■Ｂ中
 * EN: [B] active */
const char locatorBracketBActiveText[7] = "\242\243\243\302\303\346";

/* JP: ■Ｂ
 * EN: [B] */
const char locatorBracketBText[7] = "\242\243\243\302\241\241";

/* JP: ■Ｔ
 * EN: [T] */
const char locatorBracketTText[7] = "\242\243\243\324\241\241";

/* JP: ヌル
 * EN: Null */
const char locatorNoneText[7] = "\245\314\245\353\241\241";

/* JP: 北↑
 * EN: North Up */
const char directionNorthUpText[5] = "\313\314\242\254";

/* JP: 東→
 * EN: East Right */
const char directionEastRightText[5] = "\305\354\242\252";

/* JP: 南↓
 * EN: South Down */
const char directionSouthDownText[5] = "\306\356\242\255";

/* JP: 西←
 * EN: West Left */
const char directionWestLeftText[5] = "\300\276\242\253";

/* JP: 通常
 * EN: Normal */
const char freezeNormalText[5] = "\304\314\276\357";

static inline int printEnemyCounters(int y, const char *format,
    int current, int limit)
{
    xglFontDebugPrintf(8, y, format, current, limit);
    return y + 12;
}

static inline void printEnemyFinalCounters(int y, const char *format,
    int current, int limit)
{
    xglFontDebugPrintf(8, y, format, current, limit);
}

static inline int printEnemyCounter(int y, const char *format, int count)
{
    xglFontDebugPrintf(8, y, format, count);
    return y + 12;
}

static inline int printEnemyAction(int y, int action, const char *label,
    int hit_count, int hit_limit)
{
    xglFontDebugPrintf(8, y, ENEMY_MODE_FORMAT, action, label, hit_count, hit_limit);
    return y + 12;
}

static inline int printEnemyPause(int y, int pause_kind, const char *label)
{
    xglFontDebugPrintf(8, y, ENEMY_TYPE_FORMAT, pause_kind, label);
    return y + 12;
}

static inline int printEnemyFreeze(int y, int freeze_state, const char *label)
{
    xglFontDebugPrintf(8, y, ENEMY_FREEZE_FORMAT, freeze_state, label);
    return y + 12;
}

static inline int printEnemyHiddenUnit(int y, int hidden_unit)
{
    xglFontDebugPrintf(8, y, ENEMY_HIDDEN_UNIT_FORMAT, hidden_unit);
    return y + 12;
}

static inline int printEnemyFlag(int y, const char *format, int value,
    const char *label)
{
    xglFontDebugPrintf(8, y, format, value, label);
    return y + 12;
}

static inline int printEnemyLookAt(int y, const EnemyWork *enemy, const ActorPauseView *selected)
{
    const char *height_format = ENEMY_HEIGHT_LOOK_AT_FORMAT;
    double display_height = enemy->height;
    xglFontDebugPrintf(8, y, height_format, display_height,
        selected->look_at_state, selected->look_at_target);
    return y + 12;
}

static inline int printEnemyTerritoryID(int y, int index, unsigned long long id)
{
    xglFontDebugPrintf(8, y, ENEMY_MOVE_ID_FORMAT, index, id);
    return y + 12;
}

static inline int printEnemyMark(int y, int mark, const char *label)
{
    xglFontDebugPrintf(8, y, ENEMY_MARK_KIND_FORMAT, mark, label);
    return y + 12;
}

static inline int printLocatorHeaderRow(int y, int index,
    const char *const *label, double first_component,
    double third_component, double fourth_component)
{
    xglFontDebugPrintf(0, y, ENEMY_LOCATOR_ROW_FORMAT, index, first_component,
        third_component, fourth_component, *label);
    return y + 12;
}

static inline int printEnemyMotion(int y, int row, int slot,
    int motion, int motion_id)
{
    xglFontDebugPrintf(8, y, ENEMY_MOTION_FRAME_FORMAT, row, slot, motion, motion_id);
    return y + 10;
}

static inline int printEnemyPointSummary(int y, int parameter,
    int spline_count, int point_count)
{
    xglFontDebugPrintf(8, y, ENEMY_SPLINE_SUMMARY_FORMAT, parameter, spline_count * 128, point_count);
    return y + 12;
}

static inline int printEnemySplineSummary(int y, int parameter, int count)
{
    return printEnemyPointSummary(y, parameter, count, count);
}

static inline int printEnemyHeading(int x, int y, const char *format)
{
    xglFontDebugPrintf(x, y, format);
    return y + 24;
}

static inline int printEnemySound(int y, int slot, int sound_effect)
{
    xglFontDebugPrintf(8, y, ENEMY_SOUND_SLOT_FORMAT, slot, sound_effect);
    return y + 12;
}

static inline int printEnemySplinePoint(int y, int row,
    double position_x, double position_y, double position_z)
{
    xglFontDebugPrintf(8, y, ENEMY_SPLINE_POINT_FORMAT, row, position_x, position_y, position_z);
    return y + 10;
}

static inline void printEnemyPageHeader(const EnemyWork *enemy)
{
    if (FLAG_FRAME_60 == FRAME_60_MODE_60)
        xglFontDebugPrintf(8, 0x18, DEBUG_WELCOME_60FRAME_TEXT);
    if (FLAG_FRAME_60 == FRAME_60_MODE_30_DEBUG)
        xglFontDebugPrintf(8, 0x18, DEBUG_MODE_30FRAME_TEXT);
    xglFontDebugPrintf(0, 0, PAGE_HEADER_FORMAT, Page_0, DebugActor, enemy->entry_id);
}

static inline int wrappedEnemyDebugPage(int page, int increment)
{
    return (page + increment) % ENEMY_PAGE_COUNT;
}

/*
 * Enemy debug page for DebugActor. Pad input changes the page, the debug actor
 * and the frame-60 mode, then the page selected by Page_0 is printed.
 */
void PauseMenuPageEnemy(void)
{
    const ActorPauseView *selected = &actor[DebugActor];
    const EnemyWork *enemy = &enepc[selected->number];
    const EnemyParams *parameters = &selected->parameters;
    int line_spacing;
    short y;
    short row;
    short column;
    unsigned short id;
    const char *actor_labels[17] = {
        PauseMenuPageEnemyNotInitializedText, PauseMenuPageEnemyAlertPoseText, PauseMenuPageEnemyAlertIntimidationText, PauseMenuPageEnemyAlertTurningText,
        PauseMenuPageEnemyAlertPatrolText, discoveredText, chasingText, stiffenText,
        returnHomeText, PauseMenuPageEnemyUniqueActionText, tentacleActiveText, PauseMenuPageEnemyExplosionDamageText,
        PauseMenuPageEnemyElectricDamageText, PauseMenuPageEnemySealDamageText, PauseMenuPageEnemyFleeRouteText, PauseMenuPageEnemyPursueRouteText, 0
    };
    const char *pause_labels[14] = {
        pauseMenuPageEnemyNullText, ENEMY_NORMAL_MODE_LABEL, tentacleText, ENEMY_ROUTE_TENTACLE_MODE_LABEL,
        ENEMY_ROUND_TRIP_TENTACLE_MODE_LABEL, guardText, gatekeeperText, ENEMY_TENTACLE_GATE_MODE_LABEL,
        routeText, roundTripText, wallText, floatText,
        fleeText, ENEMY_ROUTE_CHASE_MODE_LABEL
    };
    const char *locator_labels[14] = {
        locatorNullText, locatorSlashBText, locatorSlashTText, locatorParenBText,
        locatorBracketBActiveText, locatorBracketBText, locatorBracketTText, locatorNoneText,
        0, 0, 0, 0, 0, 0
    };
    const char *direction_labels[4] = {
        directionNorthUpText, directionEastRightText, directionSouthDownText, directionWestLeftText
    };
    const char *freeze_labels[4] = {
        freezeNormalText, ENEMY_ALGORITHM_STOP_LABEL, ENEMY_FULL_STOP_LABEL, ENEMY_FULL_STOP_CHR_LABEL
    };
    const char *mark_labels[3] = {pauseMenuPageEnemyNullText, ENEMY_SURPRISE_MARK_LABEL, ENEMY_QUESTION_MARK_LABEL};
    const char *boolean_labels[2] = {BOOLEAN_NO_LABEL, BOOLEAN_YES_LABEL};
    const char *route_labels[2] = {
        /* JP: スプライン
         * EN: Spline */
        ENEMY_STRAIGHT_LINE_LABEL, "\245\271\245\327\245\351\245\244\245\363"
    };
    DoublewordAlignedVector4 final_target;

    /* Pad input: up/down change the page, left/right change the debug actor, start cycles the frame-60 mode. */
    if (PadData.half_2a & PAD_UP)
        Page_0 = wrappedEnemyDebugPage(Page_0, ENEMY_PAGE_COUNT - 1);
    if (PadData.half_2a & PAD_DOWN)
        Page_0 = wrappedEnemyDebugPage(Page_0, 1);
    line_spacing = 12;
    if (PadData.half_2a & PAD_LEFT) {
        for (column = ACTOR_COUNT - 1; column > 0; --column) {
            short actor_index = (DebugActor + column) % ACTOR_COUNT;
            if (actor[actor_index].in_use_id > 0) {
                DebugActor = actor_index;
                break;
            }
        }
    }
    if (PadData.half_2a & PAD_RIGHT) {
        for (column = 1; column < ACTOR_COUNT; ++column) {
            short actor_index = (DebugActor + column) % ACTOR_COUNT;
            if (actor[actor_index].in_use_id > 0) {
                DebugActor = actor_index;
                break;
            }
        }
    }
    if (PadData.half_2a & PAD_START) {
        FLAG_FRAME_60 = (FLAG_FRAME_60 + 1) % FRAME_60_MODE_COUNT;
        Vibration_Set_Strong(DEBUG_VIBRATION_STRENGTH, 0, DEBUG_VIBRATION_FRAMES);
    }
    printEnemyPageHeader(enemy);
    y = line_spacing;
    /* Actor kind 1 shows only the suspected-face heading; other actors show the page for Page_0. */
    if (actor[DebugActor].kind == 1) {
        y = printEnemyHeading(0x60, y, ENEMY_SUSPECTED_FACE_TEXT);
    } else {
        switch (Page_0) {
        case ENEMY_PAGE_MOVE: {
            const Vector4 *position = &selected->position;
            UnduParamInit((UnduParam *)&UnduTemp);
            UnduTemp.header = UnduDataGetHeader(enemy->map_index, 0x8000);
            UnduCheck(position, 0, (UnduParam *)&UnduTemp);
            {
                const char *position_format = ENEMY_CURRENT_POSITION_FORMAT;
                xglFontDebugPrintf(0x60, y, ENEMY_MOVE_HEADING_TEXT);
                y += 24;
                {
                    double position_x = position->x;
                    xglFontDebugPrintf(8, y, position_format,
                        position_x, selected->position.y, selected->position.z);
                }
                y += 12;
                xglFontDebugPrintf(8, y, ENEMY_PLANNED_OFFSET_FORMAT,
                    enemy->goback_position.x, enemy->goback_position.y,
                    enemy->goback_position.z);
            }
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_TARGET_POINT_FORMAT,
                enemy->step_limit.x, enemy->step_limit.y, enemy->step_limit.z);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_INERTIA_FORMAT,
                enemy->stop_cause.x, enemy->stop_cause.y, enemy->stop_cause.z);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_ROTATION_ANGLE_FORMAT, selected->rotation.y, selected->target_angle);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_MOVE_SPEED_DISTANCE_FORMAT, parameters->move_speed, parameters->route_step_length);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_SIGHT_DISTANCE_ANGLE_FORMAT, parameters->sight_distance, parameters->sight_angle);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_CHASE_SPEED_HEARING_FORMAT, parameters->chase_speed, parameters->hearing_distance);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_GIVE_UP_DISTANCE_TIME_FORMAT,
                parameters->chase_distance, enemy->chase_frame_limit);
            {
                y += line_spacing;
                final_target = enemy->final_target;
                xglFontDebugPrintf(8, y, ENEMY_FINAL_TARGET_SIZE_FORMAT,
                    &final_target, selected->body_radius);
            }
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_AREA_ID_FORMAT, enemy->territory_index, enemy->map_index);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_ATTRIBUTE_ID_FORMAT, enemy->territory_index,
                Get_Attr(position, enemy->map_index, 0));
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_ATTRIBUTE_ID_FORMAT, enemy->territory_index,
                Get_Attr(position, enemy->map_index, 0x10));
            y += line_spacing;
            {
                int territory_index = enemy->territory_index;
                y = printEnemyTerritoryID(y, territory_index,
                    EneTerritoryID[territory_index]);
            }
            id = selected->in_use_id;
            if ((unsigned int)id - ENEMY_IN_USE_ID_BASE < ENEMY_IN_USE_ID_COUNT)
                xglFontDebugPrintf(0xc0, 0xa4, ENEMY_TITLE_TEXT);
            else
                xglFontDebugPrintf(0xc0, 0xa4, ENEMY_NPC_LABEL);
            break;
        }
        case ENEMY_PAGE_STATE:
            y = printEnemyHeading(0x60, y, ENEMY_STATE_HEADING_TEXT);
            if (selected->number != 0) {
                int pause_kind = enemy->pause_kind;
                const char *pause_label = pause_labels[pause_kind];
                y = printEnemyPause(y, pause_kind, pause_label);
            }
            {
                int action = enemy->action;
                int hit_limit = enemy->hit_limit;
                int hit_count = enemy->hit_count;
                const char *action_label = actor_labels[action];
                y = printEnemyAction(y, action, action_label, hit_count, hit_limit);
            }
            {
                int freeze_state = enemy->freeze_state;
                y = printEnemyFreeze(y, freeze_state, freeze_labels[freeze_state]);
            }
            xglFontDebugPrintf(8, y, ENEMY_STOP_CAUSE_FORMAT, enemy->stop_cause.x, enemy->stop_cause.y, enemy->stop_cause.z);
            y += line_spacing;
            y = printEnemyHiddenUnit(y, enemy->hidden_unit);
            xglFontDebugPrintf(8, y, ENEMY_COUNTER_HEADER_TEXT);
            y += line_spacing;
            y = printEnemyCounters(y, ENEMY_RIGID_COUNTERS_FORMAT,
                parameters->stiff_duration, parameters->pause_interval);
            y = printEnemyCounters(y, ENEMY_THREAT_COUNTERS_FORMAT,
                parameters->threat_duration, parameters->threat_interval);
            y = printEnemyCounter(y, ENEMY_TURN_COUNT_FORMAT, enemy->turn_count);
            y = printEnemyCounter(y, ENEMY_EXPLOSION_COUNT_FORMAT, enemy->explosion_count);
            y = printEnemyCounter(y, ENEMY_ELECTRIC_COUNT_FORMAT, enemy->electric_count);
            y = printEnemyCounter(y, ENEMY_FALL_COUNT_FORMAT, enemy->fall_count);
            y = printEnemyLookAt(y, enemy, selected);
            xglFontDebugPrintf(8, y, ENEMY_SIGHT_POSITION_FORMAT, enemy->sight_position[0], enemy->sight_position[1], enemy->sight_position[2]);
            y += 12;
            break;
        case ENEMY_PAGE_FLAGS:
            y = printEnemyHeading(0x60, y, ENEMY_FLAGS_HEADING_TEXT);
            {
                const char *locator_label = boolean_labels[
                    (unsigned short)parameters->locator_flags & ENEMY_LOCATOR_FLAG_MOTION];
                y = printEnemyFlag(y, ENEMY_MOTION_FLAG_FORMAT, parameters->locator_flags & ENEMY_LOCATOR_FLAG_MOTION,
                    locator_label);
            }
            {
                int flags = parameters->locator_flags;
                y = printEnemyFlag(y, ENEMY_TURN_FLAG_FORMAT, (flags >> 1) & 1,
                    boolean_labels[(flags & ENEMY_LOCATOR_FLAG_TURN) != 0]);
            }
            xglFontDebugPrintf(8, y, ENEMY_HEAD_SWING_FLAG_FORMAT, (parameters->locator_flags >> 2) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_HEAD_SWING) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_CANCELABLE_FLAG_FORMAT, (parameters->locator_flags >> 3) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_CANCELABLE) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_FREEZE_FLAG_FORMAT, (parameters->locator_flags >> 4) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_FREEZE) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_JAVA_TALK_FLAG_FORMAT, (parameters->locator_flags >> 16) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_JAVA_TALK) != 0]);
            y += line_spacing;
            y = printEnemyFlag(y, ENEMY_TOUCH_FLAG_FORMAT, (parameters->locator_flags >> 17) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_TOUCH) != 0]);
            xglFontDebugPrintf(8, y, ENEMY_LOCATOR_FLAG_FORMAT, (parameters->locator_flags >> 18) & 1,
                boolean_labels[(parameters->locator_flags & ENEMY_LOCATOR_FLAG_LOCATOR) != 0]);
            y += line_spacing;
            break;
        case ENEMY_PAGE_TENTACLE:
            y = printEnemyHeading(0x60, y, ENEMY_TENTACLE_HEADING_TEXT);
            xglFontDebugPrintf(8, y, ENEMY_TENTACLE_RANGE_FORMAT, enemy->tentacle_range);
            y += 12;
            y = printEnemyCounter(y, ENEMY_TENTACLE_DURATION_FORMAT, enemy->tentacle_duration);
            printEnemyFinalCounters(y, ENEMY_CHECK_START_END_FORMAT, enemy->locator_start_frame, enemy->locator_end_frame);
            y += line_spacing;
            break;
        case ENEMY_PAGE_SCALE:
            y = printEnemyHeading(0x60, y, ENEMY_SCALE_HEADING_TEXT);
            xglFontDebugPrintf(8, y, ENEMY_CURRENT_SCALE_FORMAT, enemy->scale.current);
            y += 12;
            y = printEnemyCounters(y, ENEMY_COUNTER_FORMAT,
                enemy->scale.frame, enemy->scale.duration);
            {
                const char *scale_format = ENEMY_SCALE_TRANSITION_FORMAT;
                xglFontDebugPrintf(8, y, scale_format,
                    enemy->scale.start, enemy->scale.target);
                y += 12;
                /* Leave a blank row after the scale range. */
                y += line_spacing;
            }
            printEnemyFinalCounters(y, ENEMY_ALPHA_TRANSITION_FORMAT, parameters->light_level, enemy->applied_light_level);
            y += line_spacing;
            break;
        case ENEMY_PAGE_TRAITS:
            y = printEnemyHeading(0x60, y, ENEMY_TRAITS_HEADING_TEXT);
            xglFontDebugPrintf(8, y, ENEMY_ENCOUNTER_FORMAT, enemy->state_flags & ENEMY_STATE_ENCOUNTER,
                boolean_labels[(unsigned short)enemy->state_flags & ENEMY_STATE_ENCOUNTER]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_CONVEYOR_FORMAT, (enemy->state_flags >> 2) & 1, boolean_labels[(enemy->state_flags & ENEMY_STATE_CONVEYOR) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_OUTING_FORMAT, (enemy->state_flags >> 3) & 1, boolean_labels[(enemy->state_flags & ENEMY_STATE_OUTING) != 0]);
            y += line_spacing;
            {
                int state_flags = enemy->state_flags;
                const char *route_label = route_labels[(state_flags & ENEMY_STATE_SPLINE_MOVE) != 0];
                xglFontDebugPrintf(8, y, ENEMY_MOVE_TYPE_FORMAT, (state_flags >> 4) & 1,
                    route_label);
            }
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_FLIGHT_FORMAT, (enemy->state_flags >> 5) & 1, boolean_labels[(enemy->state_flags & ENEMY_STATE_FLIGHT) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_DESTROY_FORMAT, (enemy->state_flags >> 6) & 1, boolean_labels[(enemy->state_flags & ENEMY_STATE_DESTROY) != 0]);
            y += line_spacing;
            xglFontDebugPrintf(8, y, ENEMY_DEFORM_FORMAT, (enemy->state_flags >> 7) & 1, boolean_labels[(enemy->state_flags & ENEMY_STATE_DEFORM) != 0]);
            y += line_spacing;
            {
                int freeze_flags = enemy->state_flags;
                const char *freeze_label = boolean_labels[(freeze_flags & ENEMY_STATE_CLIPPING) != 0];
                xglFontDebugPrintf(8, y, ENEMY_CLIPPING_FORMAT, (freeze_flags >> 8) & 1,
                    freeze_label, enemy->freeze_saved_speed);
            }
            y += 12;
            {
                int gravity_mode = parameters->gravity_mode;
                xglFontDebugPrintf(8, y, ENEMY_ID_DISABLED_FORMAT, gravity_mode,
                    boolean_labels[gravity_mode == 1]);
            }
            y += 12;
            {
                int turn_lock = enemy->turn_lock;
                xglFontDebugPrintf(8, y, ENEMY_ROTATION_LOCK_FORMAT, turn_lock,
                    boolean_labels[turn_lock]);
            }
            y += 12;
            xglFontDebugPrintf(8, y, ENEMY_MOTION_TRANSITION_FORMAT,
                selected->motion_number, enemy->previous_motion);
            y += 12;
            {
                xglFontDebugPrintf(8, y, ENEMY_CHASE_TARGET_FORMAT, enemy->target_number,
                    enemy->target_position.x, enemy->target_position.y,
                    enemy->target_position.z);
            }
            y += 12;
            {
                const unsigned int *actor_flags = &selected->flags;
                xglFontDebugPrintf(8, y, ENEMY_FLAGS_LABEL);
                {
                    int x = 0xd0;
                    for (column = 0; column < 16; ++column, x -= 8) {
                        xglFontDebugPrintf(x, y, "%1d",
                            (*actor_flags >> column) & 1);
                    }
                }
                y += line_spacing;
                {
                    const char *high_flag_label = boolean_labels[(*actor_flags & ACTOR_FLAG_RETURN_HOME) != 0];
                    xglFontDebugPrintf(8, y, ENEMY_RETURN_HOME_FORMAT, *actor_flags >> 31, high_flag_label);
                }
            }
            y += 12;
            /* Leave a blank row before the footer. */
            y += line_spacing;
            break;
        case ENEMY_PAGE_LOCATOR: {
            y = printEnemyHeading(0x60, y, ENEMY_LOCATOR_HEADING_TEXT);
            for (column = 0; column < ENEMY_LOCATOR_ROW_COUNT; ++column) {
                LayoutHeader *entry = UnduDataGetHeader(ENEMY_LOCATOR_LIST_HEADER_ID, column);
                if (entry == 0)
                    break;
                {
                    int locator_type = Get_LocaterType(column / 2);
                    const char *const *label = &locator_labels[locator_type];
                    y = printLocatorHeaderRow(y, column, label, entry->locator.x,
                        entry->locator.z, entry->locator.w);
                }
            }
            break;
        }
        case ENEMY_PAGE_LADDER: {
            y = printEnemyHeading(0x60, y, ENEMY_LADDER_HEADING_TEXT);
            {
                const GameLoopStateRecord *runtime = &GameLoopState;
                const float *values = runtime->debug_values;
                for (column = 0; column < 16; ++column) {
                    const char *direction = direction_labels[runtime->debug_directions[column]];
                    xglFontDebugPrintf(0, y, ENEMY_COLUMN_DIRECTION_FORMAT, column, direction,
                        values[column], boolean_labels[runtime->debug_enabled[column]]);
                    y += line_spacing;
                }
            }
            break;
        }
        case ENEMY_PAGE_CIRCLE: {
            const short *sound_effect;
            y = printEnemyHeading(0x60, y, ENEMY_CIRCLE_HEADING_TEXT);
            sound_effect = parameters->monster_set_numbers;
            for (column = 0; column < ENEMY_MONSTER_SET_COUNT; ++column, ++sound_effect) {
                y = printEnemySound(y, column, *sound_effect);
            }
            break;
        }
        case ENEMY_PAGE_SATELLITE: {
            int mark;
            y = printEnemyHeading(0x60, y, ENEMY_SATELLITE_HEADING_TEXT);
            mark = enemy->mark;
            y = printEnemyMark(y, mark, mark_labels[mark]);
            printEnemyFinalCounters(y, ENEMY_COUNTER_RANGE_FORMAT, enemy->mark_timer, ENEMY_MARK_TIMER_MAX);
            y += line_spacing;
            break;
        }
        case ENEMY_PAGE_SPLINE_FIRST: {
            y = printEnemyHeading(0x60, y, ENEMY_SPLINE_FIRST_HEADING_TEXT);
            y = printEnemySplineSummary(y, enemy->spline_parameter,
                enemy->spline_point_count);
            y = printEnemyCounter(y, ENEMY_WINDUP_DURATION_FORMAT, enemy->fast_point_count);
            row = 0;
            if (enemy->spline_point_count > 0) {
                do {
                    const float *point_z = &enemy->spline_coordinates[row * 4 + 2];
                    y = printEnemySplinePoint(y, row,
                        point_z[-2], point_z[-1], *point_z);
                    ++row;
                } while (row < enemy->spline_point_count);
            }
            break;
        }
        case ENEMY_PAGE_SPLINE_PLAN: {
            y = printEnemyHeading(0x60, y, ENEMY_SPLINE_PLAN_HEADING_TEXT);
            y = printEnemySplineSummary(y, enemy->spline_parameter,
                enemy->spline_point_count);
            xglFontDebugPrintf(8, y, ENEMY_ROUTE_ORDER_FORMAT, enemy->route_index,
                enemy->action_index * 128, enemy->spline_point_count);
            y += line_spacing;
            for (row = 0; row < enemy->spline_point_count; ++row) {
                for (column = 0; column < 4; ++column) {
                    int motion = enemy->motion_tables[0][row][column];
                    if (motion != -1) {
                        int motion_id = enemy->motion_tables[1][row][column];
                        y = printEnemyMotion(y, row, column, motion, motion_id);
                    }
                }
            }
            break;
        }
        case ENEMY_PAGE_PAUSE_EVENT:
        case ENEMY_PAGE_THREAT_EVENT: {
            short count;
            if (Page_0 == ENEMY_PAGE_PAUSE_EVENT) {
                y = printEnemyHeading(0x60, y, ENEMY_PAUSE_EVENT_HEADING_TEXT);
                count = parameters->pause_interval;
            } else {
                y = printEnemyHeading(0x60, y, ENEMY_THREAT_EVENT_HEADING_TEXT);
                count = parameters->threat_interval;
            }
            {
                short page = Page_0 - ENEMY_PAGE_PAUSE_EVENT;
                y = printEnemyCounters(y, ENEMY_COUNTER_RANGE_FORMAT, enemy->page_count[page], count);
                for (row = 0; row < count; ++row) {
                    int status = enemy->page_status[page][row];
                    const char *status_label = boolean_labels[status == 1];
                    xglFontDebugPrintf(8, y, ENEMY_STATUS_ROW_FORMAT, row, status_label);
                    y += line_spacing;
                }
            }
            break;
        }
        case ENEMY_PAGE_RETURN_TOOL: {
            y = printEnemyHeading(8, y, DEBUG_RETURN_TOOL_TITLE_TEXT);
            {
                short spline_count = enemy->spline_point_count;
                short spline_parameter = enemy->spline_parameter;
                short point_count = enemy->route_point_count;
                xglFontDebugPrintf(8, y, ENEMY_SPLINE_SUMMARY_FORMAT, spline_parameter,
                    spline_count * 128, point_count);
            }
            y += 12;
            for (row = 0; row < enemy->route_point_count; ++row) {
                    const float *point_z = &enemy->route_coordinates[row * 4 + 2];
                    xglFontDebugPrintf(8, y, ENEMY_ROW_POINT_FORMAT, row,
                        point_z[-2], *point_z);
                    if (enemy->route_metadata[ENEMY_HOMING_POSITION_COUNT + row] != -1)
                        xglFontDebugPrintf(0xb0, y, ENEMY_ROW_INDEX_FORMAT,
                            enemy->route_metadata[ENEMY_HOMING_POSITION_COUNT + row]);
                    else
                        xglFontDebugPrintf(0xb0, y, ENEMY_NONE_LABEL);
                    for (column = 0; column < enemy->route_metadata[row]; ++column) {
                        xglFontDebugPrintf(0xc0 + column * 0x10, y, "%2d",
                            enemy->route_values[row][column]);
                    }
                    y += 10;
            }
            /* L2 and R2 move the selected route row, which the cursor is drawn on. */
            {
                unsigned short route_buttons = PadData.half_2a;
                if (route_buttons & PAD_L2)
                    SelNum = (SelNum + enemy->route_point_count - 1) % enemy->route_point_count;
                if (route_buttons & PAD_R2)
                    SelNum = (SelNum + 1) % enemy->route_point_count;
                xglFontDebugPrintf(0, SelNum * 10 + 48, selectCursorText);
            }
            break;
        }
        }
    }
    xglFontDebugPrintf(8, y, downText);
}

/* Draws the debug marker model `entry` one unit above position, spinning with Times and lit by the studio light. */
void Debug_Mark(const Vector4 *position, int entry)
{
    Matrix4 place;
    Vector4 translation;
    Vector4 rotation;
    Matrix4 colorMatrix;
    Matrix4 normalMatrix;
    StudioLight *light;

    rotation.y = (float)Times * PAUSE_MENU_PAGE_ENEMY_TWO_PI * 0.015625f;
    translation.z = position->z;
    translation.x = position->x;
    translation.y = position->y + 1.0f;
    translation.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&translation.x);
    xglMatrixStackRotY(rotation.y);
    xglMatrixStackSave(place);

    xglStudioGetLight(&light);
    xglLightCalcMatrix(light);
    /* The SDK copies the two matrix blocks through its fixed scratch GPR. */
    QuadwordCopyMatrix(colorMatrix, &light->color_modifier_matrix);
    QuadwordCopyMatrix(normalMatrix, &light->normal_matrix);
    nmlModelSetLight(&colorMatrix, &normalMatrix);
    nmlModelSetPlace(&place);
    nmlModelEntry(entry);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char selectCursorText[2] = ">";

/* JP: ↓
 * EN: Down */
const char downText[3] = "\242\255";

short EventID = 0;

signed char FlagExEvent = 0;

/* Partial byte representation: original signedness and semantic type remain unknown. */
unsigned char FlagOnEvent = 0;
