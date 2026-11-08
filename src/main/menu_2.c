#include "common.h"
#include "shared.h"
#include "main/jni.h"

/*
 * The script VM's per-thread context (see src/main/chr.h). Every Java native
 * receives (thread, arguments, result), the same convention as the natives in
 * src/main/runtime.c and src/main/window.c.
 */
typedef struct JThread JThread;

typedef struct MenuStringData MenuStringData;

/* Maximum number of query strings Menu.addQuery(String[], int) forwards to the menu. */
#define MENU_QUERY_MAX 32

/* MenuNative.flags: the menu must refresh its cursor (set by Menu.setCursor). */
#define MENU_NATIVE_FLAG_REFRESH 0x200

/*
 * A java.lang.String argument: the class word at +0x00 is not read by this TU,
 * and +0x04 holds the interned string record the String was loaded from.
 */
typedef struct MenuStringRef {
    void *unmodeled_00;
    MenuStringData *value;    /* +0x04 */
} MenuStringRef;

/* The interned string record that MenuStringRef.value points to; only the bytes pointer at +0x08 is modeled. */
struct MenuStringData {
    unsigned char unmodeled_00[8];
    char *bytes;              /* +0x08 */
};

/*
 * A java.lang.String[] argument as addQuery(String[], int) reads it: the element
 * count at +0x04 and the element pointer array at +0x08.
 */
typedef struct MenuStringArray {
    unsigned int : 32;        /* +0x00 */
    unsigned int length;      /* +0x04 */
    MenuStringRef **elements;     /* +0x08 */
} MenuStringArray;

/*
 * The native menu record this TU's natives operate on (the same object as
 * TMenu described in src/main/twsys_init.c), as far as this TU touches it: the
 * refresh-request flags word at +0x10 setCursor__I ors 0x200 into, the
 * current selection index at +0x54 (read as a signed byte) and the
 * requested cursor value setCursor__I stores at +0x68.
 */
typedef struct MenuNative {
    void *class_ref;                  /* +0x00 */
    unsigned char unmodeled_04[0x10 - 0x04];
    unsigned int flags;             /* +0x10 */
    unsigned char unmodeled_14[0x20 - 0x14];
    float x;                         /* +0x20 */
    float y;                         /* +0x24 */
    unsigned char unmodeled_28[0x54 - 0x28];
    signed char selected;           /* +0x54 */
    signed char state;              /* +0x55 */
    unsigned char unmodeled_56[0x68 - 0x56];
    int cursor;                     /* +0x68 */
} MenuNative;

/* The call block of a native taking (java.lang.String): the target menu
   and the string. */
typedef struct MenuStringCall {
    MenuNative *menu;
    MenuStringRef *string;
} MenuStringCall;

/* The call block of Menu.addQuery(String[], int): the target menu and the
   query strings; the trailing int is unused by this native. */
typedef struct MenuStringArrayCall {
    MenuNative *menu;
    MenuStringArray *array;
} MenuStringArrayCall;

/* These natives receive VM argument/result slots at four-byte intervals: the
 * original accesses the object at +0, location integers at +4/+8 and results
 * at +0. */
typedef union MenuArgumentSlot {
    int i;
    float f;
    void *ref;
} MenuArgumentSlot;

/* The call block of Menu.setCursor(int): the target menu and the
   requested cursor value. */
typedef struct MenuCursorCall {
    MenuNative *menu;
    int cursor;
} MenuCursorCall;

extern void TMENU_addQuery(MenuNative *menu, const char *text);
extern void TMENU_addQuery2(MenuNative *menu, char **texts, int count);
extern void TMENU_addItem(MenuNative *menu, const char *text);
extern MenuNative *TMENU_create(int menu_kind);

void Java_xeno_util_Menu_addQuery__Ljava_lang_String_(JThread *thread,
                                                       MenuStringCall *arguments,
                                                       unsigned int *result)
{
    TMENU_addQuery(arguments->menu, arguments->string->value->bytes);
}

/* Passes up to MENU_QUERY_MAX query strings from the String[] to the menu. */
void Java_xeno_util_Menu_addQuery__aLjava_lang_String_I(JThread *thread,
                                                         MenuStringArrayCall *arguments,
                                                         unsigned int *result)
{
    char *texts[MENU_QUERY_MAX];
    MenuStringArray *array;
    MenuNative *menu;
    int count;
    int index;

    array = arguments->array;
    count = array->length;
    if (count > MENU_QUERY_MAX) {
        count = MENU_QUERY_MAX;
    }
    menu = arguments->menu;
    for (index = 0; index < count; index++) {
        texts[index] = array->elements[index]->value->bytes;
    }
    TMENU_addQuery2(menu, texts, count);
}

void Java_xeno_util_Menu_addItem__Ljava_lang_String_(JThread *thread,
                                                      MenuStringCall *arguments,
                                                      unsigned int *result)
{
    TMENU_addItem(arguments->menu, arguments->string->value->bytes);
}

/* Creates a native menu, stores its class reference at +0x00 and returns it. */
void Java_xeno_util_Menu_create__(JThread *thread, void *arguments,
                                  MenuArgumentSlot *result)
{
    JavaClass *menuClass;
    MenuNative *menu;

    menuClass = classJava_xeno_util_Menu;
    menu = TMENU_create(-1);
    menu->class_ref = menuClass->classPointer;
    result[0].ref = menu;
}

/* Returns the state byte when it is negative, otherwise the selected index. */
void Java_xeno_util_Menu_getSelected__(JThread *thread,
                                       MenuArgumentSlot *arguments,
                                       MenuArgumentSlot *result)
{
    MenuNative *menu;

    menu = arguments[0].ref;
    if (menu->state < 0)
        result[0].i = menu->state;
    else
        result[0].i = menu->selected;
}

/* Stores the two integer location arguments as the menu's x and y floats. */
void Java_xeno_util_Menu_setLocation__II(JThread *thread,
                                         MenuArgumentSlot *arguments,
                                         MenuArgumentSlot *result)
{
    MenuNative *menu;

    menu = arguments[0].ref;
    menu->x = (float)arguments[1].i;
    menu->y = (float)arguments[2].i;
}

/* Empty native. */
void Java_xeno_util_Menu_setVisible__Z(void)
{
}

/* Stores the requested cursor and flags a refresh when it differs from the current selection. */
void Java_xeno_util_Menu_setCursor__I(JThread *thread, MenuCursorCall *arguments,
                                      unsigned int *result)
{
    MenuNative *menu;
    int cursor;

    menu = arguments->menu;
    cursor = arguments->cursor;
    if (menu->selected != cursor) {
        menu->cursor = cursor;
        menu->flags |= MENU_NATIVE_FLAG_REFRESH;
    }
}
