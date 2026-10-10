#ifndef INCLUDE_MAIN_INIT_VM_H
#define INCLUDE_MAIN_INIT_VM_H

#include "shared.h"

/*
 * The class records of java.lang.Class, java.lang.Object, java.lang.String
 * and java.lang.StringBuffer, resolved by initVM (main/tu226).  _dummyClass
 * itself is the static class record find_native_method.c allocates for
 * java.lang.Class; classClass points at it.  classObject and classClass are
 * only read by the class loader, through its full JavaClass layout;
 * classString and classStringBuffer are passed to newObject.
 */
extern JavaClass _dummyClass;
extern JavaClass *classClass;
extern JavaClass *classObject;
extern SceneClass *classString;
extern SceneClass *classStringBuffer;

#endif /* INCLUDE_MAIN_INIT_VM_H */
