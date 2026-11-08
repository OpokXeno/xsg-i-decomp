#include "common.h"
#include "game_movie.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "main/xgl_font.h"
#include "main/xgl_cd.h"
#include "main/party.h"
#include "main/game_movie.h"
#include "main/game_pause_disp.h"
#include "main/nml_model_set.h"
#include "main/res_get_path.h"
#include "main/xgl_dma.h"
#include "main/xgl_sound.h"

#define MOVIE_FLAG_SKIP_ALL 0x10000000
#define PAD_BUTTON_SKIP PAD_TRIANGLE
#define PAD_BUTTON_REPLAY PAD_CIRCLE
#define PAD_BUTTON_START PAD_START
/* SELECT + L1 + R1 held while pressing START aborts all further movies. */
#define PAD_COMBO_ABORT (PAD_SELECT | PAD_L1 | PAD_R1)

/* GameLoopState.input_delay frames before a pad press is honoured again. */
#define MOVIE_INPUT_DELAY 32
#define MOVIE_INPUT_DELAY_AFTER_SKIP 64

/* xglSoundEffectNormalDirect id played when the movie pause opens/closes. */
#define MOVIE_SE_PAUSE 4

/* Caption and seek-error text: white, at fixed screen positions. */
#define MOVIE_TEXT_WHITE 0xFFFFFF
#define MOVIE_CAPTION_X 64
#define MOVIE_CAPTION_Y 317
#define MOVIE_SEEK_ERROR_X 96
#define MOVIE_SEEK_ERROR_Y 200

/* Audio buffer handed to xglMpeg2InfoInit2, and the display buffer that
 * follows the source data in GameMovieInit's allocation. */
#define MOVIE_MPEG_AUDIO_BUFFER_SIZE 0x40000
#define MOVIE_DISPLAY_BUFFER_SIZE 0x8000

/* Byte-sized settings the Java class xeno.Movie writes through its
 * "transparent" and "alpha" fields. */
typedef enum MovieTransparency {
    MOVIE_TRANSPARENCY_OFF = 0,
    MOVIE_TRANSPARENCY_ON = 1
} __attribute__((packed)) MovieTransparency;

typedef enum MovieAlpha {
    MOVIE_ALPHA_OPAQUE = 0x80
} __attribute__((packed)) MovieAlpha;

extern MovieGameLoopState GameLoopState;
extern MovieRenderState sRender;
extern MovieTransparency GameMovieTransparent;
extern MovieAlpha GameMovieAlpha;
extern short GameMovieFrame;

/* Defined in src/main/xgl_1.c. */
extern int xglMovieClose(void *movie);

extern void xglMovieInfoInit(MovieInfo *movie);
/* Defined in src/main/xgl_1.c. */
extern int xglMovieOpen(void *movie, char *name);

extern int SCRIPT_getEventActiveFlag(void);
extern int SCRIPT_getFadeTime(void);
extern void SCRIPT_sendMovieSkipSignal(void);
extern void nmlModelSetActiveFadeInCancel(int fadeTime);
extern int xglMoviePlay(void *movie);
extern char *GameResourceGetFreeAddr(void);
extern void nmlModelSendSignalMovieStart(void);
extern void nmlModelSendSignalMovieFinish(int mode);
extern void xglRenderClearDepth(void);
extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);
extern int xglMpeg2Open(void *info, const char *name);
extern int xglMpeg2Play(void *info);
extern int xglMpeg2Close(void *info);
extern void xglRenderCopyDisp2Draw(void);
extern void PartyTimePauseStart(void);
static char *caption_nextline(char *cursor);
static char *caption_getparam(char *cursor, int *time, char *out);
static void caption_convert(char *text);

const char movieCaptionControl[] =
    /* Text: {effect 3}{style 3}{line height 13}{char width 8}{spacing 0,0} */
    "\015\003\031\003\025\002\001\015\025\002\002\010\025\003\003\000\000";

const char movieSeekErrorText[] =
    /* JP: ○ Button: Replay △ Button: Skip
     * EN: Circle Button: Replay Triangle Button: Skip */
    "\013\015\003A seek error has occured.\n\n\n"
    "\014\200  \241\373\014\200\200\200Button: Replay\n\n"
    "\014 \200 \242\244\014\200\200\200Button: Skip";

static MovieOverlayPacket TestEnv_0_00367980 = {
    { 0, 0x5100000C00000000ULL },
    { 0xB0AB400000008001ULL, 0x0000053531EEEEEEULL },
    {
        { 0, 0x3f }, { 0x60, 0x14 }, { 0, 6 },
        { 0, 8 }, { 0x7000d, 0x47 }, { 0, 0x42 }
    },
    { 0x80, 0x80, 0x80, 0x80 },
    { 0, 0, 0, 0 },
    { 0x6ff8, 0x71f7, 0x00f00000, 0 },
    { 0x2000, 0x1c00, 0, 0 },
    { 0x8ff8, 0x8df7, 0x00f00000, 0 }
};

static MovieInfo mi;

static inline char caption_peek(char *cursor)
{
    return *cursor;
}

/* Per-frame update while a movie is active: plays the next frame unless flags bit 0 is set, then sends the overlay packet. */
void GameMovieMain(void)
{
    if (mi.state != 0) {
        if (!(GameLoopState.flags & 1)) {
            mi.transparent = GameMovieTransparent;
            xglMoviePlay(&mi);
        }

        GameMovieFrame = mi.frame;
        TestEnv_0_00367980.registers[5].value =
            ((unsigned long long)GameMovieAlpha << 32) | 100;
        TestEnv_0_00367980.registers[2].value =
            0x24020000 | (sRender.flip_base << 5) | 0x2000000640000000ULL;
        nmlModelDirectSend(5, &TestEnv_0_00367980, 13);
    }
}

void GameMovieStop(void)
{
    xglMovieClose(&mi);
    mi.state = 0;
}

/* Closes the movie that is playing, if any, then opens name. Returns the xglMovieOpen result. */
int GameMoviePlay(char *name)
{
    if (mi.state != 0) {
        xglMovieClose(&mi);
    }

    return xglMovieOpen(&mi, name);
}

/* While a script event is active, marks the movie as skipped (mode 2 unless it is already 1), signals the script and cancels the fade-in. */
static void movie_skip(int *mode)
{
    if (SCRIPT_getEventActiveFlag() != 0) {
        if (*mode != 1) {
            *mode = 2;
        }

        SCRIPT_sendMovieSkipSignal();
        nmlModelSetActiveFadeInCancel(SCRIPT_getFadeTime());
    }
}

/* Skips line breaks and ';' markers after a caption entry and returns the start of the next line. */
static char *caption_nextline(char *cursor)
{
    char character;

    do {
        if (*cursor != '\r' && *cursor != '\n') {
            if (*cursor != ';') {
                break;
            }
            cursor++;
        }
        for (;;) {
            character = *cursor++;
            if (character == '\r' || character == '\n') {
                if (*cursor != '\r' && *cursor != '\n' && *cursor != ';') {
                    break;
                }
            }
        }
    } while (0);
    return cursor;
}

/*
 * Reads one caption entry: an optional time stamp (minutes:seconds:frames, or minutes and
 * seconds), then the text up to CR, LF or a backslash, which starts a further line.
 * The text goes to out after a two-byte header; *time gets the stamp in frames, or -1.
 * Returns the start of the next entry.
 */
static char *caption_getparam(char *cursor, int *time, char *out)
{
    int frames;
    int colons;
    int value;
    int scaled;
    int width;
    char ch;
    char *text;

    if ((unsigned)((unsigned char)caption_peek(cursor) - '0') < 10) {
        colons = 0;
        frames = 0;
        for (;;) {
            value = 0;
            while ((unsigned)((unsigned char)caption_peek(cursor) - '0') < 10) {
                value = value * 10 + *cursor++ - '0';
            }
            frames += value;
            if (*cursor == ':') {
                colons++;
                scaled = frames * 15;
                frames = scaled * 2;
                if (colons == 1) {
                    frames = scaled * 4;
                }
                cursor++;
            }
            if (*cursor == ' ' || *cursor == '\t') {
                break;
            }
        }
        while (*cursor == ' ' || *cursor == '\t') {
            cursor++;
        }
        for (;;) {
            out += 2;
            text = out;
            ch = *cursor++;
            while (ch != '\\' && ch != '\r' && ch != '\n') {
                *out++ = ch;
                ch = *cursor++;
            }
            *out = 0;
            width = xglFontGetStringWidth(text);
            text[-2] = 1;
            text[-1] = ~(width / 2);
            if (ch != '\\') {
                break;
            }
            cursor++;
            *out++ = '\n';
        }
    } else {
        frames = -1;
    }

    *time = frames;
    *out = 0;
    return caption_nextline(cursor);
}

/* Converts a caption file in place: each entry gets its time stamp before its text, and the list ends at the first entry without one. */
static void caption_convert(char *text)
{
    char *src;
    int time;

    src = text;
    xglFontGetStringWidth(movieCaptionControl);
    if (*text == ';') {
        src = caption_nextline(text);
    }
    *text++ = 0;
    do {
        src = caption_getparam(src, &time, text + 2);
        text[0] = time;
        text[1] = time >> 8;
        text += 2;
        while (*text++ != 0) {
        }
    } while (time >= 0);
}

/* Plays the movie name with its caption track. Handles pause (START), skip and replay, and the skip-all combo. */
void GameMpeg2Play(char *name, int mode)
{
    MovieMpegContext mpeg;
    char *text;
    unsigned char *cursor;
    char *src;
    char *dst;
    int size;
    int status;
    int replay;
    int time;
    unsigned int pad;
    int separator;
    char next;
    int pass;

    if (!(GameLoopState.flags & MOVIE_FLAG_SKIP_ALL)) {
        xglRenderClearDepth();
        xglSoundStreamStop(0);
        nmlModelSendSignalMovieStart();
        /* Each pass plays the file once; a REPLAY press on the seek-error screen repeats it. */
        do {
            replay = 0;
            text = GameResourceGetFreeAddr();
            separator = '.';
            src = name;
            dst = text;
            if (*src != 0) {
                if (*src == separator) {
                    dst[0] = '.';
                    dst[1] = 'c';
                    dst[2] = 'a';
                    dst[3] = 'p';
                    dst[4] = 0;
                } else {
                    do {
                        *dst = *src;
                        src++;
                        dst++;
                        next = *src;
                    } while (next != 0 && next != separator);
                    if (next != 0) {
                        *dst = next;
                        dst[1] = 'c';
                        dst[2] = 'a';
                        dst[3] = 'p';
                        dst[4] = 0;
                    }
                }
            }
            /* Read the caption file into text and convert it in place; a failed read leaves size 0. */
            size = xglCdReadFile(text, text, 0, 0);
            if (size < 0) {
                size = 0;
            } else {
                caption_convert(text);
            }
            xglMpeg2InfoInit2(&mpeg, text + ((size + 63) & ~63), MOVIE_MPEG_AUDIO_BUFFER_SIZE);
            if (size == 0) {
                text = 0;
            }
            if (xglMpeg2Open(&mpeg, name) >= 0) {
                GameLoopState.input_delay = MOVIE_INPUT_DELAY;
                /* Play frames and print the caption due at the current frame. A negative status is a seek error; a positive one ends the movie. */
                for (;;) {
                    status = xglMpeg2Play(&mpeg);
                    if (text != 0) {
                        cursor = (unsigned char *)text;
                        while (*cursor++ != 0) {
                        }
                        time = cursor[0] + (cursor[1] << 8);
                        if (time >= 0 && mpeg.frame >= time) {
                            text = (char *)(cursor + 2);
                        }
                        xglFontPrint(MOVIE_CAPTION_X, MOVIE_CAPTION_Y, MOVIE_TEXT_WHITE,
                                     movieCaptionControl);
                        xglFontPrintDirect(text);
                    }
                    if (status < 0) {
                        xglRenderCopyDisp2Draw();
                        for (pass = 0; pass < 2; pass++) {
                            xglFontPrint(MOVIE_SEEK_ERROR_X, MOVIE_SEEK_ERROR_Y,
                                         MOVIE_TEXT_WHITE, movieSeekErrorText);
                            GamePauseDispBG();
                            xglSleep();
                        }
                        PartyTimePauseStart();
                        for (;;) {
                            if (PadData.half_2a & PAD_BUTTON_SKIP) {
                                movie_skip(&mode);
                                break;
                            }
                            if (PadData.half_2a & PAD_BUTTON_REPLAY) {
                                replay = 1;
                                break;
                            }
                            xglSleep();
                        }
                        PartyTimePauseEnd();
                        break;
                    }
                    if (status > 0) {
                        break;
                    }
                    /* Pad input is ignored while input_delay counts down; START opens the pause screen. */
                    if (GameLoopState.input_delay != 0) {
                        GameLoopState.input_delay--;
                    } else if (PadData.half_2a & PAD_BUTTON_START) {
                        GameLoopState.input_delay = MOVIE_INPUT_DELAY;
                        if ((PadData.half_28 & PAD_COMBO_ABORT) == PAD_COMBO_ABORT) {
                            GameLoopState.flags |= MOVIE_FLAG_SKIP_ALL;
                            break;
                        }
                        /* Pause screen: SKIP skips the movie, START resumes it. */
                        xglSoundEffectNormalDirect(MOVIE_SE_PAUSE);
                        sRender.scene_disabled = 1;
                        xglRenderCopyDisp2Draw();
                        GamePauseDispEvent();
                        xglSleep();
                        GamePauseDispEvent();
                        xglSleep();
                        PartyTimePauseStart();
                        for (;;) {
                            pad = PadData.half_2a;
                            if (pad & PAD_BUTTON_SKIP) {
                                movie_skip(&mode);
                                break;
                            }
                            if (GameLoopState.input_delay != 0) {
                                GameLoopState.input_delay--;
                            } else if (pad & PAD_BUTTON_START) {
                                /* START resumes without skipping the movie. */
                                goto finish_movie_pause;
                            }
                            xglSleep();
                        }
finish_movie_pause:
                        PartyTimePauseEnd();
                        xglSoundEffectNormalDirect(MOVIE_SE_PAUSE);
                        sRender.scene_disabled = 0;
                        GameLoopState.input_delay = MOVIE_INPUT_DELAY;
                        /* Leaving the pause with SKIP ends the movie, with a longer input delay. */
                        if (PadData.half_2a & PAD_BUTTON_SKIP) {
                            GameLoopState.input_delay = MOVIE_INPUT_DELAY_AFTER_SKIP;
                            break;
                        }
                        xglSoundStreamMute();
                    }
                    xglSleep();
                }
                xglMpeg2Close(&mpeg);
            }
        } while (replay != 0);
        xglRenderCopyDisp2Draw();
        nmlModelSendSignalMovieFinish(mode);
        xglSleep();
    }
}

/* Allocates the movie buffers for size bytes of source data and resets the movie state. */
void GameMovieInit(int size)
{
    unsigned int span;
    unsigned char *buffer;
    MovieAlpha *alpha = &GameMovieAlpha;
    MovieTransparency *transparent = &GameMovieTransparent;

    span = size * 2;
    buffer = GameResourceAlloc(span + 0x10000);
    xglMovieInfoInit(&mi);
    mi.source_data = buffer;
    buffer += span;
    mi.display_base = buffer;
    buffer += MOVIE_DISPLAY_BUFFER_SIZE;
    mi.opened = 1;
    mi.source_size = size;
    mi.display_end = buffer;
    mi.copy_mode = 0;
    mi.threshold1 = 0;
    mi.transparent = 0;
    *alpha = MOVIE_ALPHA_OPAQUE;
    *transparent = MOVIE_TRANSPARENCY_OFF;
}
