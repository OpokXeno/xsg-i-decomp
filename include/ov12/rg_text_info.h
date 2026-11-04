#ifndef INCLUDE_OV12_RG_TEXT_INFO_H
#define INCLUDE_OV12_RG_TEXT_INFO_H

/* What RgTextInfoRead does with the value read for a keyword: stored at an
 * offset into the caller's data block, or through an address in the table. */
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

/* Called with each value of a callback keyword. */
typedef void (*RgTextInfoParse)(void *destination, char *pszString, int index);

/* One keyword of a table; the initialized tables are copied by doublewords. */
struct RgTextInfoEntry {
    unsigned int type;
    const char *pszName;
    void *destination; /* offset or address, by type */
    void *auxiliary;   /* element count or RgTextInfoParse, by type */
} __attribute__((aligned(8)));

struct RgReadText;
extern void RgTextInfoRead(struct RgReadText *pReadText,
                           struct RgTextInfoEntry *pParaList, char *pData);

#endif
