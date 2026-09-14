/*
 * OV12 original TU 91: 0x00a4e340..0x00a4e488 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_hostfile.h"

int XrgHostCreateOpen(const char *filename)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 46);
    return 0;
}

int XrgHostReadOpen(const char *filename)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 52);
    return 0;
}

int XrgHostSeekTop(int handle)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 58);
    return 0;
}

int XrgHostSeekLast(int handle)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 64);
    return 0;
}

void XrgHostClose(int handle)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 70);
}

int XrgHostRead(int handle, void *buffer, int size)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 75);
    return 0;
}

int XrgHostWrite(int handle, const void *buffer, int size)
{
    RgError(xrg_host_unsupported_message, xrg_host_source_file, 81);
    return 0;
}
