#ifndef INCLUDE_MAIN_NML_PACKET_ADD_H
#define INCLUDE_MAIN_NML_PACKET_ADD_H

#include "shared.h"

#include "main/ssd_init.h"
#include "main/xgl_2.h"

/* RssdWork storage is defined by main/tu107; its layout is owned by main/tu110. */
extern RssdWorkFlags RssdWork;

extern const char nmlEmptyName[4];

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

void nmlPacketAddGsScissor(u64 scissor);

#endif /* INCLUDE_MAIN_NML_PACKET_ADD_H */
