/*
 * TU-local declarations of main/tu107 (src/main/nml_packet_add.c).
 */

#ifndef SRC_MAIN_NML_PACKET_ADD_H
#define SRC_MAIN_NML_PACKET_ADD_H

#include "shared.h"

typedef struct NmlMaterialRenderState NmlMaterialRenderState;

typedef struct NmlModelRenderState NmlModelRenderState;

/*
 * The two objects the nml packet builders render from, recovered as bounded
 * partial views: only the members a recovered function reads are modeled, and
 * every unmodeled_* array stands for a span nothing here touches.
 */

/*
 * The "proreal" parameter block: the 0x1e0 bytes nmlModelSetProreal builds in
 * s_inProReal (main 0x0095b940) and nmlModelEntry copies into the packet with
 * nmlPacketSetAttributeData16N(s_inProReal, 0x1e), storing the packet address
 * it returns in NmlModelRenderState::proreal (main 0x00233028-0x00233030).
 */
typedef struct NmlProRealParam {
    unsigned char unmodeled_000[0x11c];
    /*
     * +0x11c: nmlModelSetProreal clears it (main 0x0022f9dc) and the two
     * projection entries set it to 1 immediately after calling that function,
     * nmlModelCalcEntryProjectCircle (main 0x002320ec) and
     * nmlModelCalcEntryProjectMap (main 0x00232680), so it marks a block built
     * for a projected entry.  add_exec_prog reads it to take the projected
     * slot of the "Pro" microcode family instead of the plain one.
     */
    u32 projection_entry;
} NmlProRealParam;

/*
 * One material of the model being rendered.  Its bytes come from the model
 * resource - no engine function writes either word - so the bits below are
 * evidenced by their readers only, and none of them is named.
 */
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
    unsigned char unmodeled_024[0x9c];
    /*
     * +0xc0: the material's blend and lighting flags.  add_exec_prog reads it
     * with lw (main 0x002387e8) and so does nmlPacketAddReflRot (main
     * 0x00239a40), which is why the member is a u32.  nmlPacketAddTransMicrocode
     * reads the same address with ld (main 0x00236d40): that 64-bit read spans
     * +0xc0..+0xc7 and nothing evidences what +0xc4 holds, so it keeps an
     * explicit cast in its own source instead of inventing a member here.
     */
    u32 material_flags;
};

/*
 * The model layout the engine renders from: the single s_inLayout (main
 * 0x004a91e0, 0x320 bytes) plus the per-entry copies in s_apModelLayout.
 */
struct NmlModelRenderState {
    unsigned char unmodeled_000[0x1c0];
    /* +0x1c0: the fog color parcel; its fourth word is fog intensity. */
    u32 fog_color[4];
    /* +0x1d0: the fog-distance parcel sent after fog_color. */
    u32 fog_parameters[4];
    unsigned char unmodeled_1e0[0x70];
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
    unsigned char unmodeled_254[0x0c];
    /*
     * +0x260: this layout's own proreal block, the packet address nmlModelEntry
     * stores after copying s_inProReal (main 0x00233030).
     */
    NmlProRealParam *proreal;
    /*
     * +0x264: another layout's proreal block, borrowed for the length of one
     * render.  nmlModelRenderProreal stores the entry's own +0x260 here (main
     * 0x002346dc) and clears it again afterwards (main 0x0023473c);
     * nmlModelEntry clears it as well (main 0x00233034).  add_exec_prog reads
     * it in preference to `proreal` whenever it is set.
     */
    NmlProRealParam *proreal_override;
    unsigned char unmodeled_268[0x48];
    /*
     * +0x2b0: the render-level word nmlModelSetRenderLevel ORs its whole
     * argument into (main 0x0022e97c).  nmlModelSetSpecularOff sets bit 1
     * (main 0x0022f588), and nmlModelCalcSpecularClip sets and clears that
     * same bit (main 0x00231ae0, 0x00231bf4) and sets bit 4 (0x00231bd4).
     * The bits add_exec_prog reads have no other writer with a name of its
     * own, so they stay literals.
     */
    u32 render_level;
    unsigned char unmodeled_2b4[0x3c];
    /* +0x2f0: one fog intensity multiplier for each viewport. */
    float fog_intensity[4];
};

typedef void *NmlPacket;

extern const char lenz_name[];

void nmlPacketAddExecProg(NmlMaterialRenderState *material,
                          NmlModelRenderState *model,
                          int allow_extended_programs,
                          int add_transform_data);

extern const char eye_name[];

extern char *strstr(const char *haystack, const char *needle);

extern void sceVif1PkAddCode(NmlPacket packet, u32 code);

extern void nmlPacketAddTransData(NmlMaterialRenderState *material);

extern void nmlPacketAddWaitMicrocode(void);

extern XglPacket *s_pPacket;

void nmlPacketSetCurrent(void);

extern int s_nReflRotType;

extern float s_inReflRotX;

extern float s_inReflRotY;

extern void sceVif1PkCnt(NmlPacket packet, int count);

extern void sceVif1PkOpenUpkCode(NmlPacket packet, int code, int size,
                                 int flag, int mode);

extern void sceVif1PkAddUpkData128N(NmlPacket packet, const void *data,
                                    int count);

extern void sceVif1PkCloseUpkCode(NmlPacket packet);

void nmlPacketAddReflRot(const NmlMaterialRenderState *material,
                         const NmlModelRenderState *model);

extern int s_nProgType;

void nmlPacketAddTransMicrocode(const NmlMaterialRenderState *material,
                                const NmlModelRenderState *model);

void nmlPacketAddFog(NmlModelRenderState *model, int viewport_index);

#endif /* SRC_MAIN_NML_PACKET_ADD_H */
