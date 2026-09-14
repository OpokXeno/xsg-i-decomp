/*
 * TU-local declarations of ov12/tu091 (src/ov12/xrg_hostfile.c).
 */

#ifndef SRC_OV12_XRG_HOSTFILE_H
#define SRC_OV12_XRG_HOSTFILE_H

extern void RgError(const char *message, const char *source_file, int line, ...);

int XrgHostCreateOpen(const char *filename);

int XrgHostReadOpen(const char *filename);

int XrgHostSeekTop(int handle);

int XrgHostSeekLast(int handle);

void XrgHostClose(int handle);

int XrgHostRead(int handle, void *buffer, int size);

int XrgHostWrite(int handle, const void *buffer, int size);

extern const char xrg_host_unsupported_message[];

extern const char xrg_host_source_file[];

#endif /* SRC_OV12_XRG_HOSTFILE_H */
