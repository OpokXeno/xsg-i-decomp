/*
 * OV12 original TU 63: 0x00a32688..0x00a32c48 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "rg_linkdata.h"


/* Binds the view to pBuf, which holds the link-data image. Both pointers must be non-null. */
void InitRgLinkData(RgLinkData *pAna, void *pBuf) {
    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 18);
    }
    if (pBuf == 0) {
        assert_prog(linkDataAssertBufNotNil, linkDataSourcePath, 19);
    }
    pAna->buf = pBuf;
}

/* Reads a little-endian 32-bit word. */
static unsigned int _get_uint(const unsigned char *buf) {
    return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

/* Returns 1 when name equals the tag at the start of the buffer, 0 otherwise. */
int RgLinkDataMatchDataName(RgLinkData *pAna, const char *name) {
    char *dataName = pAna->buf;

    if (strlen(name) != RG_LINKDATA_TAG_LENGTH) {
        return 0;
    }
    return strncmp(dataName, name, RG_LINKDATA_TAG_LENGTH) == 0;
}

unsigned int RgLinkDataVersion(RgLinkData *pAna) {
    RgLinkDataHeader *header = pAna->buf;

    return _get_uint(header->version);
}

unsigned int RgLinkDataTime(RgLinkData *pAna) {
    RgLinkDataHeader *header;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x30);
    }
    header = pAna->buf;
    return _get_uint(header->time);
}

unsigned int RgLinkDataNumOfData(RgLinkData *pAna) {
    RgLinkDataHeader *header;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x36);
    }
    header = pAna->buf;
    return _get_uint(header->numOfData);
}

/* Returns the name of entry nIndex from the name table. */
char *RgLinkDataGetName(RgLinkData *pAna, unsigned int nIndex) {
    RgLinkDataHeader *header;
    RgLinkDataName *nameTable;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x40);
    }
    if (nIndex >= RgLinkDataNumOfData(pAna)) {
        assert_prog(linkDataAssertNumGreaterThanIndex, linkDataSourcePath, 0x42);
    }
    header = pAna->buf;
    nameTable = (RgLinkDataName *) ((char *) header + _get_uint(header->nameTableOffset));
    return nameTable[nIndex];
}

/*
 * Returns the size of the named entry, or 0 when no entry has that name.
 * The size is the distance from this entry's offset to the next offset.
 */
unsigned int RgLinkDataGetSize(RgLinkData *pAna, const char *name) {
    RgLinkDataHeader *header;
    unsigned char *table;
    unsigned char *offset;
    unsigned int numOfData;
    unsigned int entryCount;
    unsigned int nIndex;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x4F);
    }
    if (RgLinkDataVersion(pAna) < RG_LINKDATA_MIN_VERSION_FOR_SIZE) {
        assert_prog(linkDataAssertVersionAtLeastTwo, linkDataSourcePath, 0x50);
    }
    entryCount = 0;
    numOfData = RgLinkDataNumOfData(pAna);
    header = pAna->buf;
    table = (unsigned char *) header + _get_uint(header->offsetTableOffset);
    if (numOfData != 0) {
        offset = table;
        for (;;) {
            nIndex = entryCount;  /* separate counter keeps this entry's index in its own register */
            entryCount++;
            if (strcmp(RgLinkDataGetName(pAna, nIndex), name) == 0) {
                return _get_uint(offset + sizeof(RgLinkDataOffset)) - _get_uint(offset);
            }
            offset += sizeof(RgLinkDataOffset);
            if (entryCount >= numOfData) {
                break;
            }
        }
    }
    return 0;
}

/* Returns a pointer to the data of the named entry, or 0 when no entry has that name. */
void *RgLinkDataGet(RgLinkData *pAna, const char *name) {
    RgLinkDataHeader *header;
    unsigned char *table;
    unsigned char *offset;
    unsigned int numOfData;
    unsigned int entryCount;
    unsigned int nIndex;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x66);
    }
    entryCount = 0;
    numOfData = RgLinkDataNumOfData(pAna);
    header = pAna->buf;
    table = (unsigned char *) header + _get_uint(header->offsetTableOffset);
    if (numOfData != 0) {
        offset = table;
        for (;;) {
            nIndex = entryCount;  /* separate counter keeps this entry's index in its own register */
            entryCount++;
            if (strcmp(RgLinkDataGetName(pAna, nIndex), name) == 0) {
                return (char *) header + _get_uint(offset);
            }
            offset += sizeof(RgLinkDataOffset);
            if (entryCount >= numOfData) {
                break;
            }
        }
    }
    return 0;
}

/* Returns a pointer to the data of the first entry whose name ends with ext, or 0. */
void *RgLinkDataFindExt(RgLinkData *pAna, const char *ext) {
    RgLinkDataHeader *header;
    unsigned char *table;
    unsigned char *offset;
    unsigned int numOfData;
    unsigned int extLen;
    char *name;
    unsigned int entryCount;
    unsigned int nIndex;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x7B);
    }
    entryCount = 0;
    numOfData = RgLinkDataNumOfData(pAna);
    header = pAna->buf;
    table = (unsigned char *) header + _get_uint(header->offsetTableOffset);
    if (numOfData != 0) {
        offset = table;
        for (;;) {
            nIndex = entryCount;  /* separate counter keeps this entry's index in its own register */
            entryCount++;
            name = RgLinkDataGetName(pAna, nIndex);
            extLen = strlen(ext);
            if (strncmp(&name[strlen(name)] - extLen, ext, extLen) == 0) {
                return (char *) header + _get_uint(offset);
            }
            offset += sizeof(RgLinkDataOffset);
            if (entryCount >= numOfData) {
                break;
            }
        }
    }
    return 0;
}

/* Returns a pointer to the data of entry index. */
void *RgLinkDataGetIndex(RgLinkData *pAna, int index) {
    RgLinkDataHeader *header;
    RgLinkDataOffset *offset;

    if (pAna == 0) {
        assert_prog(linkDataAssertLinkDataNotNil, linkDataSourcePath, 0x90);
    }
    if (index >= RgLinkDataNumOfData(pAna)) {
        assert_prog(linkDataAssertNumGreaterThanUnsignedIndex, linkDataSourcePath, 0x92);
    }
    header = pAna->buf;
    offset = (RgLinkDataOffset *) ((char *) header + _get_uint(header->offsetTableOffset));
    return (char *) header + _get_uint(offset[index]);
}

const char linkDataAssertLinkDataNotNil[16] = "pAna != NIL";
const char linkDataSourcePath[24] = "../rg_linkdata.euc.c";
const char linkDataAssertBufNotNil[16] = "pBuf != NIL";
const char linkDataAssertNumGreaterThanIndex[16] = "uNum > nIndex";
const char linkDataAssertVersionAtLeastTwo[32] = "RgLinkDataVersion(pAna) >= 2";
const char linkDataAssertNumGreaterThanUnsignedIndex[16] = "uNum > uIndex";
