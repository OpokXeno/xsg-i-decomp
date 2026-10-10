#include "common.h"

#include "shared.h"

typedef struct PlayerLookAtState {
    u32 unmodeled_00;
    void *player_actor;
    u32 unmodeled_08;
    u32 unmodeled_0c;
    u32 flags;
    u8 unmodeled_14[0xc];
    u32 map_flags;
    u8 unmodeled_24[0x9c];
    u8 move_mode;
    u8 unmodeled_c1[0x29e80];
    signed char look_at_target;
    u8 unmodeled_29f42[0x2a014 - 0x29f42];
    void *effect;
} PlayerLookAtState;

typedef struct ActorUndulation {
    u8 unmodeled_00[8];
    s16 attr_mask;
} ActorUndulation;

typedef struct PlayerEffect {
    u8 unmodeled_00[0x6bc];
    struct LookAtPlayerActor *owner;
} PlayerEffect;

typedef struct LookAtPlayerActor {
    u32 flags;
    void (*update)(struct LookAtPlayerActor *actor);
    void (*draw)(struct LookAtPlayerActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 external_velocity;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    float move_speed;
    Vector4 scale;
    u8 unmodeled_70[0x10];
    u8 number;
    u8 unmodeled_81[0x4c8 - 0x81];
    ActorUndulation undulation;
    u8 unmodeled_4d2[0x4e8 - 0x4d2];
    u64 terrain_flags;
    u8 unmodeled_4f0[0x694 - 0x4f0];
    signed char stick_x;
    signed char stick_y;
    u8 unmodeled_696[0x6f8 - 0x696];
    float motion_speed;
    u8 unmodeled_6fc[0x704 - 0x6fc];
    u16 motion_id;
    u8 unmodeled_706[0x9a0 - 0x706];
    u32 render_flags;
    u8 unmodeled_9a4[0x9c0 - 0x9a4];
    float filter_param;
    u8 unmodeled_9c4[0x9d8 - 0x9c4];
    u64 flash_frames;
    u8 unmodeled_9e0[0x9e4 - 0x9e0];
    float aim_angle;
    float body_radius;
    s16 look_at_timer;
    s16 look_at_target;
    s16 flash_timer;
    u8 unmodeled_9f2[0xa10 - 0x9f2];
    PlayerEffect *effects[8];
    s16 effect_timer;
    s16 effect_slot;
    u8 unmodeled_a34[0xa70 - 0xa34];
} LookAtPlayerActor;

typedef union PlayerAimVector {
    Vector4 vector;
    u64 doublewords[2];
} PlayerAimVector;

typedef struct PlayerAimMapUnit {
    u8 unmodeled_00[0x10];
    PlayerAimVector position;
    u8 unmodeled_20[0x2e0];
} PlayerAimMapUnit;

extern PlayerLookAtState GameLoopState;

extern LookAtPlayerActor actor[64];

extern PlayerAimMapUnit MapUnit[64];

int Get_MostNear_Actor(LookAtPlayerActor *player_actor);

void Actor_LookAt_Set(LookAtPlayerActor *player_actor, int mode, Vector4 *target);

void Actor_LookAt_Release(LookAtPlayerActor *player_actor, int mode);

void Actor_LookAt(LookAtPlayerActor *player_actor);

void PlayerLookAtAim(void);

/* Player-movement speed thresholds and vector scaling rate GameCfPlayerMove
 * compares/applies each frame; GameCfPlayerMoveInit restores their defaults. */

static int WALK_THRESHOLD_I;

static float WALK_THRESHOLD_F;

static int RUN_THRESHOLD_I;

static float RUN_THRESHOLD_F;

static float VECTOR_RATE;

extern int F2I(float value);

#define D_004D7C0C 0.0007999999798f

const char D_004BF7A0[16] = "shion6_h";

const char D_004BF7B0[24] = "+shion5_h,shion1_h";

const char D_004BF7C8[24] = "+shion4_h,shion_h";

const char D_004BF7E0[16] = "shion3_h";

const char D_004BF7F0[16] = "shion2_h";

const char D_004BF800[24] = "+shion1_h,shion5_h";

const char D_004BF818[16] = "kosmos_h6";

const char D_004BF828[16] = "kosmos_h5";

const char D_004BF838[16] = "kosmos_h4";

const char D_004BF848[16] = "kosmos_h3";

const char D_004BF858[16] = "kosmos_h2";

const char D_004BF868[16] = "kosmos_h1";

const char D_004BF878[16] = "shitan_h";

const char D_004BF888[16] = "kosmos_h";

const char D_004BF898[24] = "+shion_h,shion4_h";

const char D_004BF8B0[16] = "+shion5,shion1";

const char D_004BF8C0[16] = "+shion1,shion5";

const char D_004BF8D0[16] = "shion_ch";

const char D_004BF8E0[16] = "+shion,shion4";

const char D_004BF8F0[16] = "albelt3_h";

const char D_004BF900[24] = "+hammer_h,hammer";

const char D_004BF918[16] = "+tonny_h,tonny";

const char D_004BF928[16] = "gaignun_h";

const char D_004BF938[24] = "+andrew_h,andrew6";

const char D_004BF950[24] = "+andrew_h,andrew5";

const char D_004BF968[24] = "+andrew_h,andrew4";

const char D_004BF980[24] = "+andrew_h,andrew3";

const char D_004BF998[24] = "+andrew_h,andrew2";

const char D_004BF9B0[24] = "+andrew_h,andrew1";

const char D_004BF9C8[24] = "+andrew_h,andrew";

const char D_004BF9E0[16] = "virgil3_h";

const char D_004BF9F0[16] = "virgil1_h";

const char D_004BFA00[16] = "albelt2_h";

const char D_004BFA10[16] = "virgil4_h";

const char D_004BFA20[16] = "virgil2_h";

const char D_004BFA30[16] = "kebin1_h";

const char D_004BFA40[24] = "+matehws_h,matehws";

const char D_004BFA58[16] = "virgil_h";

const char D_004BFA68[16] = "albelt_h";

const char D_004BFA78[16] = "marglis_h";

const char D_004BFA88[16] = "joachim1";

const char D_004BFA98[16] = "cecilia1";

const char D_004BFAA8[16] = "and_daug";

const char D_004BFAB8[16] = "and_wife";

const char D_004BFAC8[16] = "moriyama";

const char D_004BFAD8[16] = "step_mam";

const char D_004BFAE8[16] = "matehws1";

const char D_004BFAF8[16] = "gaignun1";

const char D_004BFB08[16] = "marglis1";

const char D_004BFB18[16] = "breal_w1";

const char D_004BFB28[16] = "breal_m4";

const char D_004BFB38[16] = "breal_m3";

const char D_004BFB48[16] = "breal_m2";

const char D_004BFB58[16] = "breal_m1";

const char D_004BFB68[16] = "king_cat";

const char D_004BFB78[16] = "pilot_no";

const char D_004BFB88[16] = "inn_girl";

const char D_004BFB98[16] = "bread_ch";

const char D_004BFBA8[16] = "kidsw_b2";

const char D_004BFBB8[16] = "kidsw_b1";

const char D_004BFBC8[16] = "kidsw_a2";

const char D_004BFBD8[16] = "kidsw_a1";

const char D_004BFBE8[16] = "kidsm_b2";

const char D_004BFBF8[16] = "kidsm_b1";

const char D_004BFC08[16] = "kidsm_a2";

const char D_004BFC18[16] = "kidsm_a1";

const char D_004BFC28[16] = "gr_final";

const char D_004BFC38[16] = "ewp_hg01";

const char D_004BFC48[16] = "boss\\boss_";

const char D_004BFC58[16] = "fere\\fere_";

const char D_004BFC68[16] = "feso\\feso_";

const char D_004BFC78[16] = "fero\\fero_";

const char D_004BFC88[16] = "fema\\fema_";

const char D_004BFC98[16] = "utro\\utro_";

const char D_004BFCA8[16] = "utso\\utso_";

const char D_004BFCB8[16] = "utre\\utre_";

const char D_004BFCC8[16] = "utma\\utma_";

const char D_004BFCD8[16] = "guno\\guno_";

const char D_004BFCE8[16] = "mobj000\\mobj_";

const char D_004BFCF8[16] = "aobj000\\acc_";

const char D_004BFD08[16] = "musobj\\mus_";

const char D_004BFD18[16] = "mufobj\\muf_";

const char D_004BFD28[16] = "muxobj\\mux_";

const char D_004BFD38[16] = "treobj\\tre_";

const char D_004BFD48[16] = "muwobj\\muw_";

const char D_004BFD58[16] = "muobj\\MU_";

const char D_004BFED8[24] = "data\\scene\\itai\\";

const char D_004BFEF0[24] = "data\\scene\\yagi\\";

const char D_004BFF08[24] = "data\\scene\\sugisawa\\";

const char D_004BFF20[24] = "data\\scene\\sato\\";

const char D_004BFF38[24] = "data\\scene\\keiichi\\";

const char D_004BFF50[24] = "data\\scene\\kuramoto\\";

const char D_004BFF68[24] = "data\\scene\\yajima\\";

const char D_004BFF80[24] = "data\\scene\\nakahara\\";

const char D_004BFF98[24] = "data\\scene\\nv-yone\\";

const char D_004BFFB0[24] = "data\\scene\\koji\\";

const char D_004BFFC8[24] = "data\\scene\\konishi\\";

const char D_004BFFE0[24] = "data\\scene\\sakisako\\";

const char D_004BFFF8[24] = "data\\scene\\kojima\\";

const char D_004C0010[24] = "data\\scene\\fuji\\";

const char D_004C0028[24] = "data\\scene\\gash\\";

const char D_004C0040[24] = "data\\scene\\event\\";

const char D_004C0058[16] = "data\\scene\\cf\\";

const char D_004C0068[24] = "data\\scene\\check\\";

const char D_004C0080[24] = "data\\scene\\test\\";

const char D_004C0098[16] = "data\\scene\\";

const char D_004D8AA8[8] = "momo6";

const char D_004D8AB0[8] = "momo5_h";

const char D_004D8AB8[8] = "momo5";

const char D_004D8AC8[8] = "jr1_h";

const char D_004D8AD0[8] = "ziggy_h";

const char D_004D8AD8[8] = "jr_h";

const char D_004D8AE0[8] = "momo_h";

const char D_004D8AE8[8] = "chaos_h";

const char D_004D8AF0[8] = "shitan2";

const char D_004D8AF8[8] = "shitan1";

const char D_004D8B00[8] = "ziggy1";

const char D_004D8B08[8] = "jr3";

const char D_004D8B10[8] = "jr2";

const char D_004D8B18[8] = "jr1";

const char D_004D8B20[8] = "momo4";

const char D_004D8B28[8] = "momo3";

const char D_004D8B30[8] = "momo2";

const char D_004D8B38[8] = "momo1";

const char D_004D8B40[8] = "chaos2";

const char D_004D8B48[8] = "chaos1";

const char D_004D8B50[8] = "kosmos5";

const char D_004D8B58[8] = "kosmos3";

const char D_004D8B60[8] = "kosmos2";

const char D_004D8B68[8] = "kosmos1";

const char D_004D8B70[8] = "shion";

const char D_004D8B78[8] = "shion3";

const char D_004D8B80[8] = "shion2";

const char D_004D8B88[8] = "shitan";

const char D_004D8B90[8] = "ziggy";

const char D_004D8BA0[8] = "momo";

const char D_004D8BA8[8] = "chaos";

const char D_004D8BB0[8] = "kosmos";

const char D_004D8BB8[8] = "andrew9";

const char D_004D8BC0[8] = "feb1";

const char D_004D8BC8[8] = "andrew8";

const char D_004D8BD0[8] = "andrew7";

const char D_004D8BD8[8] = "andrew6";

const char D_004D8BE0[8] = "andrew5";

const char D_004D8BE8[8] = "elly_h";

const char D_004D8BF0[8] = "fried_h";

const char D_004D8BF8[8] = "kebin_h";

const char D_004D8C00[8] = "allen_h";

const char D_004D8C08[8] = "cath1";

const char D_004D8C10[8] = "chief";

const char D_004D8C18[8] = "van";

const char D_004D8C20[8] = "elly2";

const char D_004D8C28[8] = "k_joach";

const char D_004D8C30[8] = "shalron";

const char D_004D8C38[8] = "hynlin";

const char D_004D8C40[8] = "togashi";

const char D_004D8C48[8] = "pelegri";

const char D_004D8C50[8] = "shi_mam";

const char D_004D8C58[8] = "shi_dad";

const char D_004D8C60[8] = "cath";

const char D_004D8C68[8] = "cecilia";

const char D_004D8C70[8] = "lapis";

const char D_004D8C78[8] = "miyuki";

const char D_004D8C80[8] = "voyager";

const char D_004D8C88[8] = "helmer";

const char D_004D8C90[8] = "abel";

const char D_004D8C98[8] = "shelley";

const char D_004D8CA0[8] = "mary";

const char D_004D8CA8[8] = "sellers";

const char D_004D8CB0[8] = "joachim";

const char D_004D8CB8[8] = "yuri";

const char D_004D8CC0[8] = "feb";

const char D_004D8CC8[8] = "andrew4";

const char D_004D8CD0[8] = "andrew3";

const char D_004D8CD8[8] = "andrew2";

const char D_004D8CE0[8] = "andrew1";

const char D_004D8CE8[8] = "andrew";

const char D_004D8CF0[8] = "hammer";

const char D_004D8CF8[8] = "tonny";

const char D_004D8D00[8] = "matehws";

const char D_004D8D08[8] = "elly";

const char D_004D8D10[8] = "fried";

const char D_004D8D18[8] = "virgil4";

const char D_004D8D20[8] = "virgil3";

const char D_004D8D28[8] = "virgil2";

const char D_004D8D30[8] = "virgil1";

const char D_004D8D38[8] = "virgil";

const char D_004D8D40[8] = "kebin2";

const char D_004D8D48[8] = "kebin1";

const char D_004D8D50[8] = "kebin";

const char D_004D8D58[8] = "allen1";

const char D_004D8D60[8] = "allen";

const char D_004D8D68[8] = "albelt1";

const char D_004D8D70[8] = "albelt";

const char D_004D8D78[8] = "gaignun";

const char D_004D8D80[8] = "marglis";

const char D_004D8D88[8] = "breal_w";

const char D_004D8D90[8] = "breal_m";

const char D_004D8D98[8] = "esold";

const char D_004D8DA0[8] = "rw_w2";

const char D_004D8DA8[8] = "rw_w1";

const char D_004D8DB0[8] = "rw_w";

const char D_004D8DB8[8] = "rw_m2";

const char D_004D8DC0[8] = "rw_m1";

const char D_004D8DC8[8] = "rw_m";

const char D_004D8DD0[8] = "suuki";

const char D_004D8DD8[8] = "eng_off";

const char D_004D8DE0[8] = "eng";

const char D_004D8DE8[8] = "sold";

const char D_004D8DF0[8] = "pilot";

const char D_004D8DF8[8] = "off_w1";

const char D_004D8E00[8] = "off_w";

const char D_004D8E08[8] = "off_m3";

const char D_004D8E10[8] = "off_m2";

const char D_004D8E18[8] = "off_m1";

const char D_004D8E20[8] = "off_m";

const char D_004D8E28[8] = "vec_w1";

const char D_004D8E30[8] = "vec_w";

const char D_004D8E38[8] = "vec_m2";

const char D_004D8E40[8] = "vec_m1";

const char D_004D8E48[8] = "vec_m";

const char D_004D8E50[8] = "space";

const char D_004D8E58[8] = "u_solds";

const char D_004D8E60[8] = "real_w2";

const char D_004D8E68[8] = "real_w1";

const char D_004D8E70[8] = "real_w";

const char D_004D8E78[8] = "real_m2";

const char D_004D8E80[8] = "real_m1";

const char D_004D8E88[8] = "real_m";

const char D_004D8E90[8] = "u_space";

const char D_004D8E98[8] = "u_sold";

const char D_004D8EA0[8] = "gove_w2";

const char D_004D8EA8[8] = "gove_w1";

const char D_004D8EB0[8] = "gove_w";

const char D_004D8EB8[8] = "commi5";

const char D_004D8EC0[8] = "commi4";

const char D_004D8EC8[8] = "commi3";

const char D_004D8ED0[8] = "commi2";

const char D_004D8ED8[8] = "commi1";

const char D_004D8EE0[8] = "commi";

const char D_004D8EE8[8] = "ass_m4";

const char D_004D8EF0[8] = "ass_m3";

const char D_004D8EF8[8] = "kancho";

const char D_004D8F00[8] = "gove_m2";

const char D_004D8F08[8] = "gove_m1";

const char D_004D8F10[8] = "gove_m";

const char D_004D8F18[8] = "ass_w";

const char D_004D8F20[8] = "ass_m2";

const char D_004D8F28[8] = "ass_m1";

const char D_004D8F30[8] = "ass_m";

const char D_004D8F38[8] = "hyaku";

const char D_004D8F40[8] = "du2";

const char D_004D8F48[8] = "du1";

const char D_004D8F58[8] = "pol_w";

const char D_004D8F60[8] = "pol_m";

const char D_004D8F68[8] = "utic_w2";

const char D_004D8F70[8] = "utic_w1";

const char D_004D8F78[8] = "utic_w";

const char D_004D8F80[8] = "utic_m2";

const char D_004D8F88[8] = "utic_m1";

const char D_004D8F90[8] = "utic_m";

const char D_004D8F98[8] = "cont";

const char D_004D8FA0[8] = "eng_oji";

const char D_004D8FA8[8] = "scot";

const char D_004D8FB0[8] = "inn_oji";

const char D_004D8FB8[8] = "spy";

const char D_004D8FC0[8] = "barten";

const char D_004D8FC8[8] = "goro";

const char D_004D8FD0[8] = "mouth";

const char D_004D8FD8[8] = "cle_oba";

const char D_004D8FE0[8] = "king";

const char D_004D8FE8[8] = "robot";

const char D_004D8FF0[8] = "cle_ch";

const char D_004D8FF8[8] = "nurse";

const char D_004D9000[8] = "doctor";

const char D_004D9008[8] = "bar_m";

const char D_004D9010[8] = "bar_oji";

const char D_004D9018[8] = "bread";

const char D_004D9020[8] = "girl_b2";

const char D_004D9028[8] = "girl_b1";

const char D_004D9030[8] = "girl_b";

const char D_004D9038[8] = "girl_a2";

const char D_004D9040[8] = "girl_a1";

const char D_004D9048[8] = "girl_a";

const char D_004D9050[8] = "kidsw_b";

const char D_004D9058[8] = "kidsw_a";

const char D_004D9060[8] = "baba_b2";

const char D_004D9068[8] = "baba_b1";

const char D_004D9070[8] = "baba_b";

const char D_004D9078[8] = "baba_a2";

const char D_004D9080[8] = "baba_a1";

const char D_004D9088[8] = "baba_a";

const char D_004D9090[8] = "oba_b2";

const char D_004D9098[8] = "oba_b1";

const char D_004D90A0[8] = "oba_b";

const char D_004D90A8[8] = "oba_a2";

const char D_004D90B0[8] = "oba_a1";

const char D_004D90B8[8] = "oba_a";

const char D_004D90C0[8] = "wman_b2";

const char D_004D90C8[8] = "wman_b1";

const char D_004D90D0[8] = "wman_b";

const char D_004D90D8[8] = "wman_a2";

const char D_004D90E0[8] = "wman_a1";

const char D_004D90E8[8] = "wman_a";

const char D_004D90F0[8] = "boy_b2";

const char D_004D90F8[8] = "boy_b1";

const char D_004D9100[8] = "boy_b";

const char D_004D9108[8] = "boy_a2";

const char D_004D9110[8] = "boy_a1";

const char D_004D9118[8] = "boy_a";

const char D_004D9120[8] = "kidsm_b";

const char D_004D9128[8] = "kidsm_a";

const char D_004D9130[8] = "jiji_b2";

const char D_004D9138[8] = "jiji_b1";

const char D_004D9140[8] = "jiji_b";

const char D_004D9148[8] = "jiji_a2";

const char D_004D9150[8] = "jiji_a1";

const char D_004D9158[8] = "jiji_a";

const char D_004D9160[8] = "oji_b2";

const char D_004D9168[8] = "oji_b1";

const char D_004D9170[8] = "oji_b";

const char D_004D9178[8] = "oji_a2";

const char D_004D9180[8] = "oji_a1";

const char D_004D9188[8] = "oji_a";

const char D_004D9190[8] = "man_b2";

const char D_004D9198[8] = "man_b1";

const char D_004D91A0[8] = "man_b";

const char D_004D91A8[8] = "man_a2";

const char D_004D91B0[8] = "man_a1";

const char D_004D91B8[8] = "man_a";

const char D_004D9210[8] = "_02_ho";

const char D_004D9218[8] = "_01_ho2";

const char D_004D9220[8] = "_01_ho";

const char D_004D9228[8] = "_01_cv";

const char D_004D9240[8] = "_03";

const char D_004D9248[8] = "_02";

const char D_004D9250[8] = "_01";

const char D_004D9258[8] = "gr_leg";

const char D_004D9260[8] = "gr_body";

const char D_004D9268[8] = "gr_hand";

const char D_004D9270[8] = "gr_head";

const char D_004D9278[8] = "ukunS";

const char D_004D9280[8] = "ukunB";

const char D_004D9288[8] = "ts_01";

const char D_004D9290[8] = "vx_1m";

const char D_004D9298[8] = "ukn_g";

const char D_004D92A0[8] = "bw04";

const char D_004D92A8[8] = "bw03";

const char D_004D92B0[8] = "bw02";

const char D_004D92B8[8] = "bw01";

const char D_004D92C0[8] = "hw06";

const char D_004D92C8[8] = "hw05";

const char D_004D92D0[8] = "hw04";

const char D_004D92D8[8] = "hw03";

const char D_004D92E0[8] = "hw02";

const char D_004D92E8[8] = "hw01";

const char D_004D92F0[8] = "rd17";

const char D_004D92F8[8] = "rd16";

const char D_004D9300[8] = "rd15";

const char D_004D9308[8] = "rd14";

const char D_004D9310[8] = "rd13";

const char D_004D9318[8] = "rd12";

const char D_004D9320[8] = "rd11";

const char D_004D9328[8] = "rd10";

const char D_004D9330[8] = "rd09";

const char D_004D9338[8] = "rd08";

const char D_004D9340[8] = "rd07";

const char D_004D9348[8] = "rd06";

const char D_004D9350[8] = "rd05";

const char D_004D9358[8] = "rd04";

const char D_004D9360[8] = "rd03";

const char D_004D9368[8] = "rd02";

const char D_004D9370[8] = "rd01";

const char D_004D9378[8] = "sw01";

const char D_004D9380[8] = "jr_hg08";

const char D_004D9388[8] = "jr_hg07";

const char D_004D9390[8] = "jr_hg06";

const char D_004D9398[8] = "gun05";

const char D_004D93A0[8] = "gun04";

const char D_004D93A8[8] = "gun03";

const char D_004D93B0[8] = "gun02";

const char D_004D93B8[8] = "gun01";

const char D_004D93C0[8] = "sw05";

const char D_004D93C8[8] = "sw04";

const char D_004D93D0[8] = "sw03";

const char D_004D93D8[8] = "sw02";

const char D_004D93E0[8] = "mbox";

const char D_004D93E8[8] = "msp02r";

const char D_004D93F0[8] = "hb00r";

const char D_004D93F8[8] = "glg00r";

const char D_004D9400[8] = "deff00";

const char D_004D9408[8] = "sol00";

const char D_004D9410[8] = "wire00";

const char D_004D9418[8] = "smp01";

const char D_004D9420[8] = "ecm02";

const char D_004D9428[8] = "ecm01";

const char D_004D9430[8] = "ead00";

const char D_004D9438[8] = "bw00";

const char D_004D9440[8] = "bbc00";

const char D_004D9448[8] = "vsd00";

const char D_004D9450[8] = "fsd00";

const char D_004D9458[8] = "shb00";

const char D_004D9460[8] = "msp03";

const char D_004D9468[8] = "msp01";

const char D_004D9470[8] = "amp02";

const char D_004D9478[8] = "lgc00";

const char D_004D9480[8] = "bp00";

const char D_004D9488[8] = "eac00";

const char D_004D9490[8] = "cb00";

const char D_004D9498[8] = "bl00";

const char D_004D94A0[8] = "msp02";

const char D_004D94A8[8] = "msp00";

const char D_004D94B0[8] = "hb00";

const char D_004D94B8[8] = "glg00";

const char D_004D94C0[8] = "gnd00";

const char D_004D94C8[8] = "fl00";

const char D_004D94D0[8] = "pbk00";

const char D_004D94D8[8] = "lsr00";

const char D_004D94E0[8] = "drl01";

const char D_004D94E8[8] = "drl00";

const char D_004D94F0[8] = "bs00";

const char D_004D94F8[8] = "bam00";

const char D_004D9500[8] = "rf02";

const char D_004D9508[8] = "rf01";

const char D_004D9510[8] = "rf00";

const char D_004D9518[8] = "mg01";

const char D_004D9520[8] = "mg00";

const char D_004D9528[8] = "hmr02";

const char D_004D9530[8] = "dlc00";

const char D_004D9538[8] = "hmr00";

const char D_004D9540[8] = "hg01";

const char D_004D9548[8] = "hg00";

const char D_004D9550[8] = "wct00";

const char D_004D9558[8] = "bd01";

const char D_004D9560[8] = "bd00";

const char D_004D9568[8] = "ax00";

const char D_004D9570[8] = "acc036";

const char D_004D9578[8] = "acc035";

const char D_004D9580[8] = "acc034";

const char D_004D9588[8] = "acc033";

const char D_004D9590[8] = "acc032";

const char D_004D9598[8] = "acc031";

const char D_004D95A0[8] = "acc030";

const char D_004D95A8[8] = "acc029";

const char D_004D95B0[8] = "acc028";

const char D_004D95B8[8] = "acc018";

const char D_004D95C0[8] = "wing";

const char D_004D95E8[8] = "323";

const char D_004D95F0[8] = "322";

const char D_004D95F8[8] = "321";

const char D_004D9600[8] = "320";

const char D_004D9638[8] = "313";

const char D_004D9640[8] = "312";

const char D_004D9648[8] = "311";

const char D_004D9650[8] = "310";

const char D_004D9688[8] = "303";

const char D_004D9690[8] = "302";

const char D_004D9698[8] = "301";

const char D_004D96B0[8] = "023";

const char D_004D96C0[8] = "021";

const char D_004D96C8[8] = "020";

const char D_004D9708[8] = "012";

const char D_004D9710[8] = "011";

const char D_004D9718[8] = "010";

const char D_004D9750[8] = "003";

const char D_004D9758[8] = "002";

const char D_004D9760[8] = "001";

const char D_004D9768[8] = "033";

const char D_004D9770[8] = "032";

const char D_004D9778[8] = "031";

const char D_004D9780[8] = "030";

const char D_004D97A8[8] = "180";

const char D_004D97E0[8] = "173";

const char D_004D97E8[8] = "172";

const char D_004D97F0[8] = "171";

const char D_004D97F8[8] = "170";

const char D_004D9830[8] = "163";

const char D_004D9838[8] = "162";

const char D_004D9840[8] = "161";

const char D_004D9848[8] = "160";

const char D_004D9880[8] = "153";

const char D_004D9888[8] = "152";

const char D_004D9890[8] = "151";

const char D_004D9898[8] = "150";

const char D_004D98D0[8] = "143";

const char D_004D98D8[8] = "142";

const char D_004D98E0[8] = "141";

const char D_004D98E8[8] = "140";

const char D_004D9920[8] = "133";

const char D_004D9928[8] = "132";

const char D_004D9930[8] = "131";

const char D_004D9938[8] = "130";

const char D_004D9970[8] = "123";

const char D_004D9978[8] = "122";

const char D_004D9980[8] = "121";

const char D_004D9988[8] = "120";

const char D_004D99B8[8] = "113";

const char D_004D99C0[8] = "112";

const char D_004D99C8[8] = "111";

const char D_004D99D0[8] = "110";

const char D_004D9A08[8] = "103";

const char D_004D9A10[8] = "102";

const char D_004D9A18[8] = "101";

const char D_004D9A20[8] = "100";

const char D_004D9A58[8] = "093";

const char D_004D9A60[8] = "092";

const char D_004D9A70[8] = "090";

const char D_004D9AB0[8] = "082";

const char D_004D9AB8[8] = "081";

const char D_004D9AC0[8] = "080";

const char D_004D9AF8[8] = "073";

const char D_004D9B00[8] = "072";

const char D_004D9B08[8] = "071";

const char D_004D9B10[8] = "070";

const char D_004D9B48[8] = "063";

const char D_004D9B50[8] = "062";

const char D_004D9B58[8] = "061";

const char D_004D9B60[8] = "060";

const char D_004D9BA0[8] = "052";

const char D_004D9BA8[8] = "051";

const char D_004D9BB0[8] = "050";

const char D_004D9BE8[8] = "043";

const char D_004D9BF0[8] = "042";

const char D_004D9BF8[8] = "041";

const char D_004D9C40[8] = "citi\\";

const char D_004D9C48[8] = "ppl\\";

const char D_004D9C50[8] = "npc\\";

const char D_004D9C58[8] = "pc\\";

const char D_004D9C60[8] = "etc\\";

const char D_004D9C68[8] = "ag\\ag";

const char D_004D9C78[8] = "vx\\vx_";

const char D_004D9C80[8] = "vx_";

const char D_004D9C88[8] = "pc\\cho_";

const char D_004D9C90[8] = "cho_";

const char D_004D9C98[8] = "enemy\\";

const char D_004D9CA0[8] = "robo\\";

const char D_004D9CA8[8] = "pc\\son_";

const char D_004D9CB0[8] = "son_";

const char D_004D9CB8[8] = "pc\\stn_";

const char D_004D9CC0[8] = "stn_";

const char D_004D9CC8[8] = "pc\\jr_";

const char D_004D9CD0[8] = "jr_";

const char D_004D9CD8[8] = "pc\\zig_";

const char D_004D9CE0[8] = "zig_";

const char D_004D9CE8[8] = "pc\\mmo_";

const char D_004D9CF0[8] = "mmo_";

const char D_004D9CF8[8] = "pc\\kos_";

const char D_004D9D00[8] = "kos_";

const char D_004D9D08[8] = "boss_";

const char D_004D9D18[8] = "fere_";

const char D_004D9D20[8] = "feso_";

const char D_004D9D28[8] = "fero_";

const char D_004D9D30[8] = "fema_";

const char D_004D9D38[8] = "utro_";

const char D_004D9D40[8] = "utso_";

const char D_004D9D48[8] = "utre_";

const char D_004D9D50[8] = "utma_";

const char D_004D9D58[8] = "guno_";

const char D_004D9D60[8] = "mus_";

const char D_004D9D68[8] = "muf_";

const char D_004D9D70[8] = "mux_";

const char D_004D9D78[8] = "tre_";

const char D_004D9D80[8] = "muw_";

const char D_004D9D88[8] = "mu_";

const char D_004D9D90[8] = "aid_";

const char D_004D9D98[8] = "obj\\";

const char D_004D9DA0[8] = "mid_";

const char D_004D9DA8[8] = "eid_";

const char D_004D9DB0[8] = "weapon\\";

const char D_004D9DB8[8] = "wid_";

const char D_004D9DC0[8] = "rid_";

const char D_004D9DC8[8] = "char\\";

const char D_004D9DD0[8] = "cid_";

const char D_004D8AC0[8] = "jr4";

const char D_004D8B98[8] = "jr";

const char D_004D8F50[8] = "du";

const char D_004D91C0[8] = "06";

const char D_004D91C8[8] = "05";

const char D_004D91D0[8] = "04";

const char D_004D91D8[8] = "03";

const char D_004D91E0[8] = "02";

const char D_004D91E8[8] = "01";

const char D_004D91F0[8] = "_09";

const char D_004D91F8[8] = "_08";

const char D_004D9200[8] = "_07";

const char D_004D9208[8] = "_06";

const char D_004D9230[8] = "_05";

const char D_004D9238[8] = "_04";

const char D_004D95C8[8] = "114";

const char D_004D95D0[8] = "";

const char D_004D95D8[8] = "325";

const char D_004D95E0[8] = "324";

const char D_004D9608[8] = "319";

const char D_004D9610[8] = "318";

const char D_004D9618[8] = "317";

const char D_004D9620[8] = "316";

const char D_004D9628[8] = "315";

const char D_004D9630[8] = "314";

const char D_004D9658[8] = "309";

const char D_004D9660[8] = "308";

const char D_004D9668[8] = "307";

const char D_004D9670[8] = "306";

const char D_004D9678[8] = "305";

const char D_004D9680[8] = "304";

const char D_004D96A0[8] = "025";

const char D_004D96A8[8] = "024";

const char D_004D96B8[8] = "022";

const char D_004D96D0[8] = "019";

const char D_004D96D8[8] = "018";

const char D_004D96E0[8] = "017";

const char D_004D96E8[8] = "016";

const char D_004D96F0[8] = "015";

const char D_004D96F8[8] = "014";

const char D_004D9700[8] = "013";

const char D_004D9720[8] = "009";

const char D_004D9728[8] = "008";

const char D_004D9730[8] = "007";

const char D_004D9738[8] = "006";

const char D_004D9740[8] = "005";

const char D_004D9748[8] = "004";

const char D_004D9788[8] = "029";

const char D_004D9790[8] = "028";

const char D_004D9798[8] = "027";

const char D_004D97A0[8] = "026";

const char D_004D97B0[8] = "179";

const char D_004D97B8[8] = "178";

const char D_004D97C0[8] = "177";

const char D_004D97C8[8] = "176";

const char D_004D97D0[8] = "175";

const char D_004D97D8[8] = "174";

const char D_004D9800[8] = "169";

const char D_004D9808[8] = "168";

const char D_004D9810[8] = "167";

const char D_004D9818[8] = "166";

const char D_004D9820[8] = "165";

const char D_004D9828[8] = "164";

const char D_004D9850[8] = "159";

const char D_004D9858[8] = "158";

const char D_004D9860[8] = "157";

const char D_004D9868[8] = "156";

const char D_004D9870[8] = "155";

const char D_004D9878[8] = "154";

const char D_004D98A0[8] = "149";

const char D_004D98A8[8] = "148";

const char D_004D98B0[8] = "147";

const char D_004D98B8[8] = "146";

const char D_004D98C0[8] = "145";

const char D_004D98C8[8] = "144";

const char D_004D98F0[8] = "139";

const char D_004D98F8[8] = "138";

const char D_004D9900[8] = "137";

const char D_004D9908[8] = "136";

const char D_004D9910[8] = "135";

const char D_004D9918[8] = "134";

const char D_004D9940[8] = "129";

const char D_004D9948[8] = "128";

const char D_004D9950[8] = "127";

const char D_004D9958[8] = "126";

const char D_004D9960[8] = "125";

const char D_004D9968[8] = "124";

const char D_004D9990[8] = "119";

const char D_004D9998[8] = "118";

const char D_004D99A0[8] = "117";

const char D_004D99A8[8] = "116";

const char D_004D99B0[8] = "115";

const char D_004D99D8[8] = "109";

const char D_004D99E0[8] = "108";

const char D_004D99E8[8] = "107";

const char D_004D99F0[8] = "106";

const char D_004D99F8[8] = "105";

const char D_004D9A00[8] = "104";

const char D_004D9A28[8] = "099";

const char D_004D9A30[8] = "098";

const char D_004D9A38[8] = "097";

const char D_004D9A40[8] = "096";

const char D_004D9A48[8] = "095";

const char D_004D9A50[8] = "094";

const char D_004D9A68[8] = "091";

const char D_004D9A78[8] = "089";

const char D_004D9A80[8] = "088";

const char D_004D9A88[8] = "087";

const char D_004D9A90[8] = "086";

const char D_004D9A98[8] = "085";

const char D_004D9AA0[8] = "084";

const char D_004D9AA8[8] = "083";

const char D_004D9AC8[8] = "079";

const char D_004D9AD0[8] = "078";

const char D_004D9AD8[8] = "077";

const char D_004D9AE0[8] = "076";

const char D_004D9AE8[8] = "075";

const char D_004D9AF0[8] = "074";

const char D_004D9B18[8] = "069";

const char D_004D9B20[8] = "068";

const char D_004D9B28[8] = "067";

const char D_004D9B30[8] = "066";

const char D_004D9B38[8] = "065";

const char D_004D9B40[8] = "064";

const char D_004D9B68[8] = "059";

const char D_004D9B70[8] = "058";

const char D_004D9B78[8] = "057";

const char D_004D9B80[8] = "056";

const char D_004D9B88[8] = "055";

const char D_004D9B90[8] = "054";

const char D_004D9B98[8] = "053";

const char D_004D9BB8[8] = "049";

const char D_004D9BC0[8] = "048";

const char D_004D9BC8[8] = "047";

const char D_004D9BD0[8] = "046";

const char D_004D9BD8[8] = "045";

const char D_004D9BE0[8] = "044";

const char D_004D9C00[8] = "040";

const char D_004D9C08[8] = "039";

const char D_004D9C10[8] = "038";

const char D_004D9C18[8] = "037";

const char D_004D9C20[8] = "036";

const char D_004D9C28[8] = "035";

const char D_004D9C30[8] = "034";

const char D_004D9C38[8] = "000";

const char D_004D9C70[8] = "ag";

const char D_004D9D10[8] = "_";



typedef struct PlayerPadView {
    u8 unmodeled_00[0x28];
    u16 held;
    u8 unmodeled_2a[0x66 - 0x2a];
    signed char stick_x;
    signed char stick_y;
} PlayerPadView;

typedef struct CameraDefinition {
    u8 unmodeled_00[0x34];
    float yaw;
} CameraDefinition;

extern PlayerPadView PadData;

extern CameraDefinition CfCameraDefine;

extern Vector4 EnemyTarget;

extern short EfTab_Code[15];

extern short EfTab_Intr[15];

extern short EfTab_Loop[15];

int Check_EnemyBurn(void);

int Check_EnemyElec(void);

int Check_EnemyFound(void);

int xglSoundEffectCheckID(int id, int arg);

void xglSoundEffectStopID(int id, int arg);

void sefDeleteEffectCf(void *effect);

PlayerEffect *sefCreateEffectCf(int code, int a, int b);

int Ladder_Main(LookAtPlayerActor *player_actor);

int SCRIPT_getCfTime(void);

float I2F(int value);

float atan2f(float y, float x);

float floorf(float value);

float xglSin(float angle);

void xglMatrixStackRotY(float angle);

float xglCos(float angle);

int Get_Attr(const Vector4 *position, int map_index, int attr_mask);

int Get_EffectCode(int attribute);

void ACT_setMotion2(LookAtPlayerActor *player_actor, int motion, int blend);

void Enemy_FindByEar(LookAtPlayerActor *player_actor, const Vector4 *position);

void HitCheckMapUnitWithNyuru(LookAtPlayerActor *player_actor);

static short HitCheckActor(LookAtPlayerActor *player_actor);

static int NyuruActor(LookAtPlayerActor *player_actor, LookAtPlayerActor *other);

float UnduCheck(const Vector4 *position, void *velocity, ActorUndulation *undulation);

int HitCheckMapUnit(LookAtPlayerActor *player_actor);

float Get_Cursol_by_Reduce_Speed_Angle_Loop(float cursor, float target, float rate);

void ShootMapUnit(void);

void ACT_updateMotion(LookAtPlayerActor *player_actor);

void GameIdLightSet(LookAtPlayerActor *player_actor, int arg);

void Set_Shadow(LookAtPlayerActor *player_actor);

void Get_EnemyIDPos(const Vector4 *position, Vector4 *target);

void Check_Locater(LookAtPlayerActor *player_actor);

#include "main/get.h"

int CheckActorExist(LookAtPlayerActor *target);

float Get_Angle(const Point4 *first, const Point4 *second);

/* HitCheckActor's minimum collision radius and maximum vertical separation
 * thresholds. These literal-pool values are read once for each actor scan. */
extern const float D_004D7C34;

extern const float D_004D7C38;

extern const float D_004D7C3C;

void LookAt_Player(LookAtPlayerActor *player_actor)
{
    int nearest_actor_index;
    signed char look_at_target;

    nearest_actor_index = Get_MostNear_Actor(player_actor);
    if (player_actor->look_at_timer == 2) {
        Actor_LookAt(player_actor);
        return;
    }
    if (GameLoopState.flags & 0x8000) {
        Actor_LookAt_Release(player_actor, 0);
        return;
    }
    look_at_target = GameLoopState.look_at_target;
    if (look_at_target != -1) {
        PlayerLookAtAim();
        return;
    }

    Actor_LookAt_Release(player_actor, 0);
    if (nearest_actor_index != look_at_target &&
        !(actor[nearest_actor_index].flags & 8)) {
        actor[player_actor->number].look_at_target = nearest_actor_index;
        Actor_LookAt_Set(player_actor, 1, &actor[nearest_actor_index].position);
        Actor_LookAt(player_actor);
        return;
    }
    Actor_LookAt_Release(player_actor, 1);
}

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", Player_System_Init);

void GameCfPlayerMoveParamSet(float walkThreshold, float runThreshold, float vectorRate)
{
    int walkThresholdInt;
    int runThresholdInt;

    walkThresholdInt = F2I(walkThreshold);
    WALK_THRESHOLD_F = walkThreshold;
    WALK_THRESHOLD_I = walkThresholdInt;
    runThresholdInt = F2I(runThreshold);
    RUN_THRESHOLD_F = runThreshold;
    VECTOR_RATE = vectorRate;
    RUN_THRESHOLD_I = runThresholdInt;
}

void GameCfPlayerMoveInit(void)
{
    WALK_THRESHOLD_I = 32;
    WALK_THRESHOLD_F = 32.0f;
    RUN_THRESHOLD_I = 96;
    RUN_THRESHOLD_F = 96.0f;
    VECTOR_RATE = D_004D7C0C;
}

void GameCfPlayerMove(void)
{
    LookAtPlayerActor *player = GameLoopState.player_actor;
    float run_scale = 1.0f;
    float speed;
    int stick_speed;
    int terrain_bits;
    int ground_effect;
    int hit;
    PlayerAimVector position_before_move;
    const Vector4 *before_move;

    if (Check_EnemyBurn() == 0 && xglSoundEffectCheckID(0x10009, 0) != 0) {
        xglSoundEffectStopID(0x10009, 0);
    }
    if (Check_EnemyElec() == 0 && xglSoundEffectCheckID(0x10008, 0) != 0) {
        xglSoundEffectStopID(0x10008, 0);
    }
    if (Check_EnemyFound() == 0) {
        if (GameLoopState.effect != 0) {
            sefDeleteEffectCf(GameLoopState.effect);
        }
        GameLoopState.effect = 0;
    }
    if (player->flash_timer != 0) {
        if (--player->flash_timer <= 0) {
            player->flash_timer = 0;
            player->flags &= ~0x800;
        } else {
            unsigned short phase = player->flash_timer;

            if (!(phase & 1) || (short)phase >= 19) {
                player->flags |= 0x800;
                player->render_flags |= 3;
                player->flash_frames = 0x48;
                player->filter_param = 0.5f;
            } else {
                player->flags &= ~0x800;
            }
        }
    }
    if (Ladder_Main(player) == 1) {
        player->flags &= ~0x20;
        return;
    }
    speed = 0.0f;
    stick_speed = 0;
    player->flags |= 0x20;
    player->external_velocity.y = -0.05f;
    player->previous_position.x = player->position.x;
    player->previous_position.y = player->position.y;
    player->previous_position.z = player->position.z;
    player->velocity.x = 0.0f;
    player->velocity.z = 0.0f;
    player->external_velocity.x = 0.0f;
    player->external_velocity.z = 0.0f;
    if (!(GameLoopState.flags & 0x8000) && SCRIPT_getCfTime() >= 3) {
        signed char stick_x = PadData.stick_x;
        signed char stick_y = PadData.stick_y;
        float length;
        float angle;

        if (PadData.held & 0xa000) {
            stick_x = (PadData.held & 0x8000) ? -127 : 127;
        }
        if (PadData.held & 0x5000) {
            stick_y = (PadData.held & 0x1000) ? -127 : 127;
        }
        if (GameLoopState.flags & 0x100000) {
            stick_x = player->stick_x;
            stick_y = player->stick_y;
        } else {
            player->stick_x = stick_x;
            player->stick_y = stick_y;
        }
        if (GameLoopState.move_mode != 2) {
            float x = I2F(stick_x);
            float y = I2F(stick_y);

            length = __builtin_sqrtf(x * x + y * y);
            angle = floorf((atan2f(x, y) + 0.19634955f) / 0.3926991f) * 0.3926991f
                + CfCameraDefine.yaw;
        } else {
            length = -I2F(stick_y);
            angle = player->rotation_y - I2F(stick_x) * 0.001f;
        }
        if (128.0f <= length) {
            length = 128.0f;
        }
        if (WALK_THRESHOLD_F <= length) {
            player->aim_angle = angle;
            if (player->terrain_flags & 0x10) {
                if (RUN_THRESHOLD_F <= length) {
                    length = RUN_THRESHOLD_F - 0.1f;
                }
            }
            if (PadData.held & 2) {
                length = (RUN_THRESHOLD_F - 0.1f) / 1.5f;
            }
            stick_speed = F2I(length);
            speed = length * VECTOR_RATE;
            terrain_bits = (int)(player->terrain_flags & 0x3f000000);
            if (!(terrain_bits & 0x0f000000)) {
                static float rate[4] = { 1.0f, 2.0f, 0.5f, 3.0f };

                speed *= rate[(u32)terrain_bits >> 28];
            }
        } else {
            stick_speed = 0;
            speed = 0.0f;
        }
        player->move_speed = speed;
        player->velocity.x += xglSin(player->aim_angle) * speed;
        player->velocity.y += 0.0f;
        player->velocity.z += xglCos(player->aim_angle) * speed;
        {
            static float rate[4] = { 1.0f, 2.0f, 0.5f, 3.0f };
            static u8 idx[16] = { 0, 1, 3, 2, 5, 0, 4, 0, 7, 8, 0, 0, 6, 0, 0, 0 };
            static float vec[9][2] = {
                { 0.0f, 0.0f },  { 0.0f, 1.0f },  { 1.0f, 1.0f },
                { 1.0f, 0.0f },  { 1.0f, -1.0f }, { 0.0f, -1.0f },
                { -1.0f, -1.0f }, { -1.0f, 0.0f }, { -1.0f, 1.0f },
            };

            terrain_bits = (int)(player->terrain_flags & 0x3f000000);
            if (terrain_bits & 0x0f000000) {
                float scale = VECTOR_RATE * 128.0f * rate[(u32)terrain_bits >> 28];
                float *direction = vec[idx[((u32)terrain_bits & 0x0f000000) >> 24]];

                player->velocity.x += scale * direction[0];
                player->velocity.z += scale * direction[1];
            }
            run_scale = rate[(u32)terrain_bits >> 28];
        }
    }
    if (!(GameLoopState.flags & 0x4000)) {
        int abs_speed = stick_speed;

        if (abs_speed < 0) {
            abs_speed = -abs_speed;
        }

        if (abs_speed < WALK_THRESHOLD_I) {
            if (player->motion_id != 0x1b) {
                ACT_setMotion2(player, 0, 9);
            }
            player->motion_speed = 0.033333335f;
        } else if (abs_speed < RUN_THRESHOLD_I) {
            ACT_setMotion2(player, 1, 9);
            player->motion_speed = speed * 0.5f / run_scale;
        } else {
            ACT_setMotion2(player, 3, 9);
            player->motion_speed = speed * 0.23333335f / run_scale;
            Enemy_FindByEar(player, &player->position);
        }
    }
    speed = __builtin_fabsf(speed);
    ground_effect = Get_EffectCode(Get_Attr(&player->position, 0, 0));
    if (0.01f < speed || EfTab_Loop[ground_effect] == 1) {
        if (ground_effect != -1) {
            if (EfTab_Intr[ground_effect] != -1) {
                player->effect_timer++;
            }
            if ((EfTab_Intr[ground_effect] != -1 && player->effect_timer >= EfTab_Intr[ground_effect])
                || player->effect_timer == -1) {
                PlayerEffect *effect = sefCreateEffectCf(EfTab_Code[ground_effect], 0, 0);

                player->effects[player->effect_slot] = effect;
                if (effect != 0) {
                    effect->owner = player;
                    player->effect_timer = 0;
                    player->effect_slot = (player->effect_slot + 1) % 8;
                }
            }
        } else {
            player->effect_timer = ground_effect;
        }
    }
    player->velocity.x += player->external_velocity.x;
    player->velocity.y += player->external_velocity.y;
    player->velocity.z += player->external_velocity.z;
    __builtin_memcpy(&position_before_move, &player->position, sizeof(position_before_move));
    before_move = &position_before_move.vector;
    player->position.x += player->velocity.x;
    player->position.z += player->velocity.z;
    HitCheckMapUnitWithNyuru(player);
    hit = HitCheckActor(player);
    if (hit != -1) {
        NyuruActor(player, &actor[hit]);
    }
    player->velocity.x = player->position.x - before_move->x;
    player->velocity.z = player->position.z - before_move->z;
    player->undulation.attr_mask = (player->undulation.attr_mask & 0xfff8) | 2;
    player->position.x = before_move->x;
    player->position.z = before_move->z;
    {
        float floor_height = UnduCheck(&player->position, &player->velocity, &player->undulation);

        if (floor_height != -1000.0f && player->position.y < floor_height) {
            player->position.y = floor_height;
            player->velocity.y = 0.0f;
        }
    }
    if (HitCheckMapUnit(player) != -1) {
        __builtin_memcpy(&player->position, &position_before_move, sizeof(position_before_move));
    }
    player->rotation_y = Get_Cursol_by_Reduce_Speed_Angle_Loop(player->rotation_y, player->aim_angle, 4.0f);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&player->position.x);
    xglMatrixStackRotZ(player->rotation_z);
    xglMatrixStackRotY(player->rotation_y);
    xglMatrixStackRotX(player->rotation_x);
    if (GameLoopState.map_flags & 1) {
        ShootMapUnit();
    }
    LookAt_Player(player);
    ACT_updateMotion(player);
    GameIdLightSet(player, 0);
    Set_Shadow(player);
    Get_EnemyIDPos(&player->position, &EnemyTarget);
    if (!(GameLoopState.flags & 0x8000)) {
        Check_Locater(player);
    }
}

static short HitCheckActor(LookAtPlayerActor *player_actor)
{
    short i;
    float dx;
    float dz;
    float distance;
    float minimum_body_radius;
    float maximum_height_difference;
    Vector4 *pos;

    minimum_body_radius = D_004D7C34;
    maximum_height_difference = D_004D7C38;
    for (i = 0; i < 64; i++) {
        if (!CheckActorExist(&actor[i])) {
            continue;
        }
        if (player_actor->number == i) {
            continue;
        }
        pos = &actor[i].position;
        if (actor[i].body_radius < minimum_body_radius) {
            continue;
        }
        if (maximum_height_difference < __builtin_fabsf(player_actor->position.y - pos->y)) {
            continue;
        }
        dx = player_actor->position.x - pos->x;
        dz = player_actor->position.z - pos->z;
        distance = __builtin_sqrtf(dx * dx + dz * dz);
        if (distance <= player_actor->body_radius + actor[i].body_radius) {
            return i;
        }
    }
    return -1;
}

static int NyuruActor(LookAtPlayerActor *player_actor, LookAtPlayerActor *other)
{
    float angle;
    float reach;
    Vector4 contact;
    int hit;

    angle = Get_Angle((const Point4 *)&player_actor->position,
                      (const Point4 *)&other->position);
    player_actor->position.x -= player_actor->velocity.x;
    player_actor->position.z -= player_actor->velocity.z;
    reach = player_actor->body_radius + other->body_radius + D_004D7C3C;
    contact.x = other->position.x - reach * xglSin(angle);
    contact.z = other->position.z - reach * xglCos(angle);
    player_actor->velocity.x = contact.x - player_actor->position.x;
    player_actor->velocity.z = contact.z - player_actor->position.z;
    player_actor->position.x += player_actor->velocity.x;
    player_actor->position.z += player_actor->velocity.z;
    hit = HitCheckActor(player_actor);
    if (hit != -1) {
        player_actor->position.x -= player_actor->velocity.x;
        player_actor->position.z -= player_actor->velocity.z;
        return 0;
    }
    if (HitCheckMapUnit(player_actor) != hit) {
        player_actor->position.x -= player_actor->velocity.x;
        player_actor->position.z -= player_actor->velocity.z;
        return 0;
    }
    return 1;
}

void PlayerLookAtAim(void)
{
    int look_at_target;
    void *player_actor;
    PlayerAimVector target_position;

    look_at_target = GameLoopState.look_at_target;
    player_actor = GameLoopState.player_actor;
    target_position = MapUnit[look_at_target].position;
    target_position.vector.y += 1.0f;
    Actor_LookAt_Set(player_actor, 0, &target_position.vector);
    Actor_LookAt(player_actor);
}

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", GameCfPlayerLoadResource);
