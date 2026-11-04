#ifndef INCLUDE_OV12_RG_ERROR_H
#define INCLUDE_OV12_RG_ERROR_H

/* Functions and data used by other translation units. */
void RgError(const char *format, const char *source_file, int line, ...);
void RgWarn(const char *format, const char *source_file, int line, ...);

#endif /* INCLUDE_OV12_RG_ERROR_H */
