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
set -euo pipefail
IN="$1"; OUT="$2"; CCDIR="$3"; AS="$4"; G="$5"; UNIT="$6"; ROOT="$7"
mkdir -p "$(dirname "$OUT")"
ENV=(env -i PATH=/usr/bin:/bin LC_ALL=C)
T=(timeout -k 5 30)
"${T[@]}" "${ENV[@]}" "$CCDIR/ee-gcc" -B"$CCDIR/" -nostdinc -fno-builtin -E -O2 "$G" \
    -I include -I "$ROOT/include" "$IN" -o "${OUT%.o}.i"
"${T[@]}" "${ENV[@]}" "$CCDIR/ee-gcc" -B"$CCDIR/" -nostdinc -fno-builtin -S -O2 "$G" \
    "${OUT%.o}.i" -o "${OUT%.o}.s"
if ! ( "${T[@]}" "${ENV[@]}" "$AS" -EL -m5900 -mabi=eabi "$G" -I . -I "$UNIT" -I "$ROOT" \
        -o "$OUT" "${OUT%.o}.s" > "$OUT.asmsg" 2>&1 ) || [ -s "$OUT.asmsg" ]; then
    cat "$OUT.asmsg" 2>/dev/null
    rm -f "$OUT"
    exit 1
fi
