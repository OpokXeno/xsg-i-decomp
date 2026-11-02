#ifndef INCLUDE_OV02_UMN_INTERFACE2_H
#define INCLUDE_OV02_UMN_INTERFACE2_H

/*
 * State reached through the global UmnInterface2 pointer; only the two
 * frame counters UmnInterface2Main touches are evidenced.
 */
typedef struct UmnInterface2State {
    unsigned int base_color;
    unsigned char color_a[3];
    unsigned char intensity_a; /* decremented by 8 each frame until zero */
    unsigned char color_b[3];
    unsigned char intensity_b; /* decremented by 2 each frame until zero */
} UmnInterface2State;

/* Functions and data used by other translation units. */
extern UmnInterface2State *UmnInterface2;
void *UmnBgCubeInit(void *memory);
void UmnInterface2Main(void);
void *UmnInterface2Init(void *memory);

#endif /* INCLUDE_OV02_UMN_INTERFACE2_H */
