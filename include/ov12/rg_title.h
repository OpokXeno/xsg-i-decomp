#ifndef INCLUDE_OV12_RG_TITLE_H
#define INCLUDE_OV12_RG_TITLE_H

#include "ov12/xrg_paint2d.h"

#define RG_TITLE_BXX_COUNT 9
#define RG_TITLE_PICTURE_COUNT 14

/* The title screen: menu state, cursor and the textures it draws. */
struct RgTitle {
    XrgPaint2D *paint;
    RgBxx *bxx[RG_TITLE_BXX_COUNT];
    RgBxxPic *pictures[RG_TITLE_PICTURE_COUNT];
    int cursor_id;
    int menu_state;
    float transition_time;
    int count;
};

typedef struct RgTitle RgTitle;

/* Values returned by RgTitleGetResult. */
enum {
    RG_TITLE_RESULT_NONE,
    RG_TITLE_RESULT_ONE_PLAYER,
    RG_TITLE_RESULT_ONE_PLAYER_LEVEL_SELECT,
    RG_TITLE_RESULT_VERSUS,
    RG_TITLE_RESULT_HELP,
    RG_TITLE_RESULT_EXIT
};

/* Functions and data used by other translation units. */
extern RgTitle *CreateRgTitle(int count);
extern void DisposeRgTitle(RgTitle *pTitle);
extern int RgTitleGetResult(RgTitle *pTitle);
extern void RgTitlePassTime(RgTitle *pTitle, float delta);
extern void RgTitleDisp(RgTitle *pTitle);

#endif /* INCLUDE_OV12_RG_TITLE_H */
