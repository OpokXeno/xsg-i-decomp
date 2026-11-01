#ifndef INCLUDE_UMBRELLA_PUBLIC_H
#define INCLUDE_UMBRELLA_PUBLIC_H

void ACT_draw(void);

extern u8 *WorkEnd;

extern HomogeneousVector cursor[];

extern void eBattleWinClose2(void);
extern int eBattleWinPageCheck4(void);
extern void eBattleWinClose4(void);
void eBattleWinClose(void *window);
void eBattleWinClose3(void);
void eBattleWinInit(void);

void endPrintInit(void);
void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh);

extern int Get_ActorNumber(int target);

extern int MenuFileMain(int mode);

void *newObject(SceneClass *scene_class);
void *newClass(void);
void loadStaticClass(void *class_slot, const char *name);

void MenuGameDataPush(void);
void MenuGameDataPop(void);
void GamePushSaveDataUser(void);
void Game(void);

void GameCameraStateSave(void);
void GameCameraStateRestore(void);

void GameMovieMain(void);

void GameSnapShotCheck(void);

int Get_Rnd(int min, int max);
int Check_Angle(float angle, float rangeStart, float rangeEnd);

extern short Times;

int CrossPointUwamono(const Vector4 *position, const Vector4 *previous,
                      float radius, Vector4 *result);
int HitCheckMapUnitPos(const Vector4 *position);

extern SceneClass *classString;

extern SceneClass *classJava_xeno_Scene;
extern SceneClass *classJava_xeno_Stage;
extern SceneClass *classJava_xeno_Chr;
extern SceneClass *classJava_xeno_Unit;
extern SceneClass *classJava_xeno_Enepc;
extern SceneClass *classJava_xeno_util_Window;
extern SceneClass *classJava_xeno_Uwamono;
extern SceneClass *classJava_xeno_util_Vector4f;
extern SceneClass *classJava_xeno_util_Layout;
extern SceneClass *classJava_xeno_Effect;
extern SceneClass *classJava_xeno_Light;
extern SceneClass *classJava_xeno_util_Menu;
extern SceneClass *classJava_xeno_util_TCHParams;
extern SceneClass *classJava_xeno_util_Spline;
extern SceneClass *classJava_xeno_util_Input;
extern SceneClass *classJava_xeno_Camera;
extern SceneClass *classJava_xeno_Movie;
void JNI_popFrame(void);

extern char *STR_tokenGetNext(char *text, const char *delimiters);
extern int STR_tokenGetType(void);

void GameCfPlayerLoadResource(int use_cache);

int MEfObjDestroy(void *object);

float *MMathCalcHermite(float *destination, float parameter,
                         HermiteVector *tangent_first, HermiteVector *tangent_second,
                         HermiteVector *endpoint_first, HermiteVector *endpoint_second);
void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                          const HermiteVector *point_prev, const HermiteVector *point_start,
                          const HermiteVector *point_end, const HermiteVector *point_next);

void MapDraw(void);

void MAP_drawUnit(void);

void CUR_MATRIX_Unit4s(void);
void CUR_MATRIX_Get(Matrix4 matrix);
void CUR_MATRIX_SetT(const Vector4 *translation);
void CUR_MATRIX_Sxyz4s(const Vector4 *scale);

int MenuMaryIdChange(int id);
int MenuScenarioNoGet(void);
int MenuEquipStealMaskCheck(void);
const unsigned char *MenuSaveMapNameGet(int mapId);
int MenuMainCharCheck(int charId);
int MenuAccessoryEquipCheck(int charId, int accessoryId, int slotIndex);

int MenuEtherWhoCheck(int etherId);

void MenuModelMemorySet(int state);
void MenuModelMemoryInit(void *base, int size);
void MenuModelMenuMotionLoad(void);
void MenuModelMain(void);
void MenuModelAllBreak(void);

void MenuShopMain(int shopType);

void MenuPasWindow(void);

void nmlFilterSetTexClear(int frameBase);

void nmlModelSetSortOffset(float offset);
void nmlModelSetGlobalPointLightCol(int index, const Vector4 *color);
void nmlModelSetSpecularOff(int disableSpecular);
void nmlModelSetRenderStatus(int status);
void nmlModelSetClip(int clip);
int nmlModelSetGlobalFogReset(void);
void nmlModelSetFogCancel(void);
void nmlModelFlush(void);

extern const char nmlEmptyName[4];
void nmlPacketAddGsScissor(u64 scissor);

int CheckPointLine(const Vector4 *first, const Vector4 *second,
                   const Vector4 *point);

unsigned char *PartyDataGet(void);
extern void PartyDataInit(void);
void PartyFriendOff(int id);
int PartyFriendCheck(int id);
void PartyOutFriendOn(int id);
void PartyOutFriendOff(int id);
int PartyOutFriendCheck(int id);
int PartyFriendLockCheck(int id, int flag);
int PartyAllPartyGet(int *out, int mode);

void GameResourceWorkReload(void);

int scGetImmAdrImmIdx2(int base, unsigned short *table, int index);

extern void CallMethod(const char *method_name);
int SCRIPT_getCfTime(void);
void SCRIPT_execTalkto(void *actor);
int SCRIPT_load2(const char *path);

int sefIsDeadSchduler(unsigned int scheduler_index);
void sefKillEffect(int effect_no);
int sefSearchMapperIndex(int effect_id);
void sefInitEffectCf(void);
void sefExecEffect(void);
int sefLoadEffectCf(int cf_id, int effect_no);
int sefIsFinishEffect2(int effect_no);
void sefSetLoadQue(int effect_no);
int *sefGetLoadQue(void);
unsigned char *sevGetPtAllocator2(int allocator_index);

void *smAlloc(unsigned int size);

int srsLoadEffectData(void *buffer, int effectNo);
int srsFileLoadCf(int cfId, int effectNo);

void SsdSetOutputMode(int mode);

void reloadConstString(void *heap_boundary);

void TWSYS_draw(void);

void Vibration_Set_Weak(int duration);
void VibrationEngine(void);

void *MenuWorkEndGet(void);
void UmnMailBoxSet(int box_id);
unsigned char *UmnMailDataGet(int box_id);
void MoveSlide(short *current, short *target, float rate);
int MenuLoadSync(void);
void MenuLoadInit(void);
void MenuLoadCancel(void);
extern void MenuBgTaskBreak(void);
void ChangeTopLevel(int top_level);
void MenuKeepSelectReset(void);
void MenuFontLoad(int original_font, int asynchronous);
void MenuCfTaikiPush(void);
void MenuCfTaikiPop(void);
void MenuBgTaskMain(void);

extern void xglVectorLength(float *destination, const Vector4 *vector);
extern void xglMatrixStackUnit(void);
void xglMatrixStackReverse(void);
void xglMatrixStackInverse(void);
void xglMatrixStackScale(const float scale[4]);
void xglMatrixStackTrans(const float translation[4]);
extern void xglMatrixStackRotX(float angle);
extern void xglMatrixStackRotY(float angle);
extern void xglMatrixStackRotZ(float angle);
extern void xglMatrixStackSave(float matrix[4][4]);
unsigned short xglSRand(void);
float xglSin(float angle);
float xglCos(float angle);
void xglVectorScale(Vector4 *destination, const Vector4 *source, float scale);

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
void xglClockRead(XglClock *clock);
extern int xglCdGetFileSize(const char *name);

extern void xglFlagsInitial(void);

extern void xglFontPrint(int x, int y, int color, const char *text);

int xglHddMount(void);
extern int xglHddActivate(int state);
extern int xglHddMcUmount(void);
extern int xglHddMcLoadMount(void);
extern int xglHddMcLoadCore(void *save);

extern unsigned char SaveData[];
extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);
void xglMcWriteMapName(char *buffer, int slot);

extern XglPacket *xglPacketGetCurrent(void);

extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count);

extern int xglSoundStreamMain(void);
extern void xglSoundEffectNormalID(int sound_id, int variant);
void xglSoundSendEffect(void *swd, void *sed, int bank);

extern StudioCamera *xglStudioGetCamera2(int camera_id);
extern void xglStudioChange(int studio_index);
extern StudioCamera *xglStudioMainCameraMove(void);
extern void xglStudioMove(void);

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);
int xglTaskWaitRemove(XglTaskPrefix *task);
void xglTaskExecute(XglTaskScheduler *scheduler);

extern void xglSleep(void);

int xglArxExtract(u32 *dest, u32 *src);

void calcPara2Org(void);

void objRemovePure(ObjectTask *task);

void resultProcInit(void);

char *RgFileSysDataGetName(RgFileSysData *pFile);

void RgGeomRobotSetDir(RgGeom *geom, RgVector direction);

void RgRobotSetWeapon(RgStatus *pRobot, int eSide, int weaponID);
void RgRobotSetSpareWeapon(RgStatus *pRobot, int eSide, int weaponID);

extern void DisposeRgSimpleDB(RgSimpleDB *database);
extern RgSimpleDB *CreateRgSimpleDB(int capacity, int entry_size);

void *RgSingletonIDGet(unsigned int singleton_id);
extern void RgSingletonIDEntry(
    int singleton_id,
    RgSimpleDB *database,
    void (*destructor)(RgSimpleDB *database));

int XrgHostRead(int handle, void *buffer, int size);

extern void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);
extern void XrgCopyVector(RgVector destination, RgVector source);
void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);
void XrgClearVector(RgVector destination);
void XrgSubVector(RgVector destination, RgVector first, RgVector second);
void XrgCalcMatrixXtoZ(RgMatrix matrix, RgVector xAxis, RgVector zAxis);

#endif /* INCLUDE_UMBRELLA_PUBLIC_H */
