#include "common.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in the TU
 * map (src/main/runtime.c, src/main/window.c) show.
 */
typedef struct JThread JThread;

/*
 * A java.lang.String argument, as the natives below read it: +0x00 (the
 * class word every object reference begins with) is untouched by this TU,
 * and +0x04 holds the interned string record the String was loaded from
 * (the same shape src/main/runtime.c's StringRef and src/main/window.c's
 * StringRef read for their own String arguments; named separately here
 * because those TUs already own that tag, config/header-canon.json).
 */
typedef struct MenuStringData MenuStringData;

typedef struct MenuStringRef {
    void *unmodeled_00;
    MenuStringData *value;    /* +0x04 */
} MenuStringRef;

/*
 * The interned constant-string record MenuStringRef.value points to, modeled
 * here only for the bytes pointer the natives below read at +0x08 (the
 * same offset src/main/runtime.c's InternedString.bytes and
 * src/main/window.c's StringStorage.bytes read).
 */
struct MenuStringData {
    unsigned char unmodeled_00[8];
    char *bytes;              /* +0x08 */
};

/*
 * A java.lang.String[] argument, as addQuery(String[], int) reads it: the
 * class word at +0x00, the element count at +0x04 and the element pointer
 * array at +0x08 (the same shape src/main/window.c's JavaStringArray reads
 * for its own String[] argument; named separately here because that TU
 * already owns that tag).
 */
typedef struct MenuStringArray {
    unsigned int : 32;        /* +0x00 */
    unsigned int length;      /* +0x04 */
    MenuStringRef **elements;     /* +0x08 */
} MenuStringArray;

/*
 * The native menu record this TU's natives operate on (the same object as
 * TMenu in src/main/twsys_init.c), as far as this TU touches it: the
 * refresh-request flags word at +0x10 setCursor__I ors 0x200 into, the
 * current selection index at +0x54 (read as a signed byte) and the
 * requested cursor value setCursor__I stores at +0x68.
 */
typedef struct MenuNative {
    unsigned char unmodeled_00[0x10];
    unsigned int flags;             /* +0x10 */
    unsigned char unmodeled_14[0x54 - 0x14];
    signed char selected;           /* +0x54 */
    unsigned char unmodeled_55[0x68 - 0x55];
    int cursor;                     /* +0x68 */
} MenuNative;

extern void TMENU_addQuery(MenuNative *menu, const char *text);
extern void TMENU_addQuery2(MenuNative *menu, char **texts, int count);
extern void TMENU_addItem(MenuNative *menu, const char *text);

/* The call block of a native taking (java.lang.String): the target menu
   and the string. */
typedef struct MenuStringCall {
    MenuNative *menu;
    MenuStringRef *string;
} MenuStringCall;

void Java_xeno_util_Menu_addQuery__Ljava_lang_String_(JThread *thread,
                                                       MenuStringCall *arguments,
                                                       unsigned int *result)
{
    TMENU_addQuery(arguments->menu, arguments->string->value->bytes);
}

/* The call block of Menu.addQuery(String[], int): the target menu and the
   query strings; the trailing int is unused by this native. */
typedef struct MenuStringArrayCall {
    MenuNative *menu;
    MenuStringArray *array;
} MenuStringArrayCall;

void Java_xeno_util_Menu_addQuery__aLjava_lang_String_I(JThread *thread,
                                                         MenuStringArrayCall *arguments,
                                                         unsigned int *result)
{
    char *texts[32];
    MenuStringArray *array;
    MenuNative *menu;
    int count;
    int index;

    array = arguments->array;
    count = array->length;
    if (count > 32) {
        count = 32;
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

INCLUDE_ASM("asm/main/nonmatchings/menu_2", Java_xeno_util_Menu_create__);

INCLUDE_ASM("asm/main/nonmatchings/menu_2", Java_xeno_util_Menu_getSelected__);

INCLUDE_ASM("asm/main/nonmatchings/menu_2", Java_xeno_util_Menu_setLocation__II);

void Java_xeno_util_Menu_setVisible__Z(void)
{
}

/* The call block of Menu.setCursor(int): the target menu and the
   requested cursor value. */
typedef struct MenuCursorCall {
    MenuNative *menu;
    int cursor;
} MenuCursorCall;

void Java_xeno_util_Menu_setCursor__I(JThread *thread, MenuCursorCall *arguments,
                                      unsigned int *result)
{
    MenuNative *menu;
    int cursor;

    menu = arguments->menu;
    cursor = arguments->cursor;
    if (menu->selected != cursor) {
        menu->cursor = cursor;
        menu->flags |= 0x200;
    }
}
