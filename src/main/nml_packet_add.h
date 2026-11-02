/*
 * TU-local declarations of main/tu107 (src/main/nml_packet_add.c).
 */

#ifndef SRC_MAIN_NML_PACKET_ADD_H
#define SRC_MAIN_NML_PACKET_ADD_H

#include "main/xgl_2.h"
#include "shared.h"
#include "main/ssd_init.h"
#include "main/nml_packet_add.h"
#include "main/nml_model_set.h"

typedef unsigned int NmlScreenQuadword __attribute__((mode(TI)));

typedef struct NmlTextureResourceSet {
    int count;
    unsigned char unmodeled_04[0x2c];
    /* +0x30: the serialized offset list that follows the fixed set header. */
    u32 entry_offsets[0];
} NmlTextureResourceSet;

typedef struct NmlTextureResourceEntry {
    short width;
    unsigned char unmodeled_02[2];
    short height;
    unsigned char unmodeled_06[2];
    short psm;
    unsigned char unmodeled_0a[2];
    short buffer_width;
    unsigned char unmodeled_0e[2];
    int vram_offset;
    int pixel_bytes;
    unsigned char unmodeled_18[0x28];
} NmlTextureResourceEntry;

extern const char lenz_name[];
extern const char eye_name[];

void nmlPacketAddExecProg(NmlModelBlock *material,
                          LayoutStore *model,
                          int allow_extended_programs,
                          int add_transform_data);

extern char *strstr(const char *haystack, const char *needle);
extern void sceVif1PkAddCode(XglPacket *packet, u32 code);
void nmlPacketAddTransData(NmlModelBlock *material);
void nmlPacketAddWaitMicrocode(void);
extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkOpenUpkCode(XglPacket *packet, int code, int size,
                                 int flag, int mode);

extern void sceVif1PkAddUpkData128N(XglPacket *packet, const void *data,
                                    int count);

extern void sceVif1PkCloseUpkCode(XglPacket *packet);

void nmlPacketAddReflRot(const NmlModelBlock *material,
                         const LayoutStore *model);

void nmlPacketAddTransMicrocode(const NmlModelBlock *material,
                                const LayoutStore *model);

void nmlPacketAddFog(LayoutStore *model, int viewport_index);

#endif /* SRC_MAIN_NML_PACKET_ADD_H */
