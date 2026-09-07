/*
 * The MATRIX initializer pair writes a four-by-four float storage block in
 * row-major order.  MATRIX_identity4s uses the fourth value of each of the
 * first three rows as a basis scale; its name's historical suffix is not
 * established by the available ELF evidence.
 */

typedef char matrix_float_width_check[(sizeof(float) == 4) ? 1 : -1];
typedef char matrix_row_stride_check[(sizeof(float[4]) == 16) ? 1 : -1];
typedef char matrix_storage_size_check[(sizeof(float[4][4]) == 64) ? 1 : -1];

void MATRIX_identity(float destination[4][4])
{
    float zero = 0.0f;
    float one = 1.0f;

    destination[3][3] = one;
    destination[2][2] = one;
    destination[1][1] = one;
    destination[3][2] = zero;
    destination[3][1] = zero;
    destination[3][0] = zero;
    destination[2][3] = zero;
    destination[2][1] = zero;
    destination[2][0] = zero;
    destination[1][3] = zero;
    destination[1][2] = zero;
    destination[1][0] = zero;
    destination[0][3] = zero;
    destination[0][2] = zero;
    destination[0][0] = one;
    destination[0][1] = zero;
}

void MATRIX_identity4s(float destination[4][4])
{
    float zero = 0.0f;
    float one = 1.0f;

    destination[3][3] = one;
    destination[2][3] = one;
    destination[2][2] = one;
    destination[1][3] = one;
    destination[1][1] = one;
    destination[0][3] = one;
    destination[3][2] = zero;
    destination[3][1] = zero;
    destination[3][0] = zero;
    destination[2][1] = zero;
    destination[2][0] = zero;
    destination[1][2] = zero;
    destination[1][0] = zero;
    destination[0][2] = zero;
    destination[0][0] = one;
    destination[0][1] = zero;
}
