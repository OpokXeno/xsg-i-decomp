#ifndef INCLUDE_OV12_RG_READ_TEXT_H
#define INCLUDE_OV12_RG_READ_TEXT_H

#include "shared.h"

/* Capacity of one token, and of the unget-token slot of RgReadText. */
#define RG_READ_TEXT_TOKEN_MAX 0x40

typedef struct RgReadText RgReadText;

enum RgUngetFlag { RG_UNGET_CLEAR = 0, RG_UNGET_SET = 1 };

/* Text reader over a file: tokens separated by white space, '#' comments,
 * '%' paragraph delimiters. */
struct RgReadText {
    RgFileSysData *m_pFile;
    char *m_pszBuf;
    unsigned int m_nSize;
    char *m_pszCur;
    char *m_pszDelim; /* the '%' that ended the current paragraph */
    /* RgReadTextUnget stores a terminating NUL at index RG_READ_TEXT_TOKEN_MAX. */
    char m_szUngetToken[RG_READ_TEXT_TOKEN_MAX + 1];
    enum RgUngetFlag m_bUngetPending;
};

/* Functions and data used by other translation units. */
RgReadText *CreateRgReadText(const char *pszName);
void DisposeRgReadText(RgReadText *pReader);
int RgReadTextIsEOF(RgReadText *pReader);
int RgReadTextIsOnDelimitor(RgReadText *pReader);
void RgReadTextUnget(RgReadText *pReader, char *pszUngetToken);
void RgReadTextRewind(RgReadText *pReader);
void RgReadTextGetString(RgReadText *pReader, char *pszOut);
float RgReadTextGetFloat(RgReadText *pReader);
int RgReadTextGetInt(RgReadText *pReader);
int RgReadTextGetBool(RgReadText *pReader);
void RgReadTextNextParagraph(RgReadText *pReader);
int RgReadTextFindParagraph(RgReadText *pReader, const char *pszTag,
                            const char *pszSubTag);

#endif /* INCLUDE_OV12_RG_READ_TEXT_H */
