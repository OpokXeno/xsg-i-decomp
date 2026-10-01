typedef struct XglPadRecord {
    unsigned char unmodeled_00[0x48];
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
void xglPadSetRepeat(int port, unsigned short mask, int delay, int interval);

#define PAD_REPEAT_MASK 0xf00c
#define PAD_REPEAT_DELAY 8
#define PAD_REPEAT_INTERVAL 1
#define PAD_DEFAULT_DEAD_ZONE 64
#define PAD_AXIS_THRESHOLD 32

int scePadInit(int mode);
int scePadPortOpen(int port, int slot, unsigned char *dma_buffer);
