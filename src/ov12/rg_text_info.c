/*
 * OV12 original TU 37: 0x00a22668..0x00a22a78 (2 functions)
 */
#include "common.h"
#include "ov12/rg_text_info.h"

const char D_00A54480[24] = "pParaList != NIL";
const char D_00A54498[24] = "../rg_text_info.euc.c";
const char D_00A544B0[16] = "pszName != NIL";
const char D_00A544C0[24] = "pReadText != NIL";
const char D_00A544D8[16] = "pStr != NIL";
const char D_00A544E8[24] = "unknown key word '%s'";
const char D_00A54500[8] = ".";
const char D_00A54508[40] = "unknown parameter type %d";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern int strcmp(const char *s1, const char *s2);
extern int RgReadTextIsEOF(struct RgReadText *pReader);
extern void RgReadTextGetString(struct RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(struct RgReadText *pReader);
extern int RgReadTextGetInt(struct RgReadText *pReader);
extern int RgReadTextGetBool(struct RgReadText *pReader);

struct RgTextInfoEntry *_FindEntry(struct RgTextInfoEntry *pParaList,
                                   const char *pszName)
{
    struct RgTextInfoEntry *pEntry;

    if (pParaList == 0) {
        assert_prog(D_00A54480, D_00A54498, 22);
    }
    if (pszName == 0) {
        assert_prog(D_00A544B0, D_00A54498, 23);
    }
    for (pEntry = pParaList; pEntry->type != RG_TEXT_INFO_END; pEntry++) {
        if (strcmp(pEntry->pszName, pszName) == 0) {
            return pEntry;
        }
    }
    return 0;
}

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
            *(int *)(pData + (unsigned int) pEntry->destination) = integer;
            break;
        case RG_TEXT_INFO_INT_ADDRESS:
            integer = RgReadTextGetInt(pReadText);
            *(int *) pEntry->destination = integer;
            break;
        case RG_TEXT_INFO_FLOAT_OFFSET:
            real = RgReadTextGetFloat(pReadText);
            *(float *)(pData + (unsigned int) pEntry->destination) = real;
            break;
        case RG_TEXT_INFO_FLOAT_ADDRESS:
            real = RgReadTextGetFloat(pReadText);
            *(float *) pEntry->destination = real;
            break;
        case RG_TEXT_INFO_STRING_OFFSET:
            RgReadTextGetString(pReadText,
                                pData + (unsigned int) pEntry->destination);
            break;
        case RG_TEXT_INFO_STRING_ADDRESS:
            RgReadTextGetString(pReadText, (char *) pEntry->destination);
            break;
        case RG_TEXT_INFO_BOOL_OFFSET:
            integer = RgReadTextGetBool(pReadText);
            *(int *)(pData + (unsigned int) pEntry->destination) = integer;
            break;
        case RG_TEXT_INFO_BOOL_ADDRESS:
            integer = RgReadTextGetBool(pReadText);
            *(int *) pEntry->destination = integer;
            break;
        case RG_TEXT_INFO_FLOAT_ARRAY_OFFSET:
            if ((unsigned int) pEntry->auxiliary != 0) {
                value_index = 0;
                do {
                    real = RgReadTextGetFloat(pReadText);
                    offset_elements = (float *)(pData + (unsigned int) pEntry->destination);
                    offset_count = (unsigned int) pEntry->auxiliary;
                    offset_elements[value_index] = real;
                    value_index++;
                } while (value_index < offset_count);
            }
            break;
        case RG_TEXT_INFO_FLOAT_ARRAY_ADDRESS:
            if ((unsigned int) pEntry->auxiliary != 0) {
                value_index = 0;
                do {
                    real = RgReadTextGetFloat(pReadText);
                    address_elements = (float *) pEntry->destination;
                    address_count = (unsigned int) pEntry->auxiliary;
                    address_elements[value_index] = real;
                    value_index++;
                } while (value_index < address_count);
            }
            break;
        case RG_TEXT_INFO_CALLBACK_OFFSET:
            value_index = 0;
            RgReadTextGetString(pReadText, pszString);
            while (strcmp(pszString, D_00A54500) != 0) {
                if (pEntry->auxiliary != 0) {
                    ((RgTextInfoParse) pEntry->auxiliary)(
                        pData + (unsigned int) pEntry->destination, pszString,
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
                if (pEntry->auxiliary != 0) {
                    ((RgTextInfoParse) pEntry->auxiliary)(pEntry->destination,
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
