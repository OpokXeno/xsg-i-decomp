#ifndef INCLUDE_MAIN_OPEN_CLOSE_MAIN_H
#define INCLUDE_MAIN_OPEN_CLOSE_MAIN_H

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
