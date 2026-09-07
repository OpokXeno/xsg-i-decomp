/*
 * MenuLoadCount is the original GLOBAL .sdata word used by the menu-load
 * status API.  Its detailed negative-status meanings remain unresolved; this
 * partial TU recovers only the two GLOBAL accessors below and does not define
 * the external storage.
 */
extern int MenuLoadCount;

int MenuLoadSync(void)
{
    return MenuLoadCount;
}

void MenuLoadInit(void)
{
    MenuLoadCount = 0;
}
