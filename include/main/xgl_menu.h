#ifndef INCLUDE_MAIN_XGL_MENU_H
#define INCLUDE_MAIN_XGL_MENU_H

typedef struct XglMenuNode XglMenuNode;

typedef struct XglMenuRow {
    const char *text;
    XglMenuNode *next;
} XglMenuRow;

struct XglMenuNode {
    unsigned char item_count;
    unsigned char unmodeled_01[3];
    signed char selected_item;
    unsigned char unmodeled_05[19];
    XglMenuRow *rows;
};

#endif /* INCLUDE_MAIN_XGL_MENU_H */
