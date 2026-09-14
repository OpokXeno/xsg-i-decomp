typedef unsigned int u32;
typedef unsigned long long u64;
typedef void *NmlPacket;

/*
 * The material and the model layout this function selects a microcode family
 * from, as the same bounded partial views main/tu107 recovered for them
 * (src/main/nml_packet_add.h): only the members read here are modeled and the
 * unmodeled_* arrays make no claim about their contents.
 */
struct NmlMaterialRenderState {
    char name[0x20];                   /* +0x00: the material name */
    u32 render_flags;                  /* +0x20: bit 0 selects the env family */
    unsigned char unmodeled_024[0x9c];
    u32 material_flags;                /* +0xc0: blend and lighting flags */
};
typedef struct NmlMaterialRenderState NmlMaterialRenderState;

struct NmlModelRenderState {
    unsigned char unmodeled_000[0x250];
    u32 render_status;                 /* +0x250: the nmlModelSet* request bits */
    unsigned char unmodeled_254[0xcc];
};
typedef struct NmlModelRenderState NmlModelRenderState;

extern NmlPacket s_pPacket;
extern int s_nProgType;

extern NmlPacket xglPacketGetCurrent(void);
extern void sceVif1PkRef(NmlPacket, const void *, int, int, int, int);
extern void sceVif1PkCnt(NmlPacket, int);
extern void sceVif1PkOpenUpkCode(NmlPacket, int, int, int, int);
extern void sceVif1PkAddUpkData128N(NmlPacket, const void *, int);
extern void sceVif1PkCloseUpkCode(NmlPacket);

extern const u32 PacketSizeNewVu1MicroCode[];
extern const u32 PacketSizeNewVu1TexEnvMicroCode[];
extern const u32 PacketSizeNewVu1EnvMicroCode[];
extern const u32 PacketSizeNewVu1ProMicroCode[];
extern const u32 PacketSizeNewVu1AddMicroCode[];
extern const u32 PacketSizeNewVu1AddEnvMicroCode[];
extern const u32 PacketSizeNewVu1DropMicroCode[];

extern unsigned char PacketDataNewVu1MicroCode[];
extern unsigned char PacketDataNewVu1TexEnvMicroCode[];
extern unsigned char PacketDataNewVu1EnvMicroCode[];
extern unsigned char PacketDataNewVu1ProMicroCode[];
extern unsigned char PacketDataNewVu1AddMicroCode[];
extern unsigned char PacketDataNewVu1AddEnvMicroCode[];
extern unsigned char PacketDataNewVu1DropMicroCode[];
extern unsigned char s_aNormalTbl[];

void nmlPacketAddTransMicrocode(const NmlMaterialRenderState *material,
                                const NmlModelRenderState *model)
{
    int program_type = 2;
    u32 flags;

    s_pPacket = xglPacketGetCurrent();
    /*
     * Evidenced as written: read through a cast, not as a plain member.  The
     * original keeps the store to s_pPacket ahead of this load (main
     * 0x00236ccc, 0x00236cd0); `flags = model->render_status;` lets gcc 2.96
     * prove the two cannot alias and hoists the load above the store, which is
     * the only difference the whole function then has.
     */
    flags = *(const u32 *)&model->render_status;
    if (flags & 0x8000) {
        program_type = 6;
    } else if (flags & 0x800) {
        program_type = 3;
    } else if (flags & 0x80000) {
        program_type = 7;
    } else if (flags & 0x02001000) {
        program_type = 5;
        if (material->render_flags & 1) {
            program_type = 4;
        /*
         * The original reads +0xc0 here with ld (main 0x00236d40) while
         * add_exec_prog and nmlPacketAddReflRot read the same address with
         * lw, so this 64-bit access spans material_flags and the four bytes
         * after it.  Nothing evidences what +0xc4 holds, so the pair is not
         * made a member.
         */
        } else if ((*(const u64 *)&material->material_flags & 0x10010) == 0x10) {
            program_type = 2;
        }
    } else if (material->render_flags & 1) {
        program_type = 1;
    }

    if ((u32)(program_type - 1) >= 7)
        return;

    switch (program_type) {
    case 3:
        if (s_nProgType == 3)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1ProMicroCode,
                     PacketSizeNewVu1ProMicroCode[0], 0, 0, 0);
        s_nProgType = 3;
        sceVif1PkCnt(s_pPacket, 0);
        sceVif1PkOpenUpkCode(s_pPacket, 848, 108, 1, 1);
        sceVif1PkAddUpkData128N(s_pPacket, s_aNormalTbl, 114);
        sceVif1PkCloseUpkCode(s_pPacket);
        return;
    case 4:
        if (s_nProgType == 4)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1AddEnvMicroCode,
                     PacketSizeNewVu1AddEnvMicroCode[0], 0, 0, 0);
        s_nProgType = 4;
        break;
    case 5:
        if (s_nProgType == 5)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1AddMicroCode,
                     PacketSizeNewVu1AddMicroCode[0], 0, 0, 0);
        s_nProgType = 5;
        sceVif1PkCnt(s_pPacket, 0);
        sceVif1PkOpenUpkCode(s_pPacket, 848, 108, 1, 1);
        sceVif1PkAddUpkData128N(s_pPacket, s_aNormalTbl, 114);
        sceVif1PkCloseUpkCode(s_pPacket);
        return;
    case 6:
        if (s_nProgType == 6)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1TexEnvMicroCode,
                     PacketSizeNewVu1TexEnvMicroCode[0], 0, 0, 0);
        s_nProgType = 6;
        break;
    case 7:
        if (s_nProgType == 7)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1DropMicroCode,
                     PacketSizeNewVu1DropMicroCode[0], 0, 0, 0);
        s_nProgType = 7;
        break;
    case 1:
        if (s_nProgType == 1)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1EnvMicroCode,
                     PacketSizeNewVu1EnvMicroCode[0], 0, 0, 0);
        s_nProgType = 1;
        break;
    case 2:
        if (s_nProgType == 2)
            return;
        sceVif1PkRef(s_pPacket, PacketDataNewVu1MicroCode,
                     PacketSizeNewVu1MicroCode[0], 0, 0, 0);
        s_nProgType = 2;
        break;
    }
}
