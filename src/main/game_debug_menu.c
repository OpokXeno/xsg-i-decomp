#include "common.h"

#include "shared.h"

#include "game_debug_menu.h"

static const char * tbl_0_00367A50[7] = { cfCameraSwitchLabel, modelNumberTagLabel, snapCreditLabel, snapQualityLabel, modelNumberCheckLabel, alphaOnCaptureLabel, discCheckLabel };

static const char * tbl_1_00367A70[10] = { changeMapLabel, changeLeaderLabel, runBattleLabel, snapLabel, modelFolderLabel, mapFolderLabel, sceneFolderLabel, runCfLabel, runEventLabel, scanlineInterpolationLabel };

char hexIdFormat[];

static void PauseMenuPage0(void)
{
    int y;
    int i;

    if (PadData.half_2a & PAD_CROSS) {
        GameResourceDump(0);
    }

    xglFontDebugPrintf(136, 52, "WrkEnd:%8x", WorkEnd);

    y = 64;
    for (i = 0; i < 16; i++) {
        xglFontDebugPrintf(8, y, hexIdFormat, actor[i].inUseId);
        y += 8;
    }

    GameResourceDump(1);
}

/* JP: ディスクチェック
 * EN: Disc check */
const char discCheckLabel[17] = "\245\307\245\243\245\271\245\257\245\301\245\247\245\303\245\257";

/* JP: キャプチャ時のα
 * EN: Alpha on capture */
const char alphaOnCaptureLabel[17] = "\245\255\245\343\245\327\245\301\245\343\273\376\244\316\246\301";

/* JP: モデル番号チェック
 * EN: Model number check */
const char modelNumberCheckLabel[19] = "\245\342\245\307\245\353\310\326\271\346\245\301\245\247\245\303\245\257";

/* JP: スナップ品質
 * EN: Snap quality */
const char snapQualityLabel[13] = "\245\271\245\312\245\303\245\327\311\312\274\301";

/* JP: スナップクレジット
 * EN: Snap credit */
const char snapCreditLabel[19] = "\245\271\245\312\245\303\245\327\245\257\245\354\245\270\245\303\245\310";

/* JP: モデル番号タグ
 * EN: Model number tag */
const char modelNumberTagLabel[15] = "\245\342\245\307\245\353\310\326\271\346\245\277\245\260";

/* JP: ＣＦカメラ切り替え
 * EN: CF camera switch */
const char cfCameraSwitchLabel[19] = "\243\303\243\306\245\253\245\341\245\351\300\332\244\352\302\330\244\250";

/* JP: ＝＝　デバッグフラグ　＝＝
 * EN: == Debug flags == */
const char debugFlagsHeaderLabel[27] = "\241\341\241\341\241\241\245\307\245\320\245\303\245\260\245\325\245\351\245\260\241\241\241\341\241\341";

static void PauseMenuPage2(void)
{
    int i;
    int y;
    int step;
    int stepMagnitude;
    int quality;
    unsigned int right;
    unsigned int left;
    unsigned int *word;
    unsigned int bit;

    if (GameLoopState.debugPageInitialized == 0) {
        int slot;
        GameLoopState.debugPageInitialized = 1;
        GameLoopState.target.modelNumberCheck = MODEL_CHECK_ACTOR;
        for (slot = 0; slot < ACTOR_COUNT; slot++) {
            word = &GameLoopState.scratch.actorFlag8Clear[slot >> 5];
            bit = 1 << (slot & 0x1f);
            if (actor[slot].flags & ACTOR_CHECK_HIDDEN) {
                *word &= ~bit;
            } else {
                *word |= bit;
            }
        }
        for (slot = 0; slot < MAP_UNIT_COUNT; slot++) {
            word = &GameLoopState.mapUnitFlag4Clear[slot >> 5];
            bit = 1 << (slot & 0x1f);
            if (MapUnit[slot].flags & MAP_UNIT_CHECK_HIDDEN) {
                *word &= ~bit;
            } else {
                *word |= bit;
            }
        }
    }

    if (PadData.half_2c & PAD_UP) {
        GameLoopState.debugCursor--;
    }
    if (PadData.half_2c & PAD_DOWN) {
        GameLoopState.debugCursor++;
    }
    if (GameLoopState.debugCursor < 0) {
        GameLoopState.debugCursor = DEBUG_FLAG_COUNT - 1;
    }
    if (GameLoopState.debugCursor >= DEBUG_FLAG_COUNT) {
        GameLoopState.debugCursor = 0;
    }

    switch (GameLoopState.debugCursor) {
    case 0:
        if (PadData.half_2c & (PAD_LEFT | PAD_RIGHT | PAD_CIRCLE)) {
            GameLoopState.cfCameraFlags ^= CF_CAMERA_SWITCH;
        }
        break;
    case 1:
        if (PadData.half_2c & (PAD_LEFT | PAD_RIGHT | PAD_CIRCLE)) {
            GameLoopState.runtimeFlags ^= RUNTIME_MODEL_NUMBER_TAG;
        }
        break;
    case 2:
        if (PadData.half_2c & (PAD_LEFT | PAD_RIGHT | PAD_CIRCLE)) {
            SnapDrawCreditFlag ^= 1;
        }
        break;
    case 3: {
        int step;
        int quality;
        unsigned int right;
        unsigned int left;

        stepMagnitude = 1;
        right = PadData.half_2c & PAD_RIGHT;
        left = PadData.half_2c & PAD_LEFT;
        step = right != 0;
        if (left) {
            step = -stepMagnitude;
        }
        quality = step + GameLoopState.snapQuality;
        if (quality < -5) {
            quality = -5;
        }
        if (quality > 100) {
            quality = 100;
        }
        GameLoopState.snapQuality = quality;
        break;
    }
    case 4:
        right = PadData.half_2c & PAD_RIGHT;
        left = PadData.half_2c & PAD_LEFT;
        step = right != 0;
        if (left) {
            step = -1;
        }
        if (PadData.half_2a & 0x40) {
            GameLoopState.target.modelNumberCheck ^= 0x100;
        }
        GameLoopState.target.modelNumberCheck += step;
        if (PadData.half_28 & PAD_CIRCLE) {
            int slot;
            unsigned int kind;

            for (slot = 0; slot < ACTOR_COUNT; slot++) {
                actor[slot].flags |= ACTOR_CHECK_HIDDEN;
            }
            for (slot = 0; slot < MAP_UNIT_COUNT; slot++) {
                MapUnit[slot].flags |= MAP_UNIT_CHECK_HIDDEN;
            }
            kind = GameLoopState.target.modelNumberCheck & 0xff00;
            slot = GameLoopState.target.modelNumberCheck & 0xff;
            switch (kind) {
            case MODEL_CHECK_ACTOR:
                actor[slot].flags &= ~ACTOR_CHECK_HIDDEN;
                break;
            case MODEL_CHECK_MAP_UNIT:
                MapUnit[slot].flags &= ~MAP_UNIT_CHECK_HIDDEN;
                break;
            }
        }
        if (PadData.half_2e & PAD_CIRCLE) {
            int slot;
            unsigned int *saved;
            unsigned int (*actorSaved)[2];
            unsigned int clearMask;

            for (slot = 0; slot < ACTOR_COUNT; slot++) {
                unsigned int bit = 1 << (slot & 0x1f);
                clearMask = ~ACTOR_CHECK_HIDDEN;
                actorSaved = &GameLoopState.scratch.actorFlag8Clear;
                if ((*actorSaved)[slot >> 5] & bit) {
                    actor[slot].flags &= clearMask;
                } else {
                    actor[slot].flags |= ACTOR_CHECK_HIDDEN;
                }
            }
            saved = GameLoopState.mapUnitFlag4Clear;
            clearMask = ~MAP_UNIT_CHECK_HIDDEN;
            for (slot = 0; slot < MAP_UNIT_COUNT; slot++) {
                if (saved[slot >> 5] & (1 << (slot & 0x1f))) {
                    MapUnit[slot].flags &= clearMask;
                } else {
                    MapUnit[slot].flags |= MAP_UNIT_CHECK_HIDDEN;
                }
            }
        }
        break;
    case 5:
        if (PadData.half_2c & (PAD_LEFT | PAD_RIGHT | PAD_CIRCLE)) {
            GameLoopState.runtimeFlags ^= RUNTIME_CAPTURE_ALPHA;
        }
        break;
    case 6:
        if (PadData.half_2c & (PAD_LEFT | PAD_RIGHT | PAD_CIRCLE)) {
            GameDiskChangeT10K ^= 1;
        }
        break;
    }

    xglFontDebugPrintf(8, 52, debugFlagsHeaderLabel);
    y = 64;
    for (i = 0; i < DEBUG_FLAG_COUNT; i++) {
        if (i == GameLoopState.debugCursor) {
            xglFontDebugPrintf(16, y, fullWidthCursorText);
        }
        xglFontDebugPrintf(32, y, tbl_0_00367A50[i]);
        switch (i) {
        case 0:
            xglFontDebugPrintf(128, y + 3, hexDigitFormat,
                               (GameLoopState.cfCameraFlags >> 1) & 1);
            break;
        case 1:
            xglFontDebugPrintf(128, y + 3, hexDigitFormat,
                               (GameLoopState.runtimeFlags >> 2) & 1);
            break;
        case 2:
            xglFontDebugPrintf(128, y + 3, hexDigitFormat, SnapDrawCreditFlag);
            break;
        case 3:
            xglFontDebugPrintf(128, y + 3, snapQualityFormat, GameLoopState.snapQuality);
            break;
        case 4:
            xglFontDebugPrintf(128, y + 3, hexIdFormat, GameLoopState.target.modelNumberCheck);
            break;
        case 5:
            xglFontDebugPrintf(128, y + 3, hexDigitFormat,
                               (GameLoopState.runtimeFlags >> 25) & 1);
            break;
        case 6:
            xglFontDebugPrintf(128, y + 3, hexDigitFormat, GameDiskChangeT10K);
            break;
        }
        y += 12;
    }
}

char hexIdFormat[8] = "%4x";

/* JP: 走査線補完
 * EN: Scanline interpolation */
const char scanlineInterpolationLabel[11] = "\301\366\272\272\300\376\312\344\264\260";

/* JP: イベント実行
 * EN: Run event */
const char runEventLabel[13] = "\245\244\245\331\245\363\245\310\274\302\271\324";

/* JP: ＣＦ実行
 * EN: Run CF */
const char runCfLabel[9] = "\243\303\243\306\274\302\271\324";

/* JP: シーンフォルダ
 * EN: Scene folder */
const char sceneFolderLabel[15] = "\245\267\241\274\245\363\245\325\245\251\245\353\245\300";

/* JP: マップフォルダ
 * EN: Map folder */
const char mapFolderLabel[15] = "\245\336\245\303\245\327\245\325\245\251\245\353\245\300";

/* JP: モデルフォルダ
 * EN: Model folder */
const char modelFolderLabel[15] = "\245\342\245\307\245\353\245\325\245\251\245\353\245\300";

/* JP: スナップ
 * EN: Snap */
const char snapLabel[9] = "\245\271\245\312\245\303\245\327";

/* JP: バトル実行
 * EN: Run battle */
const char runBattleLabel[11] = "\245\320\245\310\245\353\274\302\271\324";

/* JP: リーダー変更
 * EN: Change leader */
const char changeLeaderLabel[13] = "\245\352\241\274\245\300\241\274\312\321\271\271";

/* JP: マップ変更
 * EN: Change map */
const char changeMapLabel[11] = "\245\336\245\303\245\327\312\321\271\271";

/* JP: ＝　デバッグメニュー　＝
 * EN: = Debug menu = */
const char debugMenuHeaderLabel[25] = "\241\341\241\241\245\307\245\320\245\303\245\260\245\341\245\313\245\345\241\274\241\241\241\341";

const char memoryUsedFormat[16] = "USED  :%9d";

const char memoryRemainFormat[16] = "REMAIN:%9d";

const char snapShotFileNameFormat[16] = "game%c000";

const char dataDirectoryPath[16] = "data\\[path]";

const char dataTestDirectoryPath[24] = "data\\test\\[path]";

/* JP: あり(DISP1)
 * EN: Yes (DISP1) */
const char yesDisp1Text[12] = "\244\242\244\352(DISP1)";

/* JP: あり(SOFT)
 * EN: Yes (SOFT) */
const char yesSoftText[11] = "\244\242\244\352(SOFT)";

/* JP: プレイヤー位置
 * EN: Player position */
const char playerPositionLabel[15] = "\245\327\245\354\245\244\245\344\241\274\260\314\303\326";

/* JP: ＞
 * EN: > */
char fullWidthCursorText[3] = "\241\344";

char hexDigitFormat[4] = "%1x";

char snapQualityFormat[4] = "%3d";

/* Text: {glyph 17} */
char memoryUsedGlyph[3] = "\033\021";

char debugCursorText[2] = ">";

char mapChangeIdNameFormat[7] = "%4d.%s";

char debugPathFormat[3] = "%s";

char scriptJumpIdFormat[4] = "%4d";

static void PauseMenuPage3(void)
{
    int i;
    int y;
    int dir;
    int number;
    int last;
    int used;
    int remain;
    unsigned int repeat;
    DebugPlayerUnit *player;
    DebugLayoutHeader *header;
    const char *text;

    if (GameLoopState.debugPageInitialized == 0) {
        GameLoopState.debugPageInitialized = 1;
        GameLoopState.target.mapChangeId = GameLoopState.currentMapId;
        GameLoopState.leaderBattleId = ((DebugSavedParty *)SaveData)->leaderBattleId;
        GameLoopState.scratch.scriptJump.cfId = 0;
        GameLoopState.scratch.scriptJump.eventId = 0;
    }

    repeat = PadData.half_2c;
    if (repeat & PAD_UP) {
        GameLoopState.debugCursor--;
    }
    if (repeat & PAD_DOWN) {
        GameLoopState.debugCursor++;
    }
    if (GameLoopState.debugCursor < 0) {
        GameLoopState.debugCursor = DEBUG_ROW_COUNT - 1;
    }
    if (GameLoopState.debugCursor >= DEBUG_ROW_COUNT) {
        GameLoopState.debugCursor = 0;
    }

    dir = 0;
    if (repeat & PAD_LEFT) {
        dir = -1;
    }
    if (repeat & PAD_RIGHT) {
        dir = 1;
    }
    if (PadData.half_28 & PAD_CROSS) {
        dir *= 100;
    }
    if (dir != 0) {
        switch (GameLoopState.debugCursor) {
        case 0:
            for (;;) {
                GameLoopState.target.mapChangeId += dir;
                if (GameLoopState.target.mapChangeId < 0) {
                    GameLoopState.target.mapChangeId = MAP_COUNT - 1;
                }
                if (GameLoopState.target.mapChangeId >= MAP_COUNT) {
                    GameLoopState.target.mapChangeId = 0;
                }
                if (MapGetName(GameLoopState.target.mapChangeId) == 0) {
                    dir = dir > 0 ? 1 : -1;
                    continue;
                }
                break;
            }
            break;
        case 1:
            GameLoopState.leaderBattleId += dir;
            break;
        case 3:
            number = GameSnapShotNumber(-1) + dir;
            if (number < 0) {
                number = 0;
            }
            GameSnapShotNumber(number < 10 ? number : 9);
            break;
        case 4:
            ModelPath = (ModelPath + dir) & 1;
            break;
        case 5:
            UseTestPath = (UseTestPath + dir + 3) % 3;
            break;
        case 6:
            ScenePath = (ScenePath + dir + 20) % 20;
            break;
        case 7:
            GameLoopState.scratch.scriptJump.cfId =
                (GameLoopState.scratch.scriptJump.cfId + dir + 10000) % 10000;
            break;
        case 8:
            GameLoopState.scratch.scriptJump.eventId =
                (GameLoopState.scratch.scriptJump.eventId + dir + 10000) % 10000;
            break;
        case 9:
            ScanLineInterpolate = (ScanLineInterpolate + dir + 3) % 3;
            break;
        }
    }

    if (PadData.half_2a & PAD_CIRCLE) {
        switch (GameLoopState.debugCursor) {
        case 0: {
            DebugPlayerUnit *leader;

            MapChange(GameLoopState.target.mapChangeId);
            leader = GameLoopState.player;
            leader->x = 0;
            leader->y = 0;
            leader->z = 0;
            leader->angle = 0;
            header = UnduDataGetHeader(513, 0x8000);
            if (header != 0) {
                leader->x = header->components[0];
                leader->y = header->components[1];
                leader->z = header->components[2];
                leader->layoutAngle = leader->angle = header->components[3];
            }
            break;
        }
        case 1:
            ((DebugSavedParty *)SaveData)->leaderBattleId = GameLoopState.leaderBattleId;
            GameCfPlayerLoadResource(0);
            break;
        case 2:
            GameStateSave();
            GameLoopState.mode = 5;
            func_A22668();
            GameStateRestore();
            GameLoopState.debugPageInitialized = 0;
            break;
        case 7:
            Java_xeno_util_Runtime_jump_sub(GameLoopState.scratch.scriptJump.cfId, -1);
            break;
        case 8:
            Java_xeno_util_Runtime_jump_sub(
                SCRIPT_ID_EVENT_BASE + GameLoopState.scratch.scriptJump.eventId, -1);
            GameLoopState.runtimeFlags &= ~1;
            break;
        }
        PadData.half_2a = 0;
    }

    xglFontDebugPrintf(8, 52, debugMenuHeaderLabel);
    last = GAME_RESOURCE_COUNT - 1;
    while (GameResource[last].address == 0 && last != 0) {
        last--;
    }
    remain = GameResource[last].size;
    used = 0;
    for (last--; last >= 0; last--) {
        used += GameResource[last].size;
    }
    xglFontDebugPrintf(136, 48, memoryUsedFormat, used + DebugMenuResourceSizeAdd);
    if (DebugMenuResourceSizeAdd != 0) {
        xglFontDebugPrintf(246, 48, memoryUsedGlyph);
    }
    xglFontDebugPrintf(136, 56, memoryRemainFormat, remain);

    y = 64;
    for (i = 0; i < DEBUG_ROW_COUNT; i++) {
        if (i == GameLoopState.debugCursor) {
            xglFontDebugPrintf(8, y + 3, debugCursorText);
        }
        xglFontDebugPrintf(16, y, tbl_1_00367A70[i]);
        switch (i) {
        case 0:
            xglFontDebugPrintf(80, y + 3, mapChangeIdNameFormat, GameLoopState.target.mapChangeId,
                               MapGetName(GameLoopState.target.mapChangeId));
            break;
        case 1:
            xglFontDebugPrintf(80, y + 3, hexIdFormat, GameLoopState.leaderBattleId);
            break;
        case 3:
            xglFontDebugPrintf(72, y + 3, snapShotFileNameFormat, GameSnapShotNumber(-1) + '0');
            break;
        case 4:
            if (ModelPath == 0) {
                xglFontDebugPrintf(92, y + 3, dataDirectoryPath);
            } else {
                xglFontDebugPrintf(92, y + 3, dataTestDirectoryPath);
            }
            break;
        case 5:
            xglFontDebugPrintf(92, y + 3, debugPathFormat, MAP_getPath());
            break;
        case 6:
            xglFontDebugPrintf(92, y + 3, debugPathFormat, RES_getPath());
            break;
        case 7:
            xglFontDebugPrintf(88, y + 3, scriptJumpIdFormat, GameLoopState.scratch.scriptJump.cfId);
            break;
        case 8:
            xglFontDebugPrintf(88, y + 3, scriptJumpIdFormat, GameLoopState.scratch.scriptJump.eventId);
            {
                char fileName[7] = "cf0000";
                int scriptId = GameLoopState.scriptId;

                if (scriptId <= 0x0fffffff) {
                    fileName[0] = 'c';
                    fileName[1] = 'f';
                } else {
                    fileName[0] = 'e';
                    fileName[1] = 'v';
                    scriptId &= 0xffff;
                }
                fileName[5] = scriptId % 10 + '0';
                scriptId /= 10;
                fileName[4] = scriptId % 10 + '0';
                scriptId /= 10;
                fileName[3] = scriptId % 10 + '0';
                scriptId /= 10;
                fileName[2] = scriptId % 10 + '0';
                xglFontDebugPrintf(120, y - 3, fileName);
            }
            break;
        case 9:
            switch (ScanLineInterpolate) {
            case 0:
            default:
                text = noText;
                break;
            case 1:
                text = yesDisp1Text;
                break;
            case 2:
                text = yesSoftText;
                break;
            }
            xglFontDebugPrintf(80, y, text);
            break;
        }
        y += 12;
    }

    player = GameLoopState.player;
    xglFontDebugPrintf(176, 160, playerPositionLabel);
    xglFontDebugPrintf(176, 172, "%f\n%f\n%f", player->x, player->y, player->z);
}

/* JP: なし
 * EN: No */
char noText[5] = "\244\312\244\267";

int DebugMenuResourceSizeAdd = 0;

void GameDebugMenu(void)
{
    int pageStep;

    pageStep = 0;
    if (PadData.half_2a & PAD_L1) {
        pageStep--;
    }
    if (PadData.half_2a & PAD_R1) {
        pageStep = 1;
    }
    if (pageStep) {
        unsigned int nextPage =
            (GameLoopState.debugPage + pageStep + 6) % 6;
        GameLoopState.debugPageInitialized = 0;
        GameLoopState.debugPage = nextPage;
    }
    switch (GameLoopState.debugPage) {
    case 0:
        PauseMenuPage3();
        return;
    case 1:
        PauseMenuPage0();
        return;
    case 2:
        PauseMenuPage2();
        return;
    case 3:
        PauseMenuPageUwamono();
        return;
    case 4:
        PauseMenuPageEnemy();
        return;
    case 5:
        PauseMenuPagePartyDebug();
        break;
    }
}
