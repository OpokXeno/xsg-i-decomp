#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc are declared in src/main/ssd_init.h. The generated
 * include/main/ssd_init.h cannot carry RssdWork's type yet, so that header is included directly.
 */
#include "ssd_init.h"

enum {
    RSSD_CMD_SET_OUTPUT_MODE      = 0x10,
    RSSD_CMD_SET_SPDIF_MODE       = 0x11,
    RSSD_CMD_SET_SPU_VOLUME       = 0x12,
    RSSD_CMD_SET_CD_VOLUME        = 0x13,
    RSSD_CMD_SET_REVERB_PARAMETER = 0x14
};

void SsdSetOutputMode(int mode)
{
    RssdRequest request;

    request.arg[0].value = mode;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_OUTPUT_MODE, &request, 0, 0);
}

void SsdSetSPDIFMode(int mode)
{
    RssdRequest request;

    request.arg[0].value = mode;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SPDIF_MODE, &request, 0, 0);
}

/* The first argument is unused. */
void SsdSetSpuVolume(int unused, int volume)
{
    RssdRequest request;

    request.arg[0].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SPU_VOLUME, &request, 0, 0);
}

/* The first argument is unused. */
void SsdSetCDVolume(int unused, int volume)
{
    RssdRequest request;

    request.arg[0].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_CD_VOLUME, &request, 0, 0);
}

/*
 * Forwards the four words unchanged to RSSD command 0x14. On the IOP side, SsdSetReverbParameter
 * reads the low byte of the first word as an effect type and its high bits as an effect-attribute
 * entry; the second word is stored as a halfword at that entry's +0xa, the third and fourth as
 * bytes at +0x8 and +0x9. Their numeric meaning is not established.
 */
void SsdSetReverbParameter(int effectSelector, int effectAttr1, int effectAttr2, int effectAttr3)
{
    RssdRequest request;

    request.arg[0].value = effectSelector;
    request.arg[1].value = effectAttr1;
    request.arg[2].value = effectAttr2;
    request.arg[3].value = effectAttr3;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_REVERB_PARAMETER, &request, 0, 0);
}
