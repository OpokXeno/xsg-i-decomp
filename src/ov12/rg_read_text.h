/*
 * TU-local declarations of ov12/tu036 (src/ov12/rg_read_text.c).
 */

#ifndef SRC_OV12_RG_READ_TEXT_H
#define SRC_OV12_RG_READ_TEXT_H

#include "shared.h"

/*
 * RgFileSysData is ov12/tu065's (rg_filesys) file handle, published in
 * include/shared.h. This TU only stores and forwards the pointer (m_pFile,
 * _DisposeReader); it never reads or writes any of its members.
 */

/*
 * RgReadTextGetString's destination capacity and the unget-token slot of
 * RgReadText share the same evidenced immediate, 0x40 (64) bytes.
 */
#define RG_READ_TEXT_TOKEN_MAX 0x40

typedef struct RgReadText RgReadText;

struct RgReadText {
    RgFileSysData *m_pFile;      /* 0x00, disposed by _DisposeReader */
    char *m_pszBuf;              /* 0x04, "pReader->m_pszBuf" (D_00A543E0) */
    unsigned int m_nSize;        /* 0x08, "pReader->m_nSize" (D_00A543E0) */
    char *m_pszCur;              /* 0x0C, set by _SetCurrent/RgReadTextRewind/-NextParagraph */
    char *m_pszDelim;            /* 0x10, RgReadTextIsOnDelimitor/-Rewind/-NextParagraph */
    /*
     * 0x14, RgReadTextUnget copies at most 0x40 bytes into this slot
     * (strncpy at 0x00a223a4) and then stores a terminating NUL at index 0x40
     * (sb at 0x00a223ac, through the source pointer in the original), so the
     * buffer holds RG_READ_TEXT_TOKEN_MAX + 1 characters. Ordinary 4-byte
     * alignment then places m_bUngetPending at 0x58, the offset _InitReader,
     * _GetString, RgReadTextUnget and RgReadTextRewind use.
     */
    char m_szUngetToken[RG_READ_TEXT_TOKEN_MAX + 1];
    int m_bUngetPending;         /* 0x58, RgReadTextUnget/-Rewind */
};

RgReadText *CreateRgReadText(const char *pszName);
int RgReadTextIsEOF(RgReadText *pReader);
int RgReadTextIsOnDelimitor(RgReadText *pReader);
void RgReadTextRewind(RgReadText *pReader);
void RgReadTextGetString(RgReadText *pReader, char *pszOut);
float RgReadTextGetFloat(RgReadText *pReader);
int RgReadTextGetInt(RgReadText *pReader);
int RgReadTextGetBool(RgReadText *pReader);
void RgReadTextNextParagraph(RgReadText *pReader);

#endif /* SRC_OV12_RG_READ_TEXT_H */
