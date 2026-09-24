/*
 * TU-local declarations of ov12/tu022 (src/ov12/rg_simple_db.c).
 */

#ifndef SRC_OV12_RG_SIMPLE_DB_H
#define SRC_OV12_RG_SIMPLE_DB_H

/*
 * _EntryDB's assert message "pDB->m_nNumOfData < pDB->m_nDataCapa"
 * (D_00A53238) names m_nNumOfData and m_nDataCapa; include/ov12/rg_simple_db.h
 * records that _InitDB stores capacity at +4 and entry_size at +8.
 * _ClearDB, _EntryDB and _GetDataDB index the data array at +0xC by the
 * 4-byte pointer stride, and _EntryDB and _GetDataID index the name array
 * at +0x10 the same way; _EntryDB copies each name into its slot with
 * strncpy(..., m_nEntrySize - 1) and terminates it at m_nEntrySize - 1.
 */
struct RgSimpleDB {
    int m_nNumOfData;    /* 0x00 */
    int m_nDataCapa;     /* 0x04 */
    int m_nEntrySize;    /* 0x08 */
    void **m_apData;     /* 0x0C */
    char **m_apName;     /* 0x10 */
};

#endif /* SRC_OV12_RG_SIMPLE_DB_H */
