#include "common.h"
#include "shared.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map (and in main/tu238, src/main/runtime.c) show.
 */
typedef struct JThread JThread;

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_getSignal__);

/*
 * The toolkit text-window record TWIN_create2 returns, as far as this TU's
 * natives touch it. +0x32 is what signal__I stores its requested value
 * into; src/main/twsys_init.c's TComponent models the same offset of the
 * same record as closeState, cleared with the component's flags when its
 * slot is (re)claimed. +0x186/+0x187 are the bytes setSize__II overwrites
 * from its two arguments; src/main/twsys_init.c's TWindow models the same
 * two offsets as layout_offset (TWIN_create2's own per-kind default) and
 * line_count (the bound TWIN_update2/TWIN_popCF/TWIN_popScene loop up to).
 */
typedef struct UtilWindow {
    int class_ref;                         /* +0x00 */
    unsigned char unmodeled_04[0x32 - 4];
    unsigned short closeState;             /* +0x32 */
    unsigned char unmodeled_34[0x186 - 0x34];
    unsigned char layout_offset;           /* +0x186 */
    unsigned char line_count;              /* +0x187 */
} UtilWindow;

extern void MSG_print2(UtilWindow *window, const char *text, int length);

/* The call block of Window.signal(int): the target window and the value
   stored into its closeState field. */
typedef struct WindowSignalCall {
    UtilWindow *window;
    unsigned short value;
} WindowSignalCall;

void Java_xeno_util_Window_signal__I(JThread *thread, WindowSignalCall *arguments,
                                     void *result)
{
    arguments->window->closeState = arguments->value;
}

/* The call block of every Window native taking no Java arguments beyond the
   window itself. */
typedef struct WindowCall {
    UtilWindow *window;
} WindowCall;

extern const char D_004D1658[]; /* "/[clear()]" */

void Java_xeno_util_Window_clear__(JThread *thread, WindowCall *arguments,
                                   void *result)
{
    MSG_print2(arguments->window, D_004D1658, -1);
}

extern const char D_004D1668[]; /* "/[close()]" */

void Java_xeno_util_Window_close__(JThread *thread, WindowCall *arguments,
                                   void *result)
{
    MSG_print2(arguments->window, D_004D1668, -1);
}

/* The call block of Window.waitkey(int)/Window.wait(int): the target window
   and the value formatted into the control string. */
typedef struct WindowIntCall {
    UtilWindow *window;
    int value;
} WindowIntCall;

extern const char D_004D1678[]; /* "/[waitkey(%d)]" */
extern int sprintf(char *destination, const char *format, ...);

void Java_xeno_util_Window_waitkey__I(JThread *thread, WindowIntCall *arguments,
                                      void *result)
{
    UtilWindow *window;
    char buffer[0x40];

    window = arguments->window;
    sprintf(buffer, D_004D1678, arguments->value);
    MSG_print2(window, buffer, -1);
}

extern const char D_004D1688[]; /* "/[wait(%d)]" */

void Java_xeno_util_Window_wait__I(JThread *thread, WindowIntCall *arguments,
                                   void *result)
{
    UtilWindow *window;
    char buffer[0x40];

    window = arguments->window;
    sprintf(buffer, D_004D1688, arguments->value);
    MSG_print2(window, buffer, -1);
}

/*
 * The xeno/util/Window class record the VM loaded: create__I stamps the
 * window it builds with the class's own instance class ref, so the native
 * window can be handed back as a xeno/util/Window instance. It is the same
 * word newObject (main 0x002f4828) seeds a freshly allocated instance with
 * and SCENE_instance (src/main/scene_1.c) writes into the window it builds
 * with the same TWIN_create2 call.
 *
 * That word is the class ref of the object header every VM instance
 * reference begins with, but this TU only copies it and never follows it,
 * and UtilWindow.class_ref keeps it as the plain word it is copied as:
 * create__I writes it (main 0x002f88f0) and only then reads its call
 * block's argument word (main 0x002f88f4), an order the compiler holds
 * for two accesses of one type and not for a header modeled with the
 * pointer member SceneObjectHeader declares.
 */
extern SceneClass *classJava_xeno_util_Window;

/*
 * The call block of Window.create(int): the single int argument selects
 * which toolkit initialiser prepares the new window.
 */
typedef struct WindowCreateCall {
    int kind;
} WindowCreateCall;

extern UtilWindow *TWIN_create2(int component_id);
extern void TWIN_initCF(UtilWindow *window);
extern void TWIN_initScene(UtilWindow *window);

void Java_xeno_util_Window_create__I(JThread *thread, WindowCreateCall *arguments,
                                     UtilWindow **result)
{
    SceneClass *window_class;
    UtilWindow *window;

    window_class = classJava_xeno_util_Window;
    window = TWIN_create2(-1);
    window->class_ref = (int)window_class->instance_class_ref;
    switch (arguments->kind) {
    case 0:
        TWIN_initCF(window);
        break;
    case 1:
        TWIN_initScene(window);
        break;
    }
    *result = window;
}

/* The call block of Window.setSize(int, int): the target window and the two
   byte-sized values stored into its layout_offset/line_count fields. */
typedef struct WindowSizeCall {
    UtilWindow *window;              /* +0x00 */
    unsigned char first;          /* +0x04 */
    unsigned char unmodeled_05[3];
    unsigned char second;         /* +0x08 */
} WindowSizeCall;

extern void TWIN_init2(UtilWindow *window);

void Java_xeno_util_Window_setSize__II(JThread *thread, WindowSizeCall *arguments,
                                       void *result)
{
    UtilWindow *window;
    unsigned char second;

    window = arguments->window;
    second = arguments->second;
    window->layout_offset = second;
    window->line_count = arguments->first;
    TWIN_init2(window);
}

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_setLocation__II);

/*
 * A java.lang.String argument, as the print natives below read it: +0x00
 * (the class word every object reference begins with) is untouched by this
 * TU, and +0x04 holds the interned string record the String was loaded from
 * (the same shape src/main/runtime.c's StringRef reads for its own String
 * arguments).
 */
typedef struct StringStorage StringStorage;

typedef struct StringRef {
    void *unmodeled_00;
    StringStorage *value;   /* +0x04 */
} StringRef;

/*
 * The interned constant-string record StringRef.value points to: the print
 * natives below read the byte count at +0x04 and the bytes at +0x08 as
 * MSG_print2's length/text pair.
 */
struct StringStorage {
    unsigned char unmodeled_00[4];
    int length;              /* +0x04 */
    const char *bytes;       /* +0x08 */
};

/* The call block of Window.print(String): the target window and the string
   to print. */
typedef struct WindowStringCall {
    UtilWindow *window;
    StringRef *string;
} WindowStringCall;

void Java_xeno_util_Window_print__Ljava_lang_String_(JThread *thread,
                                                      WindowStringCall *arguments,
                                                      void *result)
{
    UtilWindow *window;
    StringRef *string;
    StringStorage *storage;

    window = arguments->window;
    string = arguments->string;
    if (window != 0 && string != 0) {
        storage = string->value;
        MSG_print2(window, storage->bytes, storage->length);
    }
}

/*
 * A java.lang.String[] argument, as print__aLjava_lang_String_I reads it:
 * the object header word every VM array reference begins with (unread
 * here), the element count and the element pointer array (the same shape
 * src/main/camera.h's JavaFloatArray reads for a float[] argument).
 */
typedef struct JavaStringArray {
    unsigned int : 32;      /* +0x00 */
    unsigned int length;    /* +0x04 */
    StringRef **elements;   /* +0x08 */
} JavaStringArray;

/* The call block of Window.print(String[], int): the target window and the
   string array; the trailing int is unused by this native. */
typedef struct WindowStringArrayCall {
    UtilWindow *window;
    JavaStringArray *array;
} WindowStringArrayCall;

void Java_xeno_util_Window_print__aLjava_lang_String_I(JThread *thread,
                                                        WindowStringArrayCall *arguments,
                                                        void *result)
{
    UtilWindow *window;
    JavaStringArray *array;
    StringRef *element;
    StringStorage *storage;
    unsigned int index;
    unsigned int length;

    window = arguments->window;
    array = arguments->array;
    if (window != 0) {
        if (array != 0) {
            index = 0;
            if (array->length != 0) {
                do {
                    element = array->elements[index];
                    index++;
                    storage = element->value;
                    MSG_print2(window, storage->bytes, storage->length);
                    length = array->length;
                } while (index < length);
            }
        }
    }
}

void Java_xeno_util_Window_setName__Ljava_lang_String_(JThread *thread, void *arguments,
                                                        unsigned int *result)
{
}

extern const char D_004D1698[]; /* "/[waitkey(0);close()]" */

void Java_xeno_util_Window_closeWaitKey__(JThread *thread, WindowCall *arguments,
                                          void *result)
{
    MSG_print2(arguments->window, D_004D1698, -1);
}
