/*
 * TU-local declarations of ov12/tu063 (src/ov12/rg_linkdata.c).
 */

#ifndef SRC_OV12_RG_LINKDATA_H
#define SRC_OV12_RG_LINKDATA_H

#include "ov12/rg_linkdata.h"

/* Archives older than this version have no offset table RgLinkDataGetSize can use. */
#define RG_LINKDATA_MIN_VERSION_FOR_SIZE 2
/* Every data name is a four-character tag. */
#define RG_LINKDATA_TAG_LENGTH 4

/*
 * The original literals at ov12:0x00a556c0 ("pAna != NIL"), 0x00a556d0
 * ("../rg_linkdata.euc.c") and 0x00a556e8 ("pBuf != NIL") are scaffold-owned
 * (config/tu-build.json data_ownership: this .rodata window is still owner
 * "asm") and have no entry in config/symbols/ov12.txt, so they keep their
 * splat names (docs/naming.md, "Scaffold-owned data keeps its splat name").
 */
extern const char linkDataAssertLinkDataNotNil[];
extern const char linkDataSourcePath[];
extern const char linkDataAssertBufNotNil[];

/*
 * The archive header pAna->buf points at, read through _get_uint's
 * unaligned load (the same helper RgLinkDataMatchDataName's own raw buf
 * access already parallels for +0x00). RgLinkDataVersion, RgLinkDataTime and
 * RgLinkDataNumOfData decode version/numOfData/time; RgLinkDataGetName reads
 * nameTableOffset; RgLinkDataGetSize, RgLinkDataGet, RgLinkDataFindExt and
 * RgLinkDataGetIndex read offsetTableOffset. Nothing in this allocation
 * reads +0x0C.
 */
typedef struct RgLinkDataHeader {
    unsigned char unmodeled_000[4];
    unsigned char nameTableOffset[4];   /* +0x04: RgLinkDataGetName */
    unsigned char offsetTableOffset[4]; /* +0x08: RgLinkDataGetSize,
                                          * RgLinkDataGet, RgLinkDataFindExt,
                                          * RgLinkDataGetIndex */
    unsigned char unmodeled_00C[4];
    unsigned char version[4];   /* +0x10: RgLinkDataVersion */
    unsigned char numOfData[4]; /* +0x14: RgLinkDataNumOfData */
    unsigned char time[4];      /* +0x18: RgLinkDataTime */
} RgLinkDataHeader;

/*
 * One name-table record: RgLinkDataGetName steps by sizeof(RgLinkDataName)
 * (0x20 bytes) per index from the header's nameTableOffset.
 */
typedef char RgLinkDataName[0x20];

/*
 * One offset-table record: RgLinkDataGetSize, RgLinkDataGet and
 * RgLinkDataFindExt walk this table by sizeof(RgLinkDataOffset) (4 bytes)
 * from the header's offsetTableOffset, and RgLinkDataGetIndex indexes it
 * directly by argument. Each decoded value is a byte offset from the
 * header; RgLinkDataGetSize subtracts one entry from the next entry to
 * get a payload's size.
 */
typedef unsigned char RgLinkDataOffset[4];

/*
 * The literals at ov12:0x00a55708 ("RgLinkDataVersion(pAna) >= 2"),
 * 0x00a556f8 ("uNum > nIndex") and 0x00a55728 ("uNum > uIndex") are
 * scaffold-owned splat data in the same window as linkDataAssertLinkDataNotNil above.
 */
extern const char linkDataAssertVersionAtLeastTwo[];
extern const char linkDataAssertNumGreaterThanIndex[];
extern const char linkDataAssertNumGreaterThanUnsignedIndex[];

#endif /* SRC_OV12_RG_LINKDATA_H */
