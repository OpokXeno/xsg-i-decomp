/*
 * OV12 original TU 36: 0x00a21c58..0x00a22668 (23 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_read_text.h"
#include "ov12/xrg_filesys_mode.h"

/* Mode for XrgFileSysPushMode: allocate from the RgHeap. */
#define XRG_FILESYS_MODE_HEAP 1

static int _InitReader(RgReadText *pReader, const char *pszName);
static int _IsEOF(RgReadText *pReader);
static char *_SkipWhiteSpace(RgReadText *pReader);
static void _GetString(RgReadText *pReader, char *pszOut, unsigned int nCapacity);
static float _GetFloat(RgReadText *pReader);
static int _GetInt(RgReadText *pReader);
static int _GetBool(RgReadText *pReader);

/* Defined at the end of this file: the data directory and the message of the
 * token-capacity assertion. */
extern const char readTextDirectory[];
extern const char readTextAssertCapacityPositive[];

const char readTextAssertReaderNotNil[] = "pReader != NIL";
const char readTextSourcePath[] = "../rg_read_text.euc.c";
const char readTextCurrentOutOfRangeFormat[] = "pCur - pReader->m_pszBuf = %d\npReader->m_nSize = %d\n";
const char readTextAssertCursorNotNil[] = "pCur != NIL";
const char readTextTrueToken[] = "true";
const char readTextOnToken[] = "on";
const char readTextAssertUngetTokenNotNil[] = "pszUngetToken != NIL";
const char readTextDelimiterNotFoundError[0x18] = "not find delimiter";
const char readTextQuestionMarkString[8] = "?";

/* Opens the named file from the data directory into the reader. Returns 0 when it cannot be read. */
static int _InitReader(RgReadText *pReader, const char *pszName)
{
    RgFileSysData *pFile;
    unsigned int nSize;
    char *pBuf;

    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 40);
    }
    XrgFileSysPushMode(XRG_FILESYS_MODE_HEAP);
    pFile = RgFileSysRead(InstanceOfRgFileSys(), pszName, readTextDirectory);
    pReader->m_pFile = pFile;
    XrgFileSysPopMode();
    if (pFile == 0) {
        return 0;
    }
    nSize = pFile->size;
    pBuf = pFile->data;
    pReader->m_pszDelim = 0;
    pReader->m_bUngetPending = RG_UNGET_CLEAR;
    pReader->m_pszBuf = pBuf;
    pReader->m_pszCur = pBuf;
    pReader->m_nSize = nSize;
    return 1;
}

static void _DisposeReader(RgReadText *pReader)
{
    RgFileSysData *pFile;

    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 83);
    }
    pFile = pReader->m_pFile;
    if (pFile != 0) {
        DisposeRgFileSysData_sub(pFile, readTextSourcePath, 85);
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

/*
 * Skips white space and '#' comment lines from the cursor. Returns the start of the next
 * token, or 0 at the end of the buffer or at a '%' paragraph delimiter, which is recorded.
 */
static char *_SkipWhiteSpace(RgReadText *pReader)
{
    char *pCur;

    pCur = pReader->m_pszCur;
    if (pCur == 0) {
        return 0;
    }
    for (;;) {
        if ((unsigned char) pCur[0] == '#') {
            while (!_IsCurOnEOF(pReader, pCur) && (unsigned char) pCur[0] != '\n') {
                ++pCur;
            }
        }
        while (!_IsCurOnEOF(pReader, pCur) && _IsWhiteSpace((unsigned char) pCur[0])) {
            ++pCur;
        }
        if ((unsigned char) pCur[0] != '#') {
            break;
        }
    }
    if (_IsCurOnEOF(pReader, pCur) || (unsigned char) pCur[0] == '%') {
        if ((unsigned char) pCur[0] == '%') {
            pReader->m_pszDelim = pCur;
        }
        pCur = 0;
    }
    pReader->m_pszCur = pCur;
    return pCur;
}

static void _SetCurrent(RgReadText *pReader, char *pCur)
{
    unsigned int nSize;
    unsigned int nOffset;

    nSize = pReader->m_nSize;
    nOffset = pCur - pReader->m_pszBuf;
    if (nSize + 1 < nOffset) {
        RgError(readTextCurrentOutOfRangeFormat, readTextSourcePath, 141, nOffset, nSize);
    }
    pReader->m_pszCur = pCur;
}

static int _IsEOF(RgReadText *pReader)
{
    if (pReader->m_bUngetPending != 0) {
        return 0;
    }
    return _SkipWhiteSpace(pReader) == 0;
}

/*
 * Copies the next white-space-delimited token into pszOut, turning '@' into a space.
 * A pending ungotten token is returned first.
 */
static void _GetString(RgReadText *pReader, char *pszOut, unsigned int nCapacity)
{
    unsigned char *pCur;

    if (pReader->m_bUngetPending != 0) {
        strcpy(pszOut, pReader->m_szUngetToken);
        pReader->m_bUngetPending = RG_UNGET_CLEAR;
    } else {
        pCur = (unsigned char *)_SkipWhiteSpace(pReader);
        if (pCur == 0) {
            assert_prog(readTextAssertCursorNotNil, readTextSourcePath, 171);
        }
        while (!_IsWhiteSpace(*pCur)) {
            if (nCapacity == 0) {
                assert_prog(readTextAssertCapacityPositive, readTextSourcePath, 173);
            }
            if (*pCur == '@') {
                *pszOut = ' ';
            } else {
                *pszOut = *pCur;
            }
            pszOut++;
            do {
            } while (0);
            pCur++;
            _SetCurrent(pReader, (char *)pCur);
            nCapacity--;
        }
        *pszOut = 0;
    }
}

static float _GetFloat(RgReadText *pReader)
{
    char szToken[RG_READ_TEXT_TOKEN_MAX];
    char *pCur;

    pCur = _SkipWhiteSpace(pReader);
    if (pCur == 0) {
        assert_prog(readTextAssertCursorNotNil, readTextSourcePath, 198);
    }
    _GetString(pReader, szToken, RG_READ_TEXT_TOKEN_MAX - 1);
    return (float) atof(szToken);
}

int _GetInt(RgReadText *pReader)
{
    char szToken[RG_READ_TEXT_TOKEN_MAX];
    char *pCur;

    pCur = _SkipWhiteSpace(pReader);
    if (pCur == 0) {
        assert_prog(readTextAssertCursorNotNil, readTextSourcePath, 208);
    }
    _GetString(pReader, szToken, RG_READ_TEXT_TOKEN_MAX - 1);
    return atoi(szToken);
}

/* Reads a token as a flag: "true" and "on" give 1, any other token gives 0. */
static int _GetBool(RgReadText *pReader)
{
    char szToken[RG_READ_TEXT_TOKEN_MAX];
    char *pCur;

    pCur = _SkipWhiteSpace(pReader);
    if (pCur == 0) {
        assert_prog(readTextAssertCursorNotNil, readTextSourcePath, 218);
    }
    _GetString(pReader, szToken, RG_READ_TEXT_TOKEN_MAX - 1);
    if (strcmp(szToken, readTextTrueToken) == 0 ||
        strcmp(szToken, readTextOnToken) == 0) {
        return 1;
    }
    return 0;
}

/* Creates a reader over the named file. Returns 0 when the file cannot be opened. */
RgReadText *CreateRgReadText(const char *pszName)
{
    RgReadText *pReader;

    pReader = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgReadText), readTextSourcePath, 233);
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 234);
    }
    if (_InitReader(pReader, pszName) == 0) {
        RgHeapFree(InstanceOfRgHeap(), pReader, readTextSourcePath, 236);
        pReader = 0;
    }
    return pReader;
}

void DisposeRgReadText(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 244);
    }
    _DisposeReader(pReader);
    RgHeapFree(InstanceOfRgHeap(), pReader, readTextSourcePath, 246);
}

int RgReadTextIsEOF(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 256);
    }
    return _IsEOF(pReader);
}

int RgReadTextIsOnDelimitor(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 264);
    }
    return pReader->m_pszDelim != 0;
}

/* Stores a token to be returned by the next get call instead of reading the file. */
void RgReadTextUnget(RgReadText *pReader, char *pszUngetToken)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 279);
    }
    if (pszUngetToken == 0) {
        assert_prog(readTextAssertUngetTokenNotNil, readTextSourcePath, 280);
    }
    if (strlen(pszUngetToken) != 0) {
        pReader->m_bUngetPending = RG_UNGET_SET;
        strncpy(pReader->m_szUngetToken, pszUngetToken, RG_READ_TEXT_TOKEN_MAX);
        pszUngetToken[RG_READ_TEXT_TOKEN_MAX] = 0;
    }
}

void RgReadTextRewind(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 294);
    }
    pReader->m_bUngetPending = RG_UNGET_CLEAR;
    pReader->m_pszDelim = 0;
    pReader->m_pszCur = pReader->m_pszBuf;
}

void RgReadTextGetString(RgReadText *pReader, char *pszOut)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 306);
    }
    _GetString(pReader, pszOut, RG_READ_TEXT_TOKEN_MAX);
}

float RgReadTextGetFloat(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 313);
    }
    return _GetFloat(pReader);
}

int RgReadTextGetInt(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 320);
    }
    return _GetInt(pReader);
}

int RgReadTextGetBool(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 327);
    }
    return _GetBool(pReader);
}

/* Moves the cursor past the current '%' paragraph delimiter. Raises an error when there is none. */
void RgReadTextNextParagraph(RgReadText *pReader)
{
    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 340);
    }
    if (pReader->m_pszDelim == 0) {
        RgError(readTextDelimiterNotFoundError, readTextSourcePath, 344);
    }
    pReader->m_pszCur = pReader->m_pszDelim + 1;
    pReader->m_pszDelim = 0;
}

/*
 * Reads tokens until pszTag is found (and pszSubTag, when given, is the token after it).
 * A paragraph delimiter reached on the way is stepped over. Returns 1 when found, 0 at
 * the end of the file.
 */
int RgReadTextFindParagraph(RgReadText *pReader, const char *pszTag,
                            const char *pszSubTag)
{
    char szToken[256];

    if (pReader == 0) {
        assert_prog(readTextAssertReaderNotNil, readTextSourcePath, 357);
    }
    for (;;) {
        if (RgReadTextIsEOF(pReader)) {
            if (!RgReadTextIsOnDelimitor(pReader)) {
                return 0;
            }
            RgReadTextNextParagraph(pReader);
        }
        RgReadTextGetString(pReader, szToken);
        if (strcmp(szToken, pszTag) == 0) {
            if (pszSubTag == 0) {
                return 1;
            }
            RgReadTextGetString(pReader, szToken);
            if (strcmp(szToken, pszSubTag) == 0) {
                return 1;
            }
        }
    }
}



const char readTextDirectory[16] = "data\\nisimori\\";

const char readTextAssertCapacityPositive[16] = "nCapa > 0";
