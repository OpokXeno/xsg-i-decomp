#ifndef INCLUDE_OV12_INIT_XRG_INPUT_H
#define INCLUDE_OV12_INIT_XRG_INPUT_H

/* The per-pad input state; its layout is private to src/ov12/init_xrg_input.c. */
typedef struct XrgInput XrgInput;

/* Functions and data used by other translation units. */
void InitXrgInput(XrgInput *pInput, int padId);
void XrgInputDeviceCheck(XrgInput *pInput, float frameTime);

#endif /* INCLUDE_OV12_INIT_XRG_INPUT_H */
