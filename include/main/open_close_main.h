#ifndef INCLUDE_MAIN_OPEN_CLOSE_MAIN_H
#define INCLUDE_MAIN_OPEN_CLOSE_MAIN_H

/*
 * Mode (state) values shared by the e* widgets (eWindow, eCursol, eMessage,
 * eRibbon, eTagFont, eNumber, ...): each phase has an entry value that does the
 * setup and falls into the value that follows it. Phases are 0x10 apart in the
 * order init, show, open, run, hide, end.
 */
enum {
    E_MODE_INIT = 0x00,
    E_MODE_IDLE = 0x01,
    E_MODE_SHOW_START = 0x20,
    E_MODE_SHOW_STEP = 0x21,
    E_MODE_OPEN_START = 0x22, /* opens with a short animation */
    E_MODE_OPEN_STEP = 0x23,
    E_MODE_RUN_START = 0x50,
    E_MODE_RUN_STEP = 0x51,
    E_MODE_HIDE_START = 0x70,
    E_MODE_HIDE_STEP = 0x71,
    E_MODE_END_START = 0xF0,
    E_MODE_END_STEP = 0xF1
};

typedef struct ECursolSprite {
    short x;
    short y;
    int work;
    short width;
    short height;
    signed char color[4];
    short style;
    signed char frame;
    signed char flags;
} ECursolSprite;

typedef struct ECursolMainData {
    unsigned char state;
    unsigned char active;
    unsigned char timer;
    unsigned char width;
    unsigned short x;
    unsigned short y;
    int work;
    unsigned char color[4];
    ECursolSprite sprite;
} ECursolMainData;

void eCursolMain(ECursolMainData *cursol);
void eCursolModeChange(ECursolMainData *cursol, unsigned int mode);
void eCursolSet(ECursolMainData *cursol, int width);

#endif /* INCLUDE_MAIN_OPEN_CLOSE_MAIN_H */
