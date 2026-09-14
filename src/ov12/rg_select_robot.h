/*
 * TU-local declarations of ov12/tu071 (src/ov12/rg_select_robot.c).
 */

#ifndef SRC_OV12_RG_SELECT_ROBOT_H
#define SRC_OV12_RG_SELECT_ROBOT_H

/* Loader state evidenced by the bounded adjacent load routines. */
extern int s_bLoading;

extern unsigned int s_uReqNum;

extern unsigned int s_uOkNum;

extern void *s_pWhoAreYou;

/* Shared main-unit CD read cancellation/reset entry. */
extern void xglCdReadCancel(void);

#endif /* SRC_OV12_RG_SELECT_ROBOT_H */
