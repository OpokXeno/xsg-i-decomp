/*
 * TU-local declarations of main/tu107 (src/main/nml_packet_add.c).
 */

#ifndef SRC_MAIN_NML_PACKET_ADD_H
#define SRC_MAIN_NML_PACKET_ADD_H

#include "main/xgl_2.h"
#include "shared.h"
#include "main/ssd_init.h"

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

typedef struct NmlMaterialRenderState NmlMaterialRenderState;

typedef struct NmlModelRenderState NmlModelRenderState;

typedef unsigned int NmlScreenQuadword __attribute__((mode(TI)));

typedef struct NmlModelResource {
    unsigned char unmodeled_000[0x40];
    u32 matrix_base_index;
    unsigned char unmodeled_044[0x1c];
    u32 geometry_block_table_offset;
    unsigned char unmodeled_064[0x08];
    u32 texture_reference;
} NmlModelResource;

typedef struct NmlTextureResourceSet {
    int count;
    unsigned char unmodeled_04[0x2c];
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

/*
 * The two objects the nml packet builders render from, recovered as bounded
 * partial views: only the members a recovered function reads are modeled, and
 * every unmodeled_* array stands for a span nothing here touches.
 */
/* Partial view for the geometry and vertex indices consumed by AddParts. */
typedef struct NmlPartsBlockView {
    unsigned char unmodeled_000[0x20];
    u32 render_flags;
    unsigned char unmodeled_024[0x08];
    int part_id;
    u16 bone_indices[48];
    Vector4 bounding_sphere;
    u32 geometry_index;
    u32 vertex_index_offset;
    u32 vertex_index_count;
    u16 vertex_index_base;
    unsigned char unmodeled_0ae[0x12];
    u32 material_flags;
} NmlPartsBlockView;

typedef union NmlPartsReferenceScratch {
    u64 halves[2];
    Vector4 vector;
    float components[4];
} NmlPartsReferenceScratch;

/*
 * The "proreal" parameter block: the 0x1e0 bytes nmlModelSetProreal builds in
 * s_inProReal (main 0x0095b940) and nmlModelEntry copies into the packet with
 * nmlPacketSetAttributeData16N(s_inProReal, 0x1e), storing the packet address
 * it returns in NmlModelRenderState::proreal (main 0x00233028-0x00233030).
 */
typedef struct NmlProRealParam {
    unsigned char unmodeled_000[0x40];
    Matrix projection_transform;
    Matrix4 normal_transform;
    u32 block_material_data[16];
    unsigned char unmodeled_100[0x1c];
    /*
     * +0x11c: nmlModelSetProreal clears it (main 0x0022f9dc) and the two
     * projection entries set it to 1 immediately after calling that function,
     * nmlModelCalcEntryProjectCircle (main 0x002320ec) and
     * nmlModelCalcEntryProjectMap (main 0x00232680), so it marks a block built
     * for a projected entry.  add_exec_prog reads it to take the projected
     * slot of the "Pro" microcode family instead of the plain one.
     */
    u32 projection_entry;
    unsigned char unmodeled_120[0x10];
    GsTag texture_gs_parameters[2];
    unsigned char unmodeled_140[0x20];
    NmlScreenQuadword final_normal_row;
    unsigned char unmodeled_170[0x20];
    NmlPartsReferenceScratch reference_point;
    unsigned char unmodeled_1a0[0x20];
    void *texture;
} NmlProRealParam;

/*
 * One material of the model being rendered.  Its bytes come from the model
 * resource - no engine function writes either word - so the bits below are
 * evidenced by their readers only, and none of them is named.
 */
typedef union NmlBlockMaterialParameter {
    u32 words[20];
    float lanes[20];
    NmlScreenQuadword quadwords[5];
} NmlBlockMaterialParameter;

struct NmlMaterialRenderState {
    /*
     * +0x00: the material name.  add_exec_prog searches it with strstr for
     * lenz_name and eye_name (main 0x0023877c and 0x00238794), and
     * nmlModelFlushSubNonAlpha passes the same pointer straight to strstr
     * (main 0x00235108).
     */
    char name[0x20];
    /*
     * +0x20: the material's own render flags.  Bit 0 picks the environment
     * microcode family in nmlPacketAddTransMicrocode (main 0x00236d24 and
     * 0x00236d68); bits 1 and 2 select the face and back program variants the
     * s_aUcode*FaceAdr / s_aUcode*BackAdr rows of s_aUcodeTbl hold; bits 7, 8
     * and 9 choose the program index in nmlPacketAddExecProg and bit 10
     * suppresses its extended pair.
     */
    u32 render_flags;
    unsigned char unmodeled_024[0x08];
    u32 geometry_block_index;
    unsigned char unmodeled_030[0x02];
    u16 secondary_geometry_block_index;
    unsigned char unmodeled_034[0x7c];
    NmlScreenQuadword gif_tag;
    /*
     * +0xc0: the material's blend and lighting flags.  add_exec_prog reads it
     * with lw (main 0x002387e8) and so does nmlPacketAddReflRot (main
     * 0x00239a40), which is why the member is a u32.  nmlPacketAddTransMicrocode
     * reads the same address with ld (main 0x00236d40), spanning +0xc0..+0xc7.
     * AddBlockMaterial reads the alpha scale at +0xc4 as a float.
     */
    union {
        u64 material_flags_qword;
        struct {
            u32 material_flags;
            float block_material_alpha_scale;
        };
    };
    u32 unmodeled_c8;
    u32 alpha_tag;
    union {
        NmlBlockMaterialParameter block_material_parameters;
        struct {
            unsigned char unmodeled_d0[0x2c];
            float reflect_alpha; /* +0xfc */
            unsigned char unmodeled_100[0x20];
        };
    };
    struct {
        u32 buffer;
        u32 width;
    } texture_parameters[6];
    unsigned char unmodeled_150[0x1f];
    signed char alpha_mode;
};

typedef struct NmlPixelTestEntry {
    short reference;
    short geometry_block_index;
} NmlPixelTestEntry;

/*
 * The model layout the engine renders from: the single s_inLayout (main
 * 0x004a91e0, 0x320 bytes) plus the per-entry copies in s_apModelLayout.
 */
struct NmlModelRenderState {
    unsigned char unmodeled_000[0x3c];
    float block_material_alpha;
    Vector4 normal_vectors[3];
    NmlScreenQuadword screen_translation;
    union {
        NmlScreenQuadword screen_scale;
        Matrix projection_matrix;
        Matrix4 projection_rows;
    };
    Matrix4 reference_transform;
    Vector4 light_points[3];
    unsigned char unmodeled_130[0x10];
    NmlScreenQuadword light_parameters[3];
    unsigned char unmodeled_170[0x10];
    Matrix4 shadow_transform;
    /* +0x1c0: the fog color parcel; its fourth word is fog intensity. */
    u32 fog_color[4];
    /* +0x1d0: the fog-distance parcel sent after fog_color. */
    u32 fog_parameters[4];
    unsigned char unmodeled_1e0[0x10];
    float circle_center_x;
    unsigned char unmodeled_1f4[4];
    float circle_center_z;
    unsigned char unmodeled_1fc[0x24];
    float material_alpha_scale;
    float reflect_alpha;
    float default_block_alpha;
    float alpha_limit;
    union { u32 alpha_options; float alpha_options_value; };
    u32 unmodeled_234;
    int pixel_test_reference;
    u32 unmodeled_23c;
    void *texture;
    u32 unmodeled_244;
    u64 pixel_test_parameters;
    /*
     * +0x250: the render-status word.  nmlModelSetRenderStatus is the setter
     * that names it, and it ORs the value it is given (main 0x0022fab0 sets
     * 0x100, 0x0022fad0 sets 0x200); the rest of the nmlModelSet* API ORs one
     * request bit each into the same word - nmlModelSetPlace 0x1 (main
     * 0x0022ff24), nmlModelSetFogCol 0x2 (0x00230030), nmlModelSetStencil 0x4
     * (0x0022fd28), nmlModelSetZwrite 0x8 (0x0022fd48), nmlModelSetLight 0x10
     * (0x0022fe50), nmlModelSetToumei 0x20 (0x0022fdf0), nmlModelSetHumanModel
     * 0x40 (0x0022e760), nmlModelSetTexProreal 0x800 (0x0022fa5c),
     * nmlModelSetPointLight 0x1000 (0x0022f55c), nmlModelSetFogCancel 0x2000
     * (0x0023034c), nmlModelSetTexMap 0x4000 (0x0022f670) and
     * nmlModelSetFaceModel 0x100000 (0x0022e73c).  The three render entries
     * set the bit that picks their microcode family just before calling
     * nmlPacketAddTransMicrocode: nmlModelRenderProreal 0x800 (main
     * 0x002346c4), nmlModelRenderTexture 0x8000 (0x00234938) and
     * nmlModelRenderDrop 0x80000 (0x00234d58).
     */
    u32 render_status;
    struct NmlModelResource *resource;
    const struct NmlBlockVectorBlock *geometry_blocks;
    unsigned char unmodeled_25c[4];
    /*
     * +0x260: this layout's own proreal block, the packet address nmlModelEntry
     * stores after copying s_inProReal (main 0x00233030).
     */
    struct NmlProRealParam *proreal;
    /*
     * +0x264: another layout's proreal block, borrowed for the length of one
     * render.  nmlModelRenderProreal stores the entry's own +0x260 here (main
     * 0x002346dc) and clears it again afterwards (main 0x0023473c);
     * nmlModelEntry clears it as well (main 0x00233034).  add_exec_prog reads
     * it in preference to `proreal` whenever it is set.
     */
    struct NmlProRealParam *proreal_override;
    unsigned char unmodeled_268[8];
    int block_alpha_multiplier;
    unsigned char unmodeled_274[8];
    int block_alpha_view;
    unsigned char unmodeled_280[0x28];
    int shadow_height_enabled;
    float shadow_height;
    /*
     * +0x2b0: the render-level word nmlModelSetRenderLevel ORs its whole
     * argument into (main 0x0022e97c).  nmlModelSetSpecularOff sets bit 1
     * (main 0x0022f588), and nmlModelCalcSpecularClip sets and clears that
     * same bit (main 0x00231ae0, 0x00231bf4) and sets bit 4 (0x00231bd4).
     * The bits add_exec_prog reads have no other writer with a name of its
     * own, so they stay literals.
     */
    u32 render_level;
    int pixel_test_count;
    u32 unmodeled_2b8;
    float special_block_alpha;
    unsigned char unmodeled_2c0[4];
    float circle_scale;
    unsigned char unmodeled_2c8[8];
    NmlPixelTestEntry pixel_test_entries[8];
    /* +0x2f0: one fog intensity multiplier for each viewport. */
    float fog_intensity[4];
    float block_alpha_by_view[4];
    unsigned char unmodeled_310[0x10];
};

typedef void *NmlPacket;

typedef struct NmlBlockVectorBlock {
    Vector4 vectors[4];
} NmlBlockVectorBlock;

/* RssdWork storage is defined by main/tu107; its layout is owned by main/tu110. */
extern RssdWorkFlags RssdWork;

extern const char nmlEmptyName[4];
extern const char lenz_name[];
extern const char eye_name[];

void nmlPacketAddExecProg(NmlMaterialRenderState *material,
                          NmlModelRenderState *model,
                          int allow_extended_programs,
                          int add_transform_data);

extern char *strstr(const char *haystack, const char *needle);
extern void sceVif1PkAddCode(NmlPacket packet, u32 code);
void nmlPacketAddTransData(NmlMaterialRenderState *material);
void nmlPacketAddWaitMicrocode(void);
void nmlPacketSetCurrent(void);
extern void sceVif1PkCnt(NmlPacket packet, int count);

extern void sceVif1PkOpenUpkCode(NmlPacket packet, int code, int size,
                                 int flag, int mode);

extern void sceVif1PkAddUpkData128N(NmlPacket packet, const void *data,
                                    int count);

extern void sceVif1PkCloseUpkCode(NmlPacket packet);

void nmlPacketAddReflRot(const NmlMaterialRenderState *material,
                         const NmlModelRenderState *model);

void nmlPacketAddTransMicrocode(const NmlMaterialRenderState *material,
                                const NmlModelRenderState *model);

void nmlPacketAddFog(NmlModelRenderState *model, int viewport_index);

#endif /* SRC_MAIN_NML_PACKET_ADD_H */
