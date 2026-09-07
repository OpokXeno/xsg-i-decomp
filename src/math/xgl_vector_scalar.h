#ifndef XGL_VECTOR_SCALAR_H
#define XGL_VECTOR_SCALAR_H

/*
 * The selected routines access four adjacent single-precision storage slots.
 * This is a private reconstruction type, not a recovered historical typedef.
 */
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

typedef char Vector4_size_check[(sizeof(Vector4) == 16) ? 1 : -1];

/* The target compiler has no builtin offsetof; these checks use prefix sizes. */
typedef struct Vector4_xy_prefix {
    float x;
    float y;
} Vector4_xy_prefix;
typedef struct Vector4_xyz_prefix {
    float x;
    float y;
    float z;
} Vector4_xyz_prefix;
typedef char Vector4_x_offset_check[(sizeof(((Vector4 *)0)->x) == 4) ? 1 : -1];
typedef char Vector4_y_offset_check[(sizeof(Vector4_xy_prefix) == 8) ? 1 : -1];
typedef char Vector4_z_offset_check[(sizeof(Vector4_xyz_prefix) == 12) ? 1 : -1];
typedef char Vector4_w_offset_check[(sizeof(Vector4) - sizeof(Vector4_xyz_prefix) == 4) ? 1 : -1];

#endif
