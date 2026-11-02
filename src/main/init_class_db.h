/*
 * TU-local declarations of main/tu223 (src/main/init_class_db.c).
 */

#ifndef SRC_MAIN_INIT_CLASS_DB_H
#define SRC_MAIN_INIT_CLASS_DB_H

#include "shared.h"
#include "main/data_buffer.h"
#include "main/init_class_db.h"

typedef struct ConstString ConstString;

/*
 * DataBuffer and its readers are defined by main/tu230
 * (src/main/data_buffer.c); the two entry points readClass calls are
 * declared verbatim from that TU's own src/main/data_buffer.h.
 */
typedef struct DataBuffer DataBuffer;

void setupClass(JavaClass *class_info, u32 this_class_index,
                u32 super_class_index, u32 access_flags,
                void *class_loader);

void addCode(DataBuffer *buffer, JavaClass *class_info,
             JavaMethod *method, u32 attribute_length);

/*
 * readConstantPool/readInterfaces/readFields/readMethods/readAttributes
 * (0x002f1460/0x002f1280/0x002f10e8/0x002f11c0/0x002f1318) have local
 * binding in the original symbol table, so readClass's forward declarations
 * of them are static. Parameter types come from readClass's own call sites;
 * each keeps its DataBuffer cursor and the class record being built.
 * readAttributes receives a field or method record from readFields/readMethods;
 * readClass passes a null target for class-level attributes.
 */
static void readConstantPool(DataBuffer *buffer, JavaClass *class_info);

static void readInterfaces(DataBuffer *buffer, JavaClass *class_info);
static void readFields(DataBuffer *buffer, JavaClass *class_info);
static void readMethods(DataBuffer *buffer, JavaClass *class_info);

static void readAttributes(DataBuffer *buffer, JavaClass *class_info,
                            void *attribute_target);

JavaClass *readClass(DataBuffer *buffer, JavaClass *class_info,
                     void *class_loader);

#endif /* SRC_MAIN_INIT_CLASS_DB_H */
