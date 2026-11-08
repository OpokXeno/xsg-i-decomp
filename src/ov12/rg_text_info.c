/*
 * OV12 original TU 37: 0x00a22668..0x00a22a78 (2 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_text_info.h"

/* Buffer size for one keyword or string token. */
#define RG_TEXT_INFO_TOKEN_MAX 128

const char textInfoAssertParaListNotNil[24] = "pParaList != NIL";
const char textInfoSourcePath[24] = "../rg_text_info.euc.c";
const char textInfoAssertNameNotNil[16] = "pszName != NIL";
const char textInfoAssertReadTextNotNil[24] = "pReadText != NIL";
const char textInfoAssertStringNotNil[16] = "pStr != NIL";
const char textInfoUnknownKeyWordFormat[24] = "unknown key word '%s'";
const char textInfoEndMarkerString[8] = ".";
const char textInfoUnknownParameterTypeFormat[40] = "unknown parameter type %d";


/* Returns the entry named pszName, or 0 when no entry before the end marker matches. */
struct RgTextInfoEntry *_FindEntry(struct RgTextInfoEntry *pParaList,
                                   const char *pszName)
{
    struct RgTextInfoEntry *pEntry;

    if (pParaList == 0) {
        assert_prog(textInfoAssertParaListNotNil, textInfoSourcePath, 22);
    }
    if (pszName == 0) {
        assert_prog(textInfoAssertNameNotNil, textInfoSourcePath, 23);
    }
    for (pEntry = pParaList; pEntry->type != RG_TEXT_INFO_END; pEntry++) {
        if (strcmp(pEntry->pszName, pszName) == 0) {
            return pEntry;
        }
    }
    return 0;
}

/* Reads keyword and value pairs until the end of the text, storing each value through its entry. */
void RgTextInfoRead(struct RgReadText *pReadText,
                    struct RgTextInfoEntry *pParaList, char *pData)
{
    char pszName[RG_TEXT_INFO_TOKEN_MAX];
    char pszString[RG_TEXT_INFO_TOKEN_MAX];
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
        assert_prog(textInfoAssertReadTextNotNil, textInfoSourcePath, 41);
    }
    if (pParaList == 0) {
        assert_prog(textInfoAssertParaListNotNil, textInfoSourcePath, 42);
    }
    if (pData == 0) {
        assert_prog(textInfoAssertStringNotNil, textInfoSourcePath, 43);
    }

    while (RgReadTextIsEOF(pReadText) == 0) {
        RgReadTextGetString(pReadText, pszName);
        pEntry = _FindEntry(pParaList, pszName);
        if (pEntry == 0) {
            RgError(textInfoUnknownKeyWordFormat, textInfoSourcePath, 53, pszName);
        }
        type = pEntry->type;
        /* The entry type selects how the value is read and where it is stored. */
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
        /* Callbacks receive each string token up to the end marker "." */
        case RG_TEXT_INFO_CALLBACK_OFFSET:
            value_index = 0;
            RgReadTextGetString(pReadText, pszString);
            while (strcmp(pszString, textInfoEndMarkerString) != 0) {
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
            while (strcmp(pszString, textInfoEndMarkerString) != 0) {
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
            RgError(textInfoUnknownParameterTypeFormat, textInfoSourcePath, 119, type);
            break;
        }
    }
}
