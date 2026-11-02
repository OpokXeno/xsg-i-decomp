#ifndef INCLUDE_MAIN_NML_FILTER_SET_H
#define INCLUDE_MAIN_NML_FILTER_SET_H

#include "main/nml_packet_add.h"
#include "main/nml_model_set.h"

/* nmlFilterStealthMake allocates 31 sixteen-byte records. Its draw consumer
 * witnesses fourteen XYZ records at +0x30 and fourteen UV records at +0x110.
 * Each record is one vector slot: the projection helper writes its integer
 * screen coordinates into the same storage it would hold a float vector in. */
union NmlStealthXYZ {
    Vector4 vector;
    struct {
        int x;
        int y;
        int z;
        int w;
    };
};

union NmlStealthUV {
    Vector4 vector;
    struct {
        int u;
        int v;
        int clipFlags;
        unsigned char unmodeled_0c[4];
    };
};

/*
 * The allocator's 31 sixteen-byte records, including actual endpoint
 * vectors.  The first endpoint's w is the squared camera distance of the
 * part centre; nmlModelEntry depth sorts a layout's stealth parts by it
 * within a sort class.
 */
struct NmlStealthGeometry {
    Vector4 endpoints[2];
    float sortClass;
    unsigned char unmodeled_24[0x0c];
    union NmlStealthXYZ xyz[14];
    union NmlStealthUV uv[14];
};

void nmlFilterSetTexClear(int frameBase);
void nmlFilterSetTexFillDraw(int frameBase, LayoutStore *model);

#endif /* INCLUDE_MAIN_NML_FILTER_SET_H */
