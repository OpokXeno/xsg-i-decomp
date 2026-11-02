#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"

unsigned int SsdGetCheckSum(const int *data);

int SsdGetFuncErrorCode(void)
{
    return SsdWork.errors.function_code;
}

void SsdResetFuncErrorCode(void)
{
    SsdWork.errors.function_code = 0;
    SsdWork.errors.warning_code = 0;
}

int SsdCheckSumMusicData(const void *data)
{
    return 0;
}

int SsdCheckData(const void *data, int signature)
{
    const int *words = data;

    if (words[0] != signature)
        return SSD_ERROR_BAD_SIGNATURE;
    if (SsdGetCheckSum(words) != 0)
        return SSD_ERROR_BAD_CHECKSUM;
    return 0;
}

unsigned int SsdGetCheckSum(const int *data)
{
    int words_remaining = (data[2] + 3) >> 2;
    unsigned int checksum = 0;

    do {
        checksum += *data++;
    } while (--words_remaining != 0);
    return checksum;
}

void SsdErrorProc(int code, int detail)
{
    SsdWork.errors.function_code = code;
    SsdWork.errors.detail = detail;
    printf("Ssd function error : %d,0x%08x\n", code, detail);
    SsdWork.flags |= SSD_FLAG_ERROR;
}

void SsdWarringProc(int code, int detail)
{
    SsdWork.errors.warning_code = code;
    SsdWork.errors.detail = detail;
    printf("Ssd function warning : %d,0x%08x\n", code, detail);
}

void SsdSeqErrorProc(int code, int detail)
{
    SsdWork.errors.sequence_code = code;
    SsdWork.errors.detail = detail;
    SsdWork.flags |= SSD_FLAG_ERROR;
}
