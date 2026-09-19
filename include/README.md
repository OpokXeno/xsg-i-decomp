# Project headers

Everything under `include/` except `common.h`, `include_asm.h`, `labels.inc`,
`macro.inc` and this file is **generated** by `tools/header_harvest.py` from
published source (user decision 2026-09-12, docs/header-canon.md). Never edit a
generated header by hand: change the source that declares the name, or record a
decision, and regenerate (`python3 -B tools/header_harvest.py`, then `--check`).

## Where a declaration lives

| The declaration is | Home |
| --- | --- |
| defined by one published TU and needed by another | `include/<unit>/<tu>.h`, generated from the **defining** TU's own `src/<unit>/<tu>.h` or `.c` |
| needed by more than one TU and defined by none | `include/shared.h` |
| needed by only one TU | that TU's `src/<unit>/<tu>.h` or `.c` prelude (tracked source) |
| a prototype or extern data declaration of any generated TU header | also repeated in `include/umbrella/public.h` (below) |

`include/shared.h` is included by every TU. Include another TU's generated
header (`#include "main/xgl_packet.h"`) only for what the umbrella does not
carry: its **types**, and the few declarations the umbrella withholds.
Never include another TU's `src/<unit>/<tu>.h`.

## The umbrella (`include/umbrella/public.h`)

`include/shared.h` includes `umbrella/public.h` at its end. The umbrella
repeats every function prototype and extern data declaration that a generated
`include/<unit>/<tu>.h` publishes, so every TU sees them. It exists because the
layout moves a declaration: while no TU defines `xglPacketGetCurrent` it lives in
`include/shared.h`; once main/tu103 is published in C, the harvest homes it in
`include/main/xgl_packet.h`. Without the umbrella every caller that never
included that header lost the prototype (implicit declarations, toolchain
warnings, audit failures in unrelated TUs: integration batches b09/b10).

The umbrella carries **only ordinary declarations**. C lets a compatible
function or object declaration be repeated, never a typedef, tag or
enumerator, and every type a TU header publishes is also its owner's own
definition, so including whole TU headers everywhere would redefine each
owner's types inside the owner. A declaration is withheld from the umbrella
(and stays in its owner's header only) when repeating it could change what a
published TU compiles:

- a published TU declares the name in another spelling, or defines it with
  another head (a `static` definition included);
- a TU other than the owner defines the same identifier (in main and an overlay,
  or in two overlays, one name is two symbols);
- it names a type that `include/shared.h` alone does not provide.

The first two rules are also what keeps codegen unchanged: no TU sees a
composite type it did not already have, so -G8 small-data placement of an
`extern` object and the argument promotions of a call cannot move. The whole-file
gate stays the proof. `reports/header-harvest.md` ("The umbrella") lists what is
carried and every withheld declaration with its reason.

## Types: one definition per shared type

A struct or union more than one TU reads has one definition, in its owner's
header (or in `include/shared.h` when no TU owns it). `config/header-types.json`
records the owner of each shared type, and `tools/header_types.py restated`
lists the types several TUs still restate (for example `Actor`). A TU that
needs a member the owner's definition does not name yet does not restate the
type: it proposes an additive, size-preserving extension of the owner's body
(`tools/header_types.py propose`), which the reviewer checks and the integrator
applies (`tools/header_types.py apply`) before regenerating the headers.

## Callees that are still assembly

A function no published TU defines in C has no generated header. Its canonical
prototype is recorded in `config/header-callees.json`; a TU that calls it
declares it locally with exactly that text (`header_harvest.py lint` and the
review preflight check `callee_prototype` flag any other spelling).
`header_harvest.py callees` surveys every such local declaration and its
spellings.

Comments never matter to any of these checks: declarations are compared
without comments or whitespace.
