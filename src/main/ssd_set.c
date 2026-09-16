#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet (config/header-canon.json, RssdWorkFlags), so that
 * header is included directly.
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

void SsdSetSpuVolume(int unused, int volume)
{
    RssdRequest request;

    request.arg[0].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SPU_VOLUME, &request, 0, 0);
}

void SsdSetCDVolume(int unused, int volume)
{
    RssdRequest request;

    request.arg[0].value = volume;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_CD_VOLUME, &request, 0, 0);
}

/*
 * These four words are forwarded to RSSD command 0x14 verbatim. On the IOP
 * side (game/extracted-iso/IOP/SSD.IRX SsdSetReverbParameter, 0x00001c58,
 * same name and arity), the low byte of the first word selects an effect
 * type and its high bits select an effect-attribute table entry (0x1ca0/
 * 0x1ca4, stored at the entry's +0x3 by 0x1d24); the second word is stored
 * as a halfword at the entry's +0xa (0x1d30); the third and fourth are
 * stored as bytes at the entry's +0x8/+0x9 (0x1d28/0x1d2c). Beyond
 * "effect-attribute record fields", their numeric meaning is not evidenced
 * by either side.
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
