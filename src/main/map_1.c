#include "common.h"
#include "shared.h"
#include "main/maptbl.h"

/* Partial view of the resource record RES_loadFile hands back for a map:
 * MapReset (0x00246598, still INCLUDE_ASM) dereferences it at +0x04
 * (xglCullingMapSet argument), +0x0c (UnduInit argument) and +0x10 (second
 * argument to the still-unrecovered func_A1CFB8). Only those three offsets
 * are evidenced; the rest stays unmodeled. */
typedef struct {
    const Vector4 *place;
    int culling_map;
    const char *texture;
    unsigned char *undu_state;
    const void *think_data;
} MapResource;

typedef struct MapPlayerActorState MapPlayerActorState;

typedef struct {
    u8 unmodeled_00[4];
    MapPlayerActorState *player_actor;
    u8 unmodeled_08[4];
    u16 map_draw_mode;
    u8 unmodeled_0e[2];
    u32 render_flags;
    u8 unmodeled_14[0x3c];
    short map_no;
    short entrance_no;
    MapResource *map_resource;
    u8 unmodeled_58[0x28];
    float render_color[4];
    u8 unmodeled_90[0x20];
    int render_level;
    int clip_mode;
    int fade_countdown;
    u8 unmodeled_bc[5];
    u8 map_flags;
} MapGameState;

typedef struct {
    u8 unmodeled_00[0x10];
    u8 state;
    u8 unmodeled_11[3];
} MapTaskState;

typedef void (*MapTaskCallback)(void *task);

typedef struct {
    u8 unmodeled_00[0x14];
    u16 packet_offset_units;
    u8 unmodeled_16[0x1a];
    MapTaskCallback callback;
} MapRenderState;

struct MapPlayerActorState {
    u8 unmodeled_00[0x10];
    float position_x;
    float position_y;
    float position_z;
    u8 unmodeled_1c[0x38];
    float entrance_rotation;
    u8 unmodeled_58[0x98c];
    float entrance_rotation_copy;
};

typedef struct {
    MapResource *resource;
    int size;
    int handle;
    int state;
} GameResourceEntry;

typedef struct {
    u8 unmodeled_00[0x20];
    u64 task_state_word;
    u8 unmodeled_28[0x28];
    u64 vif_command_word;
    u8 unmodeled_58[0x58];
} MapTestPrimData;

typedef struct {
    float components[4];
} MapEntranceHeader;

typedef struct {
    Vector4 color;
    Vector4 direction;
} MapDrawLightEntry;

typedef struct {
    Vector4 ambient_color;
    MapDrawLightEntry lights[3];
    float normal_matrix[4][4];
    float color_modifier_matrix[4][4];
} MapDrawLight;

extern MapGameState GameLoopState;
extern u8 UseTestPath;
extern MapTaskState tsk;
extern MapRenderState sRender;
extern GameResourceEntry GameResource[];

extern void MapChangeFadeSet(void);
extern void MapChange2(int map_id);
extern MapResource *RES_loadFile(int file_id, int resource_type, int map_id, int flags);
extern void MapReset(void);
extern XglPacket *xglPacketGetCurrent(void);
extern void xglRenderDrawFlipPk(XglPacket *packet);
extern void nmlModelSetFadeInCancel(int model_id);
extern void nmlModelSetFadeOutCancel(int model_id);
static void taskMapChange(void *task);
extern MapEntranceHeader *UnduDataGetHeader(int map_index, int unit_index);
extern void UnduInit(unsigned char *data);
extern int func_A1CFB8(int think_no, const void *data);
extern void nmlModelSetMapLastInit(void);
extern void xglCullingMapSet(int culling_map);
extern MapDrawLight *xglStudioGetLight2(void);
extern void xglLightCalcMatrix(MapDrawLight *light);
extern void nmlModelSetLight(void *color_modifier, void *normal);
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetMapEntry(void);
extern void nmlModelSetMulColor(const float color[4]);
extern void nmlModelSetRenderLevel(int level);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelSetClip(int clip);
extern void nmlModelEntry(int entry);
extern void sceVif1PkCnt(NmlPacket packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern void nmlModelSendBackBufferSignal(void);

const char mapDefaultPath[16] = "data\\map\\";

const char mapTestPath[16] = "data\\map\\test\\";

const char mapCheckPath[16] = "data\\map\\check\\";

const char MapFileNameMcVok12b[16] = "MC_VOK12B";

const char MapFileNameMcVok14b[16] = "MC_VOK14B";

const char MapFileNameMcVok13b[16] = "MC_VOK13B";

const char MapFileNameMcVok10b[16] = "MC_VOK10B";

const char MapFileNameMcVok09b[16] = "MC_VOK09B";

const char MapFileNameMcVok08b[16] = "MC_VOK08B";

const char MapFileNameMcVok07b[16] = "MC_VOK07B";

const char MapFileNameMcVok06b[16] = "MC_VOK06B";

const char MapFileNameMcVok05b[16] = "MC_VOK05B";

const char MapFileNameMcVok04b[16] = "MC_VOK04B";

const char MapFileNameMcVok03b[16] = "MC_VOK03B";

const char MapFileNameMcEls01[16] = "MC_ELS01";

const char MapFileNameMcVok26[16] = "MC_VOK26";

const char MapFileNameMcVok25[16] = "MC_VOK25";

const char MapFileNameMcVok24[16] = "MC_VOK24";

const char MapFileNameMcVok23[16] = "MC_VOK23";

const char MapFileNameMcVok22[16] = "MC_VOK22";

const char MapFileNameMcVok21[16] = "MC_VOK21";

const char MapFileNameMcVok20[16] = "MC_VOK20";

const char MapFileNameMcVok19[16] = "MC_VOK19";

const char MapFileNameMcVok18[16] = "MC_VOK18";

const char MapFileNameMcVok16[16] = "MC_VOK16";

const char MapFileNameMcVok15[16] = "MC_VOK15";

const char MapFileNameMcVok14[16] = "MC_VOK14";

const char MapFileNameMcVok13[16] = "MC_VOK13";

const char MapFileNameMcVok12[16] = "MC_VOK12";

const char MapFileNameMcVok11[16] = "MC_VOK11";

const char MapFileNameMcVok10[16] = "MC_VOK10";

const char MapFileNameMcVok09[16] = "MC_VOK09";

const char MapFileNameMcVok08[16] = "MC_VOK08";

const char MapFileNameMcVok07[16] = "MC_VOK07";

const char MapFileNameMcVok06[16] = "MC_VOK06";

const char MapFileNameMcVok05[16] = "MC_VOK05";

const char MapFileNameMcVok04[16] = "MC_VOK04";

const char MapFileNameMcVok03[16] = "MC_VOK03";

const char MapFileNameMcVok02[16] = "MC_VOK02";

const char MapFileNameMcVok01[16] = "MC_VOK01";

const char MapFileNameMcBtl01[16] = "MC_BTL01";

const char MapFileNameMcTst13[16] = "MC_TST13";

const char MapFileNameMcTst12[16] = "MC_TST12";

const char MapFileNameMcTst11[16] = "MC_TST11";

const char MapFileNameMcTst10[16] = "MC_TST10";

const char MapFileNameMcTst09[16] = "MC_TST09";

const char MapFileNameMcTst08[16] = "MC_TST08";

const char MapFileNameMcTst07[16] = "MC_TST07";

const char MapFileNameMcTst06[16] = "MC_TST06";

const char MapFileNameMcTst05[16] = "MC_TST05";

const char MapFileNameMcTst04[16] = "MC_TST04";

const char MapFileNameMcTst03[16] = "MC_TST03";

const char MapFileNameMcTst02[16] = "MC_TST02";

const char MapFileNameMcTst01[16] = "MC_TST01";

const char MapFileNameAraikabe[16] = "araikabe";

const char MapFileNameMcEls06h[16] = "MC_ELS06H";

const char MapFileNameMcEls01h[16] = "MC_ELS01H";

const char MapFileNameMcEls13b[16] = "MC_ELS13B";

const char MapFileNameMcEls12b[16] = "MC_ELS12B";

const char MapFileNameMcEls08b[16] = "MC_ELS08B";

const char MapFileNameMcEls07b[16] = "MC_ELS07B";

const char MapFileNameMcEls06b[16] = "MC_ELS06B";

const char MapFileNameMcEls05b[16] = "MC_ELS05B";

const char MapFileNameMcEls04b[16] = "MC_ELS04B";

const char MapFileNameMcEls02b[16] = "MC_ELS02B";

const char MapFileNameMcEls16[16] = "MC_ELS16";

const char MapFileNameMcEls15[16] = "MC_ELS15";

const char MapFileNameMcEls13[16] = "MC_ELS13";

const char MapFileNameMcEls12[16] = "MC_ELS12";

const char MapFileNameMcEls11[16] = "MC_ELS11";

const char MapFileNameMcEls10[16] = "MC_ELS10";

const char MapFileNameMcEls09[16] = "MC_ELS09";

const char MapFileNameMcEls08[16] = "MC_ELS08";

const char MapFileNameMcEls07[16] = "MC_ELS07";

const char MapFileNameMcEls06[16] = "MC_ELS06";

const char MapFileNameMcEls05[16] = "MC_ELS05";

const char MapFileNameMcEls04[16] = "MC_ELS04";

const char MapFileNameMcEls03[16] = "MC_ELS03";

const char MapFileNameMcEls02[16] = "MC_ELS02";

const char MapFileNameMcSyu01[16] = "MC_SYU01";

const char MapFileNameMcUmn02[16] = "MC_UMN02";

const char MapFileNameMcUmn01[16] = "MC_UMN01";

const char MapFileNameMcUzu08[16] = "MC_UZU08";

const char MapFileNameMcUzu07[16] = "MC_UZU07";

const char MapFileNameMcUzu06[16] = "MC_UZU06";

const char MapFileNameMcUzu05[16] = "MC_UZU05";

const char MapFileNameMcUzu04[16] = "MC_UZU04";

const char MapFileNameMcUzu03[16] = "MC_UZU03";

const char MapFileNameMcUzu02[16] = "MC_UZU02";

const char MapFileNameMcUzu01[16] = "MC_UZU01";

const char MapFileNameMcMil02[16] = "MC_MIL02";

const char MapFileNameMcMil01[16] = "MC_MIL01";

const char MapFileNameMcEve49[16] = "MC_EVE49";

const char MapFileNameMcEve48[16] = "MC_EVE48";

const char MapFileNameMcEve47[16] = "MC_EVE47";

const char MapFileNameMcEve46[16] = "MC_EVE46";

const char MapFileNameMcEve45[16] = "MC_EVE45";

const char MapFileNameMcEve44[16] = "MC_EVE44";

const char MapFileNameMcEve43[16] = "MC_EVE43";

const char MapFileNameMcEve42[16] = "MC_EVE42";

const char MapFileNameMcEve41[16] = "MC_EVE41";

const char MapFileNameMcEve40[16] = "MC_EVE40";

const char MapFileNameMcEve39[16] = "MC_EVE39";

const char MapFileNameMcEve38[16] = "MC_EVE38";

const char MapFileNameMcEve37[16] = "MC_EVE37";

const char MapFileNameMcEve36[16] = "MC_EVE36";

const char MapFileNameMcEve35[16] = "MC_EVE35";

const char MapFileNameMcEve33[16] = "MC_EVE33";

const char MapFileNameMcEve32[16] = "MC_EVE32";

const char MapFileNameMcEve31[16] = "MC_EVE31";

const char MapFileNameMcEve30[16] = "MC_EVE30";

const char MapFileNameMcEve29[16] = "MC_EVE29";

const char MapFileNameMcEve28[16] = "MC_EVE28";

const char MapFileNameMcEve27[16] = "MC_EVE27";

const char MapFileNameMcEve26[16] = "MC_EVE26";

const char MapFileNameMcEve25[16] = "MC_EVE25";

const char MapFileNameMcEve24[16] = "MC_EVE24";

const char MapFileNameMcEve23[16] = "MC_EVE23";

const char MapFileNameMcEve22[16] = "MC_EVE22";

const char MapFileNameMcEve21[16] = "MC_EVE21";

const char MapFileNameMcEve20[16] = "MC_EVE20";

const char MapFileNameMcEve19[16] = "MC_EVE19";

const char MapFileNameMcEve18[16] = "MC_EVE18";

const char MapFileNameMcEve17[16] = "MC_EVE17";

const char MapFileNameMcEve16[16] = "MC_EVE16";

const char MapFileNameMcEve15[16] = "MC_EVE15";

const char MapFileNameMcEve14[16] = "MC_EVE14";

const char MapFileNameMcEve13[16] = "MC_EVE13";

const char MapFileNameMcEve12[16] = "MC_EVE12";

const char MapFileNameMcEve11[16] = "MC_EVE11";

const char MapMcEve10Name[16] = "MC_EVE10";

const char MapMcEve09Name[16] = "MC_EVE09";

const char MapMcEve08Name[16] = "MC_EVE08";

const char MapMcEve07Name[16] = "MC_EVE07";

const char MapMcEve06Name[16] = "MC_EVE06";

const char MapMcEve05Name[16] = "MC_EVE05";

const char MapMcEve04Name[16] = "MC_EVE04";

const char MapMcEve03Name[16] = "MC_EVE03";

const char MapMcEve02Name[16] = "MC_EVE02";

const char MapMcEve01Name[16] = "MC_EVE01";

const char MapMcDyu04bName[16] = "MC_DYU04B";

const char MapMcDyu18Name[16] = "MC_DYU18";

const char MapMcDyu17Name[16] = "MC_DYU17";

const char MapMcDyu16Name[16] = "MC_DYU16";

const char MapMcDyu15Name[16] = "MC_DYU15";

const char MapMcDyu14Name[16] = "MC_DYU14";

const char MapMcDyu13Name[16] = "MC_DYU13";

const char MapMcDyu12Name[16] = "MC_DYU12";

const char MapMcDyu10Name[16] = "MC_DYU10";

const char MapMcDyu08Name[16] = "MC_DYU08";

const char MapMcDyu07Name[16] = "MC_DYU07";

const char MapMcDyu06Name[16] = "MC_DYU06";

const char MapMcDyu05Name[16] = "MC_DYU05";

const char MapMcDyu04Name[16] = "MC_DYU04";

const char MapMcDyu03Name[16] = "MC_DYU03";

const char MapMcDyu02Name[16] = "MC_DYU02";

const char MapMcDyu01Name[16] = "MC_DYU01";

const char MapMcUta19Name[16] = "MC_UTA19";

const char MapMcUta18Name[16] = "MC_UTA18";

const char MapMcUta17Name[16] = "MC_UTA17";

const char MapMcUta16Name[16] = "MC_UTA16";

const char MapMcUta15Name[16] = "MC_UTA15";

const char MapMcUta14Name[16] = "MC_UTA14";

const char MapMcUta13Name[16] = "MC_UTA13";

const char MapMcUta12Name[16] = "MC_UTA12";

const char MapMcUta11Name[16] = "MC_UTA11";

const char MapMcUta10Name[16] = "MC_UTA10";

const char MapMcUta09Name[16] = "MC_UTA09";

const char MapMcUta08Name[16] = "MC_UTA08";

const char MapMcUta07Name[16] = "MC_UTA07";

const char MapMcUta06Name[16] = "MC_UTA06";

const char MapMcUta05Name[16] = "MC_UTA05";

const char MapMcUta04Name[16] = "MC_UTA04";

const char MapMcUta03Name[16] = "MC_UTA03";

const char MapMcUta02Name[16] = "MC_UTA02";

const char MapMcUta01Name[16] = "MC_UTA01";

const char MapMcSyo01Name[16] = "MC_SYO01";

const char MapMcPro09Name[16] = "MC_PRO09";

const char MapMcPro08Name[16] = "MC_PRO08";

const char MapMcPro07Name[16] = "MC_PRO07";

const char MapMcPro06Name[16] = "MC_PRO06";

const char MapMcPro05Name[16] = "MC_PRO05";

const char MapMcPro04Name[16] = "MC_PRO04";

const char MapMcPro03Name[16] = "MC_PRO03";

const char MapMcPro02Name[16] = "MC_PRO02";

const char MapMcPro01Name[16] = "MC_PRO01";

const char MapMcKas41Name[16] = "MC_KAS41";

const char MapMcKas40Name[16] = "MC_KAS40";

const char MapMcKas39Name[16] = "MC_KAS39";

const char MapMcKas38Name[16] = "MC_KAS38";

const char MapMcKas37Name[16] = "MC_KAS37";

const char MapMcKas36Name[16] = "MC_KAS36";

const char MapMcKas35Name[16] = "MC_KAS35";

const char MapMcKas34Name[16] = "MC_KAS34";

const char MapMcKas33Name[16] = "MC_KAS33";

const char MapMcKas32Name[16] = "MC_KAS32";

const char MapMcKas31Name[16] = "MC_KAS31";

const char MapMcKas30Name[16] = "MC_KAS30";

const char MapMcKas29Name[16] = "MC_KAS29";

const char MapMcKas28Name[16] = "MC_KAS28";

const char MapMcKas27Name[16] = "MC_KAS27";

const char MapMcKas26Name[16] = "MC_KAS26";

const char MapMcKas25Name[16] = "MC_KAS25";

const char MapMcKas24Name[16] = "MC_KAS24";

const char MapMcKas23Name[16] = "MC_KAS23";

const char MapMcKas22Name[16] = "MC_KAS22";

const char MapMcKas21Name[16] = "MC_KAS21";

const char MapMcKas20Name[16] = "MC_KAS20";

const char MapMcKas19Name[16] = "MC_KAS19";

const char MapMcKas18Name[16] = "MC_KAS18";

const char MapMcKas17Name[16] = "MC_KAS17";

const char MapMcKas16Name[16] = "MC_KAS16";

const char MapMcKas15Name[16] = "MC_KAS15";

const char MapMcKas14Name[16] = "MC_KAS14";

const char MapMcKas13Name[16] = "MC_KAS13";

const char MapMcKas12Name[16] = "MC_KAS12";

const char MapMcKas11Name[16] = "MC_KAS11";

const char MapMcKas10Name[16] = "MC_KAS10";

const char MapMcKas09Name[16] = "MC_KAS09";

const char MapMcKas08Name[16] = "MC_KAS08";

const char MapMcKas07Name[16] = "MC_KAS07";

const char MapMcKas06Name[16] = "MC_KAS06";

const char MapMcKas05Name[16] = "MC_KAS05";

const char MapMcKas04Name[16] = "MC_KAS04";

const char MapMcKas03Name[16] = "MC_KAS03";

const char MapMcKas02Name[16] = "MC_KAS02";

const char MapMcKas01Name[16] = "MC_KAS01";

const char MapMcKuk16Name[16] = "MC_KUK16";

const char MapMcKuk10bName[16] = "MC_KUK10B";

const char MapMcKuk09bName[16] = "MC_KUK09B";

const char MapMcKuk08bName[16] = "MC_KUK08B";

const char MapMcKuk07bName[16] = "MC_KUK07B";

const char MapMcKuk06bName[16] = "MC_KUK06B";

const char MapMcKuk05bName[16] = "MC_KUK05B";

const char MapMcKuk04bName[16] = "MC_KUK04B";

const char MapMcKuk03bName[16] = "MC_KUK03B";

const char MapMcKuk15Name[16] = "MC_KUK15";

const char MapMcKuk14Name[16] = "MC_KUK14";

const char MapMcKuk13Name[16] = "MC_KUK13";

const char MapMcKuk12Name[16] = "MC_KUK12";

const char MapMcKuk11Name[16] = "MC_KUK11";

const char MapMcKuk10Name[16] = "MC_KUK10";

const char MapMcKuk09Name[16] = "MC_KUK09";

const char MapMcKuk08Name[16] = "MC_KUK08";

const char MapMcKuk07Name[16] = "MC_KUK07";

const char MapMcKuk06Name[16] = "MC_KUK06";

const char MapMcKuk05Name[16] = "MC_KUK05";

const char MapMcKuk04Name[16] = "MC_KUK04";

const char MapMcKuk03Name[16] = "MC_KUK03";

const char MapMcKuk02Name[16] = "MC_KUK02";

const char MapMcKuk01Name[16] = "MC_KUK01";

const char MapMcGnk25Name[16] = "MC_GNK25";

const char MapMcGnk24Name[16] = "MC_GNK24";

const char MapMcGnk23Name[16] = "MC_GNK23";

const char MapMcGnk22Name[16] = "MC_GNK22";

const char MapMcGnk21Name[16] = "MC_GNK21";

const char MapMcGnk20Name[16] = "MC_GNK20";

const char MapMcGnk19Name[16] = "MC_GNK19";

const char MapMcGnk18Name[16] = "MC_GNK18";

const char MapMcGnk17Name[16] = "MC_GNK17";

const char MapModelGnk16Name[16] = "MC_GNK16";

const char MapModelGnk15Name[16] = "MC_GNK15";

const char MapModelGnk14Name[16] = "MC_GNK14";

const char MapModelGnk13Name[16] = "MC_GNK13";

const char MapModelGnk12Name[16] = "MC_GNK12";

const char MapModelGnk11Name[16] = "MC_GNK11";

const char MapModelGnk10Name[16] = "MC_GNK10";

const char MapModelGnk09Name[16] = "MC_GNK09";

const char MapModelGnk08Name[16] = "MC_GNK08";

const char MapModelGnk07Name[16] = "MC_GNK07";

const char MapModelGnk06Name[16] = "MC_GNK06";

const char MapModelGnk05Name[16] = "MC_GNK05";

const char MapModelGnk04Name[16] = "MC_GNK04";

const char MapModelGnk03Name[16] = "MC_GNK03";

const char MapModelGnk02Name[16] = "MC_GNK02";

const char MapModelGnk01Name[16] = "MC_GNK01";

const char MapModelUtk03BName[16] = "MC_UTK03B";

const char MapModelUtk11Name[16] = "MC_UTK11";

const char MapModelUtk10Name[16] = "MC_UTK10";

const char MapModelUtk09Name[16] = "MC_UTK09";

const char MapModelUtk08Name[16] = "MC_UTK08";

const char MapModelUtk07Name[16] = "MC_UTK07";

const char MapModelUtk06Name[16] = "MC_UTK06";

const char MapModelUtk05Name[16] = "MC_UTK05";

const char MapModelUtk04Name[16] = "MC_UTK04";

const char MapModelUtk03Name[16] = "MC_UTK03";

const char MapModelUtk02Name[16] = "MC_UTK02";

const char MapModelUtk01Name[16] = "MC_UTK01";

const char MapModelKou07Name[16] = "MC_KOU07";

const char MapModelKou05Name[16] = "MC_KOU05";

const char MapModelKou04Name[16] = "MC_KOU04";

const char MapModelKou02Name[16] = "MC_KOU02";

const char MapModelKou01Name[16] = "MC_KOU01";

const char MapModelGnu14Name[16] = "MC_GNU14";

const char MapModelGnu13Name[16] = "MC_GNU13";

const char MapModelGnu12Name[16] = "MC_GNU12";

const char MapModelGnu11Name[16] = "MC_GNU11";

const char MapModelGnu10Name[16] = "MC_GNU10";

const char MapModelGnu09Name[16] = "MC_GNU09";

const char MapModelGnu08Name[16] = "MC_GNU08";

const char MapModelGnu07Name[16] = "MC_GNU07";

const char MapModelGnu06Name[16] = "MC_GNU06";

const char MapModelGnu05Name[16] = "MC_GNU05";

const char MapModelGnu04Name[16] = "MC_GNU04";

const char MapModelGnu03Name[16] = "MC_GNU03";

const char MapModelGnu02Name[16] = "MC_GNU02";

const char MapModelGnu01Name[16] = "MC_GNU01";

const char MapModelUdo22Name[16] = "MC_UDO22";

const char MapModelUdo21Name[16] = "MC_UDO21";

const char MapModelUdo15Name[16] = "MC_UDO15";

const char MapModelUdo14Name[16] = "MC_UDO14";

const char MapModelUdo13Name[16] = "MC_UDO13";

const char MapModelUdo12Name[16] = "MC_UDO12";

const char MapModelUdo11Name[16] = "MC_UDO11";

const char MapModelUdo10Name[16] = "MC_UDO10";

const char MapModelUdo09Name[16] = "MC_UDO09";

const char MapModelUdo08Name[16] = "MC_UDO08";

const char MapModelUdo07Name[16] = "MC_UDO07";

const char MapModelUdo06Name[16] = "MC_UDO06";

const char MapModelUdo05Name[16] = "MC_UDO05";

const char MapModelUdo04Name[16] = "MC_UDO04";

const char MapModelUdo03Name[16] = "MC_UDO03";

const char MapModelUdo02Name[16] = "MC_UDO02";

const char MapModelUdo01Name[24] = "MC_UDO01";

const char *MAP_getPath(void)
{
    int test_path = UseTestPath;

    if (test_path != 1) {
        if (test_path < 2) {
            return mapDefaultPath;
        }
        if (test_path != 2) {
            return mapDefaultPath;
        }
        return mapCheckPath;
    }
    return mapTestPath;
}

static void taskMapChange(void *task)
{
    static MapTestPrimData TestPrim = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 81, 1, 128, 0, 0, 0, 64, 171, 144, 238, 238, 49, 53, 5, 0, 0, 0 }, .task_state_word = 274877907044ULL, .unmodeled_28 = { 66, 0, 0, 0, 0, 0, 0, 0, 5, 192, 127, 0, 252, 6, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 1, 48, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0 }, .vif_command_word = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 111, 0, 0, 247, 113, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 32, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 143, 0, 0, 247, 141, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0 } };

    int command_low = 0x24120000 | ((u32)sRender.packet_offset_units << 5);

    TestPrim.task_state_word = ((u64)tsk.state << 32) | 100;
    TestPrim.vif_command_word = ((u64)0xc800 << 19) | command_low;
    sceVif1PkCnt(task, 0);
    sceVif1PkAddDataN(task, &TestPrim, 0x2c);
    nmlModelSendBackBufferSignal();
    tsk.state -= 4;
    if (tsk.state == 0) {
        sRender.callback = 0;
    }
}

void MapChangeFadeSet(void)
{
    if (GameLoopState.fade_countdown <= 0) {
        xglRenderDrawFlipPk(xglPacketGetCurrent());
        tsk.state = 0x80;
        sRender.callback = taskMapChange;
        nmlModelSetFadeInCancel(5);
        nmlModelSetFadeOutCancel(5);
    }
}

void MapReset(void)
{
    UnduInit(GameLoopState.map_resource->undu_state);
    GameLoopState.map_flags |= 0x80;
    GameLoopState.render_flags |= 0x30;
    func_A1CFB8(GameLoopState.map_no, GameLoopState.map_resource->think_data);
    nmlModelSetMapLastInit();
    xglCullingMapSet(GameLoopState.map_resource->culling_map);
    GameLoopState.render_level = 0;
    GameLoopState.clip_mode = 0;
    GameLoopState.render_color[3] = 1.0f;
    GameLoopState.render_color[2] = 1.0f;
    GameLoopState.render_color[1] = 1.0f;
    GameLoopState.render_color[0] = 1.0f;
}

void MapChangeResource(int map_id, int entrance_id)
{
    if (GameResource[map_id].state == 1) {
        GameLoopState.map_no = GameResource[map_id].handle;
        GameLoopState.map_resource = GameResource[map_id].resource;
        if (map_id < 1000) {
            MapChangeFadeSet();
        }
        MapReset();
        GameLoopState.entrance_no = (short)entrance_id;
        {
            MapEntranceHeader *header = UnduDataGetHeader(0x201, entrance_id + 0x8000);

            if (header != 0) {
                float last_component;

                GameLoopState.player_actor->position_x = header->components[0];
                GameLoopState.player_actor->position_y = header->components[1];
                GameLoopState.player_actor->position_z = header->components[2];
                last_component = header->components[3];
                GameLoopState.player_actor->entrance_rotation = last_component;
                GameLoopState.player_actor->entrance_rotation_copy = last_component;
            }
        }
    }
}

void MapChange(int map_id)
{
    MapChangeFadeSet();
    MapChange2(map_id);
}

void MapChange2(int map_id)
{
    if ((unsigned int)map_id < 0x71e) {
        GameLoopState.map_resource = RES_loadFile(-1, 1, map_id, 0);
        GameLoopState.map_no = map_id;
        MapReset();
    }
}

char *MapGetName(int map_no)
{
    int table_index;
    int entry_count;
    char *map_name;

    if (map_no < 0) {
        map_no = GameLoopState.map_no;
    }
    map_name = 0;
    if (map_no < 0x71F) {
        table_index = map_no / 100;
        entry_count = maptbl[table_index].count;
        if ((map_no % 100) < entry_count) {
            map_name = maptbl[table_index].names[map_no % 100];
        }
    }
    return map_name;
}

void MapDraw(void)
{
    MapResource *resource = GameLoopState.map_resource;

    if (GameLoopState.map_draw_mode == 2 && (GameLoopState.render_flags & 0x20) != 0) {
        if ((GameLoopState.render_flags & 0x10) != 0) {
            GameLoopState.render_flags &= ~0x10;
        } else {
            GameLoopState.render_flags &= ~0x20;
        }
    }

    if (resource != 0 && resource->culling_map != 0) {
        MapDrawLight *light = xglStudioGetLight2();

        xglLightCalcMatrix(light);
        nmlModelSetLight(light->color_modifier_matrix, light->normal_matrix);
        if (resource->texture != 0) {
            nmlModelSetTexture(resource->texture);
        }
        nmlModelSetMapEntry();
        nmlModelSetMulColor(GameLoopState.render_color);
        nmlModelSetRenderLevel(GameLoopState.render_level);
        nmlModelSetPlace(resource->place);
        if (GameLoopState.clip_mode != 0) {
            nmlModelSetClip(1);
        } else {
            nmlModelSetClip(2);
        }
        nmlModelEntry(resource->culling_map);
    }
}

int MapGetNo(void)
{
    return GameLoopState.map_no;
}

void MapInit(void)
{
    UseTestPath = 0;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
MapTaskState tsk = { { 0 }, 0, { 0, 0, 0 } };
