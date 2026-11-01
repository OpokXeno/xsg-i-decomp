#define PAD_REPEAT_MASK 0xf00c
#define PAD_REPEAT_DELAY 8
#define PAD_REPEAT_INTERVAL 1
#define PAD_DEFAULT_DEAD_ZONE 64
#define PAD_AXIS_THRESHOLD 32

typedef struct XglPadRecord {
    unsigned char packet[0x20];
    unsigned char mode;
    unsigned char actuator_count;
    short mode_id;
    unsigned char unmodeled_24[2];
    unsigned char connection_state;
    unsigned char setup_state;
    unsigned short buttons;
    unsigned short pressed;
    unsigned short repeated;
    unsigned short released;
    unsigned short directions;
    unsigned short direction_pressed;
    unsigned short direction_repeated;
    unsigned short direction_released;
    signed char position[4];
    short horizontal_pressure;
    short vertical_pressure;
    signed char pressure[8];
    signed char repeat_delay;       /* +0x48 */
    signed char repeat_interval;    /* +0x49 */
    unsigned short repeat_mask;    /* +0x4a */
    signed char repeat_wait;        /* +0x4c */
    signed char repeat_count;       /* +0x4d */
    unsigned char horizontal_dead_zone; /* +0x4e */
    unsigned char vertical_dead_zone;   /* +0x4f */
    unsigned char actuator[6];      /* +0x50 */
    unsigned char actuator_pending; /* +0x56 */
    unsigned char unmodeled_57;     /* +0x57 */
    unsigned char button_map[8];    /* +0x58 */
    unsigned char axis_dead_zone[4];/* +0x60 */
    signed char axis[4];            /* +0x64 */
} XglPadRecord;

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
