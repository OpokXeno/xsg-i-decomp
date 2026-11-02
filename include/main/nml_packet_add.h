#ifndef INCLUDE_MAIN_NML_PACKET_ADD_H
#define INCLUDE_MAIN_NML_PACKET_ADD_H

#include "shared.h"
#include "main/ssd_init.h"
#include "main/xgl_2.h"

/* One stealth part: two joints, outline widths, end extensions and the
 * longitudinal stretch applied to each of the four cross-section rings. */
struct NmlStealthDescriptor {
    int firstJoint;
    int secondJoint;
    unsigned char unmodeled_08[4];
    int sortClass;
    float firstWidth;
    float secondWidth;
    float firstExtension;
    float secondExtension;
    float longitudinalStretch[4];
};

typedef union GsTag {
    u64 value;
    u32 words[2];
} GsTag;

/* RssdWork storage is defined by main/tu107; its layout is owned by main/tu110. */
extern RssdWorkFlags RssdWork;

extern const char nmlEmptyName[4];

void nmlPacketAddGsScissor(u64 scissor);

/* Functions and data used by other translation units. */
void nmlPacketGsInit(void);
void nmlPacketAddGsFlush(void);
void nmlPacketAddGsFrame(int frame_base, int frame_mask);
void nmlPacketAddGsFlushWide(void);
void nmlPacketAddGsFrame1(int frame_base, int frame_mask);
void *nmlPacketSetAttributeData16N(const void *data, int count);
void nmlPacketSetCurrent(void);

#endif /* INCLUDE_MAIN_NML_PACKET_ADD_H */
