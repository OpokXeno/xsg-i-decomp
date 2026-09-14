/*
 * TU-local declarations of main/tu198 (src/main/call_java_method.c).
 */

#ifndef SRC_MAIN_CALL_JAVA_METHOD_H
#define SRC_MAIN_CALL_JAVA_METHOD_H

/* from src/math/main/review09-002f6c50/private.h (unit math-main-002f6c50-review09, function LAYOUT_mapID_setUnit @ 0x002f6c50) */
/* UnduDataGetHeader returns this bounded four-component result. */
typedef struct LayoutHeader {
    float components[4];
} LayoutHeader;

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType @ 0x002d6668) */
char Get_LocaterType(short locator_index);

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType_Angle @ 0x002d6748) */
char Get_LocaterType_Angle(float angle);

/* from src/math/main/correction13-main-002d6668-allocation-2125441c2df4/form-4/candidate.c (unit math-correction13-4, function Get_LocaterType @ 0x002d6668) */
extern float LocaterAngle[16];

/* canon: config/header-canon.json chose src/math/main/review09-002f6c50/private.h over 1 other accepted spelling */
/* from src/math/main/review09-002f6c50/private.h (unit math-main-002f6c50-review09, function LAYOUT_mapID_setUnit @ 0x002f6c50) */
extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

#endif /* SRC_MAIN_CALL_JAVA_METHOD_H */
