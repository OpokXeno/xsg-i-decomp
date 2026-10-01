#include "common.h"
#include "shared.h"
#include "xgl_mc.h"

extern unsigned char D_0093E120[];

int sceMcOpen(int port, int slot, const char *name, int mode);
int sceMcClose(int fd);
int sceMcRead(int fd, void *buffer, int size);
int sceMcWrite(int fd, const void *buffer, int size);
int sceMcSync(int mode, int *command, int *result);
int sceMcGetInfo(int port, int slot, int *type, int *free, int *format);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcRequest);

/*
 * The word xglMcGetState reports lives at mw+4; mw+0 is the byte the request
 * state machine of xglMcMain switches on (seven states, jump table at
 * 0x004D2560) and xglMcMain writes the halfwords of the same +4 window per
 * slot (`sh` through `mw + 4 + 2 * mw[8]`, 0x00220600..0x0022069C).  The rest
 * of the record is still assembly, so mw cannot become a struct without
 * inventing the bytes between its members (docs/naming.md); the one access
 * takes docs/style.md rule 2's named-offset fallback.
 */
#define XGL_MC_STATE_OFFSET 4

int xglMcGetState(void)
{
    return *(int *)(mw + XGL_MC_STATE_OFFSET);
}

void xglMcReset(void)
{
    queue_top = 0;
    queue_end = 0;
    xglMcSetMapName(0, 0);
    mw[0] = 0;
}

/*
 * Converts one EUC-JIS double-byte character to Shift-JIS in place: hi and
 * lo hold the character's lead and trail byte, EUC-encoded on entry and
 * Shift-JIS on return. Called from xglMcSetMapName while building a memory
 * card map name.
 */
static void xglMcEUC2SJIS(u8 *hi, u8 *lo)
{
    u32 hi_byte;
    int lo_byte;
    u8 sjis_hi;
    u8 sjis_lo;

    hi_byte = ((*hi) + 0x80) & 0xFF;
    lo_byte = ((*lo) + 0x80) & 0xFF;
    sjis_hi = hi_byte;
    if (sjis_hi & 1)
    {
        lo_byte = lo_byte + 0x1F;
        sjis_hi = (sjis_hi >> 1) + 0x71;
    }
    else
    {
        lo_byte = lo_byte + 0x7D;
        sjis_hi = (sjis_hi >> 1) + 0x70;
    }
    sjis_lo = lo_byte & 0xFF;
    if (sjis_hi >= 0xA0U)
    {
        sjis_hi = (sjis_hi + 0x40) & 0xFF;
    }
    if (sjis_lo >= 0x7FU)
    {
        sjis_lo = (sjis_lo + 1) & 0xFF;
    }
    if ((sjis_hi == 0x87) && (sjis_lo == 0x54))
    {
        sjis_hi = 0x82;
        sjis_lo = 0x50;
    }
    *hi = sjis_hi;
    *lo = sjis_lo;
}

void xglMcSetMapName(const unsigned char *primary_euc_name,
                     const unsigned char *secondary_euc_name)
{
    const unsigned char *primary;
    unsigned char *destination;
    int character_count;
    unsigned char lead;
    unsigned char trail;

    primary = primary_euc_name;
    destination = D_0093E120;
    character_count = 0;

    if (primary != 0)
    {
        lead = primary[0];
        if (lead != 0)
        {
            do
            {
                trail = primary[1];
                primary += 2;
                xglMcEUC2SJIS(&lead, &trail);
                character_count++;
                destination[0] = lead;
                destination[1] = trail;
                destination += 2;
                if (character_count >= 16)
                {
                    break;
                }
                lead = ((const volatile unsigned char *)primary)[0];
            } while (lead != 0);
        }

        if ((character_count < 16) && (secondary_euc_name != 0) &&
            (((const signed char *)secondary_euc_name)[0] != 0))
        {
            destination[0] = 0x81;
            destination[1] = 0x45;
            destination += 2;
            character_count++;
        }
    }

    if ((secondary_euc_name != 0) && (character_count < 16))
    {
        lead = secondary_euc_name[0];
        if (lead != 0)
        {
            do
            {
                trail = secondary_euc_name[1];
                secondary_euc_name += 2;
                xglMcEUC2SJIS(&lead, &trail);
                character_count++;
                destination[0] = lead;
                destination[1] = trail;
                destination += 2;
                if (character_count >= 16)
                {
                    break;
                }
                lead = ((const unsigned char *)secondary_euc_name)[0];
            } while (lead != 0);
        }
    }

    while (character_count < 17)
    {
        destination[0] = 0;
        destination[1] = 0;
        destination += 2;
        character_count++;
    }
}

void xglMcWriteMapName(char *buffer, int slot)
{
    int value = slot + 1;
    int digit = value % 10;

    buffer[229] = digit + 'O';
    value /= 10;
    buffer[227] = value % 10 + 'O';
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcSetFullPath);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", execute);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", create_sub2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcMain);

int xglMcEasyLoad(const char *path, void *buffer, int size)
{
    int card_type;
    int command;
    int result;
    int descriptor;
    int read_result;

    sceMcGetInfo(0, 0, 0, 0, &card_type);
    sceMcSync(0, &command, &result);
    if (card_type == 0)
    {
        return -1;
    }
    if (result < -1)
    {
        return result;
    }

    sceMcOpen(0, 0, path, 1);
    sceMcSync(0, &command, &result);
    descriptor = result;
    if (descriptor < 0)
    {
        return descriptor;
    }

    sceMcRead(descriptor, buffer, size);
    sceMcSync(0, &command, &result);
    read_result = result;
    if (read_result < 0)
    {
        return read_result;
    }

    sceMcClose(descriptor);
    sceMcSync(0, &command, &result);
    if (result < 0)
    {
        return result;
    }
    return read_result;
}

int xglMcEasySave(const char *path, const void *buffer, int size)
{
    int card_type;
    int command;
    int result;
    int descriptor;

    sceMcGetInfo(0, 0, 0, 0, &card_type);
    sceMcSync(0, &command, &result);
    if (result < -1 || card_type == 0)
    {
        return -1;
    }

    sceMcOpen(0, 0, path, 0x202);
    sceMcSync(0, &command, &result);
    descriptor = result;
    if (descriptor < 0)
    {
        return -1;
    }

    sceMcWrite(descriptor, buffer, size);
    sceMcSync(0, &command, &result);
    if (result < 0)
    {
        return -1;
    }

    sceMcClose(descriptor);
    sceMcSync(0, &command, &result);
    return result >= 0 ? 0 : -1;
}

void xglMcInitial(void)
{
    sceMcInit();
    xglMcReset();
}
