#ifndef INCLUDE_MAIN_JNI_H
#define INCLUDE_MAIN_JNI_H

#include "shared.h"
#include "main/data_buffer.h"

/*
 * The xeno.* class slots JNI_loadNativeClass fills through loadStaticClass
 * (main/tu221, src/main/jni.c).  Every slot holds a class record; the natives
 * read it through the shared SceneClass view (instance_class_ref at +0x18,
 * lookupClassField, JNI_isInstanceOf, newObject).
 */
extern SceneClass *classJava_xeno_Camera;
extern SceneClass *classJava_xeno_Chr;
extern SceneClass *classJava_xeno_Effect;
extern SceneClass *classJava_xeno_Enepc;
extern SceneClass *classJava_xeno_Light;
extern SceneClass *classJava_xeno_Movie;
extern SceneClass *classJava_xeno_PlayControl;
extern SceneClass *classJava_xeno_Scene;
extern SceneClass *classJava_xeno_Stage;
extern SceneClass *classJava_xeno_Unit;
extern SceneClass *classJava_xeno_Uwamono;
extern SceneClass *classJava_xeno_util_Format;
extern SceneClass *classJava_xeno_util_Input;
extern SceneClass *classJava_xeno_util_Layout;
extern SceneClass *classJava_xeno_util_Menu;
extern SceneClass *classJava_xeno_util_Runtime;
extern SceneClass *classJava_xeno_util_Spline;
extern SceneClass *classJava_xeno_util_TCHParams;
extern SceneClass *classJava_xeno_util_Toolkit;
extern SceneClass *classJava_xeno_util_Vector4f;
extern SceneClass *classJava_xeno_util_Window;
extern SceneClass *classJava_xeno_vm_System;

#endif /* INCLUDE_MAIN_JNI_H */
