#ifndef SRC_MAIN_SYSTEM_H
#define SRC_MAIN_SYSTEM_H

#include "shared.h"
#include "main/jni.h"
#include "main/init_vm.h"
#include "main/scene_1.h"

/* The thread member names and offsets follow main/tu221, src/main/jni.h. */
struct JThreadNode {
    unsigned char unmodeled_00[8];
    struct JThreadNode *next;
    unsigned char kind;
    unsigned char wait_kind;
    unsigned char unmodeled_0e[2];
    SceneObject object;
    void (*reset)(struct JThreadNode *thread);
    unsigned char unmodeled_18[12];
    unsigned int flags;
    unsigned char unmodeled_28[8];
    void *wait_target;
    int wait_parameter;
    unsigned char unmodeled_38[4];
    unsigned short resume_frames;
    unsigned short frame_depth;
};

struct JThreadNode;

/* Native argument/result slots: main/tu226, src/main/init_vm.c. */
/* String storage and its owning instance follow main/tu139, src/main/scene_1.h. */
struct SceneString {
    SceneObjectHeader header;
    struct SceneStringStorage *storage;
};

extern char tmpString[];
extern const char systemFloatFormat[];

extern char *strncpy(char *destination, const char *source, unsigned int length);
extern int PARSE_int(const char *text, int length, int radix);
extern int JNI_getRegister(int register_index);
extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);
extern struct JThreadNode *JTHREAD_get(SceneObject object);

#endif
