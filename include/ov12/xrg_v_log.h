#ifndef INCLUDE_OV12_XRG_V_LOG_H
#define INCLUDE_OV12_XRG_V_LOG_H

/* The EE GCC argument pointer is a plain byte pointer: XrgLog and XrgOut hand
 * the address of their spilled register arguments down as it. */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

/* Functions and data used by other translation units. */
void XrgVLog(const char *format, const char *source_file, int line, va_list args);
void XrgVLogSys(const char *format, const char *source_file, int line, va_list args);
void XrgVOut(const char *format, va_list args);
void XrgLog(const char *format, const char *source_file, int line, ...);
void XrgLogSys(const char *format, const char *source_file, int line, ...);
void XrgOut(const char *format, ...);

#endif /* INCLUDE_OV12_XRG_V_LOG_H */
