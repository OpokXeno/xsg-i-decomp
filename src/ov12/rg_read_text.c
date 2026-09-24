/*
 * OV12 original TU 36: 0x00a21c58..0x00a22668 (23 functions)
 */
#include "common.h"
#include "rg_read_text.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void RgError(const char *message, const char *source_file, int line, ...);

extern RgHeap *InstanceOfRgHeap(void);

extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);

extern void RgHeapFree(void *heap, void *ptr, const char *source_file, int line);

extern void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *pszFile,
                                     int iLine);

static int _InitReader(RgReadText *pReader, const char *pszName);

static int _IsEOF(RgReadText *pReader);

static void _GetString(RgReadText *pReader, char *pszOut, unsigned int nCapacity);

static float _GetFloat(RgReadText *pReader);

static int _GetInt(RgReadText *pReader);

static int _GetBool(RgReadText *pReader);

/*
 * ov12:0x00a543a8, 15 bytes, contains the assertion expression
 * "pReader != NIL".
 */
extern const char D_00A543A8[];

/*
 * ov12:0x00a543b8, 23 bytes, contains the source filename
 * "../rg_read_text.euc.c".
 */
extern const char D_00A543B8[];

/*
 * ov12:0x00a543e0, 54 bytes, contains the range-check message
 * "pCur - pReader->m_pszBuf = %d\npReader->m_nSize = %d\n".
 */
extern const char D_00A543E0[];

/*
 * ov12:0x00a54460, 19 bytes, contains the message "not find delimiter".
 */
extern const char D_00A54460[];

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _InitReader);

static void _DisposeReader(RgReadText *pReader)
{
    RgFileSysData *pFile;

    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 83);
    }
    pFile = pReader->m_pFile;
    if (pFile != 0) {
        DisposeRgFileSysData_sub(pFile, D_00A543B8, 85);
    }
}

static int _IsCurOnEOF(RgReadText *pReader, char *pCur)
{
    return (unsigned int) (pCur - pReader->m_pszBuf) >= pReader->m_nSize;
}

static int _IsWhiteSpace(unsigned char ch)
{
    int isWhiteSpace;

    isWhiteSpace = 0;
    if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
        isWhiteSpace = 1;
    }
    return isWhiteSpace;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _SkipWhiteSpace);

static void _SetCurrent(RgReadText *pReader, char *pCur)
{
    unsigned int nSize;
    unsigned int nOffset;

    nSize = pReader->m_nSize;
    nOffset = pCur - pReader->m_pszBuf;
    if (nSize + 1 < nOffset) {
        RgError(D_00A543E0, D_00A543B8, 141, nOffset, nSize);
    }
    pReader->m_pszCur = pCur;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _IsEOF);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _GetString);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _GetFloat);

/*
 * ov12:0x00a54418, 12 bytes, contains the assertion expression
 * "pCur != NIL".
 */
extern const char D_00A54418[];

extern int atoi(const char *nptr);

static char *_SkipWhiteSpace(RgReadText *pReader);

int _GetInt(RgReadText *pReader)
{
    char szToken[RG_READ_TEXT_TOKEN_MAX];
    char *pCur;

    pCur = _SkipWhiteSpace(pReader);
    if (pCur == 0) {
        assert_prog(D_00A54418, D_00A543B8, 208);
    }
    _GetString(pReader, szToken, RG_READ_TEXT_TOKEN_MAX - 1);
    return atoi(szToken);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", _GetBool);

RgReadText *CreateRgReadText(const char *pszName)
{
    RgReadText *pReader;

    pReader = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgReadText), D_00A543B8, 233);
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 234);
    }
    if (_InitReader(pReader, pszName) == 0) {
        RgHeapFree(InstanceOfRgHeap(), pReader, D_00A543B8, 236);
        pReader = 0;
    }
    return pReader;
}

void DisposeRgReadText(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 244);
    }
    _DisposeReader(pReader);
    RgHeapFree(InstanceOfRgHeap(), pReader, D_00A543B8, 246);
}

int RgReadTextIsEOF(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 256);
    }
    return _IsEOF(pReader);
}

int RgReadTextIsOnDelimitor(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 264);
    }
    return pReader->m_pszDelim != 0;
}

/*
 * ov12:0x00a54448, 21 bytes, contains the assertion expression
 * "pszUngetToken != NIL".
 */
extern const char D_00A54448[];

extern unsigned int strlen(const char *string);
extern char *strncpy(char *dest, const char *src, unsigned int n);

void RgReadTextUnget(RgReadText *pReader, char *pszUngetToken)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 279);
    }
    if (pszUngetToken == 0) {
        assert_prog(D_00A54448, D_00A543B8, 280);
    }
    if (strlen(pszUngetToken) != 0) {
        pReader->m_bUngetPending = 1;
        strncpy(pReader->m_szUngetToken, pszUngetToken, RG_READ_TEXT_TOKEN_MAX);
        pszUngetToken[RG_READ_TEXT_TOKEN_MAX] = 0;
    }
}

void RgReadTextRewind(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 294);
    }
    pReader->m_bUngetPending = 0;
    pReader->m_pszDelim = 0;
    pReader->m_pszCur = pReader->m_pszBuf;
}

void RgReadTextGetString(RgReadText *pReader, char *pszOut)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 306);
    }
    _GetString(pReader, pszOut, RG_READ_TEXT_TOKEN_MAX);
}

float RgReadTextGetFloat(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 313);
    }
    return _GetFloat(pReader);
}

int RgReadTextGetInt(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 320);
    }
    return _GetInt(pReader);
}

int RgReadTextGetBool(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 327);
    }
    return _GetBool(pReader);
}

void RgReadTextNextParagraph(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(D_00A543A8, D_00A543B8, 340);
    }
    if (pReader->m_pszDelim == 0) {
        RgError(D_00A54460, D_00A543B8, 344);
    }
    pReader->m_pszCur = pReader->m_pszDelim + 1;
    pReader->m_pszDelim = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_read_text", RgReadTextFindParagraph);
