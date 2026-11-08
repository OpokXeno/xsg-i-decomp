#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/runtime.h"
#include "main/twsys_init.h"

/* Window.create(int) kind: which layout initialiser (TWIN_initCF or TWIN_initScene) prepares the window. */
#define WINDOW_KIND_CF 0
#define WINDOW_KIND_SCENE 1
#define WINDOW_BUFFER_SIZE 0x40 /* command string built for MSG_print2 */

/*
 * The script VM's per-thread context (`JThread`, see include/main/jni.h).
 * Every Java native receives (thread, arguments, result).
 */
typedef struct JThread JThread;

/*
 * The toolkit text-window record that TWIN_create2 returns, as far as the
 * natives here use it: closeState (+0x32) is what signal sets, and
 * layout_offset and line_count (+0x186/+0x187) are what setSize sets.
 */
/*
 * +0x10 flags and +0x20/+0x24 x/y, as setLocation and TW_setPos use them:
 * setLocation clears the follow-owner bit (0x40) and writes x and y, while
 * TW_setPos tests that bit before it repositions the window and writes its
 * own computed position into the same two offsets.
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
 * A java.lang.String[] argument, as print__aLjava_lang_String_I reads it: the
 * object header word (unread here), the element count and the element pointer
 * array.
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

/* Returns the window's close state and clears it. */
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
    char buffer[WINDOW_BUFFER_SIZE];

    window = arguments->window;
    sprintf(buffer, windowWaitKeyCommandFormat, arguments->value);
    MSG_print2(window, buffer, -1);
}

void Java_xeno_util_Window_wait__I(JThread *thread, WindowIntCall *arguments,
                                   void *result)
{
    TComponent *window;
    char buffer[WINDOW_BUFFER_SIZE];

    window = arguments->window;
    sprintf(buffer, windowWaitCommandFormat, arguments->value);
    MSG_print2(window, buffer, -1);
}

/* Creates a text window, initialises its layout for `kind`, and returns it. */
void Java_xeno_util_Window_create__I(JThread *thread, WindowCreateCall *arguments,
                                     TComponent **result)
{
    JavaClass *window_class;
    TComponent *window;

    window_class = classJava_xeno_util_Window;
    window = TWIN_create2(-1);
    window->class_word = (int)window_class->classPointer;
    switch (arguments->kind) {
    case WINDOW_KIND_CF:
        TWIN_initCF(window);
        break;
    case WINDOW_KIND_SCENE:
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

/* Moves the window to the integer position (x, y) and clears its follow-owner flag. */
void Java_xeno_util_Window_setLocation__II(JThread *thread, WindowLocationCall *arguments,
                                           void *result)
{
    TComponent *window;

    window = arguments->window;
    window->flags &= ~TCOMP_FLAG_FOLLOW_OWNER;
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

/* Prints each string of the array to the window, in order. */
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

/* Empty native: does nothing. */
void Java_xeno_util_Window_setName__Ljava_lang_String_(JThread *thread, void *arguments,
                                                        unsigned int *result)
{
}

void Java_xeno_util_Window_closeWaitKey__(JThread *thread, WindowCall *arguments,
                                          void *result)
{
    MSG_print2(arguments->window, windowCloseWaitKeyCommand, -1);
}
