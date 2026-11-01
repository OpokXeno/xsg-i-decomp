#include "common.h"

#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

typedef char *va_list;

extern const unsigned char tracePrintFormat[];
extern const char sdvOpNameJsrW[8];
extern const char sdvOpNameGotoW[8];
extern const char sdvOpNameIfnull[8];
extern const char sdvOpNameWide[8];
extern const char sdvOpNameAthrow[8];
extern const char sdvOpNameNew[8];
extern const char sdvOpNameReturn[8];
extern const char sdvOpNameAreturn[8];
extern const char sdvOpNameDreturn[8];
extern const char sdvOpNameFreturn[8];
extern const char sdvOpNameLreturn[8];
extern const char sdvOpNameIreturn[8];
extern const char sdvOpNameRet[8];
extern const char sdvOpNameJsr[8];
extern const char sdvOpNameGoto[8];
extern const char sdvOpNameIfle[8];
extern const char sdvOpNameIfgt[8];
extern const char sdvOpNameIfge[8];
extern const char sdvOpNameIflt[8];
extern const char sdvOpNameIfne[8];
extern const char sdvOpNameIfeq[8];
extern const char sdvOpNameDcmpg[8];
extern const char sdvOpNameDcmpl[8];
extern const char sdvOpNameFcmpg[8];
extern const char sdvOpNameFcmpl[8];
extern const char sdvOpNameLcmp[8];
extern const char sdvOpNameD2f[8];
extern const char sdvOpNameD2l[8];
extern const char sdvOpNameD2i[8];
extern const char sdvOpNameF2d[8];
extern const char sdvOpNameF2l[8];
extern const char sdvOpNameF2i[8];
extern const char sdvOpNameL2d[8];
extern const char sdvOpNameL2f[8];
extern const char sdvOpNameL2i[8];
extern const char sdvOpNameI2d[8];
extern const char sdvOpNameI2f[8];
extern const char sdvOpNameI2l[8];
extern const char sdvOpNameIinc[8];
extern const char sdvOpNameLxor[8];
extern const char sdvOpNameIxor[8];
extern const char sdvOpNameLor[8];
extern const char sdvOpNameIor[8];
extern const char sdvOpNameLand[8];
extern const char sdvOpNameIand[8];
extern const char sdvOpNameLushr[8];
extern const char sdvOpNameIushr[8];
extern const char sdvOpNameLshr[8];
extern const char sdvOpNameIshr[8];
extern const char sdvOpNameLshl[8];
extern const char sdvOpNameIshl[8];
extern const char sdvOpNameDneg[8];
extern const char sdvOpNameFneg[8];
extern const char sdvOpNameLneg[8];
extern const char sdvOpNameIneg[8];
extern const char sdvOpNameDrem[8];
extern const char sdvOpNameFrem[8];
extern const char sdvOpNameLrem[8];
extern const char sdvOpNameIrem[8];
extern const char sdvOpNameDdiv[8];
extern const char sdvOpNameFdiv[8];
extern const char sdvOpNameLdiv[8];
extern const char sdvOpNameIdiv[8];
extern const char sdvOpNameDmul[8];
extern const char sdvOpNameFmul[8];
extern const char sdvOpNameLmul[8];
extern const char sdvOpNameImul[8];
extern const char sdvOpNameDsub[8];
extern const char sdvOpNameFsub[8];
extern const char sdvOpNameLsub[8];
extern const char sdvOpNameIsub[8];
extern const char sdvOpNameDadd[8];
extern const char sdvOpNameFadd[8];
extern const char sdvOpNameLadd[8];
extern const char sdvOpNameIadd[8];
extern const char sdvOpNameSwap[8];
extern const char sdvOpNameDup2X2[8];
extern const char sdvOpNameDup2X1[8];
extern const char sdvOpNameDup2[8];
extern const char sdvOpNameDupX2[8];
extern const char sdvOpNameDupX1[8];
extern const char sdvOpNameDup[8];
extern const char sdvOpNamePop2[8];
extern const char sdvOpNamePop[8];
extern const char sdvOpNameSastore[8];
extern const char sdvOpNameCastore[8];
extern const char sdvOpNameBastore[8];
extern const char sdvOpNameAastore[8];
extern const char sdvOpNameDastore[8];
extern const char sdvOpNameFastore[8];
extern const char sdvOpNameLastore[8];
extern const char sdvOpNameIastore[8];
extern const char sdvOpNameAstore[8];
extern const char sdvOpNameDstore[8];
extern const char sdvOpNameFstore[8];
extern const char sdvOpNameLstore[8];
extern const char sdvOpNameIstore[8];
extern const char sdvOpNameSaload[8];
extern const char sdvOpNameCaload[8];
extern const char sdvOpNameBaload[8];
extern const char sdvOpNameAaload[8];
extern const char sdvOpNameDaload[8];
extern const char sdvOpNameFaload[8];
extern const char sdvOpNameLaload[8];
extern const char sdvOpNameIaload[8];
extern const char sdvOpNameAload3[8];
extern const char sdvOpNameAload2[8];
extern const char sdvOpNameAload1[8];
extern const char sdvOpNameAload0[8];
extern const char sdvOpNameDload3[8];
extern const char sdvOpNameDload2[8];
extern const char sdvOpNameDload1[8];
extern const char sdvOpNameDload0[8];
extern const char sdvOpNameFload3[8];
extern const char sdvOpNameFload2[8];
extern const char sdvOpNameFload1[8];
extern const char sdvOpNameFload0[8];
extern const char sdvOpNameLload3[8];
extern const char sdvOpNameLload2[8];
extern const char sdvOpNameLload1[8];
extern const char sdvOpNameLload0[8];
extern const char sdvOpNameIload3[8];
extern const char sdvOpNameIload2[8];
extern const char sdvOpNameIload1[8];
extern const char sdvOpNameIload0[8];
extern const char sdvOpNameAload[8];
extern const char sdvOpNameDload[8];
extern const char sdvOpNameFload[8];
extern const char sdvOpNameLload[8];
extern const char sdvOpNameIload[8];
extern const char sdvOpNameLdc2w[8];
extern const char sdvOpNameLdc2[8];
extern const char sdvOpNameLdc1[8];
extern const char sdvOpNameSipush[8];
extern const char sdvOpNameBipush[8];
extern const char sdvOpNameNop[8];
extern int vmDebugMask;
extern const char sdvOpNameUnknown[4];

extern int printf(const char *format, ...);
extern int vsprintf(char *buffer, const char *format, va_list args);

const unsigned char tracePrintFormat[8] = "%s\n";

int vmDebugMask = 0;

const char sdvOpNameJsrW[8] = "jsr_w";

const char sdvOpNameGotoW[8] = "goto_w";

const char sdvOpNameIfnull[8] = "ifnull";

const char sdvOpNameWide[8] = "wide";

const char sdvOpNameAthrow[8] = "athrow";

const char sdvOpNameNew[8] = "new";

const char sdvOpNameUnknown[4] = "\?\?\?";

const char sdvOpNameReturn[8] = "return";

const char sdvOpNameAreturn[8] = "areturn";

const char sdvOpNameDreturn[8] = "dreturn";

const char sdvOpNameFreturn[8] = "freturn";

const char sdvOpNameLreturn[8] = "lreturn";

const char sdvOpNameIreturn[8] = "ireturn";

const char sdvOpNameRet[8] = "ret";

const char sdvOpNameJsr[8] = "jsr";

const char sdvOpNameGoto[8] = "goto";

const char sdvOpNameIfle[8] = "ifle";

const char sdvOpNameIfgt[8] = "ifgt";

const char sdvOpNameIfge[8] = "ifge";

const char sdvOpNameIflt[8] = "iflt";

const char sdvOpNameIfne[8] = "ifne";

const char sdvOpNameIfeq[8] = "ifeq";

const char sdvOpNameDcmpg[8] = "dcmpg";

const char sdvOpNameDcmpl[8] = "dcmpl";

const char sdvOpNameFcmpg[8] = "fcmpg";

const char sdvOpNameFcmpl[8] = "fcmpl";

const char sdvOpNameLcmp[8] = "lcmp";

const char sdvOpNameD2f[8] = "d2f";

const char sdvOpNameD2l[8] = "d2l";

const char sdvOpNameD2i[8] = "d2i";

const char sdvOpNameF2d[8] = "f2d";

const char sdvOpNameF2l[8] = "f2l";

const char sdvOpNameF2i[8] = "f2i";

const char sdvOpNameL2d[8] = "l2d";

const char sdvOpNameL2f[8] = "l2f";

const char sdvOpNameL2i[8] = "l2i";

const char sdvOpNameI2d[8] = "i2d";

const char sdvOpNameI2f[8] = "i2f";

const char sdvOpNameI2l[8] = "i2l";

const char sdvOpNameIinc[8] = "iinc";

const char sdvOpNameLxor[8] = "lxor";

const char sdvOpNameIxor[8] = "ixor";

const char sdvOpNameLor[8] = "lor";

const char sdvOpNameIor[8] = "ior";

const char sdvOpNameLand[8] = "land";

const char sdvOpNameIand[8] = "iand";

const char sdvOpNameLushr[8] = "lushr";

const char sdvOpNameIushr[8] = "iushr";

const char sdvOpNameLshr[8] = "lshr";

const char sdvOpNameIshr[8] = "ishr";

const char sdvOpNameLshl[8] = "lshl";

const char sdvOpNameIshl[8] = "ishl";

const char sdvOpNameDneg[8] = "dneg";

const char sdvOpNameFneg[8] = "fneg";

const char sdvOpNameLneg[8] = "lneg";

const char sdvOpNameIneg[8] = "ineg";

const char sdvOpNameDrem[8] = "drem";

const char sdvOpNameFrem[8] = "frem";

const char sdvOpNameLrem[8] = "lrem";

const char sdvOpNameIrem[8] = "irem";

const char sdvOpNameDdiv[8] = "ddiv";

const char sdvOpNameFdiv[8] = "fdiv";

const char sdvOpNameLdiv[8] = "ldiv";

const char sdvOpNameIdiv[8] = "idiv";

const char sdvOpNameDmul[8] = "dmul";

const char sdvOpNameFmul[8] = "fmul";

const char sdvOpNameLmul[8] = "lmul";

const char sdvOpNameImul[8] = "imul";

const char sdvOpNameDsub[8] = "dsub";

const char sdvOpNameFsub[8] = "fsub";

const char sdvOpNameLsub[8] = "lsub";

const char sdvOpNameIsub[8] = "isub";

const char sdvOpNameDadd[8] = "dadd";

const char sdvOpNameFadd[8] = "fadd";

const char sdvOpNameLadd[8] = "ladd";

const char sdvOpNameIadd[8] = "iadd";

const char sdvOpNameSwap[8] = "swap";

const char sdvOpNameDup2X2[8] = "dup2_x2";

const char sdvOpNameDup2X1[8] = "dup2_x1";

const char sdvOpNameDup2[8] = "dup2";

const char sdvOpNameDupX2[8] = "dup_x2";

const char sdvOpNameDupX1[8] = "dup_x1";

const char sdvOpNameDup[8] = "dup";

const char sdvOpNamePop2[8] = "pop2";

const char sdvOpNamePop[8] = "pop";

const char sdvOpNameSastore[8] = "sastore";

const char sdvOpNameCastore[8] = "castore";

const char sdvOpNameBastore[8] = "bastore";

const char sdvOpNameAastore[8] = "aastore";

const char sdvOpNameDastore[8] = "dastore";

const char sdvOpNameFastore[8] = "fastore";

const char sdvOpNameLastore[8] = "lastore";

const char sdvOpNameIastore[8] = "iastore";

const char sdvOpNameAstore[8] = "astore";

const char sdvOpNameDstore[8] = "dstore";

const char sdvOpNameFstore[8] = "fstore";

const char sdvOpNameLstore[8] = "lstore";

const char sdvOpNameIstore[8] = "istore";

const char sdvOpNameSaload[8] = "saload";

const char sdvOpNameCaload[8] = "caload";

const char sdvOpNameBaload[8] = "baload";

const char sdvOpNameAaload[8] = "aaload";

const char sdvOpNameDaload[8] = "daload";

const char sdvOpNameFaload[8] = "faload";

const char sdvOpNameLaload[8] = "laload";

const char sdvOpNameIaload[8] = "iaload";

const char sdvOpNameAload3[8] = "aload_3";

const char sdvOpNameAload2[8] = "aload_2";

const char sdvOpNameAload1[8] = "aload_1";

const char sdvOpNameAload0[8] = "aload_0";

const char sdvOpNameDload3[8] = "dload_3";

const char sdvOpNameDload2[8] = "dload_2";

const char sdvOpNameDload1[8] = "dload_1";

const char sdvOpNameDload0[8] = "dload_0";

const char sdvOpNameFload3[8] = "fload_3";

const char sdvOpNameFload2[8] = "fload_2";

const char sdvOpNameFload1[8] = "fload_1";

const char sdvOpNameFload0[8] = "fload_0";

const char sdvOpNameLload3[8] = "lload_3";

const char sdvOpNameLload2[8] = "lload_2";

const char sdvOpNameLload1[8] = "lload_1";

const char sdvOpNameLload0[8] = "lload_0";

const char sdvOpNameIload3[8] = "iload_3";

const char sdvOpNameIload2[8] = "iload_2";

const char sdvOpNameIload1[8] = "iload_1";

const char sdvOpNameIload0[8] = "iload_0";

const char sdvOpNameAload[8] = "aload";

const char sdvOpNameDload[8] = "dload";

const char sdvOpNameFload[8] = "fload";

const char sdvOpNameLload[8] = "lload";

const char sdvOpNameIload[8] = "iload";

const char sdvOpNameLdc2w[8] = "ldc2w";

const char sdvOpNameLdc2[8] = "ldc2";

const char sdvOpNameLdc1[8] = "ldc1";

const char sdvOpNameSipush[8] = "sipush";

const char sdvOpNameBipush[8] = "bipush";

const char sdvOpNameNop[8] = "nop";

void tracePrint(const char *format, ...) {
    va_list args;
    char buffer[0x100];

    va_start(args, format);
    vsprintf(buffer, format, args);
    printf(tracePrintFormat, buffer);
    va_end(args);
}

/*
 * These exported debug entry points contain no instructions beyond their
 * return sequence.  Their callers are not present in the bounded resident
 * call search, so the historical argument lists remain unresolved; the
 * unspecified C parameter lists preserve that uncertainty without inventing
 * a prototype.  The original symbol spellings are retained.
 */
void sefPrintVector()
{
}

void sefPrintMatrix()
{
}
