#ifndef INCLUDE_ASM_H
#define INCLUDE_ASM_H

/*
 * INCLUDE_ASM(FOLDER, NAME): generated scaffold (splat nonmatchings), no credit.
 * ACCEPTED_ASM(FOLDER, NAME): a previously accepted, reviewed standalone ee-asm
 *   function kept as tracked .s under src/<unit>/<tu>/ (exact_asm), never a
 *   generated nonmatching.  Same expansion, its own directory.
 * INCLUDE_RODATA(FOLDER, NAME): rodata that stays scaffold inside a C TU.
 * The bodies are assembled by the TU's own assembler (legacy ee-as / LA29),
 * so they must use that assembler's dialect (include/labels.inc).
 */
#if !defined(M2CTX) && !defined(PERMUTER) && !defined(SKIP_ASM)

#ifndef INCLUDE_ASM
#define INCLUDE_ASM(FOLDER, NAME) \
    __asm__( \
        ".section .text\n" \
        "    .set noat\n" \
        "    .set noreorder\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        "    .set reorder\n" \
        "    .set at\n" \
    )
#endif
#ifndef ACCEPTED_ASM
/*
 * INCLUDE_ASM's expansion, plus:
 *  - `.align 3`: every accepted block starts at an 8-byte-aligned original address,
 *    while the tracked accepted sources start with `.align 2`; without it the
 *    alignment padding of the preceding function is lost as soon as that function
 *    becomes C (cc1 emits no trailing padding) and everything after it shifts.
 *  - `.set macro` after the file: accepted sources declare `.set nomacro`.
 */
#define ACCEPTED_ASM(FOLDER, NAME) \
    __asm__( \
        ".section .text\n" \
        "    .align 3\n" \
        "    .set noat\n" \
        "    .set noreorder\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        "    .set reorder\n" \
        "    .set at\n" \
        "    .set macro\n" \
    )
#endif
#ifndef INCLUDE_RODATA
#define INCLUDE_RODATA(FOLDER, NAME) \
    __asm__( \
        ".section .rodata\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        ".section .text" \
    )
#endif

__asm__(".include \"include/labels.inc\"\n");

#else

#ifndef INCLUDE_ASM
#define INCLUDE_ASM(FOLDER, NAME)
#endif
#ifndef ACCEPTED_ASM
#define ACCEPTED_ASM(FOLDER, NAME)
#endif
#ifndef INCLUDE_RODATA
#define INCLUDE_RODATA(FOLDER, NAME)
#endif

#endif /* !defined(M2CTX) && !defined(PERMUTER) && !defined(SKIP_ASM) */

#endif /* INCLUDE_ASM_H */
