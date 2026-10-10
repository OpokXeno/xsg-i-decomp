#ifndef SRC_MAIN_XGL_CAMERA_H
#define SRC_MAIN_XGL_CAMERA_H

typedef struct CameraTravel {
    float focus;
    unsigned char unmodeled_04[8];
    float scale;
    unsigned char unmodeled_10[0x20];
    Vector4 angle_delta;
    unsigned char unmodeled_40[0x20];
    Vector4 place_delta;
} CameraTravel;

#endif
