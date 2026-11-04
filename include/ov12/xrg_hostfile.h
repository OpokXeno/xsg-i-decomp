#ifndef INCLUDE_OV12_XRG_HOSTFILE_H
#define INCLUDE_OV12_XRG_HOSTFILE_H

/* Functions and data used by other translation units. */
int XrgHostCreateOpen(const char *filename);
int XrgHostReadOpen(const char *filename);
int XrgHostSeekTop(int handle);
int XrgHostSeekLast(int handle);
void XrgHostClose(int handle);
int XrgHostRead(int handle, void *buffer, int size);
int XrgHostWrite(int handle, const void *buffer, int size);

#endif /* INCLUDE_OV12_XRG_HOSTFILE_H */
