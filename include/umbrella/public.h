#ifndef INCLUDE_UMBRELLA_PUBLIC_H
#define INCLUDE_UMBRELLA_PUBLIC_H

float *MMathCalcHermite(float *destination, float parameter,
                         HermiteVector *tangent_first, HermiteVector *tangent_second,
                         HermiteVector *endpoint_first, HermiteVector *endpoint_second);

unsigned char *PartyDataGet(void);
extern void PartyDataInit(void);

int sefIsDeadSchduler(unsigned int scheduler_index);

void reloadConstString(void *heap_boundary);

extern void xglVectorLength(float *destination, const Vector4 *vector);
extern void xglMatrixStackUnit(void);
void xglMatrixStackScale(const float scale[4]);
extern void xglMatrixStackRotX(float angle);
extern void xglMatrixStackRotY(float angle);
extern void xglMatrixStackRotZ(float angle);
extern void xglMatrixStackSave(float matrix[4][4]);
unsigned short xglSRand(void);
void xglMatrixStackTrans(const float translation[4]);

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
void xglClockRead(XglClock *clock);

extern void xglFlagsInitial(void);

extern void xglFontPrint(int x, int y, int color, const char *text);

int xglHddMount(void);
extern int xglHddActivate(int state);

extern XglPacket *xglPacketGetCurrent(void);

extern StudioCamera *xglStudioGetCamera2(int camera_id);

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);
int xglTaskWaitRemove(XglTaskPrefix *task);
void xglTaskExecute(XglTaskScheduler *scheduler);

extern void xglSleep(void);

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
