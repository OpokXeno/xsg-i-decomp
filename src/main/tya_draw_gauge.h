/* Signed geometry and the two palette groups used to build a gauge packet. */
typedef struct TyaGaugeArgs {
    short x;
    short y;
    unsigned int vertex_attribute;
    short width;
    short height;
    unsigned short current;
    unsigned short maximum;
    unsigned char filled_color[4];
    unsigned char unfilled_color[4];
} TyaGaugeArgs;
