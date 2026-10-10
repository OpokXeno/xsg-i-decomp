#include "common.h"

#include "game_movie.h"

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */

extern int xglMovieClose(void *movie);

static MovieInfo mi;

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */

extern int xglMovieOpen(void *movie, char *name);

extern int SCRIPT_getEventActiveFlag(void);

extern int SCRIPT_getFadeTime(void);

extern void SCRIPT_sendMovieSkipSignal(void);

extern void nmlModelSetActiveFadeInCancel(int fadeTime);

#include "shared.h"

#include "main/xgl_thread.h"

#include "main/xgl_font.h"

#include "main/xgl_cd.h"

#include "main/party.h"

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

extern MovieGameLoopState GameLoopState;

extern MovieRenderState sRender;

extern PadPrefix PadData;

extern unsigned char GameMovieTransparent;

extern unsigned char GameMovieAlpha;

extern short GameMovieFrame;

extern const char D_004C0290[24];

extern const char D_004C02A8[88];

extern int xglMoviePlay(void *movie);

extern void nmlModelDirectSend(int mode, void *data, int count);

extern int xglFontGetStringWidth(const char *text);

extern char *GameResourceGetFreeAddr(void);

extern void nmlModelSendSignalMovieStart(void);

extern void nmlModelSendSignalMovieFinish(int mode);

extern void xglRenderClearDepth(void);

extern void xglSoundStreamStop(int stream);

extern void xglSoundStreamMute(void);

extern void xglSoundEffectNormalDirect(int effect);

extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);

extern int xglMpeg2Open(void *info, const char *name);

extern int xglMpeg2Play(void *info);

extern int xglMpeg2Close(void *info);

extern void xglRenderCopyDisp2Draw(void);

extern void xglFontPrintDirect(const char *text);

extern void GamePauseDispBG(void);

extern void GamePauseDispEvent(void);

extern void PartyTimePauseStart(void);

extern void PartyTimePauseEnd(void);

#define MOVIE_FLAG_SKIP_ALL 0x10000000

#define PAD_BUTTON_SKIP 0x10

#define PAD_BUTTON_REPLAY 0x20

#define PAD_BUTTON_START 0x800

#define PAD_COMBO_ABORT 0x10c

static char *caption_nextline(char *cursor);

static char *caption_getparam(char *cursor, int *time, char *out);

static void caption_convert(char *text);

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */

/*
 * Reads one caption entry: a time stamp (minutes:seconds:frames, or minutes
 * and seconds only) followed by blanks and the text up to a CR, LF or
 * backslash. The text is written to out with a two byte header (a tag byte
 * of 1 and the complement of half the text width); a backslash starts a
 * further line in the same entry. *time receives the time stamp in frames,
 * or -1 when the line has none. Returns the start of the next entry.
 */

static inline char caption_peek(char *cursor)
{
    return *cursor;
}

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

int GameMoviePlay(char *name)
{
    if (mi.state != 0) {
        xglMovieClose(&mi);
    }

    return xglMovieOpen(&mi, name);
}

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

static void caption_convert(char *text)
{
    char *src;
    int time;

    src = text;
    xglFontGetStringWidth(D_004C0290);
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
            size = xglCdReadFile(text, text, 0, 0);
            if (size < 0) {
                size = 0;
            } else {
                caption_convert(text);
            }
            xglMpeg2InfoInit2(&mpeg, text + ((size + 63) & ~63), 0x40000);
            if (size == 0) {
                text = 0;
            }
            if (xglMpeg2Open(&mpeg, name) >= 0) {
                GameLoopState.input_delay = 32;
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
                        xglFontPrint(64, 317, 0xFFFFFF, D_004C0290);
                        xglFontPrintDirect(text);
                    }
                    if (status < 0) {
                        xglRenderCopyDisp2Draw();
                        for (pass = 0; pass < 2; pass++) {
                            xglFontPrint(96, 200, 0xFFFFFF, D_004C02A8);
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
                    if (GameLoopState.input_delay != 0) {
                        GameLoopState.input_delay--;
                    } else if (PadData.half_2a & PAD_BUTTON_START) {
                        GameLoopState.input_delay = 32;
                        if ((PadData.half_28 & PAD_COMBO_ABORT) == PAD_COMBO_ABORT) {
                            GameLoopState.flags |= MOVIE_FLAG_SKIP_ALL;
                            break;
                        }
                        xglSoundEffectNormalDirect(4);
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
                        xglSoundEffectNormalDirect(4);
                        sRender.scene_disabled = 0;
                        GameLoopState.input_delay = 32;
                        if (PadData.half_2a & PAD_BUTTON_SKIP) {
                            GameLoopState.input_delay = 64;
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

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMovieInit);
