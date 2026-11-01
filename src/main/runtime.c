#include "common.h"

#include "shared.h"
#include "main/find_native_method.h"
#include "main/init_vm.h"

#include "main/party.h"

/* Runtime native lookup pairs: method signature followed by its entry point. */

#define RUNTIME_UMN_BOX_OFFSET 0x1025C

#define RUNTIME_UMN_BOX_SLOTS 128

typedef struct RuntimeNativeBinding {
    const char *signature;
    void (*entry)();
} RuntimeNativeBinding;

const char javaSystemArraycopyName[80] = "Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II";

const char javaSystemSleepName[32] = "Java_xeno_vm_System_sleep__I";

const char javaSystemPrintlnName[48] = "Java_xeno_vm_System_println__Ljava_lang_String_";

const char javaSystemWaitForName[48] = "Java_xeno_vm_System_waitFor__Ljava_lang_Object_";

const char javaSystemMethodSignalName[40] = "Java_xeno_vm_System_methodSignal__I";

const char javaThreadCreateName[32] = "Java_xeno_vm_Thread_create__";

const char javaThreadSetTargetName[72] = "Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_";

const char javaThreadStartName[32] = "Java_xeno_vm_Thread_start__";

const char javaThreadStopName[32] = "Java_xeno_vm_Thread_stop__";

const char javaMathRandomName[32] = "Java_xeno_vm_Math_random__";

const char javaMathAtan2Name[32] = "Java_xeno_vm_Math_atan2__FF";

const char vmMathCosName[32] = "Java_xeno_vm_Math_cos__F";

const char vmMathSinName[32] = "Java_xeno_vm_Math_sin__F";

const char splineCreateName[32] = "Java_xeno_util_Spline_create__";

const char splineSetCtrlVertexName[48] = "Java_xeno_util_Spline_setCtrlVertex__aFIII";

const char splineGetValueName[40] = "Java_xeno_util_Spline_getValue__I";

const char formatFloatToIntBitsName[40] = "Java_xeno_util_Format_floatToIntBits__F";

const char formatIntBitsToFloatName[40] = "Java_xeno_util_Format_intBitsToFloat__I";

const char formatToIntName[48] = "Java_xeno_util_Format_toInt__Ljava_lang_String_";

const char formatToStringCharName[40] = "Java_xeno_util_Format_toString__C";

const char formatToStringFloatName[40] = "Java_xeno_util_Format_toString__F";

const char formatToStringIntName[40] = "Java_xeno_util_Format_toString__I";

const char formatToStringBoolName[40] = "Java_xeno_util_Format_toString__Z";

const char inputCreateName[32] = "Java_xeno_util_Input_create__I";

const char inputGetButtonName[40] = "Java_xeno_util_Input_getButton__";

const char inputGetEdgeName[32] = "Java_xeno_util_Input_getEdge__";

const char inputGetRepeatName[40] = "Java_xeno_util_Input_getRepeat__";

const char layoutSetName[48] = "Java_xeno_util_Layout_set__Ljava_lang_Object_I";

const char layoutGetManagerName[40] = "Java_xeno_util_Layout_getManager__I";

const char runtimeExecBattleName[40] = "Java_xeno_util_Runtime_execBattle__II";

const char runtimeGetFlagsName[40] = "Java_xeno_util_Runtime_getFlags__II";

const char runtimeSetFlagsName[40] = "Java_xeno_util_Runtime_setFlags__III";

const char runtimeSetLocationName[40] = "Java_xeno_util_Runtime_setLocation__III";

const char runtimeGetLocationName[40] = "Java_xeno_util_Runtime_getLocation__";

const char runtimeSetRegisterName[40] = "Java_xeno_util_Runtime_setRegister__II";

const char runtimeGetRegisterName[40] = "Java_xeno_util_Runtime_getRegister__I";

const char runtimeGetEntranceName[40] = "Java_xeno_util_Runtime_getEntrance__";

const char runtimeSetPlayerControlName[48] = "Java_xeno_util_Runtime_setPlayerControl__Z";

const char runtimeSetPlayerMoveParamName[48] = "Java_xeno_util_Runtime_setPlayerMoveParam__FFF";

const char runtimeDisableName[40] = "Java_xeno_util_Runtime_disable__I";

const char runtimeEnableName[40] = "Java_xeno_util_Runtime_enable__I";

const char runtimeGetGameStateName[40] = "Java_xeno_util_Runtime_getGameState__";

const char runtimeCaptureEndName[40] = "Java_xeno_util_Runtime_CaptureEnd__";

const char runtimeCaptureStartName[64] = "Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I";

const char runtimeJumpCFName[40] = "Java_xeno_util_Runtime_jumpCF__II";

const char runtimeJumpEventName[40] = "Java_xeno_util_Runtime_jumpEvent__I";

const char runtimeSetDefocusQuickName[48] = "Java_xeno_util_Runtime_setDefocusQuick__IIII";

const char runtimeSetDefocusName[40] = "Java_xeno_util_Runtime_setDefocus__IIaI";

const char runtimeSetDefocusParamName[48] = "Java_xeno_util_Runtime_setDefocusParam__III";

const char runtimeSetEventTimerName[64] = "Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I";

const char runtimeSetMapName[40] = "Java_xeno_util_Runtime_setMap__II";

const char runtimeSetShootFlagName[40] = "Java_xeno_util_Runtime_setShootFlag__Z";

const char runtimeSetShootHeightCheckName[48] = "Java_xeno_util_Runtime_setShootHeightCheck__Z";

const char runtimeSetShootIDCheckName[48] = "Java_xeno_util_Runtime_setShootIDCheck__Z";

const char runtimeSetShootUwaCheckName[48] = "Java_xeno_util_Runtime_setShootUwaCheck__Z";

const char runtimeSetShootRangeName[40] = "Java_xeno_util_Runtime_setShootRange__F";

const char runtimeAddItemName[40] = "Java_xeno_util_Runtime_addItem__II";

const char runtimeAddItemWinName[40] = "Java_xeno_util_Runtime_addItemWin__II";

const char runtimeRemoveItemName[40] = "Java_xeno_util_Runtime_removeItem__II";

const char runtimeCheckItemName[40] = "Java_xeno_util_Runtime_checkItem__II";

const char runtimeGetItemNameName[40] = "Java_xeno_util_Runtime_getItemName__II";

const char runtimeAddGoldName[40] = "Java_xeno_util_Runtime_addGold__I";

const char runtimeRemoveGoldName[40] = "Java_xeno_util_Runtime_removeGold__I";

const char runtimeCheckGoldName[40] = "Java_xeno_util_Runtime_checkGold__";

const char runtimeProgressEffectName[48] = "Java_xeno_util_Runtime_progressEffect__I";

const char runtimeEnterShopName[40] = "Java_xeno_util_Runtime_enterShop__I";

const char runtimeGetLeaderName[40] = "Java_xeno_util_Runtime_getLeader__";

const char runtimeGetPartyDataName[40] = "Java_xeno_util_Runtime_getPartyData__I";

const char runtimeSetPartyDataName[40] = "Java_xeno_util_Runtime_setPartyData__II";

const char runtimeSetFriendName[40] = "Java_xeno_util_Runtime_setFriend__I";

const char runtimeResetFriendName[40] = "Java_xeno_util_Runtime_resetFriend__I";

const char runtimeCheckFriendName[40] = "Java_xeno_util_Runtime_checkFriend__I";

const char runtimeSetLockPartyName[40] = "Java_xeno_util_Runtime_setLockParty__I";

const char runtimeResetLockPartyName[48] = "Java_xeno_util_Runtime_resetLockParty__I";

const char runtimeCheckLockPartyName[48] = "Java_xeno_util_Runtime_checkLockParty__I";

const char runtimeSetOutFriendName[40] = "Java_xeno_util_Runtime_setOutFriend__I";

const char runtimeResetOutFriendName[48] = "Java_xeno_util_Runtime_resetOutFriend__I";

const char runtimeCheckOutFriendName[48] = "Java_xeno_util_Runtime_checkOutFriend__I";

const char runtimeSetTakeAgwsName[40] = "Java_xeno_util_Runtime_setTakeAgws__I";

const char runtimeResetTakeAgwsName[40] = "Java_xeno_util_Runtime_resetTakeAgws__I";

const char runtimeCheckTakeAgwsName[40] = "Java_xeno_util_Runtime_checkTakeAgws__I";

const char runtimeBattleChangePartyName[48] = "Java_xeno_util_Runtime_battleChangeParty__I";

const char runtimeSetIdLightColName[48] = "Java_xeno_util_Runtime_setIdLightCol__IIFFF";

const char runtimeSetIdLightDirName[48] = "Java_xeno_util_Runtime_setIdLightDir__IIFFF";

const char runtimeSetIdLightVecName[48] = "Java_xeno_util_Runtime_setIdLightVec__IIFFF";

const char runtimeSetWindParamName[48] = "Java_xeno_util_Runtime_setWindParam__IFFFF";

const char runtimeMpeg2Name[56] = "Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_";

const char runtimeMpeg2AfterCrossFadeName[48] = "Java_xeno_util_Runtime_mpeg2AfterCrossFade__I";

const char runtimeMailFlagName[40] = "Java_xeno_util_Runtime_mailFlag__I";

const char runtimeMailExecName[40] = "Java_xeno_util_Runtime_mailExec__I";

const char runtimeMinigameExecName[40] = "Java_xeno_util_Runtime_minigameExec__I";

const char runtimeSetMenuLockName[40] = "Java_xeno_util_Runtime_setMenuLock__";

const char runtimeEvsSetRetPointName[40] = "Java_xeno_util_Runtime_evsSetRetPoint__";

const char runtimeEvsExitName[40] = "Java_xeno_util_Runtime_evsExit__";

const char runtimeEtherTecSetName[40] = "Java_xeno_util_Runtime_etherTecSet__I";

const char runtimeCharAllRecoveryName[48] = "Java_xeno_util_Runtime_charAllRecovery__";

const char runtimeAGWSAllRecoveryName[48] = "Java_xeno_util_Runtime_AGWSAllRecovery__";

const char windowGetSignalName[40] = "Java_xeno_util_Window_getSignal__";

const char windowSignalName[32] = "Java_xeno_util_Window_signal__I";

const char windowClearName[32] = "Java_xeno_util_Window_clear__";

const char windowCloseName[32] = "Java_xeno_util_Window_close__";

const char windowWaitkeyName[40] = "Java_xeno_util_Window_waitkey__I";

const char windowWaitName[32] = "Java_xeno_util_Window_wait__I";

const char windowCreateName[32] = "Java_xeno_util_Window_create__I";

const char windowSetSizeName[40] = "Java_xeno_util_Window_setSize__II";

const char windowSetLocationName[40] = "Java_xeno_util_Window_setLocation__II";

const char windowPrintStringName[48] = "Java_xeno_util_Window_print__Ljava_lang_String_";

const char windowPrintStringArrayIntName[56] = "Java_xeno_util_Window_print__aLjava_lang_String_I";

const char windowSetNameName[56] = "Java_xeno_util_Window_setName__Ljava_lang_String_";

const char windowCloseWaitKeyName[40] = "Java_xeno_util_Window_closeWaitKey__";

const char menuAddQueryStringName[56] = "Java_xeno_util_Menu_addQuery__Ljava_lang_String_";

const char menuAddQueryStringArrayIntName[56] = "Java_xeno_util_Menu_addQuery__aLjava_lang_String_I";

const char menuAddItemName[48] = "Java_xeno_util_Menu_addItem__Ljava_lang_String_";

const char menuCreateName[32] = "Java_xeno_util_Menu_create__";

const char menuGetSelectedName[40] = "Java_xeno_util_Menu_getSelected__";

const char menuSetLocationName[40] = "Java_xeno_util_Menu_setLocation__II";

const char menuSetVisibleName[40] = "Java_xeno_util_Menu_setVisible__Z";

const char menuSetCursorName[40] = "Java_xeno_util_Menu_setCursor__I";

const char toolkitGetPeerName[56] = "Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_";

const char toolkitCallName[72] = "Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_";

const char toolkitLoadResourceObjectIntName[64] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I";

const char toolkitLoadResourceStringName[56] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_";

const char toolkitLoadResourceObjectObjectIntName[80] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I";

const char toolkitLoadResourceObjectIntIntName[64] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II";

const char toolkitPeerSetGroupName[40] = "Java_xeno_util_Toolkit_peerSetGroup__II";

const char soundSequencePlayIntName[32] = "Java_xeno_Sound_sequencePlay__I";

const char soundSequencePlayIntIntName[40] = "Java_xeno_Sound_sequencePlay__II";

const char soundSequenceStopIntName[32] = "Java_xeno_Sound_sequenceStop__I";

const char soundSequenceStopIntIntName[40] = "Java_xeno_Sound_sequenceStop__II";

const char soundEffectPlayName[32] = "Java_xeno_Sound_effectPlay__III";

const char soundEffectStopName[32] = "Java_xeno_Sound_effectStop__I";

const char soundStreamPlayName[40] = "Java_xeno_Sound_streamPlay__IIII";

const char movieInitName[24] = "Java_xeno_Movie_init__I";

const char movieStartName[32] = "Java_xeno_Movie_start__I";

const char movieStopName[24] = "Java_xeno_Movie_stop__";

const char movieUpdateName[32] = "Java_xeno_Movie_update__";

const char playControlCreateName[32] = "Java_xeno_PlayControl_create__";

const char playControlGetSignalName[40] = "Java_xeno_PlayControl_getSignal__";

const char playControlSignalName[32] = "Java_xeno_PlayControl_signal__I";

const char playControlInitIntIntName[32] = "Java_xeno_PlayControl_init__II";

const char playControlInitIntIntIntIntFloatName[40] = "Java_xeno_PlayControl_init__IIIIF";

const char playControlLoadCameraObjName[56] = "Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_";

const char playControlLoadTimeChartObjName[56] = "Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_";

const char playControlStartName[32] = "Java_xeno_PlayControl_start__";

const char playControlStopName[32] = "Java_xeno_PlayControl_stop__";

const char playControlSetObserverIIObjStrName[80] = "Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_";

const char playControlSetObserverIStrObjStrName[96] = "Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_";

const char playControlGetParamsName[40] = "Java_xeno_PlayControl_getParams__";

const char effectCallIName[32] = "Java_xeno_Effect_call__I";

const char effectDispZName[32] = "Java_xeno_Effect_disp__Z";

const char effectSetScaleFFFName[32] = "Java_xeno_Effect_setScale__FFF";

const char effectGetScaleName[32] = "Java_xeno_Effect_getScale__";

const char effectGetTranslateName[32] = "Java_xeno_Effect_getTranslate__";

const char effectSetTranslateName[32] = "Java_xeno_Effect_setTranslate__";

const char effectGetRotateName[32] = "Java_xeno_Effect_getRotate__";

const char effectSetRotateName[32] = "Java_xeno_Effect_setRotate__";

const char effectSetCasterChrName[40] = "Java_xeno_Effect_setCaster__Lxeno_Chr_";

const char effectSetTargetChrName[40] = "Java_xeno_Effect_setTarget__Lxeno_Chr_";

const char effectSetCasterUnitName[40] = "Java_xeno_Effect_setCaster__Lxeno_Unit_";

const char effectSetTargetUnitName[40] = "Java_xeno_Effect_setTarget__Lxeno_Unit_";

const char effectSetTransOffsetFFFName[40] = "Java_xeno_Effect_setTransOffset__FFF";

const char effectGetForceLoopName[32] = "Java_xeno_Effect_getForceLoop__";

const char effectSetForceLoopZName[40] = "Java_xeno_Effect_setForceLoop__Z";

const char effectGetClipName[32] = "Java_xeno_Effect_getClip__";

const char effectSetClipZName[32] = "Java_xeno_Effect_setClip__Z";

const char effectClearEffectName[32] = "Java_xeno_Effect_clearEffect__";

const char effectSetMotionZName[32] = "Java_xeno_Effect_setMotion__Z";

const char effectNoAttachZName[32] = "Java_xeno_Effect_noAttach__Z";

const char cameraGetRotateXName[32] = "Java_xeno_Camera_getRotateX__";

const char cameraGetRotateYName[32] = "Java_xeno_Camera_getRotateY__";

const char cameraGetRotateZName[32] = "Java_xeno_Camera_getRotateZ__";

const char cameraGetTranslateXName[40] = "Java_xeno_Camera_getTranslateX__";

const char cameraGetTranslateYName[40] = "Java_xeno_Camera_getTranslateY__";

const char cameraGetTranslateZName[40] = "Java_xeno_Camera_getTranslateZ__";

const char cameraFovSPLArrFIName[32] = "Java_xeno_Camera_fovSPL__aFI";

const char cameraChangeName[32] = "Java_xeno_Camera_change__";

const char cameraRotateSPLArrFIName[32] = "Java_xeno_Camera_rotateSPL__aFI";

const char cameraRotateSPLArrFIIIName[40] = "Java_xeno_Camera_rotateSPL__aFIII";

const char cameraSetActiveZName[32] = "Java_xeno_Camera_setActive__Z";

const char cameraCreateIName[32] = "Java_xeno_Camera_create__I";

const char cameraTransCNSObjFFFName[56] = "Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF";

const char cameraTransSPLArrFIName[32] = "Java_xeno_Camera_transSPL__aFI";

const char cameraTransSPLArrFIIIName[40] = "Java_xeno_Camera_transSPL__aFIII";

const char cameraViewCNSObjFFFName[48] = "Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF";

const char cameraViewSPLArrFIName[32] = "Java_xeno_Camera_viewSPL__aFI";

const char cameraViewSPLArrFIIIName[32] = "Java_xeno_Camera_viewSPL__aFIII";

const char cameraRollSPLArrFIName[32] = "Java_xeno_Camera_rollSPL__aFI";

const char cameraSetRotateFFFName[32] = "Java_xeno_Camera_setRotate__FFF";

const char cameraSetRollFName[32] = "Java_xeno_Camera_setRoll__F";

const char cameraSetTranslateFFFName[40] = "Java_xeno_Camera_setTranslate__FFF";

const char cameraSetFovFName[32] = "Java_xeno_Camera_setFov__F";

const char cameraGetFovName[32] = "Java_xeno_Camera_getFov__";

const char cameraSetViewFFFName[32] = "Java_xeno_Camera_setView__FFF";

const char cameraStartIObjName[48] = "Java_xeno_Camera_start__ILjava_lang_Object_";

const char cameraSetCFAngleIFFFFName[40] = "Java_xeno_Camera_setCFAngle__IFFFF";

const char cameraSetCFAnglePersIFFFFFName[40] = "Java_xeno_Camera_setCFAnglePers__IFFFFF";

const char cameraSetCFHokanIFFName[40] = "Java_xeno_Camera_setCFHokan__IFF";

const char cameraSetCFLockIIFFFName[40] = "Java_xeno_Camera_setCFLock__IIFFF";

const char cameraSetCFOffsetIFFFIFIFName[40] = "Java_xeno_Camera_setCFOffset__IFFFIFIF";

const char cameraSetModeIName[32] = "Java_xeno_Camera_setMode__I";

const char cameraGetModeName[32] = "Java_xeno_Camera_getMode__";

const char cameraSetCFPedestalIFFFFFFFFName[48] = "Java_xeno_Camera_setCFPedestal__IFFFFFFFF";

const char cameraSetCFPedestalHokanIIName[40] = "Java_xeno_Camera_setCFPedestalHokan__II";

const char cameraChangeIDIIIName[32] = "Java_xeno_Camera_changeID__III";

const char cameraSetFogIFFFFIIIIName[40] = "Java_xeno_Camera_setFog__IFFFFIIII";

const char cameraResetFogIName[32] = "Java_xeno_Camera_resetFog__I";

const char cameraSetClipRangeFFName[40] = "Java_xeno_Camera_setClipRange__FF";

const char lightSetColorFFFName[32] = "Java_xeno_Light_setColor__FFF";

const char lightSetDirectionFFFName[40] = "Java_xeno_Light_setDirection__FFF";

const char lightSetDirection2FFFName[40] = "Java_xeno_Light_setDirection2__FFF";

const char lightSetGlobalPointLightColIFFFName[48] = "Java_xeno_Light_setGlobalPointLightCol__IFFF";

const char lightSetGlobalPointLightPosIFFFName[48] = "Java_xeno_Light_setGlobalPointLightPos__IFFF";

const char lightSetGlobalPointLightResetName[48] = "Java_xeno_Light_setGlobalPointLightReset__";

const char chrGetPlayerName[32] = "Java_xeno_Chr_getPlayer__";

const char chrSetPlayerName[32] = "Java_xeno_Chr_setPlayer__";

const char chrMoveIFFZName[32] = "Java_xeno_Chr_move__IFFZ";

const char chrMoveFFFZName[32] = "Java_xeno_Chr_move__FFFZ";

const char chrMoveIObjZName[48] = "Java_xeno_Chr_move__ILjava_lang_Object_Z";

const char chrMoveObjFZName[48] = "Java_xeno_Chr_move__Ljava_lang_Object_FZ";

const char chrMoveSplineIZName[48] = "Java_xeno_Chr_move__Lxeno_util_Spline_IZ";

const char chrRotateSplineIZName[48] = "Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ";

const char chrRotXFFZName[24] = "Java_xeno_Chr_rotX__FFZ";

const char chrRotXIFZName[24] = "Java_xeno_Chr_rotX__IFZ";

const char chrRotXIObjZName[48] = "Java_xeno_Chr_rotX__ILjava_lang_Object_Z";

const char chrRotXObjFZName[48] = "Java_xeno_Chr_rotX__Ljava_lang_Object_FZ";

const char chrRotYFFZName[24] = "Java_xeno_Chr_rotY__FFZ";

const char chrRotYIFZName[24] = "Java_xeno_Chr_rotY__IFZ";

const char chrRotYIObjZName[48] = "Java_xeno_Chr_rotY__ILjava_lang_Object_Z";

const char chrRotYObjFZName[48] = "Java_xeno_Chr_rotY__Ljava_lang_Object_FZ";

const char chrRotZFFZName[24] = "Java_xeno_Chr_rotZ__FFZ";

const char chrRotZIFZName[24] = "Java_xeno_Chr_rotZ__IFZ";

const char chrRotZIObjZName[48] = "Java_xeno_Chr_rotZ__ILjava_lang_Object_Z";

const char chrRotZObjFZName[48] = "Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ";

const char chrStartIObjName[48] = "Java_xeno_Chr_start__ILjava_lang_Object_";

const char chrStopName[24] = "Java_xeno_Chr_stop__";

const char chrMtnIIIIIFZName[32] = "Java_xeno_Chr_mtn__IIIIIFZ";

const char chrMtnIIFZName[24] = "Java_xeno_Chr_mtn__IIFZ";

const char chrSignalIName[24] = "Java_xeno_Chr_signal__I";

const char chrGetSignalName[32] = "Java_xeno_Chr_getSignal__";

const char chrSetRotateName[32] = "Java_xeno_Chr_setRotate__";

const char chrSetTranslateName[32] = "Java_xeno_Chr_setTranslate__";

const char chrGetRotateName[32] = "Java_xeno_Chr_getRotate__";

const char chrGetTranslateName[32] = "Java_xeno_Chr_getTranslate__";

const char chrSetVisibleZName[32] = "Java_xeno_Chr_setVisible__Z";

const char chrSetVisibleIZName[32] = "Java_xeno_Chr_setVisible__IZ";

const char chrSetCollisionZName[32] = "Java_xeno_Chr_setCollision__Z";

const char chrSetHandIName[32] = "Java_xeno_Chr_setHand__I";

const char chrSetArgsIIIName[32] = "Java_xeno_Chr_setArgs__III";

const char chrGetArgsIIName[32] = "Java_xeno_Chr_getArgs__II";

const char chrSetArgsIObjIName[48] = "Java_xeno_Chr_setArgs__ILjava_lang_Object_I";

const char chrGetSerialName[32] = "Java_xeno_Chr_getSerial__";

const char chrGetStateName[32] = "Java_xeno_Chr_getState__";

const char chrMtnGetRootIVec4fName[48] = "Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_";

const char chrSetScaleFFFName[32] = "Java_xeno_Chr_setScale__FFF";

const char chrGetScaleName[32] = "Java_xeno_Chr_getScale__";

const char chrScaleSplineIZName[48] = "Java_xeno_Chr_scale__Lxeno_util_Spline_IZ";

const char chrSclXFFZName[24] = "Java_xeno_Chr_sclX__FFZ";

const char chrSclXIFZName[24] = "Java_xeno_Chr_sclX__IFZ";

const char chrSclYFFZName[24] = "Java_xeno_Chr_sclY__FFZ";

const char chrSclYIFZName[24] = "Java_xeno_Chr_sclY__IFZ";

const char chrSclZFFZName[24] = "Java_xeno_Chr_sclZ__FFZ";

const char chrSclZIFZName[24] = "Java_xeno_Chr_sclZ__IFZ";

const char chrRotCNSIObjName[48] = "Java_xeno_Chr_rotCNS__ILjava_lang_Object_";

const char chrRotCNSIFFFName[32] = "Java_xeno_Chr_rotCNS__IFFF";

const char chrSetRotCNSParamIFFFFFName[40] = "Java_xeno_Chr_setRotCNSParam__IFFFFF";

const char chrRelaxIIName[24] = "Java_xeno_Chr_relax__II";

const char chrGetFlagsName[32] = "Java_xeno_Chr_getFlags__";

const char chrSetFlagsIName[32] = "Java_xeno_Chr_setFlags__I";

const char chrSetEdgeFallIName[32] = "Java_xeno_Chr_setEdgeFall__I";

const char chrSetShadowIIName[32] = "Java_xeno_Chr_setShadow__II";

const char chrSetShadowArrBName[32] = "Java_xeno_Chr_setShadow__aB";

const char chrSetIDName[24] = "Java_xeno_Chr_setID__I";

const char chrSetElevatorModeName[40] = "Java_xeno_Chr_setElevatorMode__I";

const char chrSetParentName[40] = "Java_xeno_Chr_setParent__Lxeno_Chr_III";

const char chrSetMotionFlagsName[40] = "Java_xeno_Chr_setMotionFlags__IZ";

const char chrSetFilterName[32] = "Java_xeno_Chr_setFilter__I";

const char chrSetFilterParamName[40] = "Java_xeno_Chr_setFilterParam__aF";

const char chrSetClipName[32] = "Java_xeno_Chr_setClip__I";

const char chrSetSymmetryYName[32] = "Java_xeno_Chr_setSymmetryY__I";

const char chrSetSortOffsetName[32] = "Java_xeno_Chr_setSortOffset__F";

const char chrSetPointLightColName[40] = "Java_xeno_Chr_setPointLightCol__IFFF";

const char chrSetPointLightPosName[40] = "Java_xeno_Chr_setPointLightPos__IFFF";

const char chrSetPointLightResetName[40] = "Java_xeno_Chr_setPointLightReset__";

const char chrTalktoName[48] = "Java_xeno_Chr_talkto__Ljava_lang_String_";

const char chrTouchtoName[48] = "Java_xeno_Chr_touchto__Ljava_lang_String_";

const char chrChildGetPeerName[32] = "Java_xeno_Chr_childGetPeer__II";

const char chrSetPeerName[48] = "Java_xeno_Chr_setPeer__Ljava_lang_Object_";

const char chrDispRadarName[32] = "Java_xeno_Chr_dispRadar__Z";

const char chrLookCameraName[32] = "Java_xeno_Chr_look_camera__";

const char chrLookCharName[48] = "Java_xeno_Chr_look_char__Ljava_lang_Object_";

const char chrLookUnitName[48] = "Java_xeno_Chr_look_unit__Ljava_lang_Object_";

const char chrLookDefaultName[32] = "Java_xeno_Chr_look_default__";

const char chrLookPointName[32] = "Java_xeno_Chr_look_point__FFF";

const char chrLookEyeSetName[32] = "Java_xeno_Chr_look_eye_set__FF";

const char chrLookEyeControlName[40] = "Java_xeno_Chr_look_eye_control__I";

const char chrLookSpeedName[32] = "Java_xeno_Chr_look_speed__F";

const char chrLookEyeSpeedName[32] = "Java_xeno_Chr_look_eye_speed__F";

const char chrRenderCommandName[32] = "Java_xeno_Chr_renderCommand__I";

const char chrShadowClipScaleName[40] = "Java_xeno_Chr_shadow_clip_scale__F";

const char chrShadowMapIdName[32] = "Java_xeno_Chr_shadow_map_id__I";

const char chrShadowMapResetName[40] = "Java_xeno_Chr_shadow_map_reset__";

const char chrHairStopName[32] = "Java_xeno_Chr_hairStop__II";

const char chrPixelAlphaName[32] = "Java_xeno_Chr_pixelAlpha__I";

const char chrPixelAlphaPartsName[40] = "Java_xeno_Chr_pixelAlphaParts__II";

const char chrPixelAlphaPartsResetName[40] = "Java_xeno_Chr_pixelAlphaPartsReset__";

const char chrSetMotNoUpdateName[32] = "Java_xeno_Chr_setMotNoUpdate__I";

const char chrSetWeaponRName[40] = "Java_xeno_Chr_setWeaponR__Lxeno_Chr_";

const char chrResetWeaponRName[40] = "Java_xeno_Chr_resetWeaponR__Lxeno_Chr_";

const char chrResetHandName[32] = "Java_xeno_Chr_resetHand__";

const char chrResetEnvName[32] = "Java_xeno_Chr_resetEnv__";

const char chrIgnoreShapeName[32] = "Java_xeno_Chr_ignoreShape__I";

const char unitRotYCNSObjIntFloatFloatFloatName[48] = "Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF";

const char unitTransCNSObjIntFloatFloatFloatName[48] = "Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF";

const char unitGetRotateName[32] = "Java_xeno_Unit_getRotate__";

const char unitGetSignalName[32] = "Java_xeno_Unit_getSignal__";

const char unitGetTranslateName[32] = "Java_xeno_Unit_getTranslate__";

const char unitInvalidateName[32] = "Java_xeno_Unit_invalidate__";

const char unitMoveFloatFloatFloatBoolName[32] = "Java_xeno_Unit_move__FFFZ";

const char unitMoveIntFloatFloatBoolName[32] = "Java_xeno_Unit_move__IFFZ";

const char unitMoveSplineIntBoolName[48] = "Java_xeno_Unit_move__Lxeno_util_Spline_IZ";

const char unitRotateSplineIntBoolName[48] = "Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ";

const char unitMoveIntObjBoolName[48] = "Java_xeno_Unit_move__ILjava_lang_Object_Z";

const char unitMoveObjFloatBoolName[48] = "Java_xeno_Unit_move__Ljava_lang_Object_FZ";

const char unitMtnIntIntFloatBoolName[32] = "Java_xeno_Unit_mtn__IIFZ";

const char unitMtnIntIntIntIntIntFloatBoolName[32] = "Java_xeno_Unit_mtn__IIIIIFZ";

const char unitRotXFloatFloatBoolName[32] = "Java_xeno_Unit_rotX__FFZ";

const char unitRotXIntFloatBoolName[32] = "Java_xeno_Unit_rotX__IFZ";

const char unitRotXIntObjBoolName[48] = "Java_xeno_Unit_rotX__ILjava_lang_Object_Z";

const char unitRotXObjFloatBoolName[48] = "Java_xeno_Unit_rotX__Ljava_lang_Object_FZ";

const char unitRotYFloatFloatBoolName[32] = "Java_xeno_Unit_rotY__FFZ";

const char unitRotYIntFloatBoolName[32] = "Java_xeno_Unit_rotY__IFZ";

const char unitRotYIntObjBoolName[48] = "Java_xeno_Unit_rotY__ILjava_lang_Object_Z";

const char unitRotYObjFloatBoolName[48] = "Java_xeno_Unit_rotY__Ljava_lang_Object_FZ";

const char unitRotZFloatFloatBoolName[32] = "Java_xeno_Unit_rotZ__FFZ";

const char unitRotZIntFloatBoolName[32] = "Java_xeno_Unit_rotZ__IFZ";

const char unitRotZIntObjBoolName[48] = "Java_xeno_Unit_rotZ__ILjava_lang_Object_Z";

const char unitRotZObjFloatBoolName[48] = "Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ";

const char unitScaleSplineIntBoolName[48] = "Java_xeno_Unit_scale__Lxeno_util_Spline_IZ";

const char unitSclXFloatFloatBoolName[32] = "Java_xeno_Unit_sclX__FFZ";

const char unitSclXIntFloatBoolName[32] = "Java_xeno_Unit_sclX__IFZ";

const char unitSclYFloatFloatBoolName[32] = "Java_xeno_Unit_sclY__FFZ";

const char unitSclYIntFloatBoolName[32] = "Java_xeno_Unit_sclY__IFZ";

const char unitSclZFloatFloatBoolName[32] = "Java_xeno_Unit_sclZ__FFZ";

const char unitSclZIntFloatBoolName[32] = "Java_xeno_Unit_sclZ__IFZ";

const char unitSetCollisionName[32] = "Java_xeno_Unit_setCollision__Z";

const char unitSetRotateName[32] = "Java_xeno_Unit_setRotate__";

const char unitSetTranslateName[32] = "Java_xeno_Unit_setTranslate__";

const char unitSetVisibleIntBoolName[32] = "Java_xeno_Unit_setVisible__IZ";

const char unitSetVisibleBoolName[32] = "Java_xeno_Unit_setVisible__Z";

const char unitSignalName[32] = "Java_xeno_Unit_signal__I";

const char unitStartName[48] = "Java_xeno_Unit_start__ILjava_lang_Object_";

const char unitStopName[24] = "Java_xeno_Unit_stop__";

const char unitValidateName[32] = "Java_xeno_Unit_validate__";

const char unitSetParentName[48] = "Java_xeno_Unit_setParent__Ljava_lang_Object_I";

const char unitSetArgsIntIntIntName[32] = "Java_xeno_Unit_setArgs__III";

const char unitGetArgsName[32] = "Java_xeno_Unit_getArgs__II";

const char unitSetArgsIntObjIntName[48] = "Java_xeno_Unit_setArgs__ILjava_lang_Object_I";

const char unitSetArgsIntIntIntIntIntName[32] = "Java_xeno_Unit_setArgs__IIIII";

const char unitGetScaleName[32] = "Java_xeno_Unit_getScale__";

const char unitSetScaleName[32] = "Java_xeno_Unit_setScale__FFF";

const char unitGetSerialName[32] = "Java_xeno_Unit_getSerial__";

const char unitMtnSetMaskName[32] = "Java_xeno_Unit_mtnSetMask__I";

const char unitMtnGetRootName[56] = "Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_";

const char unitGetStateName[32] = "Java_xeno_Unit_getState__";

const char unitGetPivotName[48] = "Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_";

const char unitSetPivotName[32] = "Java_xeno_Unit_setPivot__FFF";

const char unitGetAxisName[48] = "Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_";

const char unitSetAxisName[32] = "Java_xeno_Unit_setAxis__FFFF";

const char unitSuspendName[32] = "Java_xeno_Unit_suspend__I";

const char unitResumeName[32] = "Java_xeno_Unit_resume__I";

const char unitInitElevatorFuncName[40] = "Java_xeno_Unit_initElevatorFunc__";

const char unitMapShadowName[32] = "Java_xeno_Unit_map_shadow__I";

const char unitRenderCommandName[32] = "Java_xeno_Unit_renderCommand__I";

const char unitSetFilterName[32] = "Java_xeno_Unit_setFilter__I";

const char unitSetFilterParamName[40] = "Java_xeno_Unit_setFilterParam__aF";

const char unitSetShadowName[32] = "Java_xeno_Unit_setShadow__II";

const char unitShadowClipScaleName[40] = "Java_xeno_Unit_shadow_clip_scale__F";

const char unitShadowMapIdName[32] = "Java_xeno_Unit_shadow_map_id__I";

const char unitShadowMapResetName[40] = "Java_xeno_Unit_shadow_map_reset__";

const char unitSetSortOffsetName[32] = "Java_xeno_Unit_setSortOffset__F";

const char unitSetClipName[32] = "Java_xeno_Unit_setClip__I";

const char unitSetMonitorPrioName[40] = "Java_xeno_Unit_setMonitorPrio__I";

const char sceneStartName[48] = "Java_xeno_Scene_start__ILjava_lang_Object_";

const char sceneStopName[24] = "Java_xeno_Scene_stop__";

const char stageStartName[48] = "Java_xeno_Stage_start__ILjava_lang_Object_";

const char stageStopName[24] = "Java_xeno_Stage_stop__";

const char stagePlayName[48] = "Java_xeno_Stage_play__Ljava_lang_String_";

const char stageSetPartsLastName[32] = "Java_xeno_Stage_setPartsLast__I";

const char stageSetPartsLastResetName[40] = "Java_xeno_Stage_setPartsLastReset__";

const char stageSetColorName[32] = "Java_xeno_Stage_setColor__FFF";

const char stageSetFadeName[32] = "Java_xeno_Stage_setFade__IIFFF";

const char stageSetEventFadeName[40] = "Java_xeno_Stage_setEventFade__IFFFIFFF";

const char stageSetFrameRenderName[40] = "Java_xeno_Stage_setFrameRender__II";

const char stageSetFadeCancelName[40] = "Java_xeno_Stage_setFadeCancel__I";

const char stageSetVisibleName[32] = "Java_xeno_Stage_setVisible__IZ";

const char stageSetCfbgName[32] = "Java_xeno_Stage_setCFBG__II_F";

const char stageSetEffectRenderName[40] = "Java_xeno_Stage_setEffectRender__I";

const char stageRenderCommandName[40] = "Java_xeno_Stage_renderCommand__I";

const char stageSetBgColorName[32] = "Java_xeno_Stage_setBgColor__FFF";

const char stageSetBgClipName[32] = "Java_xeno_Stage_setBgClip__I";

const char stageClrBackBufferName[32] = "Java_xeno_Stage_clrBackBuffer__";

extern void Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II();

extern void Java_xeno_vm_System_sleep__I();

extern void Java_xeno_vm_System_println__Ljava_lang_String_();

extern void Java_xeno_vm_System_waitFor__Ljava_lang_Object_();

extern void Java_xeno_vm_System_methodSignal__I();

extern void Java_xeno_vm_Thread_create__();

extern void Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_();

extern void Java_xeno_vm_Thread_start__();

extern void Java_xeno_vm_Thread_stop__();

extern void Java_xeno_vm_Math_random__();

extern void Java_xeno_vm_Math_atan2__FF();

extern void Java_xeno_vm_Math_cos__F();

extern void Java_xeno_vm_Math_sin__F();

extern void Java_xeno_util_Spline_create__();

extern void Java_xeno_util_Spline_setCtrlVertex__aFIII();

extern void Java_xeno_util_Spline_getValue__I();

extern void Java_xeno_util_Format_floatToIntBits__F();

extern void Java_xeno_util_Format_intBitsToFloat__I();

extern void Java_xeno_util_Format_toInt__Ljava_lang_String_();

extern void Java_xeno_util_Format_toString__C();

extern void Java_xeno_util_Format_toString__F();

extern void Java_xeno_util_Format_toString__I();

extern void Java_xeno_util_Format_toString__Z();

extern void Java_xeno_util_Input_create__I();

extern void Java_xeno_util_Input_getButton__();

extern void Java_xeno_util_Input_getEdge__();

extern void Java_xeno_util_Input_getRepeat__();

extern void Java_xeno_util_Layout_set__Ljava_lang_Object_I();

extern void Java_xeno_util_Layout_getManager__I();

extern void Java_xeno_util_Runtime_execBattle__II();

extern void Java_xeno_util_Runtime_getFlags__II();

extern void Java_xeno_util_Runtime_setFlags__III();

extern void Java_xeno_util_Runtime_setLocation__III();

extern void Java_xeno_util_Runtime_getLocation__();

extern void Java_xeno_util_Runtime_setRegister__II();

extern void Java_xeno_util_Runtime_getRegister__I();

extern void Java_xeno_util_Runtime_getEntrance__();

extern void Java_xeno_util_Runtime_setPlayerControl__Z();

extern void Java_xeno_util_Runtime_setPlayerMoveParam__FFF();

extern void Java_xeno_util_Runtime_disable__I();

extern void Java_xeno_util_Runtime_enable__I();

extern void Java_xeno_util_Runtime_getGameState__();

extern void Java_xeno_util_Runtime_CaptureEnd__();

extern void Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I();

extern void Java_xeno_util_Runtime_jumpCF__II();

extern void Java_xeno_util_Runtime_jumpEvent__I();

extern void Java_xeno_util_Runtime_setDefocusQuick__IIII();

extern void Java_xeno_util_Runtime_setDefocus__IIaI();

extern void Java_xeno_util_Runtime_setDefocusParam__III();

extern void Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I();

extern void Java_xeno_util_Runtime_setMap__II();

extern void Java_xeno_util_Runtime_setShootFlag__Z();

extern void Java_xeno_util_Runtime_setShootHeightCheck__Z();

extern void Java_xeno_util_Runtime_setShootIDCheck__Z();

extern void Java_xeno_util_Runtime_setShootUwaCheck__Z();

extern void Java_xeno_util_Runtime_setShootRange__F();

extern void Java_xeno_util_Runtime_addItem__II();

extern void Java_xeno_util_Runtime_addItemWin__II();

extern void Java_xeno_util_Runtime_removeItem__II();

extern void Java_xeno_util_Runtime_checkItem__II();

extern void Java_xeno_util_Runtime_getItemName__II();

extern void Java_xeno_util_Runtime_addGold__I();

extern void Java_xeno_util_Runtime_removeGold__I();

extern void Java_xeno_util_Runtime_checkGold__();

extern void Java_xeno_util_Runtime_progressEffect__I();

extern void Java_xeno_util_Runtime_enterShop__I();

extern void Java_xeno_util_Runtime_getLeader__();

extern void Java_xeno_util_Runtime_getPartyData__I();

extern void Java_xeno_util_Runtime_setPartyData__II();

extern void Java_xeno_util_Runtime_setFriend__I();

extern void Java_xeno_util_Runtime_resetFriend__I();

extern void Java_xeno_util_Runtime_checkFriend__I();

extern void Java_xeno_util_Runtime_setLockParty__I();

extern void Java_xeno_util_Runtime_resetLockParty__I();

extern void Java_xeno_util_Runtime_checkLockParty__I();

extern void Java_xeno_util_Runtime_setOutFriend__I();

extern void Java_xeno_util_Runtime_resetOutFriend__I();

extern void Java_xeno_util_Runtime_checkOutFriend__I();

extern void Java_xeno_util_Runtime_setTakeAgws__I();

extern void Java_xeno_util_Runtime_resetTakeAgws__I();

extern void Java_xeno_util_Runtime_checkTakeAgws__I();

extern void Java_xeno_util_Runtime_battleChangeParty__I();

extern void Java_xeno_util_Runtime_setIdLightCol__IIFFF();

extern void Java_xeno_util_Runtime_setIdLightDir__IIFFF();

extern void Java_xeno_util_Runtime_setIdLightVec__IIFFF();

extern void Java_xeno_util_Runtime_setWindParam__IFFFF();

extern void Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_();

extern void Java_xeno_util_Runtime_mpeg2AfterCrossFade__I();

extern void Java_xeno_util_Runtime_mailFlag__I();

extern void Java_xeno_util_Runtime_mailExec__I();

extern void Java_xeno_util_Runtime_minigameExec__I();

extern void Java_xeno_util_Runtime_setMenuLock__();

extern void Java_xeno_util_Runtime_evsSetRetPoint__();

extern void Java_xeno_util_Runtime_evsExit__();

extern void Java_xeno_util_Runtime_etherTecSet__I();

extern void Java_xeno_util_Runtime_charAllRecovery__();

extern void Java_xeno_util_Runtime_AGWSAllRecovery__();

extern void Java_xeno_util_Window_getSignal__();

extern void Java_xeno_util_Window_signal__I();

extern void Java_xeno_util_Window_clear__();

extern void Java_xeno_util_Window_close__();

extern void Java_xeno_util_Window_waitkey__I();

extern void Java_xeno_util_Window_wait__I();

extern void Java_xeno_util_Window_create__I();

extern void Java_xeno_util_Window_setSize__II();

extern void Java_xeno_util_Window_setLocation__II();

extern void Java_xeno_util_Window_print__Ljava_lang_String_();

extern void Java_xeno_util_Window_print__aLjava_lang_String_I();

extern void Java_xeno_util_Window_setName__Ljava_lang_String_();

extern void Java_xeno_util_Window_closeWaitKey__();

extern void Java_xeno_util_Menu_addQuery__Ljava_lang_String_();

extern void Java_xeno_util_Menu_addQuery__aLjava_lang_String_I();

extern void Java_xeno_util_Menu_addItem__Ljava_lang_String_();

extern void Java_xeno_util_Menu_create__();

extern void Java_xeno_util_Menu_getSelected__();

extern void Java_xeno_util_Menu_setLocation__II();

extern void Java_xeno_util_Menu_setVisible__Z();

extern void Java_xeno_util_Menu_setCursor__I();

extern void Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_();

extern void Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_();

extern void Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I();

extern void Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_();

extern void Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I();

extern void Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II();

extern void Java_xeno_util_Toolkit_peerSetGroup__II();

extern void Java_xeno_Sound_sequencePlay__I();

extern void Java_xeno_Sound_sequencePlay__II();

extern void Java_xeno_Sound_sequenceStop__I();

extern void Java_xeno_Sound_sequenceStop__II();

extern void Java_xeno_Sound_effectPlay__III();

extern void Java_xeno_Sound_effectStop__I();

extern void Java_xeno_Sound_streamPlay__IIII();

extern void Java_xeno_Movie_init__I();

extern void Java_xeno_Movie_start__I();

extern void Java_xeno_Movie_stop__();

extern void Java_xeno_Movie_update__();

extern void Java_xeno_PlayControl_create__();

extern void Java_xeno_PlayControl_getSignal__();

extern void Java_xeno_PlayControl_signal__I();

extern void Java_xeno_PlayControl_init__II();

extern void Java_xeno_PlayControl_init__IIIIF();

extern void Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_();

extern void Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_();

extern void Java_xeno_PlayControl_start__();

extern void Java_xeno_PlayControl_stop__();

extern void Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_();

extern void Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_();

extern void Java_xeno_PlayControl_getParams__();

extern void Java_xeno_Effect_call__I();

extern void Java_xeno_Effect_disp__Z();

extern void Java_xeno_Effect_setScale__FFF();

extern void Java_xeno_Effect_getScale__();

extern void Java_xeno_Effect_getTranslate__();

extern void Java_xeno_Effect_setTranslate__();

extern void Java_xeno_Effect_getRotate__();

extern void Java_xeno_Effect_setRotate__();

extern void Java_xeno_Effect_setCaster__Lxeno_Chr_();

extern void Java_xeno_Effect_setTarget__Lxeno_Chr_();

extern void Java_xeno_Effect_setCaster__Lxeno_Unit_();

extern void Java_xeno_Effect_setTarget__Lxeno_Unit_();

extern void Java_xeno_Effect_setTransOffset__FFF();

extern void Java_xeno_Effect_getForceLoop__();

extern void Java_xeno_Effect_setForceLoop__Z();

extern void Java_xeno_Effect_getClip__();

extern void Java_xeno_Effect_setClip__Z();

extern void Java_xeno_Effect_clearEffect__();

extern void Java_xeno_Effect_setMotion__Z();

extern void Java_xeno_Effect_noAttach__Z();

extern void Java_xeno_Camera_getRotateX__();

extern void Java_xeno_Camera_getRotateY__();

extern void Java_xeno_Camera_getRotateZ__();

extern void Java_xeno_Camera_getTranslateX__();

extern void Java_xeno_Camera_getTranslateY__();

extern void Java_xeno_Camera_getTranslateZ__();

extern void Java_xeno_Camera_fovSPL__aFI();

extern void Java_xeno_Camera_change__();

extern void Java_xeno_Camera_rotateSPL__aFI();

extern void Java_xeno_Camera_rotateSPL__aFIII();

extern void Java_xeno_Camera_setActive__Z();

extern void Java_xeno_Camera_create__I();

extern void Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF();

extern void Java_xeno_Camera_transSPL__aFI();

extern void Java_xeno_Camera_transSPL__aFIII();

extern void Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF();

extern void Java_xeno_Camera_viewSPL__aFI();

extern void Java_xeno_Camera_viewSPL__aFIII();

extern void Java_xeno_Camera_rollSPL__aFI();

extern void Java_xeno_Camera_setRotate__FFF();

extern void Java_xeno_Camera_setRoll__F();

extern void Java_xeno_Camera_setTranslate__FFF();

extern void Java_xeno_Camera_setFov__F();

extern void Java_xeno_Camera_getFov__();

extern void Java_xeno_Camera_setView__FFF();

extern void Java_xeno_Camera_start__ILjava_lang_Object_();

extern void Java_xeno_Camera_setCFAngle__IFFFF();

extern void Java_xeno_Camera_setCFAnglePers__IFFFFF();

extern void Java_xeno_Camera_setCFHokan__IFF();

extern void Java_xeno_Camera_setCFLock__IIFFF();

extern void Java_xeno_Camera_setCFOffset__IFFFIFIF();

extern void Java_xeno_Camera_setMode__I();

extern void Java_xeno_Camera_getMode__();

extern void Java_xeno_Camera_setCFPedestal__IFFFFFFFF();

extern void Java_xeno_Camera_setCFPedestalHokan__II();

extern void Java_xeno_Camera_changeID__III();

extern void Java_xeno_Camera_setFog__IFFFFIIII();

extern void Java_xeno_Camera_resetFog__I();

extern void Java_xeno_Camera_setClipRange__FF();

extern void Java_xeno_Light_setColor__FFF();

extern void Java_xeno_Light_setDirection__FFF();

extern void Java_xeno_Light_setDirection2__FFF();

extern void Java_xeno_Light_setGlobalPointLightCol__IFFF();

extern void Java_xeno_Light_setGlobalPointLightPos__IFFF();

extern void Java_xeno_Light_setGlobalPointLightReset__();

extern void Java_xeno_Chr_getPlayer__();

extern void Java_xeno_Chr_setPlayer__();

extern void Java_xeno_Chr_move__IFFZ();

extern void Java_xeno_Chr_move__FFFZ();

extern void Java_xeno_Chr_move__ILjava_lang_Object_Z();

extern void Java_xeno_Chr_move__Ljava_lang_Object_FZ();

extern void Java_xeno_Chr_move__Lxeno_util_Spline_IZ();

extern void Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ();

extern void Java_xeno_Chr_rotX__FFZ();

extern void Java_xeno_Chr_rotX__IFZ();

extern void Java_xeno_Chr_rotX__ILjava_lang_Object_Z();

extern void Java_xeno_Chr_rotX__Ljava_lang_Object_FZ();

extern void Java_xeno_Chr_rotY__FFZ();

extern void Java_xeno_Chr_rotY__IFZ();

extern void Java_xeno_Chr_rotY__ILjava_lang_Object_Z();

extern void Java_xeno_Chr_rotY__Ljava_lang_Object_FZ();

extern void Java_xeno_Chr_rotZ__FFZ();

extern void Java_xeno_Chr_rotZ__IFZ();

extern void Java_xeno_Chr_rotZ__ILjava_lang_Object_Z();

extern void Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ();

extern void Java_xeno_Chr_start__ILjava_lang_Object_();

extern void Java_xeno_Chr_stop__();

extern void Java_xeno_Chr_mtn__IIIIIFZ();

extern void Java_xeno_Chr_mtn__IIFZ();

extern void Java_xeno_Chr_signal__I();

extern void Java_xeno_Chr_getSignal__();

extern void Java_xeno_Chr_setRotate__();

extern void Java_xeno_Chr_setTranslate__();

extern void Java_xeno_Chr_getRotate__();

extern void Java_xeno_Chr_getTranslate__();

extern void Java_xeno_Chr_setVisible__Z();

extern void Java_xeno_Chr_setVisible__IZ();

extern void Java_xeno_Chr_setCollision__Z();

extern void Java_xeno_Chr_setHand__I();

extern void Java_xeno_Chr_setArgs__III();

extern void Java_xeno_Chr_getArgs__II();

extern void Java_xeno_Chr_setArgs__ILjava_lang_Object_I();

extern void Java_xeno_Chr_getSerial__();

extern void Java_xeno_Chr_getState__();

extern void Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_();

extern void Java_xeno_Chr_setScale__FFF();

extern void Java_xeno_Chr_getScale__();

extern void Java_xeno_Chr_scale__Lxeno_util_Spline_IZ();

extern void Java_xeno_Chr_sclX__FFZ();

extern void Java_xeno_Chr_sclX__IFZ();

extern void Java_xeno_Chr_sclY__FFZ();

extern void Java_xeno_Chr_sclY__IFZ();

extern void Java_xeno_Chr_sclZ__FFZ();

extern void Java_xeno_Chr_sclZ__IFZ();

extern void Java_xeno_Chr_rotCNS__ILjava_lang_Object_();

extern void Java_xeno_Chr_rotCNS__IFFF();

extern void Java_xeno_Chr_setRotCNSParam__IFFFFF();

extern void Java_xeno_Chr_relax__II();

extern void Java_xeno_Chr_getFlags__();

extern void Java_xeno_Chr_setFlags__I();

extern void Java_xeno_Chr_setEdgeFall__I();

extern void Java_xeno_Chr_setShadow__II();

extern void Java_xeno_Chr_setShadow__aB();

extern void Java_xeno_Chr_setID__I();

extern void Java_xeno_Chr_setElevatorMode__I();

extern void Java_xeno_Chr_setParent__Lxeno_Chr_III();

extern void Java_xeno_Chr_setMotionFlags__IZ();

extern void Java_xeno_Chr_setFilter__I();

extern void Java_xeno_Chr_setFilterParam__aF();

extern void Java_xeno_Chr_setClip__I();

extern void Java_xeno_Chr_setSymmetryY__I();

extern void Java_xeno_Chr_setSortOffset__F();

extern void Java_xeno_Chr_setPointLightCol__IFFF();

extern void Java_xeno_Chr_setPointLightPos__IFFF();

extern void Java_xeno_Chr_setPointLightReset__();

extern void Java_xeno_Chr_talkto__Ljava_lang_String_();

extern void Java_xeno_Chr_touchto__Ljava_lang_String_();

extern void Java_xeno_Chr_childGetPeer__II();

extern void Java_xeno_Chr_setPeer__Ljava_lang_Object_();

extern void Java_xeno_Chr_dispRadar__Z();

extern void Java_xeno_Chr_look_camera__();

extern void Java_xeno_Chr_look_char__Ljava_lang_Object_();

extern void Java_xeno_Chr_look_unit__Ljava_lang_Object_();

extern void Java_xeno_Chr_look_default__();

extern void Java_xeno_Chr_look_point__FFF();

extern void Java_xeno_Chr_look_eye_set__FF();

extern void Java_xeno_Chr_look_eye_control__I();

extern void Java_xeno_Chr_look_speed__F();

extern void Java_xeno_Chr_look_eye_speed__F();

extern void Java_xeno_Chr_renderCommand__I();

extern void Java_xeno_Chr_shadow_clip_scale__F();

extern void Java_xeno_Chr_shadow_map_id__I();

extern void Java_xeno_Chr_shadow_map_reset__();

extern void Java_xeno_Chr_hairStop__II();

extern void Java_xeno_Chr_pixelAlpha__I();

extern void Java_xeno_Chr_pixelAlphaParts__II();

extern void Java_xeno_Chr_pixelAlphaPartsReset__();

extern void Java_xeno_Chr_setMotNoUpdate__I();

extern void Java_xeno_Chr_setWeaponR__Lxeno_Chr_();

extern void Java_xeno_Chr_resetWeaponR__Lxeno_Chr_();

extern void Java_xeno_Chr_resetHand__();

extern void Java_xeno_Chr_resetEnv__();

extern void Java_xeno_Chr_ignoreShape__I();

extern void Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF();

extern void Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF();

extern void Java_xeno_Unit_getRotate__();

extern void Java_xeno_Unit_getSignal__();

extern void Java_xeno_Unit_getTranslate__();

extern void Java_xeno_Unit_invalidate__();

extern void Java_xeno_Unit_move__FFFZ();

extern void Java_xeno_Unit_move__IFFZ();

extern void Java_xeno_Unit_move__Lxeno_util_Spline_IZ();

extern void Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ();

extern void Java_xeno_Unit_move__ILjava_lang_Object_Z();

extern void Java_xeno_Unit_move__Ljava_lang_Object_FZ();

extern void Java_xeno_Unit_mtn__IIFZ();

extern void Java_xeno_Unit_mtn__IIIIIFZ();

extern void Java_xeno_Unit_rotX__FFZ();

extern void Java_xeno_Unit_rotX__IFZ();

extern void Java_xeno_Unit_rotX__ILjava_lang_Object_Z();

extern void Java_xeno_Unit_rotX__Ljava_lang_Object_FZ();

extern void Java_xeno_Unit_rotY__FFZ();

extern void Java_xeno_Unit_rotY__IFZ();

extern void Java_xeno_Unit_rotY__ILjava_lang_Object_Z();

extern void Java_xeno_Unit_rotY__Ljava_lang_Object_FZ();

extern void Java_xeno_Unit_rotZ__FFZ();

extern void Java_xeno_Unit_rotZ__IFZ();

extern void Java_xeno_Unit_rotZ__ILjava_lang_Object_Z();

extern void Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ();

extern void Java_xeno_Unit_scale__Lxeno_util_Spline_IZ();

extern void Java_xeno_Unit_sclX__FFZ();

extern void Java_xeno_Unit_sclX__IFZ();

extern void Java_xeno_Unit_sclY__FFZ();

extern void Java_xeno_Unit_sclY__IFZ();

extern void Java_xeno_Unit_sclZ__FFZ();

extern void Java_xeno_Unit_sclZ__IFZ();

extern void Java_xeno_Unit_setCollision__Z();

extern void Java_xeno_Unit_setRotate__();

extern void Java_xeno_Unit_setTranslate__();

extern void Java_xeno_Unit_setVisible__IZ();

extern void Java_xeno_Unit_setVisible__Z();

extern void Java_xeno_Unit_signal__I();

extern void Java_xeno_Unit_start__ILjava_lang_Object_();

extern void Java_xeno_Unit_stop__();

extern void Java_xeno_Unit_validate__();

extern void Java_xeno_Unit_setParent__Ljava_lang_Object_I();

extern void Java_xeno_Unit_setArgs__III();

extern void Java_xeno_Unit_getArgs__II();

extern void Java_xeno_Unit_setArgs__ILjava_lang_Object_I();

extern void Java_xeno_Unit_setArgs__IIIII();

extern void Java_xeno_Unit_getScale__();

extern void Java_xeno_Unit_setScale__FFF();

extern void Java_xeno_Unit_getSerial__();

extern void Java_xeno_Unit_mtnSetMask__I();

extern void Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_();

extern void Java_xeno_Unit_getState__();

extern void Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_();

extern void Java_xeno_Unit_setPivot__FFF();

extern void Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_();

extern void Java_xeno_Unit_setAxis__FFFF();

extern void Java_xeno_Unit_suspend__I();

extern void Java_xeno_Unit_resume__I();

extern void Java_xeno_Unit_initElevatorFunc__();

extern void Java_xeno_Unit_map_shadow__I();

extern void Java_xeno_Unit_renderCommand__I();

extern void Java_xeno_Unit_setFilter__I();

extern void Java_xeno_Unit_setFilterParam__aF();

extern void Java_xeno_Unit_setShadow__II();

extern void Java_xeno_Unit_shadow_clip_scale__F();

extern void Java_xeno_Unit_shadow_map_id__I();

extern void Java_xeno_Unit_shadow_map_reset__();

extern void Java_xeno_Unit_setSortOffset__F();

extern void Java_xeno_Unit_setClip__I();

extern void Java_xeno_Unit_setMonitorPrio__I();

extern void Java_xeno_Scene_start__ILjava_lang_Object_();

extern void Java_xeno_Scene_stop__();

extern void Java_xeno_Stage_start__ILjava_lang_Object_();

extern void Java_xeno_Stage_stop__();

extern void Java_xeno_Stage_play__Ljava_lang_String_();

extern void Java_xeno_Stage_setPartsLast__I();

extern void Java_xeno_Stage_setPartsLastReset__();

extern void Java_xeno_Stage_setColor__FFF();

extern void Java_xeno_Stage_setFade__IIFFF();

extern void Java_xeno_Stage_setEventFade__IFFFIFFF();

extern void Java_xeno_Stage_setFrameRender__II();

extern void Java_xeno_Stage_setFadeCancel__I();

extern void Java_xeno_Stage_setVisible__IZ();

extern void Java_xeno_Stage_setCFBG__II_F();

extern void Java_xeno_Stage_setEffectRender__I();

extern void Java_xeno_Stage_renderCommand__I();

extern void Java_xeno_Stage_setBgColor__FFF();

extern void Java_xeno_Stage_setBgClip__I();

extern void Java_xeno_Stage_clrBackBuffer__();

/* Retail's local table precedes the native binding table in the data section. */

RuntimeNativeBinding default_native[] = {
    { javaSystemArraycopyName, (void (*)())Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II },
    { javaSystemSleepName, (void (*)())Java_xeno_vm_System_sleep__I },
    { javaSystemPrintlnName, (void (*)())Java_xeno_vm_System_println__Ljava_lang_String_ },
    { javaSystemWaitForName, (void (*)())Java_xeno_vm_System_waitFor__Ljava_lang_Object_ },
    { javaSystemMethodSignalName, (void (*)())Java_xeno_vm_System_methodSignal__I },
    { javaThreadCreateName, (void (*)())Java_xeno_vm_Thread_create__ },
    { javaThreadSetTargetName, (void (*)())Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_ },
    { javaThreadStartName, (void (*)())Java_xeno_vm_Thread_start__ },
    { javaThreadStopName, (void (*)())Java_xeno_vm_Thread_stop__ },
    { javaMathRandomName, (void (*)())Java_xeno_vm_Math_random__ },
    { javaMathAtan2Name, (void (*)())Java_xeno_vm_Math_atan2__FF },
    { vmMathCosName, (void (*)())Java_xeno_vm_Math_cos__F },
    { vmMathSinName, (void (*)())Java_xeno_vm_Math_sin__F },
    { splineCreateName, (void (*)())Java_xeno_util_Spline_create__ },
    { splineSetCtrlVertexName, (void (*)())Java_xeno_util_Spline_setCtrlVertex__aFIII },
    { splineGetValueName, (void (*)())Java_xeno_util_Spline_getValue__I },
    { formatFloatToIntBitsName, (void (*)())Java_xeno_util_Format_floatToIntBits__F },
    { formatIntBitsToFloatName, (void (*)())Java_xeno_util_Format_intBitsToFloat__I },
    { formatToIntName, (void (*)())Java_xeno_util_Format_toInt__Ljava_lang_String_ },
    { formatToStringCharName, (void (*)())Java_xeno_util_Format_toString__C },
    { formatToStringFloatName, (void (*)())Java_xeno_util_Format_toString__F },
    { formatToStringIntName, (void (*)())Java_xeno_util_Format_toString__I },
    { formatToStringBoolName, (void (*)())Java_xeno_util_Format_toString__Z },
    { inputCreateName, (void (*)())Java_xeno_util_Input_create__I },
    { inputGetButtonName, (void (*)())Java_xeno_util_Input_getButton__ },
    { inputGetEdgeName, (void (*)())Java_xeno_util_Input_getEdge__ },
    { inputGetRepeatName, (void (*)())Java_xeno_util_Input_getRepeat__ },
    { layoutSetName, (void (*)())Java_xeno_util_Layout_set__Ljava_lang_Object_I },
    { layoutGetManagerName, (void (*)())Java_xeno_util_Layout_getManager__I },
    { runtimeExecBattleName, (void (*)())Java_xeno_util_Runtime_execBattle__II },
    { runtimeGetFlagsName, (void (*)())Java_xeno_util_Runtime_getFlags__II },
    { runtimeSetFlagsName, (void (*)())Java_xeno_util_Runtime_setFlags__III },
    { runtimeSetLocationName, (void (*)())Java_xeno_util_Runtime_setLocation__III },
    { runtimeGetLocationName, (void (*)())Java_xeno_util_Runtime_getLocation__ },
    { runtimeSetRegisterName, (void (*)())Java_xeno_util_Runtime_setRegister__II },
    { runtimeGetRegisterName, (void (*)())Java_xeno_util_Runtime_getRegister__I },
    { runtimeGetEntranceName, (void (*)())Java_xeno_util_Runtime_getEntrance__ },
    { runtimeSetPlayerControlName, (void (*)())Java_xeno_util_Runtime_setPlayerControl__Z },
    { runtimeSetPlayerMoveParamName, (void (*)())Java_xeno_util_Runtime_setPlayerMoveParam__FFF },
    { runtimeDisableName, (void (*)())Java_xeno_util_Runtime_disable__I },
    { runtimeEnableName, (void (*)())Java_xeno_util_Runtime_enable__I },
    { runtimeGetGameStateName, (void (*)())Java_xeno_util_Runtime_getGameState__ },
    { runtimeCaptureEndName, (void (*)())Java_xeno_util_Runtime_CaptureEnd__ },
    { runtimeCaptureStartName, (void (*)())Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I },
    { runtimeJumpCFName, (void (*)())Java_xeno_util_Runtime_jumpCF__II },
    { runtimeJumpEventName, (void (*)())Java_xeno_util_Runtime_jumpEvent__I },
    { runtimeSetDefocusQuickName, (void (*)())Java_xeno_util_Runtime_setDefocusQuick__IIII },
    { runtimeSetDefocusName, (void (*)())Java_xeno_util_Runtime_setDefocus__IIaI },
    { runtimeSetDefocusParamName, (void (*)())Java_xeno_util_Runtime_setDefocusParam__III },
    { runtimeSetEventTimerName, (void (*)())Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I },
    { runtimeSetMapName, (void (*)())Java_xeno_util_Runtime_setMap__II },
    { runtimeSetShootFlagName, (void (*)())Java_xeno_util_Runtime_setShootFlag__Z },
    { runtimeSetShootHeightCheckName, (void (*)())Java_xeno_util_Runtime_setShootHeightCheck__Z },
    { runtimeSetShootIDCheckName, (void (*)())Java_xeno_util_Runtime_setShootIDCheck__Z },
    { runtimeSetShootUwaCheckName, (void (*)())Java_xeno_util_Runtime_setShootUwaCheck__Z },
    { runtimeSetShootRangeName, (void (*)())Java_xeno_util_Runtime_setShootRange__F },
    { runtimeAddItemName, (void (*)())Java_xeno_util_Runtime_addItem__II },
    { runtimeAddItemWinName, (void (*)())Java_xeno_util_Runtime_addItemWin__II },
    { runtimeRemoveItemName, (void (*)())Java_xeno_util_Runtime_removeItem__II },
    { runtimeCheckItemName, (void (*)())Java_xeno_util_Runtime_checkItem__II },
    { runtimeGetItemNameName, (void (*)())Java_xeno_util_Runtime_getItemName__II },
    { runtimeAddGoldName, (void (*)())Java_xeno_util_Runtime_addGold__I },
    { runtimeRemoveGoldName, (void (*)())Java_xeno_util_Runtime_removeGold__I },
    { runtimeCheckGoldName, (void (*)())Java_xeno_util_Runtime_checkGold__ },
    { runtimeProgressEffectName, (void (*)())Java_xeno_util_Runtime_progressEffect__I },
    { runtimeEnterShopName, (void (*)())Java_xeno_util_Runtime_enterShop__I },
    { runtimeGetLeaderName, (void (*)())Java_xeno_util_Runtime_getLeader__ },
    { runtimeGetPartyDataName, (void (*)())Java_xeno_util_Runtime_getPartyData__I },
    { runtimeSetPartyDataName, (void (*)())Java_xeno_util_Runtime_setPartyData__II },
    { runtimeSetFriendName, (void (*)())Java_xeno_util_Runtime_setFriend__I },
    { runtimeResetFriendName, (void (*)())Java_xeno_util_Runtime_resetFriend__I },
    { runtimeCheckFriendName, (void (*)())Java_xeno_util_Runtime_checkFriend__I },
    { runtimeSetLockPartyName, (void (*)())Java_xeno_util_Runtime_setLockParty__I },
    { runtimeResetLockPartyName, (void (*)())Java_xeno_util_Runtime_resetLockParty__I },
    { runtimeCheckLockPartyName, (void (*)())Java_xeno_util_Runtime_checkLockParty__I },
    { runtimeSetOutFriendName, (void (*)())Java_xeno_util_Runtime_setOutFriend__I },
    { runtimeResetOutFriendName, (void (*)())Java_xeno_util_Runtime_resetOutFriend__I },
    { runtimeCheckOutFriendName, (void (*)())Java_xeno_util_Runtime_checkOutFriend__I },
    { runtimeSetTakeAgwsName, (void (*)())Java_xeno_util_Runtime_setTakeAgws__I },
    { runtimeResetTakeAgwsName, (void (*)())Java_xeno_util_Runtime_resetTakeAgws__I },
    { runtimeCheckTakeAgwsName, (void (*)())Java_xeno_util_Runtime_checkTakeAgws__I },
    { runtimeBattleChangePartyName, (void (*)())Java_xeno_util_Runtime_battleChangeParty__I },
    { runtimeSetIdLightColName, (void (*)())Java_xeno_util_Runtime_setIdLightCol__IIFFF },
    { runtimeSetIdLightDirName, (void (*)())Java_xeno_util_Runtime_setIdLightDir__IIFFF },
    { runtimeSetIdLightVecName, (void (*)())Java_xeno_util_Runtime_setIdLightVec__IIFFF },
    { runtimeSetWindParamName, (void (*)())Java_xeno_util_Runtime_setWindParam__IFFFF },
    { runtimeMpeg2Name, (void (*)())Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_ },
    { runtimeMpeg2AfterCrossFadeName, (void (*)())Java_xeno_util_Runtime_mpeg2AfterCrossFade__I },
    { runtimeMailFlagName, (void (*)())Java_xeno_util_Runtime_mailFlag__I },
    { runtimeMailExecName, (void (*)())Java_xeno_util_Runtime_mailExec__I },
    { runtimeMinigameExecName, (void (*)())Java_xeno_util_Runtime_minigameExec__I },
    { runtimeSetMenuLockName, (void (*)())Java_xeno_util_Runtime_setMenuLock__ },
    { runtimeEvsSetRetPointName, (void (*)())Java_xeno_util_Runtime_evsSetRetPoint__ },
    { runtimeEvsExitName, (void (*)())Java_xeno_util_Runtime_evsExit__ },
    { runtimeEtherTecSetName, (void (*)())Java_xeno_util_Runtime_etherTecSet__I },
    { runtimeCharAllRecoveryName, (void (*)())Java_xeno_util_Runtime_charAllRecovery__ },
    { runtimeAGWSAllRecoveryName, (void (*)())Java_xeno_util_Runtime_AGWSAllRecovery__ },
    { windowGetSignalName, (void (*)())Java_xeno_util_Window_getSignal__ },
    { windowSignalName, (void (*)())Java_xeno_util_Window_signal__I },
    { windowClearName, (void (*)())Java_xeno_util_Window_clear__ },
    { windowCloseName, (void (*)())Java_xeno_util_Window_close__ },
    { windowWaitkeyName, (void (*)())Java_xeno_util_Window_waitkey__I },
    { windowWaitName, (void (*)())Java_xeno_util_Window_wait__I },
    { windowCreateName, (void (*)())Java_xeno_util_Window_create__I },
    { windowSetSizeName, (void (*)())Java_xeno_util_Window_setSize__II },
    { windowSetLocationName, (void (*)())Java_xeno_util_Window_setLocation__II },
    { windowPrintStringName, (void (*)())Java_xeno_util_Window_print__Ljava_lang_String_ },
    { windowPrintStringArrayIntName, (void (*)())Java_xeno_util_Window_print__aLjava_lang_String_I },
    { windowSetNameName, (void (*)())Java_xeno_util_Window_setName__Ljava_lang_String_ },
    { windowCloseWaitKeyName, (void (*)())Java_xeno_util_Window_closeWaitKey__ },
    { menuAddQueryStringName, (void (*)())Java_xeno_util_Menu_addQuery__Ljava_lang_String_ },
    { menuAddQueryStringArrayIntName, (void (*)())Java_xeno_util_Menu_addQuery__aLjava_lang_String_I },
    { menuAddItemName, (void (*)())Java_xeno_util_Menu_addItem__Ljava_lang_String_ },
    { menuCreateName, (void (*)())Java_xeno_util_Menu_create__ },
    { menuGetSelectedName, (void (*)())Java_xeno_util_Menu_getSelected__ },
    { menuSetLocationName, (void (*)())Java_xeno_util_Menu_setLocation__II },
    { menuSetVisibleName, (void (*)())Java_xeno_util_Menu_setVisible__Z },
    { menuSetCursorName, (void (*)())Java_xeno_util_Menu_setCursor__I },
    { toolkitGetPeerName, (void (*)())Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_ },
    { toolkitCallName, (void (*)())Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_ },
    { toolkitLoadResourceObjectIntName, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I },
    { toolkitLoadResourceStringName, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_ },
    { toolkitLoadResourceObjectObjectIntName, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I },
    { toolkitLoadResourceObjectIntIntName, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II },
    { toolkitPeerSetGroupName, (void (*)())Java_xeno_util_Toolkit_peerSetGroup__II },
    { soundSequencePlayIntName, (void (*)())Java_xeno_Sound_sequencePlay__I },
    { soundSequencePlayIntIntName, (void (*)())Java_xeno_Sound_sequencePlay__II },
    { soundSequenceStopIntName, (void (*)())Java_xeno_Sound_sequenceStop__I },
    { soundSequenceStopIntIntName, (void (*)())Java_xeno_Sound_sequenceStop__II },
    { soundEffectPlayName, (void (*)())Java_xeno_Sound_effectPlay__III },
    { soundEffectStopName, (void (*)())Java_xeno_Sound_effectStop__I },
    { soundStreamPlayName, (void (*)())Java_xeno_Sound_streamPlay__IIII },
    { movieInitName, (void (*)())Java_xeno_Movie_init__I },
    { movieStartName, (void (*)())Java_xeno_Movie_start__I },
    { movieStopName, (void (*)())Java_xeno_Movie_stop__ },
    { movieUpdateName, (void (*)())Java_xeno_Movie_update__ },
    { playControlCreateName, (void (*)())Java_xeno_PlayControl_create__ },
    { playControlGetSignalName, (void (*)())Java_xeno_PlayControl_getSignal__ },
    { playControlSignalName, (void (*)())Java_xeno_PlayControl_signal__I },
    { playControlInitIntIntName, (void (*)())Java_xeno_PlayControl_init__II },
    { playControlInitIntIntIntIntFloatName, (void (*)())Java_xeno_PlayControl_init__IIIIF },
    { playControlLoadCameraObjName, (void (*)())Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_ },
    { playControlLoadTimeChartObjName, (void (*)())Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_ },
    { playControlStartName, (void (*)())Java_xeno_PlayControl_start__ },
    { playControlStopName, (void (*)())Java_xeno_PlayControl_stop__ },
    { playControlSetObserverIIObjStrName, (void (*)())Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_ },
    { playControlSetObserverIStrObjStrName, (void (*)())Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_ },
    { playControlGetParamsName, (void (*)())Java_xeno_PlayControl_getParams__ },
    { effectCallIName, (void (*)())Java_xeno_Effect_call__I },
    { effectDispZName, (void (*)())Java_xeno_Effect_disp__Z },
    { effectSetScaleFFFName, (void (*)())Java_xeno_Effect_setScale__FFF },
    { effectGetScaleName, (void (*)())Java_xeno_Effect_getScale__ },
    { effectGetTranslateName, (void (*)())Java_xeno_Effect_getTranslate__ },
    { effectSetTranslateName, (void (*)())Java_xeno_Effect_setTranslate__ },
    { effectGetRotateName, (void (*)())Java_xeno_Effect_getRotate__ },
    { effectSetRotateName, (void (*)())Java_xeno_Effect_setRotate__ },
    { effectSetCasterChrName, (void (*)())Java_xeno_Effect_setCaster__Lxeno_Chr_ },
    { effectSetTargetChrName, (void (*)())Java_xeno_Effect_setTarget__Lxeno_Chr_ },
    { effectSetCasterUnitName, (void (*)())Java_xeno_Effect_setCaster__Lxeno_Unit_ },
    { effectSetTargetUnitName, (void (*)())Java_xeno_Effect_setTarget__Lxeno_Unit_ },
    { effectSetTransOffsetFFFName, (void (*)())Java_xeno_Effect_setTransOffset__FFF },
    { effectGetForceLoopName, (void (*)())Java_xeno_Effect_getForceLoop__ },
    { effectSetForceLoopZName, (void (*)())Java_xeno_Effect_setForceLoop__Z },
    { effectGetClipName, (void (*)())Java_xeno_Effect_getClip__ },
    { effectSetClipZName, (void (*)())Java_xeno_Effect_setClip__Z },
    { effectClearEffectName, (void (*)())Java_xeno_Effect_clearEffect__ },
    { effectSetMotionZName, (void (*)())Java_xeno_Effect_setMotion__Z },
    { effectNoAttachZName, (void (*)())Java_xeno_Effect_noAttach__Z },
    { cameraGetRotateXName, (void (*)())Java_xeno_Camera_getRotateX__ },
    { cameraGetRotateYName, (void (*)())Java_xeno_Camera_getRotateY__ },
    { cameraGetRotateZName, (void (*)())Java_xeno_Camera_getRotateZ__ },
    { cameraGetTranslateXName, (void (*)())Java_xeno_Camera_getTranslateX__ },
    { cameraGetTranslateYName, (void (*)())Java_xeno_Camera_getTranslateY__ },
    { cameraGetTranslateZName, (void (*)())Java_xeno_Camera_getTranslateZ__ },
    { cameraFovSPLArrFIName, (void (*)())Java_xeno_Camera_fovSPL__aFI },
    { cameraChangeName, (void (*)())Java_xeno_Camera_change__ },
    { cameraRotateSPLArrFIName, (void (*)())Java_xeno_Camera_rotateSPL__aFI },
    { cameraRotateSPLArrFIIIName, (void (*)())Java_xeno_Camera_rotateSPL__aFIII },
    { cameraSetActiveZName, (void (*)())Java_xeno_Camera_setActive__Z },
    { cameraCreateIName, (void (*)())Java_xeno_Camera_create__I },
    { cameraTransCNSObjFFFName, (void (*)())Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF },
    { cameraTransSPLArrFIName, (void (*)())Java_xeno_Camera_transSPL__aFI },
    { cameraTransSPLArrFIIIName, (void (*)())Java_xeno_Camera_transSPL__aFIII },
    { cameraViewCNSObjFFFName, (void (*)())Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF },
    { cameraViewSPLArrFIName, (void (*)())Java_xeno_Camera_viewSPL__aFI },
    { cameraViewSPLArrFIIIName, (void (*)())Java_xeno_Camera_viewSPL__aFIII },
    { cameraRollSPLArrFIName, (void (*)())Java_xeno_Camera_rollSPL__aFI },
    { cameraSetRotateFFFName, (void (*)())Java_xeno_Camera_setRotate__FFF },
    { cameraSetRollFName, (void (*)())Java_xeno_Camera_setRoll__F },
    { cameraSetTranslateFFFName, (void (*)())Java_xeno_Camera_setTranslate__FFF },
    { cameraSetFovFName, (void (*)())Java_xeno_Camera_setFov__F },
    { cameraGetFovName, (void (*)())Java_xeno_Camera_getFov__ },
    { cameraSetViewFFFName, (void (*)())Java_xeno_Camera_setView__FFF },
    { cameraStartIObjName, (void (*)())Java_xeno_Camera_start__ILjava_lang_Object_ },
    { cameraSetCFAngleIFFFFName, (void (*)())Java_xeno_Camera_setCFAngle__IFFFF },
    { cameraSetCFAnglePersIFFFFFName, (void (*)())Java_xeno_Camera_setCFAnglePers__IFFFFF },
    { cameraSetCFHokanIFFName, (void (*)())Java_xeno_Camera_setCFHokan__IFF },
    { cameraSetCFLockIIFFFName, (void (*)())Java_xeno_Camera_setCFLock__IIFFF },
    { cameraSetCFOffsetIFFFIFIFName, (void (*)())Java_xeno_Camera_setCFOffset__IFFFIFIF },
    { cameraSetModeIName, (void (*)())Java_xeno_Camera_setMode__I },
    { cameraGetModeName, (void (*)())Java_xeno_Camera_getMode__ },
    { cameraSetCFPedestalIFFFFFFFFName, (void (*)())Java_xeno_Camera_setCFPedestal__IFFFFFFFF },
    { cameraSetCFPedestalHokanIIName, (void (*)())Java_xeno_Camera_setCFPedestalHokan__II },
    { cameraChangeIDIIIName, (void (*)())Java_xeno_Camera_changeID__III },
    { cameraSetFogIFFFFIIIIName, (void (*)())Java_xeno_Camera_setFog__IFFFFIIII },
    { cameraResetFogIName, (void (*)())Java_xeno_Camera_resetFog__I },
    { cameraSetClipRangeFFName, (void (*)())Java_xeno_Camera_setClipRange__FF },
    { lightSetColorFFFName, (void (*)())Java_xeno_Light_setColor__FFF },
    { lightSetDirectionFFFName, (void (*)())Java_xeno_Light_setDirection__FFF },
    { lightSetDirection2FFFName, (void (*)())Java_xeno_Light_setDirection2__FFF },
    { lightSetGlobalPointLightColIFFFName, (void (*)())Java_xeno_Light_setGlobalPointLightCol__IFFF },
    { lightSetGlobalPointLightPosIFFFName, (void (*)())Java_xeno_Light_setGlobalPointLightPos__IFFF },
    { lightSetGlobalPointLightResetName, (void (*)())Java_xeno_Light_setGlobalPointLightReset__ },
    { chrGetPlayerName, (void (*)())Java_xeno_Chr_getPlayer__ },
    { chrSetPlayerName, (void (*)())Java_xeno_Chr_setPlayer__ },
    { chrMoveIFFZName, (void (*)())Java_xeno_Chr_move__IFFZ },
    { chrMoveFFFZName, (void (*)())Java_xeno_Chr_move__FFFZ },
    { chrMoveIObjZName, (void (*)())Java_xeno_Chr_move__ILjava_lang_Object_Z },
    { chrMoveObjFZName, (void (*)())Java_xeno_Chr_move__Ljava_lang_Object_FZ },
    { chrMoveSplineIZName, (void (*)())Java_xeno_Chr_move__Lxeno_util_Spline_IZ },
    { chrRotateSplineIZName, (void (*)())Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ },
    { chrRotXFFZName, (void (*)())Java_xeno_Chr_rotX__FFZ },
    { chrRotXIFZName, (void (*)())Java_xeno_Chr_rotX__IFZ },
    { chrRotXIObjZName, (void (*)())Java_xeno_Chr_rotX__ILjava_lang_Object_Z },
    { chrRotXObjFZName, (void (*)())Java_xeno_Chr_rotX__Ljava_lang_Object_FZ },
    { chrRotYFFZName, (void (*)())Java_xeno_Chr_rotY__FFZ },
    { chrRotYIFZName, (void (*)())Java_xeno_Chr_rotY__IFZ },
    { chrRotYIObjZName, (void (*)())Java_xeno_Chr_rotY__ILjava_lang_Object_Z },
    { chrRotYObjFZName, (void (*)())Java_xeno_Chr_rotY__Ljava_lang_Object_FZ },
    { chrRotZFFZName, (void (*)())Java_xeno_Chr_rotZ__FFZ },
    { chrRotZIFZName, (void (*)())Java_xeno_Chr_rotZ__IFZ },
    { chrRotZIObjZName, (void (*)())Java_xeno_Chr_rotZ__ILjava_lang_Object_Z },
    { chrRotZObjFZName, (void (*)())Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ },
    { chrStartIObjName, (void (*)())Java_xeno_Chr_start__ILjava_lang_Object_ },
    { chrStopName, (void (*)())Java_xeno_Chr_stop__ },
    { chrMtnIIIIIFZName, (void (*)())Java_xeno_Chr_mtn__IIIIIFZ },
    { chrMtnIIFZName, (void (*)())Java_xeno_Chr_mtn__IIFZ },
    { chrSignalIName, (void (*)())Java_xeno_Chr_signal__I },
    { chrGetSignalName, (void (*)())Java_xeno_Chr_getSignal__ },
    { chrSetRotateName, (void (*)())Java_xeno_Chr_setRotate__ },
    { chrSetTranslateName, (void (*)())Java_xeno_Chr_setTranslate__ },
    { chrGetRotateName, (void (*)())Java_xeno_Chr_getRotate__ },
    { chrGetTranslateName, (void (*)())Java_xeno_Chr_getTranslate__ },
    { chrSetVisibleZName, (void (*)())Java_xeno_Chr_setVisible__Z },
    { chrSetVisibleIZName, (void (*)())Java_xeno_Chr_setVisible__IZ },
    { chrSetCollisionZName, (void (*)())Java_xeno_Chr_setCollision__Z },
    { chrSetHandIName, (void (*)())Java_xeno_Chr_setHand__I },
    { chrSetArgsIIIName, (void (*)())Java_xeno_Chr_setArgs__III },
    { chrGetArgsIIName, (void (*)())Java_xeno_Chr_getArgs__II },
    { chrSetArgsIObjIName, (void (*)())Java_xeno_Chr_setArgs__ILjava_lang_Object_I },
    { chrGetSerialName, (void (*)())Java_xeno_Chr_getSerial__ },
    { chrGetStateName, (void (*)())Java_xeno_Chr_getState__ },
    { chrMtnGetRootIVec4fName, (void (*)())Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_ },
    { chrSetScaleFFFName, (void (*)())Java_xeno_Chr_setScale__FFF },
    { chrGetScaleName, (void (*)())Java_xeno_Chr_getScale__ },
    { chrScaleSplineIZName, (void (*)())Java_xeno_Chr_scale__Lxeno_util_Spline_IZ },
    { chrSclXFFZName, (void (*)())Java_xeno_Chr_sclX__FFZ },
    { chrSclXIFZName, (void (*)())Java_xeno_Chr_sclX__IFZ },
    { chrSclYFFZName, (void (*)())Java_xeno_Chr_sclY__FFZ },
    { chrSclYIFZName, (void (*)())Java_xeno_Chr_sclY__IFZ },
    { chrSclZFFZName, (void (*)())Java_xeno_Chr_sclZ__FFZ },
    { chrSclZIFZName, (void (*)())Java_xeno_Chr_sclZ__IFZ },
    { chrRotCNSIObjName, (void (*)())Java_xeno_Chr_rotCNS__ILjava_lang_Object_ },
    { chrRotCNSIFFFName, (void (*)())Java_xeno_Chr_rotCNS__IFFF },
    { chrSetRotCNSParamIFFFFFName, (void (*)())Java_xeno_Chr_setRotCNSParam__IFFFFF },
    { chrRelaxIIName, (void (*)())Java_xeno_Chr_relax__II },
    { chrGetFlagsName, (void (*)())Java_xeno_Chr_getFlags__ },
    { chrSetFlagsIName, (void (*)())Java_xeno_Chr_setFlags__I },
    { chrSetEdgeFallIName, (void (*)())Java_xeno_Chr_setEdgeFall__I },
    { chrSetShadowIIName, (void (*)())Java_xeno_Chr_setShadow__II },
    { chrSetShadowArrBName, (void (*)())Java_xeno_Chr_setShadow__aB },
    { chrSetIDName, (void (*)())Java_xeno_Chr_setID__I },
    { chrSetElevatorModeName, (void (*)())Java_xeno_Chr_setElevatorMode__I },
    { chrSetParentName, (void (*)())Java_xeno_Chr_setParent__Lxeno_Chr_III },
    { chrSetMotionFlagsName, (void (*)())Java_xeno_Chr_setMotionFlags__IZ },
    { chrSetFilterName, (void (*)())Java_xeno_Chr_setFilter__I },
    { chrSetFilterParamName, (void (*)())Java_xeno_Chr_setFilterParam__aF },
    { chrSetClipName, (void (*)())Java_xeno_Chr_setClip__I },
    { chrSetSymmetryYName, (void (*)())Java_xeno_Chr_setSymmetryY__I },
    { chrSetSortOffsetName, (void (*)())Java_xeno_Chr_setSortOffset__F },
    { chrSetPointLightColName, (void (*)())Java_xeno_Chr_setPointLightCol__IFFF },
    { chrSetPointLightPosName, (void (*)())Java_xeno_Chr_setPointLightPos__IFFF },
    { chrSetPointLightResetName, (void (*)())Java_xeno_Chr_setPointLightReset__ },
    { chrTalktoName, (void (*)())Java_xeno_Chr_talkto__Ljava_lang_String_ },
    { chrTouchtoName, (void (*)())Java_xeno_Chr_touchto__Ljava_lang_String_ },
    { chrChildGetPeerName, (void (*)())Java_xeno_Chr_childGetPeer__II },
    { chrSetPeerName, (void (*)())Java_xeno_Chr_setPeer__Ljava_lang_Object_ },
    { chrDispRadarName, (void (*)())Java_xeno_Chr_dispRadar__Z },
    { chrLookCameraName, (void (*)())Java_xeno_Chr_look_camera__ },
    { chrLookCharName, (void (*)())Java_xeno_Chr_look_char__Ljava_lang_Object_ },
    { chrLookUnitName, (void (*)())Java_xeno_Chr_look_unit__Ljava_lang_Object_ },
    { chrLookDefaultName, (void (*)())Java_xeno_Chr_look_default__ },
    { chrLookPointName, (void (*)())Java_xeno_Chr_look_point__FFF },
    { chrLookEyeSetName, (void (*)())Java_xeno_Chr_look_eye_set__FF },
    { chrLookEyeControlName, (void (*)())Java_xeno_Chr_look_eye_control__I },
    { chrLookSpeedName, (void (*)())Java_xeno_Chr_look_speed__F },
    { chrLookEyeSpeedName, (void (*)())Java_xeno_Chr_look_eye_speed__F },
    { chrRenderCommandName, (void (*)())Java_xeno_Chr_renderCommand__I },
    { chrShadowClipScaleName, (void (*)())Java_xeno_Chr_shadow_clip_scale__F },
    { chrShadowMapIdName, (void (*)())Java_xeno_Chr_shadow_map_id__I },
    { chrShadowMapResetName, (void (*)())Java_xeno_Chr_shadow_map_reset__ },
    { chrHairStopName, (void (*)())Java_xeno_Chr_hairStop__II },
    { chrPixelAlphaName, (void (*)())Java_xeno_Chr_pixelAlpha__I },
    { chrPixelAlphaPartsName, (void (*)())Java_xeno_Chr_pixelAlphaParts__II },
    { chrPixelAlphaPartsResetName, (void (*)())Java_xeno_Chr_pixelAlphaPartsReset__ },
    { chrSetMotNoUpdateName, (void (*)())Java_xeno_Chr_setMotNoUpdate__I },
    { chrSetWeaponRName, (void (*)())Java_xeno_Chr_setWeaponR__Lxeno_Chr_ },
    { chrResetWeaponRName, (void (*)())Java_xeno_Chr_resetWeaponR__Lxeno_Chr_ },
    { chrResetHandName, (void (*)())Java_xeno_Chr_resetHand__ },
    { chrResetEnvName, (void (*)())Java_xeno_Chr_resetEnv__ },
    { chrIgnoreShapeName, (void (*)())Java_xeno_Chr_ignoreShape__I },
    { unitRotYCNSObjIntFloatFloatFloatName, (void (*)())Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF },
    { unitTransCNSObjIntFloatFloatFloatName, (void (*)())Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF },
    { unitGetRotateName, (void (*)())Java_xeno_Unit_getRotate__ },
    { unitGetSignalName, (void (*)())Java_xeno_Unit_getSignal__ },
    { unitGetTranslateName, (void (*)())Java_xeno_Unit_getTranslate__ },
    { unitInvalidateName, (void (*)())Java_xeno_Unit_invalidate__ },
    { unitMoveFloatFloatFloatBoolName, (void (*)())Java_xeno_Unit_move__FFFZ },
    { unitMoveIntFloatFloatBoolName, (void (*)())Java_xeno_Unit_move__IFFZ },
    { unitMoveSplineIntBoolName, (void (*)())Java_xeno_Unit_move__Lxeno_util_Spline_IZ },
    { unitRotateSplineIntBoolName, (void (*)())Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ },
    { unitMoveIntObjBoolName, (void (*)())Java_xeno_Unit_move__ILjava_lang_Object_Z },
    { unitMoveObjFloatBoolName, (void (*)())Java_xeno_Unit_move__Ljava_lang_Object_FZ },
    { unitMtnIntIntFloatBoolName, (void (*)())Java_xeno_Unit_mtn__IIFZ },
    { unitMtnIntIntIntIntIntFloatBoolName, (void (*)())Java_xeno_Unit_mtn__IIIIIFZ },
    { unitRotXFloatFloatBoolName, (void (*)())Java_xeno_Unit_rotX__FFZ },
    { unitRotXIntFloatBoolName, (void (*)())Java_xeno_Unit_rotX__IFZ },
    { unitRotXIntObjBoolName, (void (*)())Java_xeno_Unit_rotX__ILjava_lang_Object_Z },
    { unitRotXObjFloatBoolName, (void (*)())Java_xeno_Unit_rotX__Ljava_lang_Object_FZ },
    { unitRotYFloatFloatBoolName, (void (*)())Java_xeno_Unit_rotY__FFZ },
    { unitRotYIntFloatBoolName, (void (*)())Java_xeno_Unit_rotY__IFZ },
    { unitRotYIntObjBoolName, (void (*)())Java_xeno_Unit_rotY__ILjava_lang_Object_Z },
    { unitRotYObjFloatBoolName, (void (*)())Java_xeno_Unit_rotY__Ljava_lang_Object_FZ },
    { unitRotZFloatFloatBoolName, (void (*)())Java_xeno_Unit_rotZ__FFZ },
    { unitRotZIntFloatBoolName, (void (*)())Java_xeno_Unit_rotZ__IFZ },
    { unitRotZIntObjBoolName, (void (*)())Java_xeno_Unit_rotZ__ILjava_lang_Object_Z },
    { unitRotZObjFloatBoolName, (void (*)())Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ },
    { unitScaleSplineIntBoolName, (void (*)())Java_xeno_Unit_scale__Lxeno_util_Spline_IZ },
    { unitSclXFloatFloatBoolName, (void (*)())Java_xeno_Unit_sclX__FFZ },
    { unitSclXIntFloatBoolName, (void (*)())Java_xeno_Unit_sclX__IFZ },
    { unitSclYFloatFloatBoolName, (void (*)())Java_xeno_Unit_sclY__FFZ },
    { unitSclYIntFloatBoolName, (void (*)())Java_xeno_Unit_sclY__IFZ },
    { unitSclZFloatFloatBoolName, (void (*)())Java_xeno_Unit_sclZ__FFZ },
    { unitSclZIntFloatBoolName, (void (*)())Java_xeno_Unit_sclZ__IFZ },
    { unitSetCollisionName, (void (*)())Java_xeno_Unit_setCollision__Z },
    { unitSetRotateName, (void (*)())Java_xeno_Unit_setRotate__ },
    { unitSetTranslateName, (void (*)())Java_xeno_Unit_setTranslate__ },
    { unitSetVisibleIntBoolName, (void (*)())Java_xeno_Unit_setVisible__IZ },
    { unitSetVisibleBoolName, (void (*)())Java_xeno_Unit_setVisible__Z },
    { unitSignalName, (void (*)())Java_xeno_Unit_signal__I },
    { unitStartName, (void (*)())Java_xeno_Unit_start__ILjava_lang_Object_ },
    { unitStopName, (void (*)())Java_xeno_Unit_stop__ },
    { unitValidateName, (void (*)())Java_xeno_Unit_validate__ },
    { unitSetParentName, (void (*)())Java_xeno_Unit_setParent__Ljava_lang_Object_I },
    { unitSetArgsIntIntIntName, (void (*)())Java_xeno_Unit_setArgs__III },
    { unitGetArgsName, (void (*)())Java_xeno_Unit_getArgs__II },
    { unitSetArgsIntObjIntName, (void (*)())Java_xeno_Unit_setArgs__ILjava_lang_Object_I },
    { unitSetArgsIntIntIntIntIntName, (void (*)())Java_xeno_Unit_setArgs__IIIII },
    { unitGetScaleName, (void (*)())Java_xeno_Unit_getScale__ },
    { unitSetScaleName, (void (*)())Java_xeno_Unit_setScale__FFF },
    { unitGetSerialName, (void (*)())Java_xeno_Unit_getSerial__ },
    { unitMtnSetMaskName, (void (*)())Java_xeno_Unit_mtnSetMask__I },
    { unitMtnGetRootName, (void (*)())Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_ },
    { unitGetStateName, (void (*)())Java_xeno_Unit_getState__ },
    { unitGetPivotName, (void (*)())Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_ },
    { unitSetPivotName, (void (*)())Java_xeno_Unit_setPivot__FFF },
    { unitGetAxisName, (void (*)())Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_ },
    { unitSetAxisName, (void (*)())Java_xeno_Unit_setAxis__FFFF },
    { unitSuspendName, (void (*)())Java_xeno_Unit_suspend__I },
    { unitResumeName, (void (*)())Java_xeno_Unit_resume__I },
    { unitInitElevatorFuncName, (void (*)())Java_xeno_Unit_initElevatorFunc__ },
    { unitMapShadowName, (void (*)())Java_xeno_Unit_map_shadow__I },
    { unitRenderCommandName, (void (*)())Java_xeno_Unit_renderCommand__I },
    { unitSetFilterName, (void (*)())Java_xeno_Unit_setFilter__I },
    { unitSetFilterParamName, (void (*)())Java_xeno_Unit_setFilterParam__aF },
    { unitSetShadowName, (void (*)())Java_xeno_Unit_setShadow__II },
    { unitShadowClipScaleName, (void (*)())Java_xeno_Unit_shadow_clip_scale__F },
    { unitShadowMapIdName, (void (*)())Java_xeno_Unit_shadow_map_id__I },
    { unitShadowMapResetName, (void (*)())Java_xeno_Unit_shadow_map_reset__ },
    { unitSetSortOffsetName, (void (*)())Java_xeno_Unit_setSortOffset__F },
    { unitSetClipName, (void (*)())Java_xeno_Unit_setClip__I },
    { unitSetMonitorPrioName, (void (*)())Java_xeno_Unit_setMonitorPrio__I },
    { sceneStartName, (void (*)())Java_xeno_Scene_start__ILjava_lang_Object_ },
    { sceneStopName, (void (*)())Java_xeno_Scene_stop__ },
    { stageStartName, (void (*)())Java_xeno_Stage_start__ILjava_lang_Object_ },
    { stageStopName, (void (*)())Java_xeno_Stage_stop__ },
    { stagePlayName, (void (*)())Java_xeno_Stage_play__Ljava_lang_String_ },
    { stageSetPartsLastName, (void (*)())Java_xeno_Stage_setPartsLast__I },
    { stageSetPartsLastResetName, (void (*)())Java_xeno_Stage_setPartsLastReset__ },
    { stageSetColorName, (void (*)())Java_xeno_Stage_setColor__FFF },
    { stageSetFadeName, (void (*)())Java_xeno_Stage_setFade__IIFFF },
    { stageSetEventFadeName, (void (*)())Java_xeno_Stage_setEventFade__IFFFIFFF },
    { stageSetFrameRenderName, (void (*)())Java_xeno_Stage_setFrameRender__II },
    { stageSetFadeCancelName, (void (*)())Java_xeno_Stage_setFadeCancel__I },
    { stageSetVisibleName, (void (*)())Java_xeno_Stage_setVisible__IZ },
    { stageSetCfbgName, (void (*)())Java_xeno_Stage_setCFBG__II_F },
    { stageSetEffectRenderName, (void (*)())Java_xeno_Stage_setEffectRender__I },
    { stageRenderCommandName, (void (*)())Java_xeno_Stage_renderCommand__I },
    { stageSetBgColorName, (void (*)())Java_xeno_Stage_setBgColor__FFF },
    { stageSetBgClipName, (void (*)())Java_xeno_Stage_setBgClip__I },
    { stageClrBackBufferName, (void (*)())Java_xeno_Stage_clrBackBuffer__ },
    { 0x00000000, (void (*)())0x00000000 },
};

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives of this TU show
 * (a1 read as the argument block, a2 written as the result); these
 * wrappers only need the tag.
 */

typedef struct JThread JThread;

extern void SCRIPT_sceneChangeTimeSet(int value);

extern void CharactorAllRecovery(void);

extern void AgwsAllRecovery(void);

extern void tyaCaptureEnd(void);

/* The call block of a native taking three plain ints (java signature "(III)"). */

typedef struct {
    int first;
    int second;
    int third;
} IntTripleCall;

/*
 * An 8-byte-aligned view of Vector4, so a whole-vector assignment compiles
 * to the aligned ld/sd pair the original uses instead of an unaligned
 * ldl/ldr sequence (config/compiler-patterns.json CP-0237, same technique
 * as src/main/pp_init.c's PpVector4).
 */

typedef union RuntimeVector4 {
    Vector4 vector;
    unsigned long long words[2];
} RuntimeVector4;

/*
 * A partial, TU-local view of the engine's actor record (the same object
 * fully recovered as `Actor` in src/main/chr.h, src/main/near_dir.h and
 * src/main/set_motion.h for main's chr/near_dir/set_motion TUs) for the two
 * members scriptReset_evsExit touches: the position vector at +0x10 and the
 * status flags word at +0x4D0, both documented for the same object in
 * chr.h. Named separately from those TUs' `Actor` tag because it collides
 * with the canonical spelling elsewhere (config/header-canon.json).
 */

typedef struct RuntimeActor {
    unsigned char unmodeled_00[0x10];
    RuntimeVector4 position;              /* +0x10, ld/sd at
                                              0x002f76bc..0x002f76d8
                                              (scriptReset_evsExit) */
    unsigned char unmodeled_20[0x86 - 0x20];
    short state_flags;
    unsigned char unmodeled_88[0x4D0 - 0x88];
    unsigned short status_flags;          /* +0x4D0, lhu/sh at
                                              0x002f76c0..0x002f76e0
                                              (scriptReset_evsExit) */
    unsigned char unmodeled_4d2[0x8D0 - 0x4D2];
    int resource_guard;
} RuntimeActor;

/*
 * GameLoopState offsets this TU touches. GameLoopState is a TU-local
 * divergent view (config/header-canon.json "GameLoopState"): every unit
 * models only the offsets its own bytes evidence, and no shared header
 * declares it.
 */

typedef struct {
    unsigned char unmodeled_00[4];
    RuntimeActor *player;                  /* +0x04, lw at 0x002f76b0
                                              (scriptReset_evsExit) */
    XglTaskScheduler *task_scheduler;     /* +0x08, lw/beq at 0x002f7350
                                              (setLocation) */
    unsigned short scene_kind;
    unsigned short deferred_jump_pending; /* +0x0E, sh $0 at 0x002f767c */
    unsigned int flags;                   /* +0x10, lw/nor/and/sw at
                                              0x002f74a4..0x002f74bc (disable),
                                              lw/or/sw at 0x002f74c4..0x002f74d8
                                              (enable), lw/sw at
                                              0x002f74e4..0x002f74ec
                                              (getGameState) */
    unsigned char unmodeled_14[4];
    int current_event_id;
    unsigned char unmodeled_1c[4];
    unsigned int shoot_control;           /* +0x20, lw/ori/sw and lw/and/sw at
                                              0x002f78a0..0x002f78c8
                                              (setShootFlag, bit 0),
                                              0x002f78dc..0x002f7904
                                              (setShootHeightCheck, bit 1),
                                              0x002f791c..0x002f7944
                                              (setShootIDCheck, bit 2),
                                              0x002f795c..0x002f7984
                                              (setShootUwaCheck, bit 3) */
    unsigned char unmodeled_24[0x52 - 0x24];
    short entrance;                       /* +0x52, lh at 0x002f7424 */
    int scene_reset_counter;
    unsigned char unmodeled_58[0xA0 - 0x58];
    float fade_red;
    float fade_green;
    float fade_blue;
    float fade_frames;
    unsigned char unmodeled_b0[0xC1 - 0xB0];
    unsigned char player_flags;           /* +0xC1, lbu/ori/sb at
                                              0x002f7448..0x002f7460
                                              (setPlayerControl, bit 0x20) */
    unsigned char unmodeled_c2[0x29F44 - 0xC2];
    float shoot_range;                    /* +0x29F44, swc1 at 0x002f799c */
    unsigned char unmodeled_29f48[0x2A018 - 0x29F48];
    int active_event_id;                  /* +0x2A018, lw at 0x002f8730 */
    unsigned char unmodeled_2a01c[0x2A020 - 0x2A01C];
    RuntimeVector4 saved_position;        /* +0x2A020, ld/sd at
                                              0x002f76bc..0x002f76d8
                                              (scriptReset_evsExit) */
} GameLoopStateRuntimeView;

extern GameLoopStateRuntimeView GameLoopState;

/*
 * A partial view of the script VM thread (JThread, fully recovered in
 * src/main/chr.h for main/tu248) for the three words evsExit touches: the
 * state word at +0x24 and the resume/frame counters at +0x3C/+0x3E chr.h
 * documents for the same object. Modeled separately here because chr.h is
 * TU-local to tu248.
 */

typedef struct {
    unsigned char unmodeled_00[0x24];
    unsigned int flags;              /* +0x24 */
    unsigned char unmodeled_28[0x3C - 0x24 - 4];
    unsigned short resume_frames;    /* +0x3C */
    unsigned short frame_depth;      /* +0x3E */
} RuntimeThreadState;

/* setLocation below touches the same three RuntimeThreadState words as
   evsExit: resume_frames takes frame_depth and flags gains bit 0x8
   (native-call block) and 0x1 (generic wait) (0x002f7388..0x002f739c). */

/* The call block of a native taking two plain ints (java signature "(II)"). */

typedef struct {
    int first;
    int second;
} IntPairCall;

extern int xglFlagsGet(int group, int bit);

/* The call block of Java_xeno_util_Runtime_setFlags__III, the xglFlagsSet
   operands the native passes through unchanged and in order. */

typedef struct {
    int bit_offset;
    int bit_count;
    int value;
} FlagsSetCall;

extern int xglFlagsSet(int bit_offset, int bit_count, int value);

extern void LoadMapOnly(int map_id);

extern void MapChange2(int map_id);

extern int MapGetNo(void);

/* The script VM's 32-entry general register file, indexed with the low 5
   bits of the requested register number (VMRegister[index & 0x1F]). */

int VMRegister[32];

/* The call block of a native taking three plain floats (java signature "(FFF)"). */

typedef struct {
    float walk_threshold;
    float run_threshold;
    float vector_rate;
} PlayerMoveParamCall;

extern void GameCfPlayerMoveParamSet(float walk_threshold, float run_threshold,
                                      float vector_rate);

/*
 * A java.lang.String argument, as CaptureStart/setEventTimer below read it:
 * offset +0x00 (the class word every object reference begins with, per
 * shared.h) is untouched by this TU, and +0x04 holds the interned string
 * record the String was loaded from (lw v1,4(v0) at 0x002f7514/0x002f78ac).
 */

typedef struct {
    void *unmodeled_00;
    struct InternedString *value;
} StringRef;

/*
 * The interned constant-string record `StringRef.value` points to, modeled
 * here only for the `bytes` pointer both natives below read at +0x08 (lw
 * v0,8(v0) at 0x002f7518/0x002f78b0). Same object as `ConstString` of
 * src/main/string_utf_get_hash.h (main/tu224): next-in-bucket link and
 * packed hash/length occupy the same leading 8 bytes there.
 */

struct InternedString {
    unsigned char unmodeled_00[4];
    int length;
    char *bytes;
};

/* The call block of a native taking (java.lang.String, int). */

typedef struct {
    StringRef *string;
    int value;
} StringArgCall;

extern void tyaCaptureStart(const char *name, int count);

extern void Java_xeno_util_Runtime_jump_sub(int destination, int argument);

/* The deferred script jump scriptReset_jump below executes once scheduled. */

typedef struct {
    int destination;
    int argument;
} DeferredJumpCall;

static DeferredJumpCall args_jump;

extern void (*jthreadResetFunc)(void);

extern void SCRIPT_talkIgnoreSet(void);

/* The call block of a native taking four plain ints (java signature "(IIII)"). */

typedef struct {
    int first;
    int second;
    int third;
    int fourth;
} IntQuadCall;

extern void GameDefocusQuickSet(int first, int second, int third, int fourth);

/*
 * A partial, TU-local view of the Java int-array object (the same object
 * fully recovered as `VMArray` in src/main/init_vm.c) for the `data` pointer
 * setDefocus below reads at +0x08 (lw at 0x002f7800).
 */

typedef struct {
    unsigned char unmodeled_00[8];
    int *data;                            /* +0x08 */
} RuntimeIntArray;

/* The call block of setDefocus (java signature "(II[I)"): two plain ints and
   an optional int-array reference, null when the caller passes none. */

typedef struct {
    int mode;
    int strength;
    RuntimeIntArray *table;
} DefocusCall;

extern void GameDefocusSet(int mode, int strength, int *table);

/* A defocus parameter table of seventeen-entry rows, one word ahead of the
   symbol (index 0 is untouched by this TU). */

extern int GameDefocusParam[];

extern void setEventTimerTaskEntry(const char *method_reference, int countdown);

extern void MapChangeResource(int map_id, int resource_id);

extern int dataBoxInc(int item, int count);

extern int CreateEvtItemGetTask(int item, int count);

extern int dataBoxDec(int item, int count);

extern int dataBoxChk(int item, int count);

extern int dataMoneyBoxInc(int amount);

extern int dataMoneyBoxDec(int amount);

extern int dataMoneyBoxChk(void);

extern void sefProgressEffect(int frame_count);

extern void GameStateRestoreCameraLight(void);

extern void GameStateSaveCameraLight(void);

extern void MenuShopMain(int shop);

/*
 * The leader's battle id: read directly out of SaveData (lui/lhu at
 * 0x002f7bd0/0x002f7bd4, no call) rather than through PartyDataGet(), which
 * getPartyDataOfs below shows returns SaveData + 0x10078, so this offset is
 * that region's own +0x2C.
 */

typedef struct RuntimeSavedPartyPrefix {
    unsigned char unmodeled_00[0x100a4];
    unsigned short leader_battle_id;
} RuntimeSavedPartyPrefix;

/* Converts a Java character id to the one-based battle-party index used by
   the Party* entry points below. */

extern void PartyFriendOn(int battle_index);

extern void PartyFriendOff(int battle_index);

extern int PartyFriendCheck(int battle_index);

extern void PartyLockPartyOn(int battle_index);

extern void PartyLockPartyOff(int battle_index);

extern int PartyLockPartyCheck(int battle_index);

extern void PartyOutFriendOn(int battle_index);

extern void PartyOutFriendOff(int battle_index);

extern int PartyOutFriendCheck(int battle_index);

extern void PartyTakeAgwsOn(int battle_index);

extern void PartyTakeAgwsOff(int battle_index);

extern int PartyTakeAgwsCheck(int battle_index);

extern void PartyBattleChange(int battle_index);

extern void nmlModelSetMpeg2CrossFadeTime(int time);

static int s_nMpeg2ReserveCrossFade = 0;

extern void UmnMailMain(int mail);

extern void xglCullingIgnore(void);

extern void xglCullingIgnoreOff(void);

static void scriptReset_evsExit(void);

extern void dataEtherTecSet(int value);

/* Runtime native lookup pairs: method signature followed by its entry point. */

/* Retail's local table precedes the native binding table in the data section. */

static char base_1[18] = "data\\movie\\mpeg2\\";

/* The call block of a native taking three plain ints (java signature "(III)"). */

/* setLocation below touches the same three RuntimeThreadState words as
   evsExit: resume_frames takes frame_depth and flags gains bit 0x8
   (native-call block) and 0x1 (generic wait) (0x002f7388..0x002f739c). */

extern unsigned char xglFontLoad(int font, int option);

extern void GameStateSave(void);

extern void GameStateRestore(void);

extern void func_00A0B728(int map);

extern void func_00A0B708(int monster_set);

extern void func_A22668(void);

extern void ACT_resourceInit(void);

extern void ACT_initScene(void);

extern void ACT_initMTNResource(void);

extern void nmlModelSetFadeIn(int frames, int option);

extern void FX_ScreenMask(int mask, unsigned int color, int frames, int mode);

/* The call block of a native taking two plain ints (java signature "(II)"). */

/* The call block of Java_xeno_util_Runtime_setFlags__III, the xglFlagsSet
   operands the native passes through unchanged and in order. */

/* The script VM's 32-entry general register file, indexed with the low 5
   bits of the requested register number (VMRegister[index & 0x1F]). */

/* The call block of a native taking three plain floats (java signature "(FFF)"). */

extern char *GetItemName(int item, int language);

extern void *classByte;

extern void *newArray(void *element_class, int length);

extern unsigned int strlen(const char *string);

/* The call block of a native taking (java.lang.String, int). */

extern void xglSoundStreamStop(int stream);

extern void GameMovieStop(void);

extern void xglSoundSequenceFadeOut2(int sequence, int duration);

extern void InitCfSystem(void);

extern void UnduInit(unsigned char *data);

extern void SCRIPT_reset(int mode);

extern void GameResourceLoad(int event);

extern void ACT_setModelWrapper(void *actor, int model);

extern void ACT_setHumanHand(void *actor, int hand);

/* The deferred script jump scriptReset_jump below executes once scheduled. */

/* The call block of a native taking four plain ints (java signature "(IIII)"). */

/* The call block of setDefocus (java signature "(II[I)"): two plain ints and
   an optional int-array reference, null when the caller passes none. */

/* A defocus parameter table of seventeen-entry rows, one word ahead of the
   symbol (index 0 is untouched by this TU). */

extern void xglSleep(void);

extern void GameCfPlayerLoadResource(int load);

/* Converts a Java character id to the one-based battle-party index used by
   the Party* entry points below. */

typedef struct {
    int mode;
    float power;
    float time;
    float z;
    float w;
} RuntimeWindCall;

extern void EXM_StopWind(void);

extern void EXM_SetShakePower(float power);

extern void EXM_SetShakeTime(float time);

extern void EXM_SetDirectionalWind(const Vector4 *wind);

extern void EXM_SetPointWind(const Vector4 *wind);

extern int SCRIPT_fadeGet(void);

extern int GameMpeg2Play(const char *path, int mode);

static char ext_2_004DC078[5] = ".pss";

extern void Vibration_Stop(void);

extern void xglPadRead(void);

extern void Game_Data_Push(void);

extern void Game_Data_Pop(void);

extern void ACT_init(void);

extern void xglCdLoadOverlay(int overlay);

extern void func_00A01EA8(void);

extern void func_00A00338(void);

extern void func_00A00070(void);

extern void xglRenderClearDepth(void);

static __inline__ unsigned short *runtime_party_id_table(void)
{
    static unsigned short tbl[] = {
        0x0003, 0x0002, 0x0001, 0x0007, 0x0006, 0x0004, 0x0005,
        0x0127, 0x0128, 0x010C, 0x0120, 0x0121,
        0xF013, 0xF014, 0xF015, 0xF016,
        0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006,
        0x2101, 0x2102, 0x2103, 0x2104, 0x2105, 0x210A,
        0x2201, 0xF014, 0xF015, 0xF016, 0
    };
    return tbl;
}

void Java_xeno_util_Runtime_execBattle__II(JThread *thread, int *arguments)
{
    xglFontLoad(1, 0);
    GameStateSave();
    func_00A0B728(arguments[0]);
    func_00A0B708(arguments[1]);
    func_A22668();
    ACT_resourceInit();
    ACT_initScene();
    ACT_initMTNResource();
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0, 0x7F000000 + ((int)(GameLoopState.fade_red * 127.0f) << 16)
                            + ((int)(GameLoopState.fade_green * 127.0f) << 8)
                            + (int)(GameLoopState.fade_blue * 127.0f),
                  (int)GameLoopState.fade_frames, 3);
    GameStateRestore();
}

void Java_xeno_util_Runtime_getFlags__II(JThread *thread, IntPairCall *arguments,
                                         int *result)
{
    *result = xglFlagsGet(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_setFlags__III(JThread *thread, FlagsSetCall *arguments,
                                          unsigned int *result)
{
    xglFlagsSet(arguments->bit_offset, arguments->bit_count, arguments->value);
}

void Java_xeno_util_Runtime_setLocation__III(RuntimeThreadState *thread,
                                             IntTripleCall *arguments,
                                             unsigned int *result)
{
    int destination = arguments->first;

    if (arguments->third == 0 || GameLoopState.task_scheduler == 0) {
        MapChange2(destination);
        return;
    }
    LoadMapOnly(destination);
    thread->resume_frames = thread->frame_depth;
    thread->flags |= 9;
}

void Java_xeno_util_Runtime_getLocation__(JThread *thread, void *arguments, int *result)
{
    *result = MapGetNo();
}

void Java_xeno_util_Runtime_setRegister__II(JThread *thread, IntPairCall *arguments,
                                            unsigned int *result)
{
    VMRegister[arguments->first & 0x1F] = arguments->second;
}

void Java_xeno_util_Runtime_getRegister__I(JThread *thread, int *arguments, int *result)
{
    *result = VMRegister[*arguments & 0x1F];
}

void Java_xeno_util_Runtime_getEntrance__(JThread *thread, void *arguments, int *result)
{
    *result = GameLoopState.entrance;
}

void Java_xeno_util_Runtime_setPlayerControl__Z(JThread *thread, unsigned char *arguments,
                                                unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.flags &= ~0x8000;
        GameLoopState.player_flags |= 0x20;
    } else {
        GameLoopState.flags |= 0x8000;
    }
}

void Java_xeno_util_Runtime_setPlayerMoveParam__FFF(JThread *thread,
                                                     PlayerMoveParamCall *arguments,
                                                     unsigned int *result)
{
    GameCfPlayerMoveParamSet(arguments->walk_threshold, arguments->run_threshold,
                             arguments->vector_rate);
}

void Java_xeno_util_Runtime_disable__I(JThread *thread, unsigned int *arguments,
                                       unsigned int *result)
{
    GameLoopState.flags &= ~*arguments;
}

void Java_xeno_util_Runtime_enable__I(JThread *thread, unsigned int *arguments,
                                      unsigned int *result)
{
    GameLoopState.flags |= *arguments;
}

void Java_xeno_util_Runtime_getGameState__(JThread *thread, void *arguments, int *result)
{
    *result = GameLoopState.flags;
}

void Java_xeno_util_Runtime_CaptureEnd__(JThread *thread, void *arguments,
                                         unsigned int *result)
{
    tyaCaptureEnd();
}

void Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I(JThread *thread,
                                                               StringArgCall *arguments,
                                                               unsigned int *result)
{
    tyaCaptureStart(arguments->string->value->bytes, arguments->value);
}

void Java_xeno_util_Runtime_jump_sub(int destination, int entrance)
{
    int sequence;
    tyaCaptureEnd();
    xglSoundStreamStop(0);
    GameMovieStop();
    GameDefocusSet(-1, 0, 0);
    if (!(destination & 0x10000)) {
        for (sequence = 0; sequence < 8; sequence++) {
            xglSoundSequenceFadeOut2(sequence, 1000);
        }
    }
    if (entrance >= 0) {
        GameLoopState.entrance = entrance;
    }
    GameLoopState.current_event_id = destination & 0xFFF0FFFF;
    InitCfSystem();
    GameLoopState.scene_reset_counter = 0;
    UnduInit(0);
    SCRIPT_reset(0);
    if ((unsigned int)(GameLoopState.current_event_id - 9998) < 2) {
        GameLoopState.flags |= 0x88000000;
    }
    GameResourceLoad(GameLoopState.current_event_id);
    if (GameLoopState.player && GameLoopState.scene_kind == 2) {
        ACT_setModelWrapper(GameLoopState.player, 0);
        ACT_setHumanHand(GameLoopState.player, 0);
    }
}

static void scriptReset_jump(void)
{
    GameLoopState.deferred_jump_pending = 0;
    Java_xeno_util_Runtime_jump_sub(args_jump.destination, args_jump.argument);
}

static void scriptReset_evsExit(void)
{
    GameLoopState.deferred_jump_pending = 0;
    Java_xeno_util_Runtime_jump_sub(args_jump.destination, args_jump.argument);
    GameLoopState.player->position = GameLoopState.saved_position;
    GameLoopState.player->status_flags |= 0x1000;
}

void Java_xeno_util_Runtime_jumpCF__II(RuntimeThreadState *thread, IntPairCall *arguments,
                                       unsigned int *result)
{
    union ArgWord {
        int value;
        void (*func)(void);
    } *words = (void *)arguments;
    void (**reset_slot)(void) = &jthreadResetFunc;

    args_jump.destination = arguments->first;
    args_jump.argument = words[1].value;
    *reset_slot = scriptReset_jump;
    SCRIPT_talkIgnoreSet();
    thread->resume_frames = thread->frame_depth;
    thread->flags |= 9;
}

void Java_xeno_util_Runtime_jumpEvent__I(RuntimeThreadState *thread, IntPairCall *arguments,
                                         unsigned int *result)
{
    int entrance;
    unsigned int flags;

    args_jump.destination = arguments->first + 0x10000000;
    entrance = arguments->second;
    jthreadResetFunc = scriptReset_jump;
    args_jump.argument = entrance;
    SCRIPT_talkIgnoreSet();
    flags = thread->flags | 9;
    thread->resume_frames = thread->frame_depth;
    thread->flags = flags;
}

void Java_xeno_util_Runtime_setDefocusQuick__IIII(JThread *thread, IntQuadCall *arguments,
                                                  unsigned int *result)
{
    GameDefocusQuickSet(arguments->first, arguments->second, arguments->third,
                        arguments->fourth);
}

void Java_xeno_util_Runtime_setDefocus__IIaI(JThread *thread, DefocusCall *arguments,
                                             unsigned int *result)
{
    if (arguments->table == 0) {
        GameDefocusSet(arguments->mode, arguments->strength, 0);
    } else {
        GameDefocusSet(arguments->mode, arguments->strength, arguments->table->data);
    }
}

void Java_xeno_util_Runtime_setDefocusParam__III(JThread *thread, IntTripleCall *arguments,
                                                 unsigned int *result)
{
    GameDefocusParam[1 + arguments->first * 17 + arguments->second] = arguments->third;
}

void Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I(JThread *thread,
                                                                StringArgCall *arguments,
                                                                unsigned int *result)
{
    setEventTimerTaskEntry(arguments->string->value->bytes, arguments->value);
}

void Java_xeno_util_Runtime_setMap__II(JThread *thread, IntPairCall *arguments,
                                       unsigned int *result)
{
    MapChangeResource(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_setShootFlag__Z(JThread *thread, unsigned char *arguments,
                                            unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 1;
    } else {
        GameLoopState.shoot_control &= ~1;
    }
}

void Java_xeno_util_Runtime_setShootHeightCheck__Z(JThread *thread, unsigned char *arguments,
                                                    unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 2;
    } else {
        GameLoopState.shoot_control &= ~2;
    }
}

void Java_xeno_util_Runtime_setShootIDCheck__Z(JThread *thread, unsigned char *arguments,
                                               unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 4;
    } else {
        GameLoopState.shoot_control &= ~4;
    }
}

void Java_xeno_util_Runtime_setShootUwaCheck__Z(JThread *thread, unsigned char *arguments,
                                                unsigned int *result)
{
    if (*arguments != 0) {
        GameLoopState.shoot_control |= 8;
    } else {
        GameLoopState.shoot_control &= ~8;
    }
}

void Java_xeno_util_Runtime_setShootRange__F(JThread *thread, float *arguments,
                                             unsigned int *result)
{
    GameLoopState.shoot_range = *arguments;
}

void Java_xeno_util_Runtime_addItem__II(JThread *thread, IntPairCall *arguments,
                                        signed char *result)
{
    *result = (signed char) dataBoxInc(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_addItemWin__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) CreateEvtItemGetTask(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_removeItem__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) dataBoxDec(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_checkItem__II(JThread *thread, IntPairCall *arguments,
                                          int *result)
{
    *result = dataBoxChk(arguments->first, arguments->second);
}

void Java_xeno_util_Runtime_getItemName__II(JThread *thread, int *arguments,
                                          int *result)
{
    StringRef *string;
    struct InternedString *bytes;
    char *name = GetItemName(arguments[0], arguments[1]);
    string = newObject(classString);
    bytes = newArray(classByte, 255);
    string->value = bytes;
    bytes->bytes = name;
    bytes->length = strlen(name);
    *result = (int)string;
}

void Java_xeno_util_Runtime_addGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxInc(*arguments);
}

void Java_xeno_util_Runtime_removeGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxDec(*arguments);
}

void Java_xeno_util_Runtime_checkGold__(JThread *thread, void *arguments, int *result)
{
    *result = dataMoneyBoxChk();
}

void Java_xeno_util_Runtime_progressEffect__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    sefProgressEffect(*arguments);
}

void Java_xeno_util_Runtime_enterShop__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    GameStateSaveCameraLight();
    MenuShopMain(*arguments);
    GameStateRestoreCameraLight();
}

void Java_xeno_util_Runtime_getLeader__(JThread *thread, void *arguments, int *result)
{
    *result = ((const RuntimeSavedPartyPrefix *)SaveData)->leader_battle_id;
}

static char *getPartyDataOfs(unsigned int selector)
{
    int type = (selector >> 16) & 0xff;
    unsigned int offset = selector & 0xffff;
    char *party_data = (char *)PartyDataGet();

    char *selected;

    switch (type) {
    case 1: selected = party_data + 0x30; break;
    case 2: selected = party_data + 0x3c; break;
    case 3: selected = party_data + 0x140; break;
    case 0:
    default: selected = party_data; break;
    }
    return selected + offset;
}

void Java_xeno_util_Runtime_getPartyData__I(JThread *thread, int *arguments, int *result)
{
    int width = (unsigned int)*arguments >> 24;
    void *data = getPartyDataOfs(*arguments);
    unsigned char *bytes = data;
    short *halfwords = data;
    int *words = data;
    long long *wide_words = data;

    switch (width) {
    case 0:
    default:
        *result = *bytes;
        break;
    case 1:
        *result = *halfwords;
        break;
    case 2:
        *result = *words;
        break;
    case 3:
        *result = *wide_words;
        break;
    }
}

void Java_xeno_util_Runtime_setPartyData__II(JThread *thread,
                                           IntPairCall *arguments)
{
    int width = (unsigned int)arguments->first >> 24;
    void *data = getPartyDataOfs(arguments->first);
    unsigned char *bytes = data;
    unsigned short *halfwords = data;
    int *words = data;
    long long *wide_words = data;
    switch (width) {
    case 0:
    default:
        *bytes = arguments->second;
        break;
    case 1:
        *halfwords = arguments->second;
        break;
    case 2:
        *words = arguments->second;
        break;
    case 3:
        *wide_words = arguments->second;
        break;
    }
    if (arguments->first == 0x0100002C) {
        RuntimeActor *player = GameLoopState.player;
        int resource_guard = player->resource_guard;
        if (player->state_flags != arguments->second) {
            player->resource_guard = 0;
            xglSleep();
            xglSleep();
            player->resource_guard = resource_guard;
            GameCfPlayerLoadResource(1);
        }
    }
}

static int cid2bic(int character_id)
{
    unsigned short *tbl = runtime_party_id_table();
    int index;

    for (index = 0; tbl[index] != 0; index++) {
        if (tbl[index] == character_id) {
            return index + 1;
        }
    }
    return 0;
}

void Java_xeno_util_Runtime_setFriend__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    PartyFriendOn(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_resetFriend__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyFriendOff(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_checkFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyFriendCheck(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_setLockParty__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyLockPartyOn(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_resetLockParty__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyLockPartyOff(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_checkLockParty__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyLockPartyCheck(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_setOutFriend__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyOutFriendOn(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_resetOutFriend__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyOutFriendOff(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_checkOutFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyOutFriendCheck(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_setTakeAgws__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyTakeAgwsOn(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_resetTakeAgws__I(JThread *thread, int *arguments,
                                             unsigned int *result)
{
    PartyTakeAgwsOff(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_checkTakeAgws__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyTakeAgwsCheck(cid2bic(*arguments));
}

void Java_xeno_util_Runtime_battleChangeParty__I(JThread *thread, int *arguments,
                                                  unsigned int *result)
{
    PartyBattleChange(cid2bic(*arguments));
}

/* Native argument and result words share the VM operand-slot storage. */

typedef struct RuntimeIdLightSet {
    unsigned char unmodeled_00[0xF0];
} RuntimeIdLightSet;

extern RuntimeIdLightSet GameIdLight[];

extern void xglLightIntensityAmbient(void *light, const void *intensity);
extern void xglLightIntensityParallel(void *light, unsigned int index,
                                      const void *intensity);
extern void xglLightAngle(void *light, unsigned int index, const Vector4 *angle);
extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

static inline void runtimeLightVector(Vector4 *value, float x, float y, float z)
{
    value->x = x;
    value->y = y;
    value->z = z;
    value->w = 1.0f;
}

void Java_xeno_util_Runtime_setIdLightCol__IIFFF(JThread *thread,
                                               VMSlot *arguments,
                                               VMSlot *result)
{
    Vector4 color;
    int slot = arguments[0].i;
    int light = arguments[1].i;

    runtimeLightVector(&color, arguments[2].f, arguments[3].f, arguments[4].f);
    if (slot > 0) {
        slot--;
    }
    if (light == 0) {
        xglLightIntensityAmbient(&GameIdLight[slot], &color);
    } else {
        xglLightIntensityParallel(&GameIdLight[slot], light - 1, &color);
    }
}

void Java_xeno_util_Runtime_setIdLightDir__IIFFF(JThread *thread,
                                               VMSlot *arguments,
                                               VMSlot *result)
{
    Vector4 angle;
    int slot = arguments[0].i;
    int light = arguments[1].i;

    runtimeLightVector(&angle, arguments[2].f, arguments[3].f, arguments[4].f);
    if (slot > 0) {
        slot--;
    }
    if (light > 0) {
        xglLightAngle(&GameIdLight[slot], light - 1, &angle);
    }
}

void Java_xeno_util_Runtime_setIdLightVec__IIFFF(JThread *thread,
                                               VMSlot *arguments,
                                               VMSlot *result)
{
    Vector4 direction;
    int slot = arguments[0].i;
    int light = arguments[1].i;

    runtimeLightVector(&direction, arguments[2].f, arguments[3].f, arguments[4].f);
    if (slot > 0) {
        slot--;
    }
    if (light > 0) {
        xglLightDirection(&GameIdLight[slot], light - 1, &direction);
    }
}

void Java_xeno_util_Runtime_setWindParam__IFFFF(JThread *thread,
                                              RuntimeWindCall *arguments)
{
    Vector4 wind;
    wind.x = arguments->power;
    wind.y = arguments->time;
    wind.z = arguments->z;
    wind.w = arguments->w;
    switch (arguments->mode) {
    case 0:
        EXM_StopWind();
        break;
    case 1:
        EXM_SetShakePower(wind.x);
        EXM_SetShakeTime(wind.y);
        break;
    case 2:
        EXM_SetDirectionalWind(&wind);
        break;
    case 3:
        EXM_SetPointWind(&wind);
        break;
    }
}

void Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_(JThread *thread,
                                                   StringRef **arguments)
{
    char path[64];
    int mode;

    char *destination = path;
    const char *source = base_1;
    while ((*destination = *source) != 0) {
        destination++;
        source++;
    }
    source = (*arguments)->value->bytes;
    while ((*destination = *source) != 0) {
        destination++;
        source++;
    }
    source = ext_2_004DC078;
    while ((*destination = *source) != 0) {
        destination++;
        source++;
    }
    mode = 3;
    if (SCRIPT_fadeGet()) {
        mode = 2;
    }
    if (s_nMpeg2ReserveCrossFade) {
        s_nMpeg2ReserveCrossFade = 0;
        mode = 4;
    }
    GameMpeg2Play(path, mode);
}

void Java_xeno_util_Runtime_mpeg2AfterCrossFade__I(JThread *thread, int *arguments,
                                                    unsigned int *result)
{
    int time = *arguments;

    s_nMpeg2ReserveCrossFade = 1;
    nmlModelSetMpeg2CrossFadeTime(time);
}

void Java_xeno_util_Runtime_mailFlag__I(JThread *thread, int *arguments, int *result)
{
    int value = 0;
    unsigned char *mail = UmnMailDataGet(*arguments & 0xFFFFFF);

    switch (*arguments >> 24) {
    case 0:
        value = mail[0] & 1;
        break;
    case 1: {
        unsigned int flags = mail[0];

        value = (flags >> 1) & 1;
        break;
    }
    case 2:
        value = mail[1] == 0 ? 0 : (mail[1] & 0x80) ? 2 : 1;
        break;
    case 3:
        value = (mail[0] & 0x30) >> 4;
        break;
    case 4:
        mail[0] |= 1;
        UmnMailBoxSet(*arguments & 0xFFFFFF);
        xglSoundEffectNormalID(36, 0);
        Vibration_Set_Weak(30);
        break;
    case 5: {
        signed char *boxes = (signed char *)(SaveData + RUNTIME_UMN_BOX_OFFSET);
        int count = 0;
        int i;

        for (i = 0; i < RUNTIME_UMN_BOX_SLOTS; i++) {
            if (boxes[i] >= 0) {
                count++;
            } else {
                break;
            }
        }
        switch (*arguments & 0xFFFFFF) {
        case 0:
            value = count;
            break;
        case 1:
            boxes = (signed char *)(SaveData + RUNTIME_UMN_BOX_OFFSET);
            value = 0;
            for (i = 0; i < count; i++) {
                mail = UmnMailDataGet(boxes[i]);
                if ((mail[0] & 2) == 0) {
                    value++;
                }
            }
            break;
        }
        break;
    }
    }
    *result = value;
}

void Java_xeno_util_Runtime_mailExec__I(JThread *thread, int *arguments,
                                        unsigned int *result)
{
    GameStateSaveCameraLight();
    xglCullingIgnore();
    UmnMailMain(*arguments);
    xglCullingIgnoreOff();
    GameStateRestoreCameraLight();
}

void Java_xeno_util_Runtime_minigameExec__I(JThread *thread, int *arguments)
{
    GameLoopState.flags |= 0x4000000;
    Vibration_Stop();
    xglPadRead();
    Game_Data_Push();
    ACT_init();
    switch (*arguments) {
    case 12:
        xglCdLoadOverlay(12);
        func_00A01EA8();
        break;
    case 13:
        xglCdLoadOverlay(10);
        func_00A00338();
        xglRenderClearDepth();
        break;
    case 14:
        xglCdLoadOverlay(11);
        func_00A00070();
        break;
    }
    xglCdLoadOverlay(1);
    ACT_init();
    Game_Data_Pop();
    GameCfPlayerLoadResource(1);
    GameLoopState.flags &= ~0x4000000;
}

void Java_xeno_util_Runtime_setMenuLock__(JThread *thread, void *arguments,
                                          unsigned int *result)
{
    /*
     * Arms the script scene-change timer, which SCRIPT_sceneChangeTimeDec
     * counts down once per game frame, with 62 frames.
     */
    SCRIPT_sceneChangeTimeSet(62);
}

void Java_xeno_util_Runtime_evsSetRetPoint__(RuntimeThreadState *thread, void *arguments,
                                             unsigned int *result)
{
    RuntimeActor *player = GameLoopState.player;
    unsigned long long first_pair = player->position.words[0];
    unsigned long long last_pair;

    GameLoopState.active_event_id = GameLoopState.current_event_id;
    GameLoopState.saved_position.words[0] = first_pair;
    last_pair = player->position.words[1];
    GameLoopState.saved_position.words[1] = last_pair;
}

void Java_xeno_util_Runtime_evsExit__(RuntimeThreadState *thread, void *arguments,
                                      unsigned int *result)
{
    /* Blocks the calling thread the same way CHR_sclX/System_waitFor do
       (chr.h): resume_frames takes the current frame_depth and flags gains
       0x8 (native-call block) and 0x1 (generic wait). The updated flags word
       is staged before the resume_frames store and written back after it,
       matching the original instruction order (0x002f873c..0x002f8750). */
    unsigned int flags;

    args_jump.destination = GameLoopState.active_event_id;
    args_jump.argument = -1;
    jthreadResetFunc = scriptReset_evsExit;
    flags = thread->flags | 9;
    thread->resume_frames = thread->frame_depth;
    thread->flags = flags;
}

void Java_xeno_util_Runtime_etherTecSet__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    dataEtherTecSet(*arguments);
}

void Java_xeno_util_Runtime_charAllRecovery__(JThread *thread, void *arguments,
                                              unsigned int *result)
{
    CharactorAllRecovery();
}

void Java_xeno_util_Runtime_AGWSAllRecovery__(JThread *thread, void *arguments,
                                              unsigned int *result)
{
    AgwsAllRecovery();
}

const char jthreadDefaultName[16] = "JTHREAD_default";

const char jthreadDefaultStageName[24] = "JTHREAD_defaultStage";

const char jthreadDefaultSceneName[24] = "JTHREAD_defaultScene";

const char jthreadDefaultChrName[24] = "JTHREAD_defaultChr";

const char jthreadDefaultUnitName[24] = "JTHREAD_defaultUnit";

/* Original storage of this TU without a C reader; typed from its original bytes. */
extern void *tiSymbol[12];
extern const char *tiAlgName[6];

extern const char JTHREADAlgDefaultName[24];
extern const char JTHREADAlgChrName[16];
extern const char JTHREADAlgSceneName[24];
extern const char JTHREADAlgSystemName[24];
extern const char JTHREADAlgUnitName[24];
extern const char JTHREADAlgStageName[24];

const char * tiAlgName[6] = { JTHREADAlgSystemName, JTHREADAlgChrName, JTHREADAlgStageName, JTHREADAlgSceneName, JTHREADAlgDefaultName, JTHREADAlgUnitName };

void JTHREAD_defaultUnit(struct JThread *thread);
void JTHREAD_defaultChr(struct JThread *thread);
void JTHREAD_default(struct JThread *thread);
void JTHREAD_defaultScene(struct JThread *thread);
void JTHREAD_defaultStage(struct JThread *thread);

void *tiSymbol[12] = { (void *)JTHREAD_defaultUnit, (void *)jthreadDefaultUnitName, (void *)JTHREAD_defaultChr, (void *)jthreadDefaultChrName, (void *)JTHREAD_defaultScene, (void *)jthreadDefaultSceneName, (void *)JTHREAD_defaultStage, (void *)jthreadDefaultStageName, (void *)JTHREAD_default, (void *)jthreadDefaultName, 0, 0 };
