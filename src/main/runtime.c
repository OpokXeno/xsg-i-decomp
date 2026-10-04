#include "common.h"
#include "shared.h"
#include "main/party.h"

/* Runtime native lookup pairs: method signature followed by its entry point. */
typedef struct RuntimeNativeBinding {
    const char *signature;
    void (*entry)();
} RuntimeNativeBinding;
extern const char D_004D14F0[];
extern const char D_004D14D0[];
extern const char D_004D14A0[];
extern const char D_004D1470[];
extern const char D_004D1448[];
extern const char D_004D1428[];
extern const char D_004D13E0[];
extern const char D_004D13C0[];
extern const char D_004D13A0[];
extern const char D_004D1380[];
extern const char D_004D1360[];
extern const char D_004D1340[];
extern const char D_004D1320[];
extern const char D_004D1300[];
extern const char D_004D12D0[];
extern const char D_004D12A8[];
extern const char D_004D1280[];
extern const char D_004D1258[];
extern const char D_004D1228[];
extern const char D_004D1200[];
extern const char D_004D11D8[];
extern const char D_004D11B0[];
extern const char D_004D1188[];
extern const char D_004D1168[];
extern const char D_004D1140[];
extern const char D_004D1120[];
extern const char D_004D10F8[];
extern const char D_004D10C8[];
extern const char D_004D10A0[];
extern const char D_004D1078[];
extern const char D_004D1050[];
extern const char D_004D1028[];
extern const char D_004D1000[];
extern const char D_004D0FD8[];
extern const char D_004D0FB0[];
extern const char D_004D0F88[];
extern const char D_004D0F60[];
extern const char D_004D0F30[];
extern const char D_004D0F00[];
extern const char D_004D0ED8[];
extern const char D_004D0EB0[];
extern const char D_004D0E88[];
extern const char D_004D0E60[];
extern const char D_004D0E20[];
extern const char D_004D0DF8[];
extern const char D_004D0DD0[];
extern const char D_004D0DA0[];
extern const char D_004D0D78[];
extern const char D_004D0D48[];
extern const char D_004D0D08[];
extern const char D_004D0CE0[];
extern const char D_004D0CB8[];
extern const char D_004D0C88[];
extern const char D_004D0C58[];
extern const char D_004D0C28[];
extern const char D_004D0C00[];
extern const char D_004D0BD8[];
extern const char D_004D0BB0[];
extern const char D_004D0B88[];
extern const char D_004D0B60[];
extern const char D_004D0B38[];
extern const char D_004D0B10[];
extern const char D_004D0AE8[];
extern const char D_004D0AC0[];
extern const char D_004D0A90[];
extern const char D_004D0A68[];
extern const char D_004D0A40[];
extern const char D_004D0A18[];
extern const char D_004D09F0[];
extern const char D_004D09C8[];
extern const char D_004D09A0[];
extern const char D_004D0978[];
extern const char D_004D0950[];
extern const char D_004D0920[];
extern const char D_004D08F0[];
extern const char D_004D08C8[];
extern const char D_004D0898[];
extern const char D_004D0868[];
extern const char D_004D0840[];
extern const char D_004D0818[];
extern const char D_004D07F0[];
extern const char D_004D07C0[];
extern const char D_004D0790[];
extern const char D_004D0760[];
extern const char D_004D0730[];
extern const char D_004D0700[];
extern const char D_004D06C8[];
extern const char D_004D0698[];
extern const char D_004D0670[];
extern const char D_004D0648[];
extern const char D_004D0620[];
extern const char D_004D05F8[];
extern const char D_004D05D0[];
extern const char D_004D05A8[];
extern const char D_004D0580[];
extern const char D_004D0550[];
extern const char D_004D0520[];
extern const char D_004D04F8[];
extern const char D_004D04D8[];
extern const char D_004D04B8[];
extern const char D_004D0498[];
extern const char D_004D0470[];
extern const char D_004D0450[];
extern const char D_004D0430[];
extern const char D_004D0408[];
extern const char D_004D03E0[];
extern const char D_004D03B0[];
extern const char D_004D0378[];
extern const char D_004D0340[];
extern const char D_004D0318[];
extern const char D_004D02E0[];
extern const char D_004D02A8[];
extern const char D_004D0278[];
extern const char D_004D0258[];
extern const char D_004D0230[];
extern const char D_004D0208[];
extern const char D_004D01E0[];
extern const char D_004D01B8[];
extern const char D_004D0180[];
extern const char D_004D0138[];
extern const char D_004D00F8[];
extern const char D_004D00C0[];
extern const char D_004D0070[];
extern const char D_004D0030[];
extern const char D_004D0008[];
extern const char D_004CFFE8[];
extern const char D_004CFFC0[];
extern const char D_004CFFA0[];
extern const char D_004CFF78[];
extern const char D_004CFF58[];
extern const char D_004CFF38[];
extern const char D_004CFF10[];
extern const char D_004CFEF8[];
extern const char D_004CFED8[];
extern const char D_004CFEC0[];
extern const char D_004CFEA0[];
extern const char D_004CFE80[];
extern const char D_004CFE58[];
extern const char D_004CFE38[];
extern const char D_004CFE18[];
extern const char D_004CFDF0[];
extern const char D_004CFDB8[];
extern const char D_004CFD80[];
extern const char D_004CFD60[];
extern const char D_004CFD40[];
extern const char D_004CFCF0[];
extern const char D_004CFC90[];
extern const char D_004CFC68[];
extern const char D_004CFC48[];
extern const char D_004CFC28[];
extern const char D_004CFC08[];
extern const char D_004CFBE8[];
extern const char D_004CFBC8[];
extern const char D_004CFBA8[];
extern const char D_004CFB88[];
extern const char D_004CFB68[];
extern const char D_004CFB40[];
extern const char D_004CFB18[];
extern const char D_004CFAF0[];
extern const char D_004CFAC8[];
extern const char D_004CFAA0[];
extern const char D_004CFA80[];
extern const char D_004CFA58[];
extern const char D_004CFA38[];
extern const char D_004CFA18[];
extern const char D_004CF9F8[];
extern const char D_004CF9D8[];
extern const char D_004CF9B8[];
extern const char D_004CF998[];
extern const char D_004CF978[];
extern const char D_004CF958[];
extern const char D_004CF930[];
extern const char D_004CF908[];
extern const char D_004CF8E0[];
extern const char D_004CF8C0[];
extern const char D_004CF8A0[];
extern const char D_004CF880[];
extern const char D_004CF858[];
extern const char D_004CF838[];
extern const char D_004CF818[];
extern const char D_004CF7E0[];
extern const char D_004CF7C0[];
extern const char D_004CF798[];
extern const char D_004CF768[];
extern const char D_004CF748[];
extern const char D_004CF728[];
extern const char D_004CF708[];
extern const char D_004CF6E8[];
extern const char D_004CF6C8[];
extern const char D_004CF6A0[];
extern const char D_004CF680[];
extern const char D_004CF660[];
extern const char D_004CF640[];
extern const char D_004CF610[];
extern const char D_004CF5E8[];
extern const char D_004CF5C0[];
extern const char D_004CF598[];
extern const char D_004CF570[];
extern const char D_004CF548[];
extern const char D_004CF528[];
extern const char D_004CF508[];
extern const char D_004CF4D8[];
extern const char D_004CF4B0[];
extern const char D_004CF490[];
extern const char D_004CF468[];
extern const char D_004CF448[];
extern const char D_004CF420[];
extern const char D_004CF400[];
extern const char D_004CF3D8[];
extern const char D_004CF3B0[];
extern const char D_004CF380[];
extern const char D_004CF350[];
extern const char D_004CF320[];
extern const char D_004CF300[];
extern const char D_004CF2E0[];
extern const char D_004CF2C0[];
extern const char D_004CF2A0[];
extern const char D_004CF270[];
extern const char D_004CF240[];
extern const char D_004CF210[];
extern const char D_004CF1E0[];
extern const char D_004CF1C8[];
extern const char D_004CF1B0[];
extern const char D_004CF180[];
extern const char D_004CF150[];
extern const char D_004CF138[];
extern const char D_004CF120[];
extern const char D_004CF0F0[];
extern const char D_004CF0C0[];
extern const char D_004CF0A8[];
extern const char D_004CF090[];
extern const char D_004CF060[];
extern const char D_004CF030[];
extern const char D_004CF000[];
extern const char D_004CEFE8[];
extern const char D_004CEFC8[];
extern const char D_004CEFB0[];
extern const char D_004CEF98[];
extern const char D_004CEF78[];
extern const char D_004CEF58[];
extern const char D_004CEF38[];
extern const char D_004CEF18[];
extern const char D_004CEEF8[];
extern const char D_004CEED8[];
extern const char D_004CEEB8[];
extern const char D_004CEE98[];
extern const char D_004CEE78[];
extern const char D_004CEE58[];
extern const char D_004CEE38[];
extern const char D_004CEE08[];
extern const char D_004CEDE8[];
extern const char D_004CEDC8[];
extern const char D_004CED98[];
extern const char D_004CED78[];
extern const char D_004CED58[];
extern const char D_004CED28[];
extern const char D_004CED10[];
extern const char D_004CECF8[];
extern const char D_004CECE0[];
extern const char D_004CECC8[];
extern const char D_004CECB0[];
extern const char D_004CEC98[];
extern const char D_004CEC68[];
extern const char D_004CEC48[];
extern const char D_004CEC20[];
extern const char D_004CEC08[];
extern const char D_004CEBE8[];
extern const char D_004CEBC8[];
extern const char D_004CEBA8[];
extern const char D_004CEB88[];
extern const char D_004CEB68[];
extern const char D_004CEB50[];
extern const char D_004CEB28[];
extern const char D_004CEB00[];
extern const char D_004CEAD8[];
extern const char D_004CEAB8[];
extern const char D_004CEA90[];
extern const char D_004CEA70[];
extern const char D_004CEA50[];
extern const char D_004CEA30[];
extern const char D_004CEA08[];
extern const char D_004CE9E0[];
extern const char D_004CE9B8[];
extern const char D_004CE988[];
extern const char D_004CE958[];
extern const char D_004CE938[];
extern const char D_004CE908[];
extern const char D_004CE8E8[];
extern const char D_004CE8C8[];
extern const char D_004CE898[];
extern const char D_004CE868[];
extern const char D_004CE848[];
extern const char D_004CE828[];
extern const char D_004CE808[];
extern const char D_004CE7E0[];
extern const char D_004CE7C0[];
extern const char D_004CE7A0[];
extern const char D_004CE780[];
extern const char D_004CE758[];
extern const char D_004CE738[];
extern const char D_004CE710[];
extern const char D_004CE6F0[];
extern const char D_004CE6D0[];
extern const char D_004CE6A8[];
extern const char D_004CE680[];
extern const char D_004CE660[];
extern const char D_004CE638[];
extern const char D_004CE610[];
extern const char D_004CE5F0[];
extern const char D_004CE5D0[];
extern const char D_004CE5B0[];
extern const char D_004CE580[];
extern const char D_004CE550[];
extern const char D_004CE530[];
extern const char D_004CE510[];
extern const char D_004CE4F0[];
extern const char D_004CE4D0[];
extern const char D_004CE4B0[];
extern const char D_004CE490[];
extern const char D_004CE460[];
extern const char D_004CE430[];
extern const char D_004CE400[];
extern const char D_004CE3D0[];
extern const char D_004CE3B0[];
extern const char D_004CE390[];
extern const char D_004CE370[];
extern const char D_004CE350[];
extern const char D_004CE320[];
extern const char D_004CE2F0[];
extern const char D_004CE2D0[];
extern const char D_004CE2B0[];
extern const char D_004CE280[];
extern const char D_004CE250[];
extern const char D_004CE230[];
extern const char D_004CE210[];
extern const char D_004CE1E0[];
extern const char D_004CE1B0[];
extern const char D_004CE180[];
extern const char D_004CE160[];
extern const char D_004CE140[];
extern const char D_004CE120[];
extern const char D_004CE100[];
extern const char D_004CE0E0[];
extern const char D_004CE0C0[];
extern const char D_004CE0A0[];
extern const char D_004CE080[];
extern const char D_004CE060[];
extern const char D_004CE040[];
extern const char D_004CE020[];
extern const char D_004CE000[];
extern const char D_004CDFD0[];
extern const char D_004CDFB8[];
extern const char D_004CDF98[];
extern const char D_004CDF68[];
extern const char D_004CDF48[];
extern const char D_004CDF28[];
extern const char D_004CDEF8[];
extern const char D_004CDED8[];
extern const char D_004CDEB8[];
extern const char D_004CDE98[];
extern const char D_004CDE78[];
extern const char D_004CDE58[];
extern const char D_004CDE20[];
extern const char D_004CDE00[];
extern const char D_004CDDD0[];
extern const char D_004CDDB0[];
extern const char D_004CDD80[];
extern const char D_004CDD60[];
extern const char D_004CDD40[];
extern const char D_004CDD20[];
extern const char D_004CDCF8[];
extern const char D_004CDCD8[];
extern const char D_004CDCB8[];
extern const char D_004CDC98[];
extern const char D_004CDC70[];
extern const char D_004CDC50[];
extern const char D_004CDC28[];
extern const char D_004CDC08[];
extern const char D_004CDBE0[];
extern const char D_004CDBC0[];
extern const char D_004CDBA0[];
extern const char D_004CDB78[];
extern const char D_004CDB48[];
extern const char D_004CDB30[];
extern const char D_004CDB00[];
extern const char D_004CDAE8[];
extern const char D_004CDAB8[];
extern const char D_004CDA98[];
extern const char D_004CDA70[];
extern const char D_004CDA50[];
extern const char D_004CDA30[];
extern const char D_004CDA08[];
extern const char D_004CD9E0[];
extern const char D_004CD9B8[];
extern const char D_004CD998[];
extern const char D_004CD978[];
extern const char D_004CD950[];
extern const char D_004CD928[];
extern const char D_004CD908[];
extern const char D_004CD8E8[];
extern const char D_004CD8C8[];
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

RuntimeNativeBinding default_native[] = {
    { D_004D14F0, (void (*)())Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II },
    { D_004D14D0, (void (*)())Java_xeno_vm_System_sleep__I },
    { D_004D14A0, (void (*)())Java_xeno_vm_System_println__Ljava_lang_String_ },
    { D_004D1470, (void (*)())Java_xeno_vm_System_waitFor__Ljava_lang_Object_ },
    { D_004D1448, (void (*)())Java_xeno_vm_System_methodSignal__I },
    { D_004D1428, (void (*)())Java_xeno_vm_Thread_create__ },
    { D_004D13E0, (void (*)())Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_ },
    { D_004D13C0, (void (*)())Java_xeno_vm_Thread_start__ },
    { D_004D13A0, (void (*)())Java_xeno_vm_Thread_stop__ },
    { D_004D1380, (void (*)())Java_xeno_vm_Math_random__ },
    { D_004D1360, (void (*)())Java_xeno_vm_Math_atan2__FF },
    { D_004D1340, (void (*)())Java_xeno_vm_Math_cos__F },
    { D_004D1320, (void (*)())Java_xeno_vm_Math_sin__F },
    { D_004D1300, (void (*)())Java_xeno_util_Spline_create__ },
    { D_004D12D0, (void (*)())Java_xeno_util_Spline_setCtrlVertex__aFIII },
    { D_004D12A8, (void (*)())Java_xeno_util_Spline_getValue__I },
    { D_004D1280, (void (*)())Java_xeno_util_Format_floatToIntBits__F },
    { D_004D1258, (void (*)())Java_xeno_util_Format_intBitsToFloat__I },
    { D_004D1228, (void (*)())Java_xeno_util_Format_toInt__Ljava_lang_String_ },
    { D_004D1200, (void (*)())Java_xeno_util_Format_toString__C },
    { D_004D11D8, (void (*)())Java_xeno_util_Format_toString__F },
    { D_004D11B0, (void (*)())Java_xeno_util_Format_toString__I },
    { D_004D1188, (void (*)())Java_xeno_util_Format_toString__Z },
    { D_004D1168, (void (*)())Java_xeno_util_Input_create__I },
    { D_004D1140, (void (*)())Java_xeno_util_Input_getButton__ },
    { D_004D1120, (void (*)())Java_xeno_util_Input_getEdge__ },
    { D_004D10F8, (void (*)())Java_xeno_util_Input_getRepeat__ },
    { D_004D10C8, (void (*)())Java_xeno_util_Layout_set__Ljava_lang_Object_I },
    { D_004D10A0, (void (*)())Java_xeno_util_Layout_getManager__I },
    { D_004D1078, (void (*)())Java_xeno_util_Runtime_execBattle__II },
    { D_004D1050, (void (*)())Java_xeno_util_Runtime_getFlags__II },
    { D_004D1028, (void (*)())Java_xeno_util_Runtime_setFlags__III },
    { D_004D1000, (void (*)())Java_xeno_util_Runtime_setLocation__III },
    { D_004D0FD8, (void (*)())Java_xeno_util_Runtime_getLocation__ },
    { D_004D0FB0, (void (*)())Java_xeno_util_Runtime_setRegister__II },
    { D_004D0F88, (void (*)())Java_xeno_util_Runtime_getRegister__I },
    { D_004D0F60, (void (*)())Java_xeno_util_Runtime_getEntrance__ },
    { D_004D0F30, (void (*)())Java_xeno_util_Runtime_setPlayerControl__Z },
    { D_004D0F00, (void (*)())Java_xeno_util_Runtime_setPlayerMoveParam__FFF },
    { D_004D0ED8, (void (*)())Java_xeno_util_Runtime_disable__I },
    { D_004D0EB0, (void (*)())Java_xeno_util_Runtime_enable__I },
    { D_004D0E88, (void (*)())Java_xeno_util_Runtime_getGameState__ },
    { D_004D0E60, (void (*)())Java_xeno_util_Runtime_CaptureEnd__ },
    { D_004D0E20, (void (*)())Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I },
    { D_004D0DF8, (void (*)())Java_xeno_util_Runtime_jumpCF__II },
    { D_004D0DD0, (void (*)())Java_xeno_util_Runtime_jumpEvent__I },
    { D_004D0DA0, (void (*)())Java_xeno_util_Runtime_setDefocusQuick__IIII },
    { D_004D0D78, (void (*)())Java_xeno_util_Runtime_setDefocus__IIaI },
    { D_004D0D48, (void (*)())Java_xeno_util_Runtime_setDefocusParam__III },
    { D_004D0D08, (void (*)())Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I },
    { D_004D0CE0, (void (*)())Java_xeno_util_Runtime_setMap__II },
    { D_004D0CB8, (void (*)())Java_xeno_util_Runtime_setShootFlag__Z },
    { D_004D0C88, (void (*)())Java_xeno_util_Runtime_setShootHeightCheck__Z },
    { D_004D0C58, (void (*)())Java_xeno_util_Runtime_setShootIDCheck__Z },
    { D_004D0C28, (void (*)())Java_xeno_util_Runtime_setShootUwaCheck__Z },
    { D_004D0C00, (void (*)())Java_xeno_util_Runtime_setShootRange__F },
    { D_004D0BD8, (void (*)())Java_xeno_util_Runtime_addItem__II },
    { D_004D0BB0, (void (*)())Java_xeno_util_Runtime_addItemWin__II },
    { D_004D0B88, (void (*)())Java_xeno_util_Runtime_removeItem__II },
    { D_004D0B60, (void (*)())Java_xeno_util_Runtime_checkItem__II },
    { D_004D0B38, (void (*)())Java_xeno_util_Runtime_getItemName__II },
    { D_004D0B10, (void (*)())Java_xeno_util_Runtime_addGold__I },
    { D_004D0AE8, (void (*)())Java_xeno_util_Runtime_removeGold__I },
    { D_004D0AC0, (void (*)())Java_xeno_util_Runtime_checkGold__ },
    { D_004D0A90, (void (*)())Java_xeno_util_Runtime_progressEffect__I },
    { D_004D0A68, (void (*)())Java_xeno_util_Runtime_enterShop__I },
    { D_004D0A40, (void (*)())Java_xeno_util_Runtime_getLeader__ },
    { D_004D0A18, (void (*)())Java_xeno_util_Runtime_getPartyData__I },
    { D_004D09F0, (void (*)())Java_xeno_util_Runtime_setPartyData__II },
    { D_004D09C8, (void (*)())Java_xeno_util_Runtime_setFriend__I },
    { D_004D09A0, (void (*)())Java_xeno_util_Runtime_resetFriend__I },
    { D_004D0978, (void (*)())Java_xeno_util_Runtime_checkFriend__I },
    { D_004D0950, (void (*)())Java_xeno_util_Runtime_setLockParty__I },
    { D_004D0920, (void (*)())Java_xeno_util_Runtime_resetLockParty__I },
    { D_004D08F0, (void (*)())Java_xeno_util_Runtime_checkLockParty__I },
    { D_004D08C8, (void (*)())Java_xeno_util_Runtime_setOutFriend__I },
    { D_004D0898, (void (*)())Java_xeno_util_Runtime_resetOutFriend__I },
    { D_004D0868, (void (*)())Java_xeno_util_Runtime_checkOutFriend__I },
    { D_004D0840, (void (*)())Java_xeno_util_Runtime_setTakeAgws__I },
    { D_004D0818, (void (*)())Java_xeno_util_Runtime_resetTakeAgws__I },
    { D_004D07F0, (void (*)())Java_xeno_util_Runtime_checkTakeAgws__I },
    { D_004D07C0, (void (*)())Java_xeno_util_Runtime_battleChangeParty__I },
    { D_004D0790, (void (*)())Java_xeno_util_Runtime_setIdLightCol__IIFFF },
    { D_004D0760, (void (*)())Java_xeno_util_Runtime_setIdLightDir__IIFFF },
    { D_004D0730, (void (*)())Java_xeno_util_Runtime_setIdLightVec__IIFFF },
    { D_004D0700, (void (*)())Java_xeno_util_Runtime_setWindParam__IFFFF },
    { D_004D06C8, (void (*)())Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_ },
    { D_004D0698, (void (*)())Java_xeno_util_Runtime_mpeg2AfterCrossFade__I },
    { D_004D0670, (void (*)())Java_xeno_util_Runtime_mailFlag__I },
    { D_004D0648, (void (*)())Java_xeno_util_Runtime_mailExec__I },
    { D_004D0620, (void (*)())Java_xeno_util_Runtime_minigameExec__I },
    { D_004D05F8, (void (*)())Java_xeno_util_Runtime_setMenuLock__ },
    { D_004D05D0, (void (*)())Java_xeno_util_Runtime_evsSetRetPoint__ },
    { D_004D05A8, (void (*)())Java_xeno_util_Runtime_evsExit__ },
    { D_004D0580, (void (*)())Java_xeno_util_Runtime_etherTecSet__I },
    { D_004D0550, (void (*)())Java_xeno_util_Runtime_charAllRecovery__ },
    { D_004D0520, (void (*)())Java_xeno_util_Runtime_AGWSAllRecovery__ },
    { D_004D04F8, (void (*)())Java_xeno_util_Window_getSignal__ },
    { D_004D04D8, (void (*)())Java_xeno_util_Window_signal__I },
    { D_004D04B8, (void (*)())Java_xeno_util_Window_clear__ },
    { D_004D0498, (void (*)())Java_xeno_util_Window_close__ },
    { D_004D0470, (void (*)())Java_xeno_util_Window_waitkey__I },
    { D_004D0450, (void (*)())Java_xeno_util_Window_wait__I },
    { D_004D0430, (void (*)())Java_xeno_util_Window_create__I },
    { D_004D0408, (void (*)())Java_xeno_util_Window_setSize__II },
    { D_004D03E0, (void (*)())Java_xeno_util_Window_setLocation__II },
    { D_004D03B0, (void (*)())Java_xeno_util_Window_print__Ljava_lang_String_ },
    { D_004D0378, (void (*)())Java_xeno_util_Window_print__aLjava_lang_String_I },
    { D_004D0340, (void (*)())Java_xeno_util_Window_setName__Ljava_lang_String_ },
    { D_004D0318, (void (*)())Java_xeno_util_Window_closeWaitKey__ },
    { D_004D02E0, (void (*)())Java_xeno_util_Menu_addQuery__Ljava_lang_String_ },
    { D_004D02A8, (void (*)())Java_xeno_util_Menu_addQuery__aLjava_lang_String_I },
    { D_004D0278, (void (*)())Java_xeno_util_Menu_addItem__Ljava_lang_String_ },
    { D_004D0258, (void (*)())Java_xeno_util_Menu_create__ },
    { D_004D0230, (void (*)())Java_xeno_util_Menu_getSelected__ },
    { D_004D0208, (void (*)())Java_xeno_util_Menu_setLocation__II },
    { D_004D01E0, (void (*)())Java_xeno_util_Menu_setVisible__Z },
    { D_004D01B8, (void (*)())Java_xeno_util_Menu_setCursor__I },
    { D_004D0180, (void (*)())Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_ },
    { D_004D0138, (void (*)())Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_ },
    { D_004D00F8, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I },
    { D_004D00C0, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_ },
    { D_004D0070, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I },
    { D_004D0030, (void (*)())Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II },
    { D_004D0008, (void (*)())Java_xeno_util_Toolkit_peerSetGroup__II },
    { D_004CFFE8, (void (*)())Java_xeno_Sound_sequencePlay__I },
    { D_004CFFC0, (void (*)())Java_xeno_Sound_sequencePlay__II },
    { D_004CFFA0, (void (*)())Java_xeno_Sound_sequenceStop__I },
    { D_004CFF78, (void (*)())Java_xeno_Sound_sequenceStop__II },
    { D_004CFF58, (void (*)())Java_xeno_Sound_effectPlay__III },
    { D_004CFF38, (void (*)())Java_xeno_Sound_effectStop__I },
    { D_004CFF10, (void (*)())Java_xeno_Sound_streamPlay__IIII },
    { D_004CFEF8, (void (*)())Java_xeno_Movie_init__I },
    { D_004CFED8, (void (*)())Java_xeno_Movie_start__I },
    { D_004CFEC0, (void (*)())Java_xeno_Movie_stop__ },
    { D_004CFEA0, (void (*)())Java_xeno_Movie_update__ },
    { D_004CFE80, (void (*)())Java_xeno_PlayControl_create__ },
    { D_004CFE58, (void (*)())Java_xeno_PlayControl_getSignal__ },
    { D_004CFE38, (void (*)())Java_xeno_PlayControl_signal__I },
    { D_004CFE18, (void (*)())Java_xeno_PlayControl_init__II },
    { D_004CFDF0, (void (*)())Java_xeno_PlayControl_init__IIIIF },
    { D_004CFDB8, (void (*)())Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_ },
    { D_004CFD80, (void (*)())Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_ },
    { D_004CFD60, (void (*)())Java_xeno_PlayControl_start__ },
    { D_004CFD40, (void (*)())Java_xeno_PlayControl_stop__ },
    { D_004CFCF0, (void (*)())Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_ },
    { D_004CFC90, (void (*)())Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_ },
    { D_004CFC68, (void (*)())Java_xeno_PlayControl_getParams__ },
    { D_004CFC48, (void (*)())Java_xeno_Effect_call__I },
    { D_004CFC28, (void (*)())Java_xeno_Effect_disp__Z },
    { D_004CFC08, (void (*)())Java_xeno_Effect_setScale__FFF },
    { D_004CFBE8, (void (*)())Java_xeno_Effect_getScale__ },
    { D_004CFBC8, (void (*)())Java_xeno_Effect_getTranslate__ },
    { D_004CFBA8, (void (*)())Java_xeno_Effect_setTranslate__ },
    { D_004CFB88, (void (*)())Java_xeno_Effect_getRotate__ },
    { D_004CFB68, (void (*)())Java_xeno_Effect_setRotate__ },
    { D_004CFB40, (void (*)())Java_xeno_Effect_setCaster__Lxeno_Chr_ },
    { D_004CFB18, (void (*)())Java_xeno_Effect_setTarget__Lxeno_Chr_ },
    { D_004CFAF0, (void (*)())Java_xeno_Effect_setCaster__Lxeno_Unit_ },
    { D_004CFAC8, (void (*)())Java_xeno_Effect_setTarget__Lxeno_Unit_ },
    { D_004CFAA0, (void (*)())Java_xeno_Effect_setTransOffset__FFF },
    { D_004CFA80, (void (*)())Java_xeno_Effect_getForceLoop__ },
    { D_004CFA58, (void (*)())Java_xeno_Effect_setForceLoop__Z },
    { D_004CFA38, (void (*)())Java_xeno_Effect_getClip__ },
    { D_004CFA18, (void (*)())Java_xeno_Effect_setClip__Z },
    { D_004CF9F8, (void (*)())Java_xeno_Effect_clearEffect__ },
    { D_004CF9D8, (void (*)())Java_xeno_Effect_setMotion__Z },
    { D_004CF9B8, (void (*)())Java_xeno_Effect_noAttach__Z },
    { D_004CF998, (void (*)())Java_xeno_Camera_getRotateX__ },
    { D_004CF978, (void (*)())Java_xeno_Camera_getRotateY__ },
    { D_004CF958, (void (*)())Java_xeno_Camera_getRotateZ__ },
    { D_004CF930, (void (*)())Java_xeno_Camera_getTranslateX__ },
    { D_004CF908, (void (*)())Java_xeno_Camera_getTranslateY__ },
    { D_004CF8E0, (void (*)())Java_xeno_Camera_getTranslateZ__ },
    { D_004CF8C0, (void (*)())Java_xeno_Camera_fovSPL__aFI },
    { D_004CF8A0, (void (*)())Java_xeno_Camera_change__ },
    { D_004CF880, (void (*)())Java_xeno_Camera_rotateSPL__aFI },
    { D_004CF858, (void (*)())Java_xeno_Camera_rotateSPL__aFIII },
    { D_004CF838, (void (*)())Java_xeno_Camera_setActive__Z },
    { D_004CF818, (void (*)())Java_xeno_Camera_create__I },
    { D_004CF7E0, (void (*)())Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF },
    { D_004CF7C0, (void (*)())Java_xeno_Camera_transSPL__aFI },
    { D_004CF798, (void (*)())Java_xeno_Camera_transSPL__aFIII },
    { D_004CF768, (void (*)())Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF },
    { D_004CF748, (void (*)())Java_xeno_Camera_viewSPL__aFI },
    { D_004CF728, (void (*)())Java_xeno_Camera_viewSPL__aFIII },
    { D_004CF708, (void (*)())Java_xeno_Camera_rollSPL__aFI },
    { D_004CF6E8, (void (*)())Java_xeno_Camera_setRotate__FFF },
    { D_004CF6C8, (void (*)())Java_xeno_Camera_setRoll__F },
    { D_004CF6A0, (void (*)())Java_xeno_Camera_setTranslate__FFF },
    { D_004CF680, (void (*)())Java_xeno_Camera_setFov__F },
    { D_004CF660, (void (*)())Java_xeno_Camera_getFov__ },
    { D_004CF640, (void (*)())Java_xeno_Camera_setView__FFF },
    { D_004CF610, (void (*)())Java_xeno_Camera_start__ILjava_lang_Object_ },
    { D_004CF5E8, (void (*)())Java_xeno_Camera_setCFAngle__IFFFF },
    { D_004CF5C0, (void (*)())Java_xeno_Camera_setCFAnglePers__IFFFFF },
    { D_004CF598, (void (*)())Java_xeno_Camera_setCFHokan__IFF },
    { D_004CF570, (void (*)())Java_xeno_Camera_setCFLock__IIFFF },
    { D_004CF548, (void (*)())Java_xeno_Camera_setCFOffset__IFFFIFIF },
    { D_004CF528, (void (*)())Java_xeno_Camera_setMode__I },
    { D_004CF508, (void (*)())Java_xeno_Camera_getMode__ },
    { D_004CF4D8, (void (*)())Java_xeno_Camera_setCFPedestal__IFFFFFFFF },
    { D_004CF4B0, (void (*)())Java_xeno_Camera_setCFPedestalHokan__II },
    { D_004CF490, (void (*)())Java_xeno_Camera_changeID__III },
    { D_004CF468, (void (*)())Java_xeno_Camera_setFog__IFFFFIIII },
    { D_004CF448, (void (*)())Java_xeno_Camera_resetFog__I },
    { D_004CF420, (void (*)())Java_xeno_Camera_setClipRange__FF },
    { D_004CF400, (void (*)())Java_xeno_Light_setColor__FFF },
    { D_004CF3D8, (void (*)())Java_xeno_Light_setDirection__FFF },
    { D_004CF3B0, (void (*)())Java_xeno_Light_setDirection2__FFF },
    { D_004CF380, (void (*)())Java_xeno_Light_setGlobalPointLightCol__IFFF },
    { D_004CF350, (void (*)())Java_xeno_Light_setGlobalPointLightPos__IFFF },
    { D_004CF320, (void (*)())Java_xeno_Light_setGlobalPointLightReset__ },
    { D_004CF300, (void (*)())Java_xeno_Chr_getPlayer__ },
    { D_004CF2E0, (void (*)())Java_xeno_Chr_setPlayer__ },
    { D_004CF2C0, (void (*)())Java_xeno_Chr_move__IFFZ },
    { D_004CF2A0, (void (*)())Java_xeno_Chr_move__FFFZ },
    { D_004CF270, (void (*)())Java_xeno_Chr_move__ILjava_lang_Object_Z },
    { D_004CF240, (void (*)())Java_xeno_Chr_move__Ljava_lang_Object_FZ },
    { D_004CF210, (void (*)())Java_xeno_Chr_move__Lxeno_util_Spline_IZ },
    { D_004CF1E0, (void (*)())Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ },
    { D_004CF1C8, (void (*)())Java_xeno_Chr_rotX__FFZ },
    { D_004CF1B0, (void (*)())Java_xeno_Chr_rotX__IFZ },
    { D_004CF180, (void (*)())Java_xeno_Chr_rotX__ILjava_lang_Object_Z },
    { D_004CF150, (void (*)())Java_xeno_Chr_rotX__Ljava_lang_Object_FZ },
    { D_004CF138, (void (*)())Java_xeno_Chr_rotY__FFZ },
    { D_004CF120, (void (*)())Java_xeno_Chr_rotY__IFZ },
    { D_004CF0F0, (void (*)())Java_xeno_Chr_rotY__ILjava_lang_Object_Z },
    { D_004CF0C0, (void (*)())Java_xeno_Chr_rotY__Ljava_lang_Object_FZ },
    { D_004CF0A8, (void (*)())Java_xeno_Chr_rotZ__FFZ },
    { D_004CF090, (void (*)())Java_xeno_Chr_rotZ__IFZ },
    { D_004CF060, (void (*)())Java_xeno_Chr_rotZ__ILjava_lang_Object_Z },
    { D_004CF030, (void (*)())Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ },
    { D_004CF000, (void (*)())Java_xeno_Chr_start__ILjava_lang_Object_ },
    { D_004CEFE8, (void (*)())Java_xeno_Chr_stop__ },
    { D_004CEFC8, (void (*)())Java_xeno_Chr_mtn__IIIIIFZ },
    { D_004CEFB0, (void (*)())Java_xeno_Chr_mtn__IIFZ },
    { D_004CEF98, (void (*)())Java_xeno_Chr_signal__I },
    { D_004CEF78, (void (*)())Java_xeno_Chr_getSignal__ },
    { D_004CEF58, (void (*)())Java_xeno_Chr_setRotate__ },
    { D_004CEF38, (void (*)())Java_xeno_Chr_setTranslate__ },
    { D_004CEF18, (void (*)())Java_xeno_Chr_getRotate__ },
    { D_004CEEF8, (void (*)())Java_xeno_Chr_getTranslate__ },
    { D_004CEED8, (void (*)())Java_xeno_Chr_setVisible__Z },
    { D_004CEEB8, (void (*)())Java_xeno_Chr_setVisible__IZ },
    { D_004CEE98, (void (*)())Java_xeno_Chr_setCollision__Z },
    { D_004CEE78, (void (*)())Java_xeno_Chr_setHand__I },
    { D_004CEE58, (void (*)())Java_xeno_Chr_setArgs__III },
    { D_004CEE38, (void (*)())Java_xeno_Chr_getArgs__II },
    { D_004CEE08, (void (*)())Java_xeno_Chr_setArgs__ILjava_lang_Object_I },
    { D_004CEDE8, (void (*)())Java_xeno_Chr_getSerial__ },
    { D_004CEDC8, (void (*)())Java_xeno_Chr_getState__ },
    { D_004CED98, (void (*)())Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_ },
    { D_004CED78, (void (*)())Java_xeno_Chr_setScale__FFF },
    { D_004CED58, (void (*)())Java_xeno_Chr_getScale__ },
    { D_004CED28, (void (*)())Java_xeno_Chr_scale__Lxeno_util_Spline_IZ },
    { D_004CED10, (void (*)())Java_xeno_Chr_sclX__FFZ },
    { D_004CECF8, (void (*)())Java_xeno_Chr_sclX__IFZ },
    { D_004CECE0, (void (*)())Java_xeno_Chr_sclY__FFZ },
    { D_004CECC8, (void (*)())Java_xeno_Chr_sclY__IFZ },
    { D_004CECB0, (void (*)())Java_xeno_Chr_sclZ__FFZ },
    { D_004CEC98, (void (*)())Java_xeno_Chr_sclZ__IFZ },
    { D_004CEC68, (void (*)())Java_xeno_Chr_rotCNS__ILjava_lang_Object_ },
    { D_004CEC48, (void (*)())Java_xeno_Chr_rotCNS__IFFF },
    { D_004CEC20, (void (*)())Java_xeno_Chr_setRotCNSParam__IFFFFF },
    { D_004CEC08, (void (*)())Java_xeno_Chr_relax__II },
    { D_004CEBE8, (void (*)())Java_xeno_Chr_getFlags__ },
    { D_004CEBC8, (void (*)())Java_xeno_Chr_setFlags__I },
    { D_004CEBA8, (void (*)())Java_xeno_Chr_setEdgeFall__I },
    { D_004CEB88, (void (*)())Java_xeno_Chr_setShadow__II },
    { D_004CEB68, (void (*)())Java_xeno_Chr_setShadow__aB },
    { D_004CEB50, (void (*)())Java_xeno_Chr_setID__I },
    { D_004CEB28, (void (*)())Java_xeno_Chr_setElevatorMode__I },
    { D_004CEB00, (void (*)())Java_xeno_Chr_setParent__Lxeno_Chr_III },
    { D_004CEAD8, (void (*)())Java_xeno_Chr_setMotionFlags__IZ },
    { D_004CEAB8, (void (*)())Java_xeno_Chr_setFilter__I },
    { D_004CEA90, (void (*)())Java_xeno_Chr_setFilterParam__aF },
    { D_004CEA70, (void (*)())Java_xeno_Chr_setClip__I },
    { D_004CEA50, (void (*)())Java_xeno_Chr_setSymmetryY__I },
    { D_004CEA30, (void (*)())Java_xeno_Chr_setSortOffset__F },
    { D_004CEA08, (void (*)())Java_xeno_Chr_setPointLightCol__IFFF },
    { D_004CE9E0, (void (*)())Java_xeno_Chr_setPointLightPos__IFFF },
    { D_004CE9B8, (void (*)())Java_xeno_Chr_setPointLightReset__ },
    { D_004CE988, (void (*)())Java_xeno_Chr_talkto__Ljava_lang_String_ },
    { D_004CE958, (void (*)())Java_xeno_Chr_touchto__Ljava_lang_String_ },
    { D_004CE938, (void (*)())Java_xeno_Chr_childGetPeer__II },
    { D_004CE908, (void (*)())Java_xeno_Chr_setPeer__Ljava_lang_Object_ },
    { D_004CE8E8, (void (*)())Java_xeno_Chr_dispRadar__Z },
    { D_004CE8C8, (void (*)())Java_xeno_Chr_look_camera__ },
    { D_004CE898, (void (*)())Java_xeno_Chr_look_char__Ljava_lang_Object_ },
    { D_004CE868, (void (*)())Java_xeno_Chr_look_unit__Ljava_lang_Object_ },
    { D_004CE848, (void (*)())Java_xeno_Chr_look_default__ },
    { D_004CE828, (void (*)())Java_xeno_Chr_look_point__FFF },
    { D_004CE808, (void (*)())Java_xeno_Chr_look_eye_set__FF },
    { D_004CE7E0, (void (*)())Java_xeno_Chr_look_eye_control__I },
    { D_004CE7C0, (void (*)())Java_xeno_Chr_look_speed__F },
    { D_004CE7A0, (void (*)())Java_xeno_Chr_look_eye_speed__F },
    { D_004CE780, (void (*)())Java_xeno_Chr_renderCommand__I },
    { D_004CE758, (void (*)())Java_xeno_Chr_shadow_clip_scale__F },
    { D_004CE738, (void (*)())Java_xeno_Chr_shadow_map_id__I },
    { D_004CE710, (void (*)())Java_xeno_Chr_shadow_map_reset__ },
    { D_004CE6F0, (void (*)())Java_xeno_Chr_hairStop__II },
    { D_004CE6D0, (void (*)())Java_xeno_Chr_pixelAlpha__I },
    { D_004CE6A8, (void (*)())Java_xeno_Chr_pixelAlphaParts__II },
    { D_004CE680, (void (*)())Java_xeno_Chr_pixelAlphaPartsReset__ },
    { D_004CE660, (void (*)())Java_xeno_Chr_setMotNoUpdate__I },
    { D_004CE638, (void (*)())Java_xeno_Chr_setWeaponR__Lxeno_Chr_ },
    { D_004CE610, (void (*)())Java_xeno_Chr_resetWeaponR__Lxeno_Chr_ },
    { D_004CE5F0, (void (*)())Java_xeno_Chr_resetHand__ },
    { D_004CE5D0, (void (*)())Java_xeno_Chr_resetEnv__ },
    { D_004CE5B0, (void (*)())Java_xeno_Chr_ignoreShape__I },
    { D_004CE580, (void (*)())Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF },
    { D_004CE550, (void (*)())Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF },
    { D_004CE530, (void (*)())Java_xeno_Unit_getRotate__ },
    { D_004CE510, (void (*)())Java_xeno_Unit_getSignal__ },
    { D_004CE4F0, (void (*)())Java_xeno_Unit_getTranslate__ },
    { D_004CE4D0, (void (*)())Java_xeno_Unit_invalidate__ },
    { D_004CE4B0, (void (*)())Java_xeno_Unit_move__FFFZ },
    { D_004CE490, (void (*)())Java_xeno_Unit_move__IFFZ },
    { D_004CE460, (void (*)())Java_xeno_Unit_move__Lxeno_util_Spline_IZ },
    { D_004CE430, (void (*)())Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ },
    { D_004CE400, (void (*)())Java_xeno_Unit_move__ILjava_lang_Object_Z },
    { D_004CE3D0, (void (*)())Java_xeno_Unit_move__Ljava_lang_Object_FZ },
    { D_004CE3B0, (void (*)())Java_xeno_Unit_mtn__IIFZ },
    { D_004CE390, (void (*)())Java_xeno_Unit_mtn__IIIIIFZ },
    { D_004CE370, (void (*)())Java_xeno_Unit_rotX__FFZ },
    { D_004CE350, (void (*)())Java_xeno_Unit_rotX__IFZ },
    { D_004CE320, (void (*)())Java_xeno_Unit_rotX__ILjava_lang_Object_Z },
    { D_004CE2F0, (void (*)())Java_xeno_Unit_rotX__Ljava_lang_Object_FZ },
    { D_004CE2D0, (void (*)())Java_xeno_Unit_rotY__FFZ },
    { D_004CE2B0, (void (*)())Java_xeno_Unit_rotY__IFZ },
    { D_004CE280, (void (*)())Java_xeno_Unit_rotY__ILjava_lang_Object_Z },
    { D_004CE250, (void (*)())Java_xeno_Unit_rotY__Ljava_lang_Object_FZ },
    { D_004CE230, (void (*)())Java_xeno_Unit_rotZ__FFZ },
    { D_004CE210, (void (*)())Java_xeno_Unit_rotZ__IFZ },
    { D_004CE1E0, (void (*)())Java_xeno_Unit_rotZ__ILjava_lang_Object_Z },
    { D_004CE1B0, (void (*)())Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ },
    { D_004CE180, (void (*)())Java_xeno_Unit_scale__Lxeno_util_Spline_IZ },
    { D_004CE160, (void (*)())Java_xeno_Unit_sclX__FFZ },
    { D_004CE140, (void (*)())Java_xeno_Unit_sclX__IFZ },
    { D_004CE120, (void (*)())Java_xeno_Unit_sclY__FFZ },
    { D_004CE100, (void (*)())Java_xeno_Unit_sclY__IFZ },
    { D_004CE0E0, (void (*)())Java_xeno_Unit_sclZ__FFZ },
    { D_004CE0C0, (void (*)())Java_xeno_Unit_sclZ__IFZ },
    { D_004CE0A0, (void (*)())Java_xeno_Unit_setCollision__Z },
    { D_004CE080, (void (*)())Java_xeno_Unit_setRotate__ },
    { D_004CE060, (void (*)())Java_xeno_Unit_setTranslate__ },
    { D_004CE040, (void (*)())Java_xeno_Unit_setVisible__IZ },
    { D_004CE020, (void (*)())Java_xeno_Unit_setVisible__Z },
    { D_004CE000, (void (*)())Java_xeno_Unit_signal__I },
    { D_004CDFD0, (void (*)())Java_xeno_Unit_start__ILjava_lang_Object_ },
    { D_004CDFB8, (void (*)())Java_xeno_Unit_stop__ },
    { D_004CDF98, (void (*)())Java_xeno_Unit_validate__ },
    { D_004CDF68, (void (*)())Java_xeno_Unit_setParent__Ljava_lang_Object_I },
    { D_004CDF48, (void (*)())Java_xeno_Unit_setArgs__III },
    { D_004CDF28, (void (*)())Java_xeno_Unit_getArgs__II },
    { D_004CDEF8, (void (*)())Java_xeno_Unit_setArgs__ILjava_lang_Object_I },
    { D_004CDED8, (void (*)())Java_xeno_Unit_setArgs__IIIII },
    { D_004CDEB8, (void (*)())Java_xeno_Unit_getScale__ },
    { D_004CDE98, (void (*)())Java_xeno_Unit_setScale__FFF },
    { D_004CDE78, (void (*)())Java_xeno_Unit_getSerial__ },
    { D_004CDE58, (void (*)())Java_xeno_Unit_mtnSetMask__I },
    { D_004CDE20, (void (*)())Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_ },
    { D_004CDE00, (void (*)())Java_xeno_Unit_getState__ },
    { D_004CDDD0, (void (*)())Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_ },
    { D_004CDDB0, (void (*)())Java_xeno_Unit_setPivot__FFF },
    { D_004CDD80, (void (*)())Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_ },
    { D_004CDD60, (void (*)())Java_xeno_Unit_setAxis__FFFF },
    { D_004CDD40, (void (*)())Java_xeno_Unit_suspend__I },
    { D_004CDD20, (void (*)())Java_xeno_Unit_resume__I },
    { D_004CDCF8, (void (*)())Java_xeno_Unit_initElevatorFunc__ },
    { D_004CDCD8, (void (*)())Java_xeno_Unit_map_shadow__I },
    { D_004CDCB8, (void (*)())Java_xeno_Unit_renderCommand__I },
    { D_004CDC98, (void (*)())Java_xeno_Unit_setFilter__I },
    { D_004CDC70, (void (*)())Java_xeno_Unit_setFilterParam__aF },
    { D_004CDC50, (void (*)())Java_xeno_Unit_setShadow__II },
    { D_004CDC28, (void (*)())Java_xeno_Unit_shadow_clip_scale__F },
    { D_004CDC08, (void (*)())Java_xeno_Unit_shadow_map_id__I },
    { D_004CDBE0, (void (*)())Java_xeno_Unit_shadow_map_reset__ },
    { D_004CDBC0, (void (*)())Java_xeno_Unit_setSortOffset__F },
    { D_004CDBA0, (void (*)())Java_xeno_Unit_setClip__I },
    { D_004CDB78, (void (*)())Java_xeno_Unit_setMonitorPrio__I },
    { D_004CDB48, (void (*)())Java_xeno_Scene_start__ILjava_lang_Object_ },
    { D_004CDB30, (void (*)())Java_xeno_Scene_stop__ },
    { D_004CDB00, (void (*)())Java_xeno_Stage_start__ILjava_lang_Object_ },
    { D_004CDAE8, (void (*)())Java_xeno_Stage_stop__ },
    { D_004CDAB8, (void (*)())Java_xeno_Stage_play__Ljava_lang_String_ },
    { D_004CDA98, (void (*)())Java_xeno_Stage_setPartsLast__I },
    { D_004CDA70, (void (*)())Java_xeno_Stage_setPartsLastReset__ },
    { D_004CDA50, (void (*)())Java_xeno_Stage_setColor__FFF },
    { D_004CDA30, (void (*)())Java_xeno_Stage_setFade__IIFFF },
    { D_004CDA08, (void (*)())Java_xeno_Stage_setEventFade__IFFFIFFF },
    { D_004CD9E0, (void (*)())Java_xeno_Stage_setFrameRender__II },
    { D_004CD9B8, (void (*)())Java_xeno_Stage_setFadeCancel__I },
    { D_004CD998, (void (*)())Java_xeno_Stage_setVisible__IZ },
    { D_004CD978, (void (*)())Java_xeno_Stage_setCFBG__II_F },
    { D_004CD950, (void (*)())Java_xeno_Stage_setEffectRender__I },
    { D_004CD928, (void (*)())Java_xeno_Stage_renderCommand__I },
    { D_004CD908, (void (*)())Java_xeno_Stage_setBgColor__FFF },
    { D_004CD8E8, (void (*)())Java_xeno_Stage_setBgClip__I },
    { D_004CD8C8, (void (*)())Java_xeno_Stage_clrBackBuffer__ },
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
    unsigned char unmodeled_20[0x4D0 - 0x20];
    unsigned short status_flags;          /* +0x4D0, lhu/sh at
                                              0x002f76c0..0x002f76e0
                                              (scriptReset_evsExit) */
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
    unsigned char unmodeled_0c[0xE - 0xC];
    unsigned short deferred_jump_pending; /* +0x0E, sh $0 at 0x002f767c */
    unsigned int flags;                   /* +0x10, lw/nor/and/sw at
                                              0x002f74a4..0x002f74bc (disable),
                                              lw/or/sw at 0x002f74c4..0x002f74d8
                                              (enable), lw/sw at
                                              0x002f74e4..0x002f74ec
                                              (getGameState) */
    unsigned char unmodeled_14[0x20 - 0x14];
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
    unsigned char unmodeled_54[0xC1 - 0x54];
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

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_execBattle__II);

/* The call block of a native taking two plain ints (java signature "(II)"). */
typedef struct {
    int first;
    int second;
} IntPairCall;

extern int xglFlagsGet(int group, int bit);

void Java_xeno_util_Runtime_getFlags__II(JThread *thread, IntPairCall *arguments,
                                         int *result)
{
    *result = xglFlagsGet(arguments->first, arguments->second);
}

/* The call block of Java_xeno_util_Runtime_setFlags__III, the xglFlagsSet
   operands the native passes through unchanged and in order. */
typedef struct {
    int bit_offset;
    int bit_count;
    int value;
} FlagsSetCall;

extern int xglFlagsSet(int bit_offset, int bit_count, int value);

void Java_xeno_util_Runtime_setFlags__III(JThread *thread, FlagsSetCall *arguments,
                                          unsigned int *result)
{
    xglFlagsSet(arguments->bit_offset, arguments->bit_count, arguments->value);
}

extern void LoadMapOnly(int map_id);
extern void MapChange2(int map_id);

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

extern int MapGetNo(void);

void Java_xeno_util_Runtime_getLocation__(JThread *thread, void *arguments, int *result)
{
    *result = MapGetNo();
}

/* The script VM's 32-entry general register file, indexed with the low 5
   bits of the requested register number (VMRegister[index & 0x1F]). */
int VMRegister[32];

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

/* The call block of a native taking three plain floats (java signature "(FFF)"). */
typedef struct {
    float walk_threshold;
    float run_threshold;
    float vector_rate;
} PlayerMoveParamCall;

extern void GameCfPlayerMoveParamSet(float walk_threshold, float run_threshold,
                                      float vector_rate);

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
    unsigned char unmodeled_00[8];
    char *bytes;
};

/* The call block of a native taking (java.lang.String, int). */
typedef struct {
    StringRef *string;
    int value;
} StringArgCall;

extern void tyaCaptureStart(const char *name, int count);

void Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I(JThread *thread,
                                                               StringArgCall *arguments,
                                                               unsigned int *result)
{
    tyaCaptureStart(arguments->string->value->bytes, arguments->value);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jump_sub);

extern void Java_xeno_util_Runtime_jump_sub(int destination, int argument);

/* The deferred script jump scriptReset_jump below executes once scheduled. */
typedef struct {
    int destination;
    int argument;
} DeferredJumpCall;

static DeferredJumpCall args_jump;

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

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_jumpCF__II);

extern void (*jthreadResetFunc)(void);
extern void SCRIPT_talkIgnoreSet(void);

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

/* The call block of a native taking four plain ints (java signature "(IIII)"). */
typedef struct {
    int first;
    int second;
    int third;
    int fourth;
} IntQuadCall;

extern void GameDefocusQuickSet(int first, int second, int third, int fourth);

void Java_xeno_util_Runtime_setDefocusQuick__IIII(JThread *thread, IntQuadCall *arguments,
                                                  unsigned int *result)
{
    GameDefocusQuickSet(arguments->first, arguments->second, arguments->third,
                        arguments->fourth);
}

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

void Java_xeno_util_Runtime_setDefocus__IIaI(JThread *thread, DefocusCall *arguments,
                                             unsigned int *result)
{
    if (arguments->table == 0) {
        GameDefocusSet(arguments->mode, arguments->strength, 0);
    } else {
        GameDefocusSet(arguments->mode, arguments->strength, arguments->table->data);
    }
}

/* A defocus parameter table of seventeen-entry rows, one word ahead of the
   symbol (index 0 is untouched by this TU). */
extern int GameDefocusParam[];

void Java_xeno_util_Runtime_setDefocusParam__III(JThread *thread, IntTripleCall *arguments,
                                                 unsigned int *result)
{
    GameDefocusParam[1 + arguments->first * 17 + arguments->second] = arguments->third;
}

extern void setEventTimerTaskEntry(const char *method_reference, int countdown);

void Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I(JThread *thread,
                                                                StringArgCall *arguments,
                                                                unsigned int *result)
{
    setEventTimerTaskEntry(arguments->string->value->bytes, arguments->value);
}

extern void MapChangeResource(int map_id, int resource_id);

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

extern int dataBoxInc(int item, int count);

void Java_xeno_util_Runtime_addItem__II(JThread *thread, IntPairCall *arguments,
                                        signed char *result)
{
    *result = (signed char) dataBoxInc(arguments->first, arguments->second);
}

extern int CreateEvtItemGetTask(int item, int count);

void Java_xeno_util_Runtime_addItemWin__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) CreateEvtItemGetTask(arguments->first, arguments->second);
}

extern int dataBoxDec(int item, int count);

void Java_xeno_util_Runtime_removeItem__II(JThread *thread, IntPairCall *arguments,
                                           signed char *result)
{
    *result = (signed char) dataBoxDec(arguments->first, arguments->second);
}

extern int dataBoxChk(int item, int count);

void Java_xeno_util_Runtime_checkItem__II(JThread *thread, IntPairCall *arguments,
                                          int *result)
{
    *result = dataBoxChk(arguments->first, arguments->second);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getItemName__II);

extern int dataMoneyBoxInc(int amount);

void Java_xeno_util_Runtime_addGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxInc(*arguments);
}

extern int dataMoneyBoxDec(int amount);

void Java_xeno_util_Runtime_removeGold__I(JThread *thread, int *arguments, signed char *result)
{
    *result = (signed char) dataMoneyBoxDec(*arguments);
}

extern int dataMoneyBoxChk(void);

void Java_xeno_util_Runtime_checkGold__(JThread *thread, void *arguments, int *result)
{
    *result = dataMoneyBoxChk();
}

extern void sefProgressEffect(int frame_count);

void Java_xeno_util_Runtime_progressEffect__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    sefProgressEffect(*arguments);
}

extern void GameStateRestoreCameraLight(void);
extern void GameStateSaveCameraLight(void);
extern void MenuShopMain(int shop);

void Java_xeno_util_Runtime_enterShop__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    GameStateSaveCameraLight();
    MenuShopMain(*arguments);
    GameStateRestoreCameraLight();
}

/*
 * The leader's battle id: read directly out of SaveData (lui/lhu at
 * 0x002f7bd0/0x002f7bd4, no call) rather than through PartyDataGet(), which
 * getPartyDataOfs below shows returns SaveData + 0x10078, so this offset is
 * that region's own +0x2C.
 */
#define SAVE_LEADER_BATTLE_ID 0x100A4

typedef struct {
    unsigned short leader_battle_id;
} SaveDataLeaderView;

void Java_xeno_util_Runtime_getLeader__(JThread *thread, void *arguments, int *result)
{
    *result = ((SaveDataLeaderView *) (SaveData + SAVE_LEADER_BATTLE_ID))->leader_battle_id;
}

static char *getPartyDataOfs(unsigned int selector)
{
    int type = (selector >> 16) & 0xff;
    unsigned int offset = selector & 0xffff;
    char *party_data = (char *)PartyDataGet();

    char *selected;

    if (type == 1) {
        selected = party_data + 0x30;
    } else if (type < 2) {
        goto default_party;
    } else if (type == 2) {
        selected = party_data + 0x3c;
    } else if (type == 3) {
        selected = party_data + 0x140;
    } else {
default_party:
        selected = party_data;
    }
    return selected + offset;
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_getPartyData__I);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setPartyData__II);

/* Converts a Java character id to the one-based battle-party index used by
   the Party* entry points below. */
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

extern void PartyFriendOn(int battle_index);

void Java_xeno_util_Runtime_setFriend__I(JThread *thread, int *arguments,
                                         unsigned int *result)
{
    PartyFriendOn(cid2bic(*arguments));
}

extern void PartyFriendOff(int battle_index);

void Java_xeno_util_Runtime_resetFriend__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyFriendOff(cid2bic(*arguments));
}

extern int PartyFriendCheck(int battle_index);

void Java_xeno_util_Runtime_checkFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyFriendCheck(cid2bic(*arguments));
}

extern void PartyLockPartyOn(int battle_index);

void Java_xeno_util_Runtime_setLockParty__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyLockPartyOn(cid2bic(*arguments));
}

extern void PartyLockPartyOff(int battle_index);

void Java_xeno_util_Runtime_resetLockParty__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyLockPartyOff(cid2bic(*arguments));
}

extern int PartyLockPartyCheck(int battle_index);

void Java_xeno_util_Runtime_checkLockParty__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyLockPartyCheck(cid2bic(*arguments));
}

extern void PartyOutFriendOn(int battle_index);

void Java_xeno_util_Runtime_setOutFriend__I(JThread *thread, int *arguments,
                                            unsigned int *result)
{
    PartyOutFriendOn(cid2bic(*arguments));
}

extern void PartyOutFriendOff(int battle_index);

void Java_xeno_util_Runtime_resetOutFriend__I(JThread *thread, int *arguments,
                                              unsigned int *result)
{
    PartyOutFriendOff(cid2bic(*arguments));
}

extern int PartyOutFriendCheck(int battle_index);

void Java_xeno_util_Runtime_checkOutFriend__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyOutFriendCheck(cid2bic(*arguments));
}

extern void PartyTakeAgwsOn(int battle_index);

void Java_xeno_util_Runtime_setTakeAgws__I(JThread *thread, int *arguments,
                                           unsigned int *result)
{
    PartyTakeAgwsOn(cid2bic(*arguments));
}

extern void PartyTakeAgwsOff(int battle_index);

void Java_xeno_util_Runtime_resetTakeAgws__I(JThread *thread, int *arguments,
                                             unsigned int *result)
{
    PartyTakeAgwsOff(cid2bic(*arguments));
}

extern int PartyTakeAgwsCheck(int battle_index);

void Java_xeno_util_Runtime_checkTakeAgws__I(JThread *thread, int *arguments, int *result)
{
    *result = PartyTakeAgwsCheck(cid2bic(*arguments));
}

extern void PartyBattleChange(int battle_index);

void Java_xeno_util_Runtime_battleChangeParty__I(JThread *thread, int *arguments,
                                                  unsigned int *result)
{
    PartyBattleChange(cid2bic(*arguments));
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightCol__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightDir__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setIdLightVec__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_setWindParam__IFFFF);

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_);

extern void nmlModelSetMpeg2CrossFadeTime(int time);
static int s_nMpeg2ReserveCrossFade = 0;

void Java_xeno_util_Runtime_mpeg2AfterCrossFade__I(JThread *thread, int *arguments,
                                                    unsigned int *result)
{
    int time = *arguments;

    s_nMpeg2ReserveCrossFade = 1;
    nmlModelSetMpeg2CrossFadeTime(time);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_mailFlag__I);

extern void UmnMailMain(int mail);
extern void xglCullingIgnore(void);
extern void xglCullingIgnoreOff(void);

void Java_xeno_util_Runtime_mailExec__I(JThread *thread, int *arguments,
                                        unsigned int *result)
{
    GameStateSaveCameraLight();
    xglCullingIgnore();
    UmnMailMain(*arguments);
    xglCullingIgnoreOff();
    GameStateRestoreCameraLight();
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_minigameExec__I);

void Java_xeno_util_Runtime_setMenuLock__(JThread *thread, void *arguments,
                                          unsigned int *result)
{
    /*
     * Arms the script scene-change timer, which SCRIPT_sceneChangeTimeDec
     * counts down once per game frame, with 62 frames.
     */
    SCRIPT_sceneChangeTimeSet(62);
}

INCLUDE_ASM("asm/main/nonmatchings/runtime", Java_xeno_util_Runtime_evsSetRetPoint__);

extern void (*jthreadResetFunc)(void);
extern void scriptReset_evsExit(void);

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

extern void dataEtherTecSet(int value);

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



const char D_004CD8C8[32] = "Java_xeno_Stage_clrBackBuffer__";

const char D_004CD8E8[32] = "Java_xeno_Stage_setBgClip__I";

const char D_004CD908[32] = "Java_xeno_Stage_setBgColor__FFF";

const char D_004CD928[40] = "Java_xeno_Stage_renderCommand__I";

const char D_004CD950[40] = "Java_xeno_Stage_setEffectRender__I";

const char D_004CD978[32] = "Java_xeno_Stage_setCFBG__II_F";

const char D_004CD998[32] = "Java_xeno_Stage_setVisible__IZ";

const char D_004CD9B8[40] = "Java_xeno_Stage_setFadeCancel__I";

const char D_004CD9E0[40] = "Java_xeno_Stage_setFrameRender__II";

const char D_004CDA08[40] = "Java_xeno_Stage_setEventFade__IFFFIFFF";

const char D_004CDA30[32] = "Java_xeno_Stage_setFade__IIFFF";

const char D_004CDA50[32] = "Java_xeno_Stage_setColor__FFF";

const char D_004CDA70[40] = "Java_xeno_Stage_setPartsLastReset__";

const char D_004CDA98[32] = "Java_xeno_Stage_setPartsLast__I";

const char D_004CDAB8[48] = "Java_xeno_Stage_play__Ljava_lang_String_";

const char D_004CDAE8[24] = "Java_xeno_Stage_stop__";

const char D_004CDB00[48] = "Java_xeno_Stage_start__ILjava_lang_Object_";

const char D_004CDB30[24] = "Java_xeno_Scene_stop__";

const char D_004CDB48[48] = "Java_xeno_Scene_start__ILjava_lang_Object_";

const char D_004CDB78[40] = "Java_xeno_Unit_setMonitorPrio__I";

const char D_004CDBA0[32] = "Java_xeno_Unit_setClip__I";

const char D_004CDBC0[32] = "Java_xeno_Unit_setSortOffset__F";

const char D_004CDBE0[40] = "Java_xeno_Unit_shadow_map_reset__";

const char D_004CDC08[32] = "Java_xeno_Unit_shadow_map_id__I";

const char D_004CDC28[40] = "Java_xeno_Unit_shadow_clip_scale__F";

const char D_004CDC50[32] = "Java_xeno_Unit_setShadow__II";

const char D_004CDC70[40] = "Java_xeno_Unit_setFilterParam__aF";

const char D_004CDC98[32] = "Java_xeno_Unit_setFilter__I";

const char D_004CDCB8[32] = "Java_xeno_Unit_renderCommand__I";

const char D_004CDCD8[32] = "Java_xeno_Unit_map_shadow__I";

const char D_004CDCF8[40] = "Java_xeno_Unit_initElevatorFunc__";

const char D_004CDD20[32] = "Java_xeno_Unit_resume__I";

const char D_004CDD40[32] = "Java_xeno_Unit_suspend__I";

const char D_004CDD60[32] = "Java_xeno_Unit_setAxis__FFFF";

const char D_004CDD80[48] = "Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_";

const char D_004CDDB0[32] = "Java_xeno_Unit_setPivot__FFF";

const char D_004CDDD0[48] = "Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_";

const char D_004CDE00[32] = "Java_xeno_Unit_getState__";

const char D_004CDE20[56] = "Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_";

const char D_004CDE58[32] = "Java_xeno_Unit_mtnSetMask__I";

const char D_004CDE78[32] = "Java_xeno_Unit_getSerial__";

const char D_004CDE98[32] = "Java_xeno_Unit_setScale__FFF";

const char D_004CDEB8[32] = "Java_xeno_Unit_getScale__";

const char D_004CDED8[32] = "Java_xeno_Unit_setArgs__IIIII";

const char D_004CDEF8[48] = "Java_xeno_Unit_setArgs__ILjava_lang_Object_I";

const char D_004CDF28[32] = "Java_xeno_Unit_getArgs__II";

const char D_004CDF48[32] = "Java_xeno_Unit_setArgs__III";

const char D_004CDF68[48] = "Java_xeno_Unit_setParent__Ljava_lang_Object_I";

const char D_004CDF98[32] = "Java_xeno_Unit_validate__";

const char D_004CDFB8[24] = "Java_xeno_Unit_stop__";

const char D_004CDFD0[48] = "Java_xeno_Unit_start__ILjava_lang_Object_";

const char D_004CE000[32] = "Java_xeno_Unit_signal__I";

const char D_004CE020[32] = "Java_xeno_Unit_setVisible__Z";

const char D_004CE040[32] = "Java_xeno_Unit_setVisible__IZ";

const char D_004CE060[32] = "Java_xeno_Unit_setTranslate__";

const char D_004CE080[32] = "Java_xeno_Unit_setRotate__";

const char D_004CE0A0[32] = "Java_xeno_Unit_setCollision__Z";

const char D_004CE0C0[32] = "Java_xeno_Unit_sclZ__IFZ";

const char D_004CE0E0[32] = "Java_xeno_Unit_sclZ__FFZ";

const char D_004CE100[32] = "Java_xeno_Unit_sclY__IFZ";

const char D_004CE120[32] = "Java_xeno_Unit_sclY__FFZ";

const char D_004CE140[32] = "Java_xeno_Unit_sclX__IFZ";

const char D_004CE160[32] = "Java_xeno_Unit_sclX__FFZ";

const char D_004CE180[48] = "Java_xeno_Unit_scale__Lxeno_util_Spline_IZ";

const char D_004CE1B0[48] = "Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ";

const char D_004CE1E0[48] = "Java_xeno_Unit_rotZ__ILjava_lang_Object_Z";

const char D_004CE210[32] = "Java_xeno_Unit_rotZ__IFZ";

const char D_004CE230[32] = "Java_xeno_Unit_rotZ__FFZ";

const char D_004CE250[48] = "Java_xeno_Unit_rotY__Ljava_lang_Object_FZ";

const char D_004CE280[48] = "Java_xeno_Unit_rotY__ILjava_lang_Object_Z";

const char D_004CE2B0[32] = "Java_xeno_Unit_rotY__IFZ";

const char D_004CE2D0[32] = "Java_xeno_Unit_rotY__FFZ";

const char D_004CE2F0[48] = "Java_xeno_Unit_rotX__Ljava_lang_Object_FZ";

const char D_004CE320[48] = "Java_xeno_Unit_rotX__ILjava_lang_Object_Z";

const char D_004CE350[32] = "Java_xeno_Unit_rotX__IFZ";

const char D_004CE370[32] = "Java_xeno_Unit_rotX__FFZ";

const char D_004CE390[32] = "Java_xeno_Unit_mtn__IIIIIFZ";

const char D_004CE3B0[32] = "Java_xeno_Unit_mtn__IIFZ";

const char D_004CE3D0[48] = "Java_xeno_Unit_move__Ljava_lang_Object_FZ";

const char D_004CE400[48] = "Java_xeno_Unit_move__ILjava_lang_Object_Z";

const char D_004CE430[48] = "Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ";

const char D_004CE460[48] = "Java_xeno_Unit_move__Lxeno_util_Spline_IZ";

const char D_004CE490[32] = "Java_xeno_Unit_move__IFFZ";

const char D_004CE4B0[32] = "Java_xeno_Unit_move__FFFZ";

const char D_004CE4D0[32] = "Java_xeno_Unit_invalidate__";

const char D_004CE4F0[32] = "Java_xeno_Unit_getTranslate__";

const char D_004CE510[32] = "Java_xeno_Unit_getSignal__";

const char D_004CE530[32] = "Java_xeno_Unit_getRotate__";

const char D_004CE550[48] = "Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF";

const char D_004CE580[48] = "Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF";

const char D_004CE5B0[32] = "Java_xeno_Chr_ignoreShape__I";

const char D_004CE5D0[32] = "Java_xeno_Chr_resetEnv__";

const char D_004CE5F0[32] = "Java_xeno_Chr_resetHand__";

const char D_004CE610[40] = "Java_xeno_Chr_resetWeaponR__Lxeno_Chr_";

const char D_004CE638[40] = "Java_xeno_Chr_setWeaponR__Lxeno_Chr_";

const char D_004CE660[32] = "Java_xeno_Chr_setMotNoUpdate__I";

const char D_004CE680[40] = "Java_xeno_Chr_pixelAlphaPartsReset__";

const char D_004CE6A8[40] = "Java_xeno_Chr_pixelAlphaParts__II";

const char D_004CE6D0[32] = "Java_xeno_Chr_pixelAlpha__I";

const char D_004CE6F0[32] = "Java_xeno_Chr_hairStop__II";

const char D_004CE710[40] = "Java_xeno_Chr_shadow_map_reset__";

const char D_004CE738[32] = "Java_xeno_Chr_shadow_map_id__I";

const char D_004CE758[40] = "Java_xeno_Chr_shadow_clip_scale__F";

const char D_004CE780[32] = "Java_xeno_Chr_renderCommand__I";

const char D_004CE7A0[32] = "Java_xeno_Chr_look_eye_speed__F";

const char D_004CE7C0[32] = "Java_xeno_Chr_look_speed__F";

const char D_004CE7E0[40] = "Java_xeno_Chr_look_eye_control__I";

const char D_004CE808[32] = "Java_xeno_Chr_look_eye_set__FF";

const char D_004CE828[32] = "Java_xeno_Chr_look_point__FFF";

const char D_004CE848[32] = "Java_xeno_Chr_look_default__";

const char D_004CE868[48] = "Java_xeno_Chr_look_unit__Ljava_lang_Object_";

const char D_004CE898[48] = "Java_xeno_Chr_look_char__Ljava_lang_Object_";

const char D_004CE8C8[32] = "Java_xeno_Chr_look_camera__";

const char D_004CE8E8[32] = "Java_xeno_Chr_dispRadar__Z";

const char D_004CE908[48] = "Java_xeno_Chr_setPeer__Ljava_lang_Object_";

const char D_004CE938[32] = "Java_xeno_Chr_childGetPeer__II";

const char D_004CE958[48] = "Java_xeno_Chr_touchto__Ljava_lang_String_";

const char D_004CE988[48] = "Java_xeno_Chr_talkto__Ljava_lang_String_";

const char D_004CE9B8[40] = "Java_xeno_Chr_setPointLightReset__";

const char D_004CE9E0[40] = "Java_xeno_Chr_setPointLightPos__IFFF";

const char D_004CEA08[40] = "Java_xeno_Chr_setPointLightCol__IFFF";

const char D_004CEA30[32] = "Java_xeno_Chr_setSortOffset__F";

const char D_004CEA50[32] = "Java_xeno_Chr_setSymmetryY__I";

const char D_004CEA70[32] = "Java_xeno_Chr_setClip__I";

const char D_004CEA90[40] = "Java_xeno_Chr_setFilterParam__aF";

const char D_004CEAB8[32] = "Java_xeno_Chr_setFilter__I";

const char D_004CEAD8[40] = "Java_xeno_Chr_setMotionFlags__IZ";

const char D_004CEB00[40] = "Java_xeno_Chr_setParent__Lxeno_Chr_III";

const char D_004CEB28[40] = "Java_xeno_Chr_setElevatorMode__I";

const char D_004CEB50[24] = "Java_xeno_Chr_setID__I";

const char D_004CEB68[32] = "Java_xeno_Chr_setShadow__aB";

const char D_004CEB88[32] = "Java_xeno_Chr_setShadow__II";

const char D_004CEBA8[32] = "Java_xeno_Chr_setEdgeFall__I";

const char D_004CEBC8[32] = "Java_xeno_Chr_setFlags__I";

const char D_004CEBE8[32] = "Java_xeno_Chr_getFlags__";

const char D_004CEC08[24] = "Java_xeno_Chr_relax__II";

const char D_004CEC20[40] = "Java_xeno_Chr_setRotCNSParam__IFFFFF";

const char D_004CEC48[32] = "Java_xeno_Chr_rotCNS__IFFF";

const char D_004CEC68[48] = "Java_xeno_Chr_rotCNS__ILjava_lang_Object_";

const char D_004CEC98[24] = "Java_xeno_Chr_sclZ__IFZ";

const char D_004CECB0[24] = "Java_xeno_Chr_sclZ__FFZ";

const char D_004CECC8[24] = "Java_xeno_Chr_sclY__IFZ";

const char D_004CECE0[24] = "Java_xeno_Chr_sclY__FFZ";

const char D_004CECF8[24] = "Java_xeno_Chr_sclX__IFZ";

const char D_004CED10[24] = "Java_xeno_Chr_sclX__FFZ";

const char D_004CED28[48] = "Java_xeno_Chr_scale__Lxeno_util_Spline_IZ";

const char D_004CED58[32] = "Java_xeno_Chr_getScale__";

const char D_004CED78[32] = "Java_xeno_Chr_setScale__FFF";

const char D_004CED98[48] = "Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_";

const char D_004CEDC8[32] = "Java_xeno_Chr_getState__";

const char D_004CEDE8[32] = "Java_xeno_Chr_getSerial__";

const char D_004CEE08[48] = "Java_xeno_Chr_setArgs__ILjava_lang_Object_I";

const char D_004CEE38[32] = "Java_xeno_Chr_getArgs__II";

const char D_004CEE58[32] = "Java_xeno_Chr_setArgs__III";

const char D_004CEE78[32] = "Java_xeno_Chr_setHand__I";

const char D_004CEE98[32] = "Java_xeno_Chr_setCollision__Z";

const char D_004CEEB8[32] = "Java_xeno_Chr_setVisible__IZ";

const char D_004CEED8[32] = "Java_xeno_Chr_setVisible__Z";

const char D_004CEEF8[32] = "Java_xeno_Chr_getTranslate__";

const char D_004CEF18[32] = "Java_xeno_Chr_getRotate__";

const char D_004CEF38[32] = "Java_xeno_Chr_setTranslate__";

const char D_004CEF58[32] = "Java_xeno_Chr_setRotate__";

const char D_004CEF78[32] = "Java_xeno_Chr_getSignal__";

const char D_004CEF98[24] = "Java_xeno_Chr_signal__I";

const char D_004CEFB0[24] = "Java_xeno_Chr_mtn__IIFZ";

const char D_004CEFC8[32] = "Java_xeno_Chr_mtn__IIIIIFZ";

const char D_004CEFE8[24] = "Java_xeno_Chr_stop__";

const char D_004CF000[48] = "Java_xeno_Chr_start__ILjava_lang_Object_";

const char D_004CF030[48] = "Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ";

const char D_004CF060[48] = "Java_xeno_Chr_rotZ__ILjava_lang_Object_Z";

const char D_004CF090[24] = "Java_xeno_Chr_rotZ__IFZ";

const char D_004CF0A8[24] = "Java_xeno_Chr_rotZ__FFZ";

const char D_004CF0C0[48] = "Java_xeno_Chr_rotY__Ljava_lang_Object_FZ";

const char D_004CF0F0[48] = "Java_xeno_Chr_rotY__ILjava_lang_Object_Z";

const char D_004CF120[24] = "Java_xeno_Chr_rotY__IFZ";

const char D_004CF138[24] = "Java_xeno_Chr_rotY__FFZ";

const char D_004CF150[48] = "Java_xeno_Chr_rotX__Ljava_lang_Object_FZ";

const char D_004CF180[48] = "Java_xeno_Chr_rotX__ILjava_lang_Object_Z";

const char D_004CF1B0[24] = "Java_xeno_Chr_rotX__IFZ";

const char D_004CF1C8[24] = "Java_xeno_Chr_rotX__FFZ";

const char D_004CF1E0[48] = "Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ";

const char D_004CF210[48] = "Java_xeno_Chr_move__Lxeno_util_Spline_IZ";

const char D_004CF240[48] = "Java_xeno_Chr_move__Ljava_lang_Object_FZ";

const char D_004CF270[48] = "Java_xeno_Chr_move__ILjava_lang_Object_Z";

const char D_004CF2A0[32] = "Java_xeno_Chr_move__FFFZ";

const char D_004CF2C0[32] = "Java_xeno_Chr_move__IFFZ";

const char D_004CF2E0[32] = "Java_xeno_Chr_setPlayer__";

const char D_004CF300[32] = "Java_xeno_Chr_getPlayer__";

const char D_004CF320[48] = "Java_xeno_Light_setGlobalPointLightReset__";

const char D_004CF350[48] = "Java_xeno_Light_setGlobalPointLightPos__IFFF";

const char D_004CF380[48] = "Java_xeno_Light_setGlobalPointLightCol__IFFF";

const char D_004CF3B0[40] = "Java_xeno_Light_setDirection2__FFF";

const char D_004CF3D8[40] = "Java_xeno_Light_setDirection__FFF";

const char D_004CF400[32] = "Java_xeno_Light_setColor__FFF";

const char D_004CF420[40] = "Java_xeno_Camera_setClipRange__FF";

const char D_004CF448[32] = "Java_xeno_Camera_resetFog__I";

const char D_004CF468[40] = "Java_xeno_Camera_setFog__IFFFFIIII";

const char D_004CF490[32] = "Java_xeno_Camera_changeID__III";

const char D_004CF4B0[40] = "Java_xeno_Camera_setCFPedestalHokan__II";

const char D_004CF4D8[48] = "Java_xeno_Camera_setCFPedestal__IFFFFFFFF";

const char D_004CF508[32] = "Java_xeno_Camera_getMode__";

const char D_004CF528[32] = "Java_xeno_Camera_setMode__I";

const char D_004CF548[40] = "Java_xeno_Camera_setCFOffset__IFFFIFIF";

const char D_004CF570[40] = "Java_xeno_Camera_setCFLock__IIFFF";

const char D_004CF598[40] = "Java_xeno_Camera_setCFHokan__IFF";

const char D_004CF5C0[40] = "Java_xeno_Camera_setCFAnglePers__IFFFFF";

const char D_004CF5E8[40] = "Java_xeno_Camera_setCFAngle__IFFFF";

const char D_004CF610[48] = "Java_xeno_Camera_start__ILjava_lang_Object_";

const char D_004CF640[32] = "Java_xeno_Camera_setView__FFF";

const char D_004CF660[32] = "Java_xeno_Camera_getFov__";

const char D_004CF680[32] = "Java_xeno_Camera_setFov__F";

const char D_004CF6A0[40] = "Java_xeno_Camera_setTranslate__FFF";

const char D_004CF6C8[32] = "Java_xeno_Camera_setRoll__F";

const char D_004CF6E8[32] = "Java_xeno_Camera_setRotate__FFF";

const char D_004CF708[32] = "Java_xeno_Camera_rollSPL__aFI";

const char D_004CF728[32] = "Java_xeno_Camera_viewSPL__aFIII";

const char D_004CF748[32] = "Java_xeno_Camera_viewSPL__aFI";

const char D_004CF768[48] = "Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF";

const char D_004CF798[40] = "Java_xeno_Camera_transSPL__aFIII";

const char D_004CF7C0[32] = "Java_xeno_Camera_transSPL__aFI";

const char D_004CF7E0[56] = "Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF";

const char D_004CF818[32] = "Java_xeno_Camera_create__I";

const char D_004CF838[32] = "Java_xeno_Camera_setActive__Z";

const char D_004CF858[40] = "Java_xeno_Camera_rotateSPL__aFIII";

const char D_004CF880[32] = "Java_xeno_Camera_rotateSPL__aFI";

const char D_004CF8A0[32] = "Java_xeno_Camera_change__";

const char D_004CF8C0[32] = "Java_xeno_Camera_fovSPL__aFI";

const char D_004CF8E0[40] = "Java_xeno_Camera_getTranslateZ__";

const char D_004CF908[40] = "Java_xeno_Camera_getTranslateY__";

const char D_004CF930[40] = "Java_xeno_Camera_getTranslateX__";

const char D_004CF958[32] = "Java_xeno_Camera_getRotateZ__";

const char D_004CF978[32] = "Java_xeno_Camera_getRotateY__";

const char D_004CF998[32] = "Java_xeno_Camera_getRotateX__";

const char D_004CF9B8[32] = "Java_xeno_Effect_noAttach__Z";

const char D_004CF9D8[32] = "Java_xeno_Effect_setMotion__Z";

const char D_004CF9F8[32] = "Java_xeno_Effect_clearEffect__";

const char D_004CFA18[32] = "Java_xeno_Effect_setClip__Z";

const char D_004CFA38[32] = "Java_xeno_Effect_getClip__";

const char D_004CFA58[40] = "Java_xeno_Effect_setForceLoop__Z";

const char D_004CFA80[32] = "Java_xeno_Effect_getForceLoop__";

const char D_004CFAA0[40] = "Java_xeno_Effect_setTransOffset__FFF";

const char D_004CFAC8[40] = "Java_xeno_Effect_setTarget__Lxeno_Unit_";

const char D_004CFAF0[40] = "Java_xeno_Effect_setCaster__Lxeno_Unit_";

const char D_004CFB18[40] = "Java_xeno_Effect_setTarget__Lxeno_Chr_";

const char D_004CFB40[40] = "Java_xeno_Effect_setCaster__Lxeno_Chr_";

const char D_004CFB68[32] = "Java_xeno_Effect_setRotate__";

const char D_004CFB88[32] = "Java_xeno_Effect_getRotate__";

const char D_004CFBA8[32] = "Java_xeno_Effect_setTranslate__";

const char D_004CFBC8[32] = "Java_xeno_Effect_getTranslate__";

const char D_004CFBE8[32] = "Java_xeno_Effect_getScale__";

const char D_004CFC08[32] = "Java_xeno_Effect_setScale__FFF";

const char D_004CFC28[32] = "Java_xeno_Effect_disp__Z";

const char D_004CFC48[32] = "Java_xeno_Effect_call__I";

const char D_004CFC68[40] = "Java_xeno_PlayControl_getParams__";

const char D_004CFC90[96] = "Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_";

const char D_004CFCF0[80] = "Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_";

const char D_004CFD40[32] = "Java_xeno_PlayControl_stop__";

const char D_004CFD60[32] = "Java_xeno_PlayControl_start__";

const char D_004CFD80[56] = "Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_";

const char D_004CFDB8[56] = "Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_";

const char D_004CFDF0[40] = "Java_xeno_PlayControl_init__IIIIF";

const char D_004CFE18[32] = "Java_xeno_PlayControl_init__II";

const char D_004CFE38[32] = "Java_xeno_PlayControl_signal__I";

const char D_004CFE58[40] = "Java_xeno_PlayControl_getSignal__";

const char D_004CFE80[32] = "Java_xeno_PlayControl_create__";

const char D_004CFEA0[32] = "Java_xeno_Movie_update__";

const char D_004CFEC0[24] = "Java_xeno_Movie_stop__";

const char D_004CFED8[32] = "Java_xeno_Movie_start__I";

const char D_004CFEF8[24] = "Java_xeno_Movie_init__I";

const char D_004CFF10[40] = "Java_xeno_Sound_streamPlay__IIII";

const char D_004CFF38[32] = "Java_xeno_Sound_effectStop__I";

const char D_004CFF58[32] = "Java_xeno_Sound_effectPlay__III";

const char D_004CFF78[40] = "Java_xeno_Sound_sequenceStop__II";

const char D_004CFFA0[32] = "Java_xeno_Sound_sequenceStop__I";

const char D_004CFFC0[40] = "Java_xeno_Sound_sequencePlay__II";

const char D_004CFFE8[32] = "Java_xeno_Sound_sequencePlay__I";

const char D_004D0008[40] = "Java_xeno_util_Toolkit_peerSetGroup__II";

const char D_004D0030[64] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II";

const char D_004D0070[80] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I";

const char D_004D00C0[56] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_";

const char D_004D00F8[64] = "Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I";

const char D_004D0138[72] = "Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_";

const char D_004D0180[56] = "Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_";

const char D_004D01B8[40] = "Java_xeno_util_Menu_setCursor__I";

const char D_004D01E0[40] = "Java_xeno_util_Menu_setVisible__Z";

const char D_004D0208[40] = "Java_xeno_util_Menu_setLocation__II";

const char D_004D0230[40] = "Java_xeno_util_Menu_getSelected__";

const char D_004D0258[32] = "Java_xeno_util_Menu_create__";

const char D_004D0278[48] = "Java_xeno_util_Menu_addItem__Ljava_lang_String_";

const char D_004D02A8[56] = "Java_xeno_util_Menu_addQuery__aLjava_lang_String_I";

const char D_004D02E0[56] = "Java_xeno_util_Menu_addQuery__Ljava_lang_String_";

const char D_004D0318[40] = "Java_xeno_util_Window_closeWaitKey__";

const char D_004D0340[56] = "Java_xeno_util_Window_setName__Ljava_lang_String_";

const char D_004D0378[56] = "Java_xeno_util_Window_print__aLjava_lang_String_I";

const char D_004D03B0[48] = "Java_xeno_util_Window_print__Ljava_lang_String_";

const char D_004D03E0[40] = "Java_xeno_util_Window_setLocation__II";

const char D_004D0408[40] = "Java_xeno_util_Window_setSize__II";

const char D_004D0430[32] = "Java_xeno_util_Window_create__I";

const char D_004D0450[32] = "Java_xeno_util_Window_wait__I";

const char D_004D0470[40] = "Java_xeno_util_Window_waitkey__I";

const char D_004D0498[32] = "Java_xeno_util_Window_close__";

const char D_004D04B8[32] = "Java_xeno_util_Window_clear__";

const char D_004D04D8[32] = "Java_xeno_util_Window_signal__I";

const char D_004D04F8[40] = "Java_xeno_util_Window_getSignal__";

const char D_004D0520[48] = "Java_xeno_util_Runtime_AGWSAllRecovery__";

const char D_004D0550[48] = "Java_xeno_util_Runtime_charAllRecovery__";

const char D_004D0580[40] = "Java_xeno_util_Runtime_etherTecSet__I";

const char D_004D05A8[40] = "Java_xeno_util_Runtime_evsExit__";

const char D_004D05D0[40] = "Java_xeno_util_Runtime_evsSetRetPoint__";

const char D_004D05F8[40] = "Java_xeno_util_Runtime_setMenuLock__";

const char D_004D0620[40] = "Java_xeno_util_Runtime_minigameExec__I";

const char D_004D0648[40] = "Java_xeno_util_Runtime_mailExec__I";

const char D_004D0670[40] = "Java_xeno_util_Runtime_mailFlag__I";

const char D_004D0698[48] = "Java_xeno_util_Runtime_mpeg2AfterCrossFade__I";

const char D_004D06C8[56] = "Java_xeno_util_Runtime_mpeg2__Ljava_lang_String_";

const char D_004D0700[48] = "Java_xeno_util_Runtime_setWindParam__IFFFF";

const char D_004D0730[48] = "Java_xeno_util_Runtime_setIdLightVec__IIFFF";

const char D_004D0760[48] = "Java_xeno_util_Runtime_setIdLightDir__IIFFF";

const char D_004D0790[48] = "Java_xeno_util_Runtime_setIdLightCol__IIFFF";

const char D_004D07C0[48] = "Java_xeno_util_Runtime_battleChangeParty__I";

const char D_004D07F0[40] = "Java_xeno_util_Runtime_checkTakeAgws__I";

const char D_004D0818[40] = "Java_xeno_util_Runtime_resetTakeAgws__I";

const char D_004D0840[40] = "Java_xeno_util_Runtime_setTakeAgws__I";

const char D_004D0868[48] = "Java_xeno_util_Runtime_checkOutFriend__I";

const char D_004D0898[48] = "Java_xeno_util_Runtime_resetOutFriend__I";

const char D_004D08C8[40] = "Java_xeno_util_Runtime_setOutFriend__I";

const char D_004D08F0[48] = "Java_xeno_util_Runtime_checkLockParty__I";

const char D_004D0920[48] = "Java_xeno_util_Runtime_resetLockParty__I";

const char D_004D0950[40] = "Java_xeno_util_Runtime_setLockParty__I";

const char D_004D0978[40] = "Java_xeno_util_Runtime_checkFriend__I";

const char D_004D09A0[40] = "Java_xeno_util_Runtime_resetFriend__I";

const char D_004D09C8[40] = "Java_xeno_util_Runtime_setFriend__I";

const char D_004D09F0[40] = "Java_xeno_util_Runtime_setPartyData__II";

const char D_004D0A18[40] = "Java_xeno_util_Runtime_getPartyData__I";

const char D_004D0A40[40] = "Java_xeno_util_Runtime_getLeader__";

const char D_004D0A68[40] = "Java_xeno_util_Runtime_enterShop__I";

const char D_004D0A90[48] = "Java_xeno_util_Runtime_progressEffect__I";

const char D_004D0AC0[40] = "Java_xeno_util_Runtime_checkGold__";

const char D_004D0AE8[40] = "Java_xeno_util_Runtime_removeGold__I";

const char D_004D0B10[40] = "Java_xeno_util_Runtime_addGold__I";

const char D_004D0B38[40] = "Java_xeno_util_Runtime_getItemName__II";

const char D_004D0B60[40] = "Java_xeno_util_Runtime_checkItem__II";

const char D_004D0B88[40] = "Java_xeno_util_Runtime_removeItem__II";

const char D_004D0BB0[40] = "Java_xeno_util_Runtime_addItemWin__II";

const char D_004D0BD8[40] = "Java_xeno_util_Runtime_addItem__II";

const char D_004D0C00[40] = "Java_xeno_util_Runtime_setShootRange__F";

const char D_004D0C28[48] = "Java_xeno_util_Runtime_setShootUwaCheck__Z";

const char D_004D0C58[48] = "Java_xeno_util_Runtime_setShootIDCheck__Z";

const char D_004D0C88[48] = "Java_xeno_util_Runtime_setShootHeightCheck__Z";

const char D_004D0CB8[40] = "Java_xeno_util_Runtime_setShootFlag__Z";

const char D_004D0CE0[40] = "Java_xeno_util_Runtime_setMap__II";

const char D_004D0D08[64] = "Java_xeno_util_Runtime_setEventTimer__Ljava_lang_String_I";

const char D_004D0D48[48] = "Java_xeno_util_Runtime_setDefocusParam__III";

const char D_004D0D78[40] = "Java_xeno_util_Runtime_setDefocus__IIaI";

const char D_004D0DA0[48] = "Java_xeno_util_Runtime_setDefocusQuick__IIII";

const char D_004D0DD0[40] = "Java_xeno_util_Runtime_jumpEvent__I";

const char D_004D0DF8[40] = "Java_xeno_util_Runtime_jumpCF__II";

const char D_004D0E20[64] = "Java_xeno_util_Runtime_CaptureStart__Ljava_lang_String_I";

const char D_004D0E60[40] = "Java_xeno_util_Runtime_CaptureEnd__";

const char D_004D0E88[40] = "Java_xeno_util_Runtime_getGameState__";

const char D_004D0EB0[40] = "Java_xeno_util_Runtime_enable__I";

const char D_004D0ED8[40] = "Java_xeno_util_Runtime_disable__I";

const char D_004D0F00[48] = "Java_xeno_util_Runtime_setPlayerMoveParam__FFF";

const char D_004D0F30[48] = "Java_xeno_util_Runtime_setPlayerControl__Z";

const char D_004D0F60[40] = "Java_xeno_util_Runtime_getEntrance__";

const char D_004D0F88[40] = "Java_xeno_util_Runtime_getRegister__I";

const char D_004D0FB0[40] = "Java_xeno_util_Runtime_setRegister__II";

const char D_004D0FD8[40] = "Java_xeno_util_Runtime_getLocation__";

const char D_004D1000[40] = "Java_xeno_util_Runtime_setLocation__III";

const char D_004D1028[40] = "Java_xeno_util_Runtime_setFlags__III";

const char D_004D1050[40] = "Java_xeno_util_Runtime_getFlags__II";

const char D_004D1078[40] = "Java_xeno_util_Runtime_execBattle__II";

const char D_004D10A0[40] = "Java_xeno_util_Layout_getManager__I";

const char D_004D10C8[48] = "Java_xeno_util_Layout_set__Ljava_lang_Object_I";

const char D_004D10F8[40] = "Java_xeno_util_Input_getRepeat__";

const char D_004D1120[32] = "Java_xeno_util_Input_getEdge__";

const char D_004D1140[40] = "Java_xeno_util_Input_getButton__";

const char D_004D1168[32] = "Java_xeno_util_Input_create__I";

const char D_004D1188[40] = "Java_xeno_util_Format_toString__Z";

const char D_004D11B0[40] = "Java_xeno_util_Format_toString__I";

const char D_004D11D8[40] = "Java_xeno_util_Format_toString__F";

const char D_004D1200[40] = "Java_xeno_util_Format_toString__C";

const char D_004D1228[48] = "Java_xeno_util_Format_toInt__Ljava_lang_String_";

const char D_004D1258[40] = "Java_xeno_util_Format_intBitsToFloat__I";

const char D_004D1280[40] = "Java_xeno_util_Format_floatToIntBits__F";

const char D_004D12A8[40] = "Java_xeno_util_Spline_getValue__I";

const char D_004D12D0[48] = "Java_xeno_util_Spline_setCtrlVertex__aFIII";

const char D_004D1300[32] = "Java_xeno_util_Spline_create__";

const char D_004D1320[32] = "Java_xeno_vm_Math_sin__F";

const char D_004D1340[32] = "Java_xeno_vm_Math_cos__F";

const char D_004D1360[32] = "Java_xeno_vm_Math_atan2__FF";

const char D_004D1380[32] = "Java_xeno_vm_Math_random__";

const char D_004D13A0[32] = "Java_xeno_vm_Thread_stop__";

const char D_004D13C0[32] = "Java_xeno_vm_Thread_start__";

const char D_004D13E0[72] = "Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_";

const char D_004D1428[32] = "Java_xeno_vm_Thread_create__";

const char D_004D1448[40] = "Java_xeno_vm_System_methodSignal__I";

const char D_004D1470[48] = "Java_xeno_vm_System_waitFor__Ljava_lang_Object_";

const char D_004D14A0[48] = "Java_xeno_vm_System_println__Ljava_lang_String_";

const char D_004D14D0[32] = "Java_xeno_vm_System_sleep__I";

const char D_004D14F0[80] = "Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II";
