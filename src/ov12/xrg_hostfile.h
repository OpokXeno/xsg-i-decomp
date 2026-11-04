/*
 * TU-local declarations of ov12/tu091 (src/ov12/xrg_hostfile.c).
 */

#ifndef SRC_OV12_XRG_HOSTFILE_H
#define SRC_OV12_XRG_HOSTFILE_H

#include "ov12/xrg_hostfile.h"

extern void RgError(const char *message, const char *source_file, int line, ...);

/* Defined in xrg_hostfile.c. */
extern const char xrg_host_unsupported_message[];
extern const char xrg_host_source_file[];

#endif /* SRC_OV12_XRG_HOSTFILE_H */
