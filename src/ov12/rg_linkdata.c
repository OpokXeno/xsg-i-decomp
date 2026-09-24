/*
 * OV12 original TU 63: 0x00a32688..0x00a32c48 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_linkdata.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern unsigned int strlen(const char *string);
extern int strcmp(const char *string1, const char *string2);
extern int strncmp(const char *string1, const char *string2,
                   unsigned int count);

void InitRgLinkData(RgLinkData *pAna, void *pBuf) {
    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 18);
    }
    if (pBuf == 0) {
        assert_prog(D_00A556E8, D_00A556D0, 19);
    }
    pAna->buf = pBuf;
}

static unsigned int _get_uint(const unsigned char *buf) {
    return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

int RgLinkDataMatchDataName(RgLinkData *pAna, const char *name) {
    char *dataName = pAna->buf;

    if (strlen(name) != 4) {
        return 0;
    }
    return strncmp(dataName, name, 4) == 0;
}

unsigned int RgLinkDataVersion(RgLinkData *pAna) {
    RgLinkDataHeader *header = pAna->buf;

    return _get_uint(header->version);
}

unsigned int RgLinkDataTime(RgLinkData *pAna) {
    RgLinkDataHeader *header;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x30);
    }
    header = pAna->buf;
    return _get_uint(header->time);
}

unsigned int RgLinkDataNumOfData(RgLinkData *pAna) {
    RgLinkDataHeader *header;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x36);
    }
    header = pAna->buf;
    return _get_uint(header->numOfData);
}

char *RgLinkDataGetName(RgLinkData *pAna, unsigned int nIndex) {
    RgLinkDataHeader *header;
    RgLinkDataName *nameTable;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x40);
    }
    if (nIndex >= RgLinkDataNumOfData(pAna)) {
        assert_prog(D_00A556F8, D_00A556D0, 0x42);
    }
    header = pAna->buf;
    nameTable = (RgLinkDataName *) ((char *) header + _get_uint(header->nameTableOffset));
    return nameTable[nIndex];
}

unsigned int RgLinkDataGetSize(RgLinkData *pAna, const char *name) {
    RgLinkDataHeader *header;
    unsigned char *table;
    unsigned char *offset;
    unsigned int numOfData;
    unsigned int entryCount;
    unsigned int nIndex;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x4F);
    }
    if (RgLinkDataVersion(pAna) < 2) {
        assert_prog(D_00A55708, D_00A556D0, 0x50);
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

void *RgLinkDataGet(RgLinkData *pAna, const char *name) {
    RgLinkDataHeader *header;
    unsigned char *table;
    unsigned char *offset;
    unsigned int numOfData;
    unsigned int entryCount;
    unsigned int nIndex;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x66);
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
        assert_prog(D_00A556C0, D_00A556D0, 0x7B);
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

void *RgLinkDataGetIndex(RgLinkData *pAna, int index) {
    RgLinkDataHeader *header;
    RgLinkDataOffset *offset;

    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 0x90);
    }
    if (index >= RgLinkDataNumOfData(pAna)) {
        assert_prog(D_00A55728, D_00A556D0, 0x92);
    }
    header = pAna->buf;
    offset = (RgLinkDataOffset *) ((char *) header + _get_uint(header->offsetTableOffset));
    return (char *) header + _get_uint(offset[index]);
}
