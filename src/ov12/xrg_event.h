/*
 * TU-local declarations of ov12/tu095 (src/ov12/xrg_event.c).
 */

#ifndef SRC_OV12_XRG_EVENT_H
#define SRC_OV12_XRG_EVENT_H

extern int s_bSetEventLevel;
extern int s_eEventLevel;

/* Defined in main/xgl_flags.c; reads back a single flag bit. */
extern int xglFlagsGet1(int bit_offset);

#endif /* SRC_OV12_XRG_EVENT_H */
