#include "main/xgl_pad.h"

#define PAD_REPEAT_MASK 0xf00c
#define PAD_REPEAT_DELAY 8
#define PAD_REPEAT_INTERVAL 1
#define PAD_DEFAULT_DEAD_ZONE 64
#define PAD_AXIS_THRESHOLD 32

extern XglPadRecord PadData[2];
extern unsigned char PadDmaBuffer[0x200];
extern unsigned char act_align_0[6];

void xglPadSetRepeat(int port, unsigned short mask, int delay, int interval);
int scePadInit(int mode);
int scePadPortOpen(int port, int slot, unsigned char *dma_buffer);
int scePadGetState(int port, int slot);
int scePadInfoMode(int port, int slot, int term, int offset);
int scePadSetMainMode(int port, int slot, int mode, int lock);
int scePadGetReqState(int port, int slot);
int scePadInfoAct(int port, int slot, int actuator, int term);
int scePadSetActAlign(int port, int slot, unsigned char *alignment);
int scePadInfoPressMode(int port, int slot);
int scePadEnterPressMode(int port, int slot);
int scePadRead(int port, int slot, unsigned char *packet);
int scePadSetActDirect(int port, int slot, unsigned char *actuator);
