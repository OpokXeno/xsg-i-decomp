#ifndef INCLUDE_OV12_RG_GET_FRAME_TIME_H
#define INCLUDE_OV12_RG_GET_FRAME_TIME_H

/* One NTSC field at 30 Hz (1.0f / 30.0f), the engine's fixed frame time. */
#define RG_FRAME_TIME 0.033333335f

/* Functions and data used by other translation units. */
float RgGetFrameTime(void);

#endif /* INCLUDE_OV12_RG_GET_FRAME_TIME_H */
