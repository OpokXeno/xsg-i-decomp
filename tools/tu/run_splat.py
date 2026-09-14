#!/usr/bin/env python3
"""Run pinned splat with the prototype's R5900 decoder settings.

No installed package file is modified; three instruction families that
rabbitizer 1.16.2 would emit as `.word` are instead emitted as real mnemonics in
the syntax the pinned ps2dev GAS 2.45.1 encodes to the original word (verified by
the whole-file comparison and by tools/asm_probe.s):

* cvt.w.s   (482 in main): rabbitizer flags it "disassemble as data" because
            some GNU as builds encode it differently; ps2dev GAS is correct.
* vsqrt     (14 in main): original words have bit 21 set, which rabbitizer treats
            as invalid bits; GAS's opcode (match 4a2003bd / mask fe60ffff) sets it.
* vclipw.xyz (12 in main): GAS requires the `$vfNxyz, $vfMw` operand syntax.

The only remaining `.word` in text is the 16-byte `_maxval` literal that the
original program keeps inside `_copyRefImage` (read by `lq`); it is data.
"""
import sys

import rabbitizer
from spimdisasm.mips.symbols.MipsSymbolFunction import SymbolFunction
from splat.segtypes.common.codesubsegment import CommonSegCodeSubsegment
from splat.__main__ import splat_main

_process = CommonSegCodeSubsegment.process_insns


def process_insns(segment, func):
    _process(segment, func)
    for insn in func.instructions:
        if insn.uniqueId == rabbitizer.InstrId.cpu_cvt_w_s:
            insn.flag_r5900DisasmAsData = rabbitizer.TrinaryValue.FALSE


def gas_text(word):
    if word & 0xFE60FFFF == 0x4A2003BD:
        return f"vsqrt      $Q, $vf{(word >> 16) & 31}{'xyzw'[(word >> 23) & 3]}"
    if word & 0xFFE007FF == 0x4BC001FF:
        return f"vclipw.xyz $vf{(word >> 11) & 31}xyz, $vf{(word >> 16) & 31}w"
    return None


_emit = SymbolFunction._emitInstruction


def _emitInstruction(self, instr, instructionOffset, wasLastInstABranch, isSplittedSymbol=False):
    text = gas_text(instr.getRaw())
    if text is None:
        return _emit(self, instr, instructionOffset, wasLastInstABranch, isSplittedSymbol=isSplittedSymbol)
    comment = self.generateAsmLineComment(instructionOffset, instr.getRaw())
    if wasLastInstABranch:
        return f"{comment}   {text}"
    return f"{comment}  {text}"


CommonSegCodeSubsegment.process_insns = process_insns
SymbolFunction._emitInstruction = _emitInstruction

if __name__ == "__main__":
    sys.argv = ["splat", "split", *sys.argv[1:]]
    splat_main()
