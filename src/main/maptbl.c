#include "common.h"

/* The Java CF-camera setters use 33 records with a 0xA0 stride.
 * Fields below are the views proved by those setters. The offset/pedestal
 * parameter block has different interpretations in the two camera modes;
 * bytes not yet given a complete field model remain explicitly unmodeled. */
typedef struct CfCameraDefinition {
    unsigned char pedestal_mode;
    unsigned char pedestal_interpolation;
    unsigned char unmodeled_02[6];
    float current_yaw;
    unsigned char unmodeled_0c[4];
    float offset[3];
    float perspective;
    unsigned char offset_or_pedestal_parameters[16];
    float angles[4];
    float lock_position[3];
    int lock_mode;
    float interpolation[2];
    unsigned char unmodeled_58[8];
    float fog_parameters[4];
    float fog_color[4];
    unsigned char unmodeled_80[32];
} CfCameraDefinition;

/* The retail initialized camera table contains 33 zeroed records. */
CfCameraDefinition CfCameraDefine[33] = {{0}};
