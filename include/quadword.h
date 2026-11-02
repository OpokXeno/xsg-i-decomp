#ifndef QUADWORD_H
#define QUADWORD_H

/*
 * The original's quadword transfer macros (the SDK sceVu0CopyVector inline-asm
 * idiom): a 16-byte vector or a 64-byte four-row matrix moved through the R5900
 * 128-bit GPR load/store, and a vector cleared with `sq $0`. They are inline
 * asm in the original too: both addresses are forced into registers (the
 * offsets are never folded), the scratch is always $2, the store never fills a
 * delay slot and nothing is scheduled across them ("memory"), none of which a
 * C quadword copy reproduces.
 */

#define QuadwordCopy(destination, source) \
    __asm__ __volatile__( \
        "lq $2, 0(%1)\n" \
        "sq $2, 0(%0)\n" \
        : \
        : "r" (destination), "r" (source) \
        : "$2", "memory")

#define QuadwordZero(destination) \
    __asm__ __volatile__( \
        "sq $0, 0(%0)\n" \
        : \
        : "r" (destination) \
        : "memory")

#define QuadwordCopyMatrix(destination, source) \
    __asm__ __volatile__( \
        "lq $2, 0(%1)\n" \
        "sq $2, 0(%0)\n" \
        "lq $2, 16(%1)\n" \
        "sq $2, 16(%0)\n" \
        "lq $2, 32(%1)\n" \
        "sq $2, 32(%0)\n" \
        "lq $2, 48(%1)\n" \
        "sq $2, 48(%0)\n" \
        : \
        : "r" (destination), "r" (source) \
        : "$2", "memory")

#endif
