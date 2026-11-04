#ifndef INCLUDE_OV12_RG_LINKDATA_H
#define INCLUDE_OV12_RG_LINKDATA_H

/* A link-data archive reader; it holds only the archive buffer InitRgLinkData is given. */
typedef struct RgLinkData RgLinkData;
struct RgLinkData {
    void *buf; /* +0x00 */
};

/* Functions and data used by other translation units. */
void InitRgLinkData(RgLinkData *pAna, void *pBuf);
int RgLinkDataMatchDataName(RgLinkData *pAna, const char *name);
unsigned int RgLinkDataVersion(RgLinkData *pAna);
unsigned int RgLinkDataTime(RgLinkData *pAna);
unsigned int RgLinkDataNumOfData(RgLinkData *pAna);
char *RgLinkDataGetName(RgLinkData *pAna, unsigned int nIndex);
unsigned int RgLinkDataGetSize(RgLinkData *pAna, const char *name);
void *RgLinkDataGet(RgLinkData *pAna, const char *name);
void *RgLinkDataFindExt(RgLinkData *pAna, const char *ext);
void *RgLinkDataGetIndex(RgLinkData *pAna, int index);

#endif /* INCLUDE_OV12_RG_LINKDATA_H */
