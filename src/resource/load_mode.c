/*
 * The original srs prefix is retained; its historical expansion is not
 * established by the available evidence.  This partial translation unit
 * owns only the two GLOBAL accessors.  srsLoadMode remains the original
 * external .sdata word rather than a new definition here.
 */
extern int srsLoadMode;

void srsSetLoadMode(int load_mode)
{
    srsLoadMode = load_mode;
}

int srsGetLoadMode(void)
{
    return srsLoadMode;
}
