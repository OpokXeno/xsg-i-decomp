#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/runtime.h"
#include "main/twsys_init.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map (and in main/tu238, src/main/runtime.c) show.
 */
typedef struct JThread JThread;

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
/*
 * +0x10 flags and +0x20/+0x24 x/y, as getSignal__/setLocation__II and
 * src/main/twsys_init.c's TW_setPos (still assembler) touch them:
 * TW_setPos tests bit 0x40 of the same offset TComponent models as flags
 * before it repositions the window, and writes its own computed position
 * into the same two offsets; setLocation__II clears that bit and overwrites
 * both floats with its own arguments instead.
 */

/* The call block of every Window native taking no Java arguments beyond the
   window itself. */
typedef struct WindowCall {
    TComponent *window;
} WindowCall;

/* The call block of Window.signal(int): the target window and the value
   stored into its closeState field. */
typedef struct WindowSignalCall {
    TComponent *window;
    unsigned short value;
} WindowSignalCall;

/* The call block of Window.waitkey(int)/Window.wait(int): the target window
   and the value formatted into the control string. */
typedef struct WindowIntCall {
    TComponent *window;
    int value;
} WindowIntCall;

/*
 * The call block of Window.create(int): the single int argument selects
 * which toolkit initialiser prepares the new window.
 */
typedef struct WindowCreateCall {
    int kind;
} WindowCreateCall;

/* The call block of Window.setSize(int, int): the target window and the two
   byte-sized values stored into its layout_offset/line_count fields. */
typedef struct WindowSizeCall {
    TComponent *window;              /* +0x00 */
    unsigned char first;          /* +0x04 */
    unsigned char unmodeled_05[3];
    unsigned char second;         /* +0x08 */
} WindowSizeCall;

/* The call block of Window.setLocation(int, int): the target window and the
   integer x/y position converted to float. */
typedef struct WindowLocationCall {
    TComponent *window;
    int x;
    int y;
} WindowLocationCall;

/* The call block of Window.print(String): the target window and the string
   to print. */
typedef struct WindowStringCall {
    TComponent *window;
    StringRef *string;
} WindowStringCall;

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
    TComponent *window;
    JavaStringArray *array;
} WindowStringArrayCall;

extern void MSG_print2(TComponent *window, const char *text, int length);
extern int sprintf(char *destination, const char *format, ...);
extern void TWIN_initCF(TComponent *window);
extern void TWIN_initScene(TComponent *window);
extern void TWIN_init2(TComponent *window);

const char windowClearCommand[] = "/[clear()]";

const char windowCloseCommand[] = "/[close()]";

const char windowWaitKeyCommandFormat[] = "/[waitkey(%d)]";

const char windowWaitCommandFormat[] = "/[wait(%d)]";

const char windowCloseWaitKeyCommand[] = "/[waitkey(0);close()]";

void Java_xeno_util_Window_getSignal__(JThread *thread, WindowCall *arguments,
                                       int *result)
{
    TComponent *window;
    short signal;

    /* Window's load stays in its own do/while(0) block: inlined into the
       statements below, cc1 schedules the closeState clear right after
       the load and defers the *result store into the branch delay slot,
       the reverse of the original order (result stored first, clear
       delayed last). */
    do {
        window = arguments->window;
    } while (0);
    signal = (short)window->closeState;
    window->closeState = 0;
    *result = signal;
}

void Java_xeno_util_Window_signal__I(JThread *thread, WindowSignalCall *arguments,
                                     void *result)
{
    arguments->window->closeState = arguments->value;
}

void Java_xeno_util_Window_clear__(JThread *thread, WindowCall *arguments,
                                   void *result)
{
    MSG_print2(arguments->window, windowClearCommand, -1);
}

void Java_xeno_util_Window_close__(JThread *thread, WindowCall *arguments,
                                   void *result)
{
    MSG_print2(arguments->window, windowCloseCommand, -1);
}

void Java_xeno_util_Window_waitkey__I(JThread *thread, WindowIntCall *arguments,
                                      void *result)
{
    TComponent *window;
    char buffer[0x40];

    window = arguments->window;
    sprintf(buffer, windowWaitKeyCommandFormat, arguments->value);
    MSG_print2(window, buffer, -1);
}

void Java_xeno_util_Window_wait__I(JThread *thread, WindowIntCall *arguments,
                                   void *result)
{
    TComponent *window;
    char buffer[0x40];

    window = arguments->window;
    sprintf(buffer, windowWaitCommandFormat, arguments->value);
    MSG_print2(window, buffer, -1);
}

void Java_xeno_util_Window_create__I(JThread *thread, WindowCreateCall *arguments,
                                     TComponent **result)
{
    JavaClass *window_class;
    TComponent *window;

    window_class = classJava_xeno_util_Window;
    window = TWIN_create2(-1);
    window->class_word = (int)window_class->classPointer;
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

void Java_xeno_util_Window_setSize__II(JThread *thread, WindowSizeCall *arguments,
                                       void *result)
{
    TComponent *window;
    unsigned char second;

    window = arguments->window;
    second = arguments->second;
    window->layout_offset = second;
    window->line_count = arguments->first;
    TWIN_init2(window);
}

void Java_xeno_util_Window_setLocation__II(JThread *thread, WindowLocationCall *arguments,
                                           void *result)
{
    TComponent *window;

    window = arguments->window;
    window->flags &= ~0x40;
    /* x's store stays in its own do/while(0) block: inlined into the
       statements around it, cc1 hoists the y load/convert ahead of it and
       defers x's own store next to the branch delay slot instead of right
       after its conversion, the reverse of the original order (x stored
       first, y converted and delayed last). */
    do {
        window->x = (float)arguments->x;
    } while (0);
    window->y = (float)arguments->y;
}

void Java_xeno_util_Window_print__Ljava_lang_String_(JThread *thread,
                                                      WindowStringCall *arguments,
                                                      void *result)
{
    TComponent *window;
    StringRef *string;
    struct InternedString *storage;

    window = arguments->window;
    string = arguments->string;
    if (window != 0 && string != 0) {
        storage = string->value;
        MSG_print2(window, storage->bytes, storage->length);
    }
}

void Java_xeno_util_Window_print__aLjava_lang_String_I(JThread *thread,
                                                        WindowStringArrayCall *arguments,
                                                        void *result)
{
    TComponent *window;
    JavaStringArray *array;
    StringRef *element;
    struct InternedString *storage;
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

void Java_xeno_util_Window_closeWaitKey__(JThread *thread, WindowCall *arguments,
                                          void *result)
{
    MSG_print2(arguments->window, windowCloseWaitKeyCommand, -1);
}
