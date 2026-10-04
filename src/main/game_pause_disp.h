/*
 * TU-local declarations of main/tu117 (src/main/game_pause_disp.c).
 */

#ifndef SRC_MAIN_GAME_PAUSE_DISP_H
#define SRC_MAIN_GAME_PAUSE_DISP_H

extern void PauseMenu(void);

extern void GamePauseDispBG(void);

typedef union GifCommandData {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifCommandData;

typedef struct GifAdCommand {
    GifCommandData data;
    u32 register_address;
    u32 unused;
} GifAdCommand;

typedef struct GifTag {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} GifTag;

typedef struct PauseShadowCommand {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_shadow_context;
    GifAdCommand set_shadow_environment;
    GifAdCommand set_shadow_test;
    GifAdCommand set_shadow_alpha;
    GifAdCommand set_shadow_color;
} PauseShadowCommand;


/*
 * Only the packet handle at +0x00 is evidenced (DrawShadow). The rest of the
 * structure is populated by DrawCredit, which is still unresolved; this is
 * an explicit partial type and does not claim the layout beyond +0x00.
 */
/* DrawCredit writes the 128-byte direct-data payload beginning at +0x30. */
typedef struct PauseCreditData {
    unsigned int initial_words[4];
    unsigned long long wide_words[3];
    unsigned char unmodeled_28[8];
    int body_words[20];
} PauseCreditData;

typedef struct PauseDrawContext {
    void *packet;
    unsigned char unmodeled_04[0x2C];
    PauseCreditData credit_data;
} PauseDrawContext;

extern void sceVif1PkAddDirectDataN(void *packet, const void *data, int count);

extern void GamePauseDispCf(void);

extern int xglFontGetStringWidth(const char *text);


void GamePauseDispEvent(void);


/* The pause menu reads these fields from the game loop's global state. */
typedef struct PauseGameLoopState {
    unsigned char unmodeled_00[0xD0];
    signed char snapshot_slot;
    unsigned char unmodeled_d1[0x29E7F];
    unsigned char pause_mode;
    unsigned char unmodeled_29f51[0x1F];
    unsigned short snapshot_flags;
    unsigned short scanline_value;
} PauseGameLoopState;

extern PauseGameLoopState GameLoopState;
extern unsigned char SnapDrawCreditFlag;
extern int ScanLineInterpolate;
extern int GameSnapShotSave(signed char slot, int work);
extern int GameSnapShotSaveFile(int slot, int work, int size);
extern void xglFontPrintExtFunc(unsigned int flags,
                                void (*draw)(PauseDrawContext *context),
                                void *context);
extern void xglFontSetFlags(int flags);

#endif /* SRC_MAIN_GAME_PAUSE_DISP_H */
