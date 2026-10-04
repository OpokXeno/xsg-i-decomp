/*
 * OV12 original TU 37: 0x00a22668..0x00a22a78 (2 functions)
 */
#include "common.h"

const char D_00A54480[24] = "pParaList != NIL";
const char D_00A54498[24] = "../rg_text_info.euc.c";
const char D_00A544B0[16] = "pszName != NIL";
const char D_00A544C0[24] = "pReadText != NIL";
const char D_00A544D8[16] = "pStr != NIL";
const char D_00A544E8[24] = "unknown key word '%s'";
const char D_00A54500[8] = ".";
const char D_00A54508[40] = "unknown parameter type %d";

struct RgTextInfoEntry {
    unsigned int type;
    const char *pszName;
    union {
        unsigned int offset;
        void *address;
        int *integer;
        float *real;
        char *string;
    } destination;
    union {
        unsigned int count;
        void (*parse)(void *destination, char *pszString, int index);
    } auxiliary;
};

enum {
    RG_TEXT_INFO_INT_OFFSET,
    RG_TEXT_INFO_FLOAT_OFFSET,
    RG_TEXT_INFO_STRING_OFFSET,
    RG_TEXT_INFO_BOOL_OFFSET,
    RG_TEXT_INFO_FLOAT_ARRAY_OFFSET,
    RG_TEXT_INFO_CALLBACK_OFFSET,
    RG_TEXT_INFO_INT_ADDRESS,
    RG_TEXT_INFO_FLOAT_ADDRESS,
    RG_TEXT_INFO_STRING_ADDRESS,
    RG_TEXT_INFO_BOOL_ADDRESS,
    RG_TEXT_INFO_FLOAT_ARRAY_ADDRESS,
    RG_TEXT_INFO_CALLBACK_ADDRESS,
    RG_TEXT_INFO_END
};

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern int strcmp(const char *s1, const char *s2);
struct RgReadText;
extern int RgReadTextIsEOF(struct RgReadText *pReader);
extern void RgReadTextGetString(struct RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(struct RgReadText *pReader);
extern int RgReadTextGetInt(struct RgReadText *pReader);
extern int RgReadTextGetBool(struct RgReadText *pReader);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_text_info", _FindEntry);

struct RgTextInfoEntry *_FindEntry(struct RgTextInfoEntry *pParaList,
                                   const char *pszName);

void RgTextInfoRead(struct RgReadText *pReadText,
                    struct RgTextInfoEntry *pParaList, char *pData)
{
    char pszName[128];
    char pszString[128];
    struct RgTextInfoEntry *pEntry;
    unsigned int type;
    unsigned int value_index;
    unsigned int offset_count;
    unsigned int address_count;
    int integer;
    float real;
    float *offset_elements;
    float *address_elements;

    if (pReadText == 0) {
        assert_prog(D_00A544C0, D_00A54498, 41);
    }
    if (pParaList == 0) {
        assert_prog(D_00A54480, D_00A54498, 42);
    }
    if (pData == 0) {
        assert_prog(D_00A544D8, D_00A54498, 43);
    }

    while (RgReadTextIsEOF(pReadText) == 0) {
        RgReadTextGetString(pReadText, pszName);
        pEntry = _FindEntry(pParaList, pszName);
        if (pEntry == 0) {
            RgError(D_00A544E8, D_00A54498, 53, pszName);
        }
        type = pEntry->type;
        switch (type) {
        case RG_TEXT_INFO_INT_OFFSET:
            integer = RgReadTextGetInt(pReadText);
            /* Each descriptor supplies an offset into its caller's data layout. */
            *(int *)(pData + pEntry->destination.offset) = integer;
            break;
        case RG_TEXT_INFO_INT_ADDRESS:
            integer = RgReadTextGetInt(pReadText);
            *pEntry->destination.integer = integer;
            break;
        case RG_TEXT_INFO_FLOAT_OFFSET:
            real = RgReadTextGetFloat(pReadText);
            *(float *)(pData + pEntry->destination.offset) = real;
            break;
        case RG_TEXT_INFO_FLOAT_ADDRESS:
            real = RgReadTextGetFloat(pReadText);
            *pEntry->destination.real = real;
            break;
        case RG_TEXT_INFO_STRING_OFFSET:
            RgReadTextGetString(pReadText,
                                pData + pEntry->destination.offset);
            break;
        case RG_TEXT_INFO_STRING_ADDRESS:
            RgReadTextGetString(pReadText, pEntry->destination.string);
            break;
        case RG_TEXT_INFO_BOOL_OFFSET:
            integer = RgReadTextGetBool(pReadText);
            *(int *)(pData + pEntry->destination.offset) = integer;
            break;
        case RG_TEXT_INFO_BOOL_ADDRESS:
            integer = RgReadTextGetBool(pReadText);
            *pEntry->destination.integer = integer;
            break;
        case RG_TEXT_INFO_FLOAT_ARRAY_OFFSET:
            if (pEntry->auxiliary.count != 0) {
                value_index = 0;
                do {
                    real = RgReadTextGetFloat(pReadText);
                    offset_elements = (float *)(pData + pEntry->destination.offset);
                    offset_count = pEntry->auxiliary.count;
                    offset_elements[value_index] = real;
                    value_index++;
                } while (value_index < offset_count);
            }
            break;
        case RG_TEXT_INFO_FLOAT_ARRAY_ADDRESS:
            if (pEntry->auxiliary.count != 0) {
                value_index = 0;
                do {
                    real = RgReadTextGetFloat(pReadText);
                    address_elements = pEntry->destination.real;
                    address_count = pEntry->auxiliary.count;
                    address_elements[value_index] = real;
                    value_index++;
                } while (value_index < address_count);
            }
            break;
        case RG_TEXT_INFO_CALLBACK_OFFSET:
            value_index = 0;
            RgReadTextGetString(pReadText, pszString);
            while (strcmp(pszString, D_00A54500) != 0) {
                if (pEntry->auxiliary.parse != 0) {
                    pEntry->auxiliary.parse(
                        pData + pEntry->destination.offset, pszString,
                        value_index);
                }
                if (RgReadTextIsEOF(pReadText) != 0) {
                    break;
                }
                RgReadTextGetString(pReadText, pszString);
                value_index++;
            }
            break;
        case RG_TEXT_INFO_CALLBACK_ADDRESS:
            value_index = 0;
            RgReadTextGetString(pReadText, pszString);
            while (strcmp(pszString, D_00A54500) != 0) {
                if (pEntry->auxiliary.parse != 0) {
                    pEntry->auxiliary.parse(pEntry->destination.address,
                                            pszString, value_index);
                }
                if (RgReadTextIsEOF(pReadText) != 0) {
                    break;
                }
                RgReadTextGetString(pReadText, pszString);
                value_index++;
            }
            break;
        default:
            RgError(D_00A54508, D_00A54498, 119, type);
            break;
        }
    }
}
