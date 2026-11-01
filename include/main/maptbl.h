#ifndef INCLUDE_MAIN_MAPTBL_H
#define INCLUDE_MAIN_MAPTBL_H

typedef struct {
    char **names;
    int count;
} MapNameGroup;

typedef struct CfCameraDefinition {
    unsigned char pedestal_mode;          /* +0x00 */
    unsigned char pedestal_interpolation; /* +0x01 */
    unsigned char unmodeled_02[6];
    float current_yaw;                    /* +0x08 */
    unsigned char unmodeled_0c[4];
    float offset[3];                      /* +0x10 */
    float perspective;                    /* +0x1c */
    union {
        struct {
            int first_mode;     /* +0x20 */
            float first_value;  /* +0x24 */
            int second_mode;    /* +0x28 */
            float second_value; /* +0x2c */
        } offset;
        float pedestal[4];
    } parameters;                         /* +0x20 */
    float angles[4];                      /* +0x30 */
    float lock_position[3];               /* +0x40 */
    int lock_mode;                        /* +0x4c */
    float interpolation[2];               /* +0x50 */
    unsigned char unmodeled_58[8];
    float fog_parameters[4];              /* +0x60 */
    float fog_color[4];                   /* +0x70 */
    unsigned char unmodeled_80[32];
} CfCameraDefinition;

typedef struct CameraImage {
    unsigned long long words[0x5f0 / 8];
} CameraImage;

extern MapNameGroup maptbl[20];
extern CfCameraDefinition CfCameraDefine[33];
extern CameraImage save_cam;

#endif /* INCLUDE_MAIN_MAPTBL_H */
