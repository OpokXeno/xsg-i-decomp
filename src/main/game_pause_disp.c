#include "common.h"
#include "shared.h"
#include "game_pause_disp.h"
#include "main/xgl_packet.h"

static PauseShadowCommand ShadowEnv = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000006,
    .gif_tag = {0x00008001, 0x50234000, 0x000551EE, 0},
    .set_shadow_context = {{.value = 0x0000000000071001ULL}, 0x47, 0},
    .set_shadow_environment = {{.value = 0x44}, 0x42, 0},
    .set_shadow_test = {{.value = 0}, 0, 0x40},
    .set_shadow_alpha = {{.words = {0x00006FF8, 0x000071F7}}, 0x40000000, 0},
    .set_shadow_color = {{.words = {0x00008FF8, 0x00008DF7}}, 0x40000000, 0},
};

/* In-band font commands precede the English pause captions. */
static char pause_0[13] = "\013\015\003\031\003PAUSE\031\002";
static char msg1_1[44] = "\013\015\003\014 \200 \242\244\014\200\200\200\031\003 Button : Skip and proceed\031\002";
static char msg2_2[35] = "\013\015\003\031\003START Button : Cancel PAUSE\031\002";

static void DrawShadow(PauseDrawContext *context)
{
    sceVif1PkAddDirectDataN(context->packet, &ShadowEnv, 7);
}

static void DrawCredit(PauseDrawContext *context)
{
    PauseCreditData *data = &context->credit_data;
    int first_vertex_x = 30072;
    int first_vertex_y = 0x8C37;
    int vertex_depth = -1;

    data->initial_words[0] = 0x8001;
    data->initial_words[1] = 0x70AB4000;
    data->initial_words[2] = 0x0535316E;
    data->initial_words[3] = 0;
    data->wide_words[0] = 0x0007000D;
    data->wide_words[1] = 71;
    data->wide_words[2] = 0x2007ED8629343C00ULL;
    data->body_words[0] = 128;
    data->body_words[1] = 128;
    data->body_words[2] = 128;
    data->body_words[3] = 128;
    data->body_words[12] = 10752;
    data->body_words[13] = 752;
    data->body_words[16] = 0x8F78;
    data->body_words[17] = 0x8D77;
    data->body_words[11] = 0;
    data->body_words[19] = 0;
    data->body_words[4] = 0x1000;
    data->body_words[5] = 432;
    data->body_words[10] = vertex_depth;
    data->body_words[8] = first_vertex_x;
    data->body_words[9] = first_vertex_y;
    data->body_words[18] = vertex_depth;

    sceVif1PkAddDirectDataN(context->packet, &context->credit_data, 8);
}

void PauseMenu(void)
{
    int pause_mode = GameLoopState.pause_mode;
    unsigned int snapshot_work;
    int snapshot_size;
    int scanline_value;

    switch (pause_mode) {
    case 0:
        xglFontPrintExtFunc(0x00FFFFFF, DrawShadow, 0);
        GamePauseDispCf();
        return;
    case 100:
    case 101:
        if (SnapDrawCreditFlag != 0) {
            xglFontPrintExtFunc(0xFFFFFFFF, DrawCredit, 0);
        }
        GameLoopState.pause_mode = GameLoopState.pause_mode + 1;
        return;
    case 102:
        break;
    default:
        return;
    }
    snapshot_work = ((unsigned int)WorkEnd + 63) & ~63u;
    snapshot_size = GameSnapShotSave(GameLoopState.snapshot_slot, snapshot_work);
    GameSnapShotSaveFile(-1, snapshot_work, snapshot_size);
    xglFontSetFlags(GameLoopState.snapshot_flags);
    scanline_value = GameLoopState.scanline_value;
    GameLoopState.pause_mode = 0;
    ScanLineInterpolate = scanline_value;
}

void GamePauseDispBG(void)
{
    sceVif1PkRef(xglPacketGetCurrent(), &ShadowEnv, 7, 0, 0, 0);
}

void GamePauseDispCf(void)
{
    xglFontPrint(256 - xglFontGetStringWidth(pause_0) / 2,
                 200, 0x00ffffff, pause_0);
    GamePauseDispBG();
}

void GamePauseDispEvent(void)
{
    GamePauseDispCf();
    xglFontPrint(256 - xglFontGetStringWidth(msg1_1) / 2,
                 256, 0x00ffffff, msg1_1);
    xglFontPrint(256 - xglFontGetStringWidth(msg2_2) / 2,
                 288, 0x00ffffff, msg2_2);
}
