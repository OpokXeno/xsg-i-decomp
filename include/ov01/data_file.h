#ifndef INCLUDE_OV01_DATA_FILE_H
#define INCLUDE_OV01_DATA_FILE_H

/* Original dataFileLoadNB (0x00A1A7E8) passes filename to xglCdReadFile,
 * uses it in the failure diagnostic, and returns zero or one. */
int dataFileLoadNB(const char *filename, void *destination);

#endif
