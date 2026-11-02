#ifndef INCLUDE_OV02_OV02_H
#define INCLUDE_OV02_OV02_H

#include "shared.h"
#include "main/e_message.h"
#include "main/menu_model.h"
#include "main/xgl_light.h"

/* Clear colour of a black frame (xglRenderClearColor). */
#define OV02_CLEAR_COLOR_BLACK 0x80000000

/* Size of the display frame. */
#define OV02_SCREEN_WIDTH 512
#define OV02_SCREEN_HEIGHT 448

/* Offset of the 512x448 screen's top-left corner inside the GS primitive coordinate space. */
#define OV02_GS_ORIGIN_X 1792
#define OV02_GS_ORIGIN_Y 1824

/* The same origin as a 12.4 vertex coordinate, half a pixel (8/16) below it. */
#define OV02_GS_VERTEX_X (OV02_GS_ORIGIN_X * 16 - 8)
#define OV02_GS_VERTEX_Y (OV02_GS_ORIGIN_Y * 16 - 8)

/* Font control codes written into text streams (FontOpcode of main/xgl_font.c). */
#define OV02_FONT_OP_EFFECT_PACKET 19
#define OV02_FONT_OP_CALLBACK 23

/*
 * Partial view of a screen task's work area at +0x190, twelve bytes before the
 * EMessageParam that follows its embedded WindowDX (measured: the three
 * message stores of tskUmnSimulationPas and tskUmnDataBasePas are addressed
 * from a base register at work + 0x190, which a plain &work->message does
 * not give).
 */
typedef struct UmnWindowMessageView {
    unsigned char unmodeled_00[0x0C];
    EMessageParam message;
} UmnWindowMessageView;

/*
 * Partial view of the MenuTopInfoPanel (private to main/menu_top_command_check)
 * that a screen's info window hands to MenuInfoWindow as its callback argument:
 * the text origin and the text pointer the screen sets each frame.
 */
typedef struct UmnInfoPanel {
    short x;
    short y;
    unsigned char unmodeled_04[8];
    const char *text;
} UmnInfoPanel;

/*
 * Functions of the main program that the OV02 translation units call and whose
 * main headers either do not declare them or declare them for another view of
 * the same record (the light set is a StudioLight here, a void pointer in
 * main/db_light_write.c and main/runtime.c).
 */
StudioLight *xglStudioGetLight2(void);
void MenuModelCreate(MenuModelUnit **result, int modelId);
void MenuModelExtFuncSet(MenuModelUnit *unit, void (*extFunc1)(MenuModelUnit *, void *), void *extFunc2);
void MenuModelUnitOpen(MenuModelUnit *unit, int drawType);
void MenuModelSubWindowSet(MenuModelUnit *unit, int studioId, int mode);
void MenuModelSubWindowBreak(MenuModelUnit *unit);
void MenuLoadFile(const char *name, void *buffer);
extern short UmnKosmosSpecialBox[4];
void xglFontLoad(int mode, int variant);
void WindowTexLoad(int window_id, int texture_set);
void FlushCache(int mode);

/* The VIF1 packet builders of the Sony SDK. */
void sceVif1PkCnt(XglPacket *packet, int count);
void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
void sceVif1PkCloseDirectHLCode(XglPacket *packet);
void xglLightIntensityAmbient(StudioLight *lightSet, const XglLightQuadword *intensity);
void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index, const XglLightQuadword *intensity);
void xglLightDirection(StudioLight *lightSet, unsigned int index, const Vector4 *source);

#endif /* INCLUDE_OV02_OV02_H */
