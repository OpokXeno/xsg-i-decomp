#!/bin/bash
# cc_tu.sh <in.c> <out.o> <compiler -B dir> <assembler> <-G0|-G8> <unit-dir> <root>
#
# One original C TU under its own contract: the tools/match.py pipeline
# (cpp -> cc1 -> as) with the same argv shapes.  The INCLUDE_ASM bodies are
# `.include`d by the assembler relative to the unit directory (cwd, -I .), and
# the tracked ACCEPTED_ASM sources relative to the repository root (-I <root>).
# Every step: GNU timeout 30 s, own process group, minimal environment.
# Any assembler or compiler message fails the edge (config/toolchains-tu.json
# include_asm.rules): the message is printed and the object is removed.
# A failing step also prints one `cc_tu.sh:` line naming the step, its exit
# status and whether the 30 s timeout killed it (exit 124/137), so a silent
# failure (a timeout under host load, a killed process) is never mistaken for a
# source error by the log reader (tools/tu_publish.py build attribution).
set -euo pipefail
IN="$1"; OUT="$2"; CCDIR="$3"; AS="$4"; G="$5"; UNIT="$6"; ROOT="$7"
mkdir -p "$(dirname "$OUT")"
ENV=(env -i PATH=/usr/bin:/bin LC_ALL=C)
T=(timeout -k 5 30)
step_failed() {    # <step> <exit status>
    if [ "$2" -eq 124 ] || [ "$2" -eq 137 ]; then
        echo "cc_tu.sh: $1 timed out after 30 s (exit $2): $IN" >&2
    else
        echo "cc_tu.sh: $1 failed (exit $2): $IN" >&2
    fi
}
rc=0
# XENO_TU_DEPFILE (set by the ninja `cc` rule, tools/tu/ninja_ovl.py): also write the
# header dependencies there, target rewritten to OUT, for ninja's `deps = gcc`. The
# preprocessed text is identical with or without it.
DEP=()
if [ -n "${XENO_TU_DEPFILE:-}" ]; then DEP=("-Wp,-MD,$XENO_TU_DEPFILE.raw"); fi
"${T[@]}" "${ENV[@]}" "$CCDIR/ee-gcc" -B"$CCDIR/" -nostdinc -fno-builtin -E -O2 "$G" \
    -I include -I "$ROOT/include" "$IN" -o "${OUT%.o}.i" "${DEP[@]}" || rc=$?
if [ "$rc" -ne 0 ]; then step_failed "cpp (ee-gcc -E)" "$rc"; exit "$rc"; fi
if [ -n "${XENO_TU_DEPFILE:-}" ]; then
    sed -e "1s|^[^:]*:|$OUT:|" "$XENO_TU_DEPFILE.raw" > "$XENO_TU_DEPFILE" && rm -f "$XENO_TU_DEPFILE.raw"
fi
"${T[@]}" "${ENV[@]}" "$CCDIR/ee-gcc" -B"$CCDIR/" -nostdinc -fno-builtin -S -O2 "$G" \
    "${OUT%.o}.i" -o "${OUT%.o}.s" || rc=$?
if [ "$rc" -ne 0 ]; then step_failed "cc1 (ee-gcc -S)" "$rc"; exit "$rc"; fi
( "${T[@]}" "${ENV[@]}" "$AS" -EL -m5900 -mabi=eabi "$G" -I . -I "$UNIT" -I "$ROOT" \
        -o "$OUT" "${OUT%.o}.s" > "$OUT.asmsg" 2>&1 ) || rc=$?
if [ "$rc" -ne 0 ] || [ -s "$OUT.asmsg" ]; then
    cat "$OUT.asmsg" 2>/dev/null
    if [ "$rc" -ne 0 ]; then step_failed "assembler" "$rc"; fi
    rm -f "$OUT"
    exit 1
fi
