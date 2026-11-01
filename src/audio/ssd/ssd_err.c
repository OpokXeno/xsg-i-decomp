/* Partial view of the SSD work prefix used by this FDR; SsdWork is defined in ssd_init.c. */
struct SsdErrorState {
    signed char function_code;
    signed char warning_code;
    signed char sequence_code;
    int detail;
};

struct SsdWorkPrefix {
    unsigned int flags;
    struct SsdErrorState errors;
};

extern struct SsdWorkPrefix SsdWork;
int printf(const char *format, ...);
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

int SsdCheckSumMusicData(void)
{
    return 0;
}

int SsdCheckData(const int *data, int signature)
{
    if (data[0] != signature)
        return -1;
    if (SsdGetCheckSum(data) != 0)
        return -2;
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
    SsdWork.flags |= 8;
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
    SsdWork.flags |= 8;
}
