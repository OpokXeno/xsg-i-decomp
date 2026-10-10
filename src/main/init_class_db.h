/*
 * TU-local declarations of main/tu223 (src/main/init_class_db.c).
 */

#ifndef SRC_MAIN_INIT_CLASS_DB_H
#define SRC_MAIN_INIT_CLASS_DB_H

#include "shared.h"
#include "main/data_buffer.h"

typedef struct ClassDescriptor ClassDescriptor;
typedef struct ConstString ConstString;
extern ConstString **constStringTable;

typedef u32 ConstantPoolWord;

typedef union ClassConstantPoolValue {
    void *object;
    unsigned char *tags;
} ClassConstantPoolValue;

typedef struct ClassFieldRecord {
    SceneString *name;
    u16 flags;
    unsigned char unmodeled_06[2];
    void *type_or_descriptor;
    u32 field_size;
    u32 instance_offset;
} ClassFieldRecord;

typedef struct ClassSignatureResult {
    unsigned char unmodeled_00[0x3a];
    unsigned char field_size;
} ClassSignatureResult;

/*
 * This is the runtime class-file descriptor used by the native loader.
 */
struct ClassDescriptor {
    void *object_methods;
    void * volatile class_name;
    u16 volatile access_flags;
    u16 type_flags;
    u32 volatile class_loader;
    u32 volatile super_class_index;
    void *constant_pool;
    void *dispatch_methods;
    void *fields;
    void *methods;
    void *interfaces;
    u16 constant_pool_count;
    u16 dispatch_method_count;
    u16 method_count;
    u16 interface_class_count;
    u16 field_count;
    u16 static_field_count;
    u16 interface_count;
    u16 volatile processing_state;
    u32 volatile instance_size;
    ClassDescriptor *component_class;
};

void setupClass(ClassDescriptor *class_info, u32 this_class_index,
                u32 super_class_index, u32 access_flags,
                u32 class_loader);

/*
 * classDB (0x0099d430, size 0x20) is the native loader's 8-slot table of
 * resolved classes indexed by class id (JNI_resolveClass in
 * src/main/jni.c); initClassDB clears every slot at JNI_initSystem startup.
 */
extern int classDB[8];

void initClassDB(void);

/*
 * SceneMethod (forward-declared by shared.h) is completed here from
 * addCode's evidenced Code_attribute fields (JVM class file format): the
 * code pointer at +0x14, max_stack/max_locals at +0x18/+0x1a and a
 * 16-bit code_length at +0x1c holding the low half of the classfile's
 * 32-bit attribute length. addMethod also establishes the name, descriptor,
 * parameter and return sizes, declaring class, access flags and state.
 */
struct SceneMethod {
    SceneString *name;
    SceneTypeDescriptor *descriptor;
    s16 parameter_size;
    s16 return_size;
    ClassDescriptor *declaring_class;
    u16 access_flags;
    u16 state;
    void *code;
    u16 max_stack;
    u16 max_locals;
    u16 code_length;
    u16 unmodeled_1e;
};

/*
 * DataBuffer_seek is defined by main/tu230 (src/main/data_buffer.c) but,
 * like DataBuffer_getUShortAt/DataBuffer_getUIntAt above, is not declared
 * by that TU's own src/main/data_buffer.h, so addCode's use of it is
 * declared verbatim here too.
 */
void DataBuffer_seek(DataBuffer *buffer, int offset);

void addCode(DataBuffer *buffer, ClassDescriptor *class_info,
             SceneMethod *method, u32 attribute_length);

/*
 * DataBuffer and its readers are defined by main/tu230
 * (src/main/data_buffer.c); the two entry points readClass calls are
 * declared verbatim from that TU's own src/main/data_buffer.h.
 */
typedef struct DataBuffer DataBuffer;

unsigned short DataBuffer_getUShortAt(DataBuffer *buffer);

unsigned int DataBuffer_getUIntAt(DataBuffer *buffer);

/*
 * readConstantPool/readInterfaces/readFields/readMethods/readAttributes
 * (0x002f1460/0x002f1280/0x002f10e8/0x002f11c0/0x002f1318) have local
 * binding in the original symbol table, so readClass's forward declarations
 * of them are static. Parameter types come from readClass's own call sites;
 * each keeps its DataBuffer cursor and the ClassDescriptor being built.
 * readAttributes receives a field or method record from readFields/readMethods;
 * readClass passes a null target for class-level attributes.
 */
static void readConstantPool(DataBuffer *buffer, ClassDescriptor *class_info);

static void readInterfaces(DataBuffer *buffer, ClassDescriptor *class_info);

static void readFields(DataBuffer *buffer, ClassDescriptor *class_info);

static void readMethods(DataBuffer *buffer, ClassDescriptor *class_info);

static void readAttributes(DataBuffer *buffer, ClassDescriptor *class_info,
                            void *attribute_target);

ClassDescriptor *readClass(DataBuffer *buffer, ClassDescriptor *class_info,
                           u32 class_loader);

#endif /* SRC_MAIN_INIT_CLASS_DB_H */
