#ifndef INCLUDE_MAIN_OPEN_CLOSE_MAIN_H
#define INCLUDE_MAIN_OPEN_CLOSE_MAIN_H

typedef struct ECursol {
    signed char state;    /* +0x00 */
    signed char active;    /* +0x01 */
    signed char timer;      /* +0x02 */
    signed char width;       /* +0x03 */
    short x;                  /* +0x04 */
    short y;                   /* +0x06 */
    int work;                   /* +0x08 */
    signed char color[4];        /* +0x0c..0x0f */
} ECursol;

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
void eCursolSet(ECursol *cursol, int width);

#endif /* INCLUDE_MAIN_OPEN_CLOSE_MAIN_H */
