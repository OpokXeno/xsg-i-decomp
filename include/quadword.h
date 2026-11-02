#ifndef QUADWORD_H
#define QUADWORD_H

/*
 * The original's quadword transfer macros (the SDK sceVu0CopyVector inline-asm
 * idiom): a 16-byte vector or a 64-byte four-row matrix moved through the R5900
 * 128-bit GPR load/store, and a vector cleared with `sq $0`. They are inline
 * asm in the original too: both addresses are forced into registers (the
 * offsets are never folded), the scratch is fixed, the store never fills a
 * delay slot and nothing is scheduled across them ("memory"), none of which a
 * C quadword copy reproduces.
 */

/*
 * Through the $2 scratch (rows interleaved: load, store, load, store), and
 * clears from $0.
 *
 *   QuadwordCopy(d, v)        *d = *v
 *   QuadwordZero(d)           *d = 0
 *   QuadwordCopyMatrix(d, v)  d[0..3] = v[0..3]
 *   QuadwordZeroMatrix(d)     d[0..3] = 0
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

#define QuadwordZeroMatrix(destination) \
    __asm__ __volatile__( \
        "sq $0, 0(%0)\n" \
        "sq $0, 16(%0)\n" \
        "sq $0, 32(%0)\n" \
        "sq $0, 48(%0)\n" \
        : \
        : "r" (destination) \
        : "memory")

/*
 * Through the $8..$11 (t0-t3) scratch: all loads first, then all stores.
 *
 *   QuadwordCopyT0(d, v)                  *d = *v
 *   QuadwordCopyMatrixT0(d, v)            d[0..3] = v[0..3]
 *   QuadwordMergeMatrixPosition(d, m, p)  d[0..2] = m[0..2], d[3] = *p
 */

#define QuadwordCopyT0(destination, source) \
    __asm__ __volatile__( \
        "lq $8, 0(%1)\n" \
        "sq $8, 0(%0)\n" \
        : \
        : "r" (destination), "r" (source) \
        : "$8", "memory")

#define QuadwordCopyMatrixT0(destination, source) \
    __asm__ __volatile__( \
        "lq $8, 0(%1)\n" \
        "lq $9, 16(%1)\n" \
        "lq $10, 32(%1)\n" \
        "lq $11, 48(%1)\n" \
        "sq $8, 0(%0)\n" \
        "sq $9, 16(%0)\n" \
        "sq $10, 32(%0)\n" \
        "sq $11, 48(%0)\n" \
        : \
        : "r" (destination), "r" (source) \
        : "$8", "$9", "$10", "$11", "memory")

#define QuadwordMergeMatrixPosition(destination, matrix, position) \
    __asm__ __volatile__( \
        "lq $8, 0(%1)\n" \
        "lq $9, 16(%1)\n" \
        "lq $10, 32(%1)\n" \
        "lq $11, 0(%2)\n" \
        "sq $8, 0(%0)\n" \
        "sq $9, 16(%0)\n" \
        "sq $10, 32(%0)\n" \
        "sq $11, 48(%0)\n" \
        : \
        : "r" (destination), "r" (matrix), "r" (position) \
        : "$8", "$9", "$10", "$11", "memory")

/*
 * Copy loops (xglMemCopy16 / xglMemCopy64): n > 0 quadwords or 64-byte
 * blocks; d, v and n are advanced by the loop, so they must be lvalues.
 *
 *   QuadwordCopyArray(d, v, n)        d[0..n-1] = v[0..n-1], quadwords
 *   QuadwordCopyMatrixArray(d, v, n)  d[0..n-1] = v[0..n-1], 64-byte blocks
 */

#define QuadwordCopyArray(destination, source, count) \
    __asm__ __volatile__( \
        "1:\n" \
        "lq $2, 0(%1)\n" \
        "sq $2, 0(%0)\n" \
        "addi %2, %2, -1\n" \
        "addi %0, %0, 16\n" \
        "addi %1, %1, 16\n" \
        "bne $0, %2, 1b\n" \
        : "+r" (destination), "+r" (source), "+r" (count) \
        : \
        : "$2", "memory")

#define QuadwordCopyMatrixArray(destination, source, count) \
    __asm__ __volatile__( \
        "1:\n" \
        "lq $2, 0(%1)\n" \
        "sq $2, 0(%0)\n" \
        "lq $2, 16(%1)\n" \
        "sq $2, 16(%0)\n" \
        "lq $2, 32(%1)\n" \
        "sq $2, 32(%0)\n" \
        "lq $2, 48(%1)\n" \
        "sq $2, 48(%0)\n" \
        "addi %2, %2, -1\n" \
        "addi %0, %0, 64\n" \
        "addi %1, %1, 64\n" \
        "bne $0, %2, 1b\n" \
        : "+r" (destination), "+r" (source), "+r" (count) \
        : \
        : "$2", "memory")

#endif
