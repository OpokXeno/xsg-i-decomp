#!/usr/bin/env python3
"""Source-driven SSD/RSSD assembly/link and complete IRX reconstruction.

Unclaimed instructions are generated as ordinary MIPS-I assembler. Loader tables,
other original data and container/debug/symbol metadata remain uncredited
scaffolding. Claimed instructions retain the compiler's assembly, with bijective
namespaces for private labels, assembled in the complete original module context.
No original-file overlay,
emitted-object slicing, byte patch, relocation mask or normalization is used.
"""

import argparse
from bisect import bisect_right
from collections import defaultdict
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools import iop_compare, iop_inventory, iop_prepare


def require(condition, message):
    if not condition:
        raise ValueError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def integer(value):
    return int(value, 0) if isinstance(value, str) else int(value)


def string_at(data, offset):
    require(0 <= offset < len(data), "string offset outside table")
    end = data.find(b"\0", offset)
    require(end >= 0, "unterminated string")
    return data[offset:end].decode("ascii")


class Elf:
    """Small strict ELF32 reader; does not treat SCE IRX as an EE executable."""

    def __init__(self, path):
        self.path = Path(path).resolve()
        self.raw = self.path.read_bytes()
        require(52 <= len(self.raw) <= 4 * 1024 * 1024, "ELF size outside bound")
        self.header = struct.unpack_from("<16sHHIIIIIHHHHHH", self.raw)
        h = self.header
        require(h[0][:7] == b"\x7fELF\x01\x01\x01" and h[2] == 8,
                "not a little-endian ELF32 MIPS file")
        require(h[11] == 40 and h[6] + h[12] * 40 <= len(self.raw),
                "invalid ELF section table")
        self.sections = []
        for index in range(h[12]):
            values = struct.unpack_from("<10I", self.raw, h[6] + index * 40)
            item = dict(zip(("name_offset", "type", "flags", "address", "offset",
                             "size", "link", "info", "alignment", "entry_size"), values))
            item.update(index=index, raw_header=values)
            require(item["type"] == 8 or item["offset"] + item["size"] <= len(self.raw),
                    "section outside ELF")
            self.sections.append(item)
        require(0 <= h[13] < len(self.sections), "invalid section-name table")
        names = self.payload(self.sections[h[13]])
        for item in self.sections:
            item["name"] = string_at(names, item["name_offset"])
        self.by_name = {s["name"]: s for s in self.sections}
        self.symbols = []
        self.relocations = []
        for section in self.sections:
            if section["type"] == 2:
                require(section["entry_size"] == 16, "invalid symbol width")
                names = self.payload(self.sections[section["link"]])
                for index, values in enumerate(struct.iter_unpack("<IIIBBH", self.payload(section))):
                    name, value, size, info, other, shndx = values
                    self.symbols.append(dict(index=index, name=string_at(names, name),
                                             value=value, size=size, type=info & 15,
                                             binding=info >> 4, other=other, section_index=shndx))
            elif section["type"] == 9:
                require(section["entry_size"] == 8, "invalid REL width")
                for offset, info in struct.iter_unpack("<II", self.payload(section)):
                    self.relocations.append(dict(section=section["name"],
                                                 target=self.sections[section["info"]]["name"],
                                                 offset=offset, type=info & 255,
                                                 symbol_index=info >> 8))
            else:
                require(section["type"] != 4, "RELA is outside this IOP contract")

    def payload(self, section):
        if isinstance(section, str):
            section = self.by_name[section]
        if section["type"] == 8:
            return b""
        return self.raw[section["offset"]:section["offset"] + section["size"]]

    def named_symbol(self, name):
        matches = [s for s in self.symbols if s["name"] == name and s["section_index"]]
        require(len(matches) == 1, f"expected one defined symbol {name}")
        return matches[0]


def run(command, directory, log):
    command = [str(v) for v in command]
    with Path(log).open("x", encoding="utf-8") as stream:
        stream.write(json.dumps(command) + "\n")
        stream.flush()
        process = subprocess.Popen(command, cwd=directory, stdout=stream,
                                   stderr=subprocess.STDOUT, start_new_session=True,
                                   env=dict(os.environ, LC_ALL="C", LANG="C"))
        try:
            code = process.wait(timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            raise ValueError(f"30-second process timeout; see {log}") from None
    require(code == 0, f"command exited {code}; see {log}")
    return dict(command=command, log=str(log), returncode=code)


def load_contract(root):
    path = root / "config/iop-toolchains.json"
    contract = json.loads(path.read_text())
    require(contract["schema_version"] == 1 and contract["profile"] == "iop-gcc2_8_1",
            "unsupported IOP toolchain contract")
    require(contract["output_arch"] == "mips:3000" and
            contract["output_format"] == "elf32-littlemips", "incorrect IOP linker target")
    require(contract["assembler_flags"] == ["-EL", "-mips1", "-32", "-G0", "-O2"],
            "unsupported native IOP assembler flags")
    require(contract["linker_flags"] == ["-m", "elf32elmip"], "unsupported IOP linker emulation")
    pins = {p["path"]: p["sha256"] for p in contract["pins"]}
    for name, expected in pins.items():
        tool = (root / name).resolve()
        require(tool.is_relative_to(root / ".work/toolchains") and tool.is_file() and
                sha(tool.read_bytes()) == expected, f"IOP contract pin differs: {name}")
    tools = {}
    for name in ("assembler", "linker", "objdump"):
        spelling = contract["tools"][name]
        require(spelling in pins, f"unhashed IOP module tool {name}")
        tools[name] = (root / spelling).resolve()
    return contract, tools, dict(path=str(path), sha256=sha(path.read_bytes()),
                                 profile=contract["profile"], pins_verified=len(pins))


def section_directive(name):
    return {".text": ".text", ".rodata": ".rdata", ".data": ".data",
            ".bss": '.section .iop_bss_scaffold,"aw",@nobits',
            ".sdata": ".sdata", ".sbss": '.section .iop_sbss_scaffold,"aw",@nobits'}[name]


def compiler_streams(assembly, object_elf=None):
    """Package unchanged compiler lines by section; never rewrite instructions."""
    streams = defaultdict(list)
    current = ".text"
    aliases = {".rdata": ".rodata", ".rodata": ".rodata", ".text": ".text",
               ".data": ".data", ".sdata": ".sdata", ".bss": ".bss", ".sbss": ".sbss"}
    for line in assembly.splitlines():
        stripped = line.strip()
        common = re.fullmatch(r"\.(l?comm)\s+([A-Za-z_.$][A-Za-z_0-9.$]*),\s*(\d+)(?:,\s*\d+)?", stripped)
        if common:
            # COMMON is source-defined zero storage. Package its declaration in
            # the mapped BSS cursor, rather than letting an orphan COMMON block
            # silently extend the original module. Instruction lines are intact.
            kind, name, size = common.groups()
            symbol = object_elf.named_symbol(name)
            require(symbol["size"] == int(size), "COMMON declaration/object size differs")
            alignment = symbol["value"] if symbol["section_index"] == 0xfff2 else 1
            require(alignment > 0 and not alignment & (alignment-1), "invalid COMMON alignment")
            streams[".bss"].extend([f".balign {alignment}",
                                      (".local " if kind == "lcomm" else ".globl ") + name,
                                      f".type {name},@object", name + ":",
                                      f".space {size}", f".size {name},{size}"])
            continue
        match = re.match(r"\.(?:section|pushsection)\s+([^,\s]+)", stripped)
        if stripped in aliases or match:
            name = match.group(1) if match else stripped
            require(name in aliases, f"unsupported compiler section packaging {name}")
            current = aliases[name]
            continue
        require(not re.match(r"\.(?:incbin|org|previous|popsection)\b", stripped),
                "compiler assembly has unsupported layout/binary directive")
        streams[current].append(line)
        end_function = re.fullmatch(r"\.end\s+([A-Za-z_.$][A-Za-z_0-9.$]*)", stripped)
        if end_function:
            # Old GAS .ent/.end size bookkeeping loses the subtraction across
            # an .org fragment. Record the actual emitted end-start expression;
            # never write an original/expected size constant into the symbol.
            name = end_function.group(1)
            streams[current].append(f".size {name},.-{name}")
    return streams


def assembler_metadata(section):
    """Native GAS register-mask bookkeeping, not source-defined storage."""
    return (section["name"] == ".reginfo" and section["type"] == 0x70000006
            and section["size"] == 24)


def namespace_streams(streams, obj, original, index):
    """Give each compiler input its object-local label scope during composition.

    Only unregistered compiler labels and the two zero-size compiler identity
    markers are transported. Original FUNC/OBJECT and other registered local
    names remain intact. The original symbol table is serialized unchanged.
    Strings, comments, directives, opcodes, registers and constants are intact;
    this is symbol alpha-renaming, never a byte/diff-dependent transformation.
    """
    markers = {"gcc2_compiled.", "__gnu_compiled_c"}
    definitions = set()
    for lines in streams.values():
        for line in lines:
            match = re.fullmatch(r"\s*([A-Za-z_.$][A-Za-z_0-9.$]*):\s*(?:#.*)?", line)
            if match:
                definitions.add(match.group(1))
    registered = defaultdict(list)
    emitted = defaultdict(list)
    for symbol in original.symbols:
        registered[symbol["name"]].append(symbol)
    for symbol in obj.symbols:
        emitted[symbol["name"]].append(symbol)
    private = []
    for name in sorted(definitions):
        if name in markers:
            records = registered[name] + emitted[name]
            require(emitted[name] and all(s["binding"] == 0 and s["type"] == 0 and
                                         s["size"] == 0 for s in records),
                    f"compiler identity marker has non-metadata ownership: {name}")
        elif re.fullmatch(r"(?:\$L|\.L)[A-Za-z_0-9.$]*", name):
            if registered[name]:
                continue
            require(all(s["binding"] == 0 and s["type"] == 0 for s in emitted[name]),
                    f"compiler private label has public/data ownership: {name}")
        else:
            continue
        private.append(name)
    mapping = {name: f"_iop_compiler_{index:04d}_{n:04d}"
               for n, name in enumerate(private)}
    all_names = set(registered) | set(emitted) | definitions
    require(not set(mapping.values()).intersection(all_names), "compiler namespace collision")
    # Match quoted strings/comments first so identifier-looking text in .ascii,
    # .file or diagnostic comments cannot be changed by symbol transport.
    tokens = re.compile(r'"(?:\\.|[^"\\])*"|#[^\n]*|/\*.*?\*/|'
                        r'(?<![A-Za-z_0-9.$])[A-Za-z_.$][A-Za-z_0-9.$]*', re.S)
    def transport(text, names):
        return tokens.sub(lambda match: names.get(match.group(0), match.group(0)), text)
    scoped = {}
    sections = []
    inverse = {value: key for key, value in mapping.items()}
    for name, lines in streams.items():
        before = "\n".join(lines)
        after = transport(before, mapping)
        require(transport(after, inverse) == before, "non-bijective compiler namespace transport")
        scoped[name] = after.split("\n")
        sections.append(dict(section=name, input_sha256=sha(before.encode()),
                             composed_sha256=sha(after.encode())))
    return scoped, dict(kind="compiler_private_symbol_namespace", mapping=mapping,
                        metadata_markers=sorted(markers.intersection(mapping)),
                        original_registered_symbols_preserved=True,
                        original_marker_metadata_preserved_in_container=True,
                        inverse_transport_equal=True, sections=sections)


def normalize_candidates(candidates, original):
    normalized = []
    functions = [s for s in original.symbols if s["type"] == 2 and s["size"]]
    all_claims = []
    for index, descriptor in enumerate(candidates):
        item = dict(descriptor)
        item["object"] = Path(item["object"]).resolve()
        item["assembly"] = Path(item["assembly"]).resolve()
        obj = Elf(item["object"])
        require(obj.header[1] == 1 and obj.header[7] in (0, 1),
                "candidate object must be native MIPS-I ET_REL flags=0 or noreorder=1")
        claims = (item["functions"] if "functions" in item else
                  [dict(name=item["function"], offset=item["offset"], size=item["size"])])
        claims = [dict(name=f.get("name", f.get("function")), offset=integer(f["offset"]),
                       size=integer(f["size"])) for f in claims]
        claims.sort(key=lambda f: f["offset"])
        require(claims or item.get("generated_data"), "candidate has no function claims")
        for claim in claims:
            original_symbol = original.named_symbol(claim["name"])
            emitted = obj.named_symbol(claim["name"])
            require(original_symbol["type"] == emitted["type"] == 2 and
                    original_symbol["binding"] == emitted["binding"] and
                    original_symbol["value"] == claim["offset"] and
                    original_symbol["size"] == claim["size"],
                    f"candidate FUNC identity/size/binding mismatch: {claim['name']}")
            if emitted["size"] != claim["size"]:
                # Native GAS can schedule address macros differently once a
                # referenced text symbol is defined in the same module. Only a
                # singleton without generated data may defer size identity to
                # the fresh complete-module link, which checks the actual size.
                require(item.get("module_context_identity") is True and
                        len(claims) == 1 and not item.get("generated_data"),
                        f"candidate standalone FUNC size mismatch: {claim['name']}")
            claim["object_symbol"] = emitted
            claim["original_symbol"] = original_symbol
        defined = {s["name"] for s in obj.symbols if s["section_index"] and s["name"]}
        extra_functions = [s["name"] for s in obj.symbols if s["type"] == 2 and s["size"] and
                           s["name"] not in {f["name"] for f in claims}]
        require(not extra_functions, f"unclaimed compiler functions: {extra_functions}")
        # A data-only source owns no text span.
        start = claims[0]["offset"] if claims else None
        end = max(f["offset"] + f["size"] for f in claims) if claims else None
        object_start = claims[0]["object_symbol"]["value"] if claims else None
        for claim in claims:
            require(claim["offset"] - start == claim["object_symbol"]["value"] - object_start,
                    "candidate function ordering/internal spacing differs from original scope")
        # No original function may be hidden inside a compiler-owned contiguous span.
        claim_names = {f["name"] for f in claims}
        for symbol in functions if claims else ():
            if start <= symbol["value"] < end:
                require(symbol["name"] in claim_names, "compiler text span crosses an unclaimed function")
        streams, namespace = namespace_streams(
            compiler_streams(item["assembly"].read_text(), obj), obj, original, index)
        item.update(functions=claims, object_elf=obj, defined=defined,
                    streams=streams, namespace_transport=namespace,
                    start=start, end=end, object_start=object_start)
        maps = []
        for data in item.get("generated_data", []):
            name = ".rodata" if data["section"] == ".rdata" else data["section"]
            require(name in original.by_name and name != ".text", "invalid generated-data section mapping")
            maps.append(dict(section=name, offset=integer(data["offset"]), size=integer(data["size"])))
        for section in obj.sections:
            if (section["flags"] & 2 and section["size"] and section["name"] != ".text"
                    and not assembler_metadata(section)):
                name = ".rodata" if section["name"] == ".rdata" else section["name"]
                require(any(m["section"] == name for m in maps),
                        f"unmapped source-generated allocation {section['name']}")
        item["data_maps"] = maps
        require(len({m["section"] for m in maps}) == len(maps),
                "one contiguous mapping is required per compiler data section")
        if any(s["section_index"] == 0xfff2 for s in obj.symbols):
            require(any(m["section"] == ".bss" for m in maps), "unmapped compiler COMMON storage")
        normalized.append(item)
        all_claims.extend(claims)
    spans = sorted((f["offset"], f["offset"] + f["size"]) for f in all_claims)
    require(all(a[1] <= b[0] for a, b in zip(spans, spans[1:])), "overlapping candidate functions")
    return normalized


def relocation_expressions(original):
    """Recover source relocation expressions for unclaimed assembly scaffolding."""
    expressions = {}
    pending = defaultdict(list)
    for rel in original.relocations:
        section = original.by_name[rel["target"]]
        offset = rel["offset"] - section["address"]
        word = struct.unpack_from("<I", original.payload(section), offset)[0]
        kind = rel["type"]
        require(rel["symbol_index"] == 0 and kind in (2, 4, 5, 6),
                "unsupported original loader relocation")
        key = (rel["target"], rel["offset"])
        if kind == 2:
            expressions[key] = f"_iop_module_base+0x{word:x}"
        elif kind == 4:
            expressions[key] = f"_iop_module_base+0x{(word & 0x3ffffff) << 2:x}"
        elif kind == 5:
            pending[rel["target"]].append((key, word & 65535))
        else:
            low = word & 65535
            low = low - 65536 if low & 32768 else low
            highs = pending.pop(rel["target"], [])
            for high_key, high in highs:
                address = ((high << 16) + low) & 0xffffffff
                expressions[high_key] = f"%hi(_iop_module_base+0x{address:x})"
            address = (((highs[-1][1] << 16) if highs else 0) + low) & 0xffffffff
            expressions[key] = f"%lo(_iop_module_base+0x{address:x})"
    require(not any(pending.values()), "unpaired original HI16 relocation")
    return expressions


def nobits_padding(lines, offset):
    """Resolve packaged COMMON `.balign` at its known mapped offset.

    The mapping fixes where the storage starts; an alignment directive would
    also raise the container section alignment and round its size past the
    original extent. Explicit zero-fill of the same length keeps both.
    """
    resolved = []
    for line in lines:
        align = re.fullmatch(r"\s*\.balign\s+(\d+)\s*", line)
        space = re.fullmatch(r"\s*\.space\s+(\d+)\s*", line)
        if align:
            pad = -offset % int(align.group(1))
            if pad:
                resolved.append(f".space {pad}")
            offset += pad
            continue
        require(not re.match(r"\s*\.(?:p2align|align|byte|half|word|ascii|asciz)\b", line),
                "unsupported NOBITS compiler stream directive")
        if space:
            offset += int(space.group(1))
        resolved.append(line)
    return resolved


def generate_assembly(original, module, original_bytes, candidates, disassembly):
    instructions = {}
    for line in disassembly.splitlines():
        match = re.match(r"^\s*([0-9a-f]+):\s+(\S+)(?:\s+(.*))?$", line)
        if match:
            instructions[int(match.group(1), 16)] = (match.group(2), (match.group(3) or "").split("#", 1)[0].strip())
    tables = iop_prepare.loader_tables(module, original_bytes)
    table_words = {p for table in tables for p in range(table["module_offset"], table["module_offset"] + table["size"], 4)}
    code_words = {p for f in module["functions"] for p in range(f["module_offset"], f["module_offset"] + f["size"], 4)}
    require(not table_words.intersection(code_words), "loader DATA overlaps an original mapped function")
    expressions = relocation_expressions(original)
    relocated_data_words = {(r["target"], r["offset"]) for r in original.relocations if r["type"] == 2}
    symbols = defaultdict(list)
    candidate_names = set().union(*(c["defined"] for c in candidates)) if candidates else set()
    seen_names = set()
    for symbol in original.symbols:
        if symbol["name"] in candidate_names or symbol["binding"] not in (1, 2):
            continue
        if not (0 < symbol["section_index"] < len(original.sections)):
            continue
        name = symbol["name"]
        require(not name or re.fullmatch(r"[A-Za-z_.$][A-Za-z_0-9.$]*", name), "unsafe original assembler symbol")
        if name and name not in seen_names:
            symbols[(original.sections[symbol["section_index"]]["name"], symbol["value"])].append(symbol)
            seen_names.add(name)
    source_header = ["/* Original scaffold plus compiler streams with private symbol namespaces. */",
                     ".set noreorder", ".set noat", ".globl _iop_module_base"]
    provenance = []
    # Each compiler stream keeps its own assembly-time view: the initialized
    # data a source defines (its string/constant labels) precedes that
    # source's code, as in the compiler output, while data owned by later
    # sources, scaffolding and COMMON/BSS storage stay forward references
    # (undefined at their use, as in the original separate assembly). GAS
    # relaxes `la`/symbolic loads and fills delay slots differently for a
    # defined local label than for a forward or external one.
    section_lines, block_marks = {}, {}
    for name in (".text", ".rodata", ".data", ".sdata", ".sbss", ".bss"):
        if name not in original.by_name:
            continue
        section = original.by_name[name]
        source = section_lines[name] = []
        block_marks[name] = {}
        source.extend([section_directive(name), ".set noreorder", ".set noat"])
        if name == ".text":
            source.append("_iop_module_base:")
        blocks = []
        for c in candidates:
            if name == ".text":
                if c["functions"]:
                    blocks.append((c["start"], c["end"], c["streams"].get(name, []), c))
            else:
                for mapping in c["data_maps"]:
                    if mapping["section"] == name:
                        blocks.append((mapping["offset"], mapping["offset"] + mapping["size"], c["streams"].get(name, []), c))
        blocks.sort(key=lambda b: b[0])
        require(all(a[1] <= b[0] for a, b in zip(blocks, blocks[1:])), "overlapping compiler section mappings")
        by_start = {block[0]: block for block in blocks}
        position, end = section["address"], section["address"] + section["size"]
        boundaries = sorted(set(by_start) | {end} |
                            {address for section_name, address in symbols
                             if section_name == name and section["address"] <= address < end})
        emitted_blocks = set()
        payload = original.payload(section)
        while position < end:
            if position in by_start:
                start, stop, lines, candidate = by_start[position]
                if name == ".text":
                    block_marks[name][id(candidate)] = len(source)
                require(start >= section["address"] and stop <= end and lines, "empty/outside compiler stream mapping")
                if section["type"] == 8:
                    lines = nobits_padding(lines, start - section["address"])
                source.extend([f".org 0x{position-section['address']:x}",
                               ".set reorder", ".set at", *lines,
                               section_directive(name), ".set noreorder", ".set noat"])
                provenance.append(dict(section=name, start=start, size=stop-start,
                                       origin="compiler_assembly", assembly=str(candidate["assembly"]),
                                       assembly_sha256=sha(candidate["assembly"].read_bytes()),
                                       namespace_transport=candidate["namespace_transport"]))
                emitted_blocks.add(start)
                if name != ".text":
                    block_marks[name][id(candidate)] = len(source)
                position = stop
                continue
            for symbol in symbols.get((name, position), []):
                source.extend([(".weak " if symbol["binding"] == 2 else ".globl ") + symbol["name"], symbol["name"] + ":"])
            source.append(f"_iop_addr_{position:08x}:")
            delta = position - section["address"]
            # Compiler string/data extents need not end on a word boundary.
            # Never let a scaffold chunk cross the next mapped input or symbol.
            next_boundary = boundaries[bisect_right(boundaries, position)]
            count = min(4 - position % 4, end-position, next_boundary-position)
            if section["type"] == 8:
                source.append(f".space {count}")
                position += count
                continue
            expression = expressions.get((name, position))
            # Unmapped executable gaps contain real padding instructions (and a
            # return leaf), not opaque code bytes. Only positively decoded loader
            # tables and explicit R_MIPS_32 pointer data use the DATA route.
            if (name == ".text" and position not in table_words and
                    (name, position) not in relocated_data_words):
                require(count == 4, "instruction scaffold crosses an unaligned boundary")
                require(position in instructions, f"missing instruction at 0x{position:x}")
                opcode, operands = instructions[position]
                require(opcode not in (".word", ".byte", "...", ".4byte"), f"unsupported decoded instruction at 0x{position:x}")
                if expression:
                    if expression.startswith("%hi"):
                        operands = operands.rsplit(",", 1)[0] + "," + expression
                    elif expression.startswith("%lo"):
                        operands = re.sub(r"[^,()]+(?=\([^()]+\)$)", expression, operands) if "(" in operands else operands.rsplit(",", 1)[0] + "," + expression
                    else:
                        operands = expression
                else:
                    operands = re.sub(r"(?:0x)?([0-9a-f]+)\s+<[^>]+>", lambda m: f"_iop_addr_{int(m.group(1),16):08x}", operands)
                source.append(opcode + (" " + operands if operands else ""))
                origin = "original_instruction_assembly"
            elif expression:
                require(count == 4, "relocated data scaffold crosses an unaligned boundary")
                require(not expression.startswith("%"), "instruction relocation in loader/data scaffolding")
                source.append(".word " + expression)
                origin = "original_relocated_data_scaffold"
            else:
                source.append(".byte " + ",".join(str(v) for v in payload[delta:delta+count]))
                origin = "original_loader_data_scaffold" if position in table_words else "original_data_scaffold"
            provenance.append(dict(section=name, start=position, size=count, origin=origin))
            position += count
        require(emitted_blocks == set(by_start), "compiler section mapping was not emitted")
        source.append(f".org 0x{section['size']:x}")
    header = source_header
    source = list(header)
    interleaved = [n for n in (".rodata", ".data", ".sdata") if n in section_lines]
    cursor = {n: 0 for n in interleaved}
    text = section_lines.get(".text", [])
    text_marks = sorted((index, key) for key, index in block_marks.get(".text", {}).items())
    previous = 0
    for index, key in text_marks:
        source.extend(text[previous:index])
        previous = index
        for n in interleaved:
            stop = block_marks[n].get(key)
            if stop is not None and stop > cursor[n]:
                chunk = section_lines[n][cursor[n]:stop]
                if cursor[n]:
                    chunk = [section_directive(n), ".set noreorder", ".set noat", *chunk]
                source.extend([*chunk, ".text", ".set noreorder", ".set noat"])
                cursor[n] = stop
    source.extend(text[previous:])
    for n in (".rodata", ".data", ".sdata", ".sbss", ".bss"):
        if n not in section_lines:
            continue
        rest = section_lines[n][cursor.get(n, 0):]
        if cursor.get(n, 0):
            rest = [section_directive(n), ".set noreorder", ".set noat", *rest]
        source.extend(rest)
    return "\n".join(source) + "\n", provenance, tables


def link_script(original):
    lines = ['OUTPUT_FORMAT("elf32-littlemips")', 'OUTPUT_ARCH(mips:3000)', "SECTIONS", "{"]
    for name in (".text", ".rodata", ".data", ".sdata", ".sbss", ".bss"):
        if name in original.by_name:
            section = original.by_name[name]
            inputs = {".bss": ".bss .iop_bss_scaffold", ".sbss": ".sbss .iop_sbss_scaffold"}.get(name, name)
            inputs += " .rdata" if name == ".rodata" else ""
            lines.append(f" {name} 0x{section['address']:x} : ALIGN({section['alignment'] or 1}) {{ *({inputs}) }}")
    # IRX GP is part of the original module ABI, not an inferred default.
    module = original.by_name[".iopmod"]
    gp = struct.unpack_from("<I", original.payload(module), 8)[0]
    lines.extend([f" _gp = 0x{gp:x};", "}"])
    return "\n".join(lines) + "\n"


def irx_relocations(linked, target):
    """Emit loader fixup groups at their first instruction's module offset.

    Native GAS keeps a HI16 pending until its LO16 appears. A call between the
    two instructions can therefore precede the pair in the object fixup stream.
    IRX emission orders the paired address fixup at the HI instruction, keeping
    HI/LO members together. This is a linker emission rule on actual fixups; no
    original record ordering, sorting during comparison or equality mask enters.
    """
    records = [r for r in linked.relocations if r["target"] == target]
    groups, pending = [], []
    for record in records:
        if record["type"] == 5:
            pending.append(record)
        elif record["type"] == 6 and pending:
            require(all(r["symbol_index"] == record["symbol_index"] for r in pending),
                    "HI16/LO16 fixup symbol mismatch")
            groups.append(pending + [record])
            pending = []
        else:
            groups.append([record])
    require(not pending, "linked HI16 has no LO16")
    groups.sort(key=lambda group: min(r["offset"] for r in group))
    return [record for group in groups for record in group]


def function_bytes(image, symbol):
    """Read the actual bounded FUNC payload, without changing its representation."""
    index = symbol['section_index']
    require(symbol['type'] == 2 and 0 < index < len(image.sections), 'invalid IOP FUNC section')
    section = image.sections[index]
    start = symbol['value'] - section['address']
    require(section['type'] != 8 and 0 <= start < start + symbol['size'] <= section['size'],
            'invalid IOP FUNC extent')
    return image.payload(section)[start:start + symbol['size']]


def reporting_comparison(comparison):
    """Allow measured code differences, retaining valid fixed IRX identities."""
    require(comparison.get('valid_irx') is True, 'reporting output is not a valid IRX')
    diagnostics = comparison['diagnostics']
    require(not diagnostics.get('header') and not diagnostics.get('module') and
            diagnostics['symbols']['changed_records'] == 0 and
            diagnostics['relocations']['changed_records'] == 0,
            'reporting IRX metadata/symbol/relocation identities changed')


def serialize(original, linked, candidate_functions):
    """Fresh ELF serialization: linked alloc sections/REL + pinned container data."""
    generated = {}
    for section in original.sections:
        if section["flags"] & 2 and section["type"] in (1, 8):
            require(section["name"] in linked.by_name, f"linked section missing {section['name']}")
            emitted = linked.by_name[section["name"]]
            require(emitted["address"] == section["address"] and emitted["size"] == section["size"],
                    f"linked section extent differs: {section['name']}")
            generated[section["name"]] = linked.payload(emitted)
        elif section["type"] == 9:
            target = original.sections[section["info"]]["name"]
            records = irx_relocations(linked, target)
            require(all(r["type"] in (2, 4, 5, 6) for r in records), "unsupported linked IRX relocation type")
            generated[section["name"]] = b"".join(struct.pack("<II", r["offset"], r["type"]) for r in records)
            require(len(generated[section["name"]]) == section["size"], f"linked relocation count differs: {section['name']}")
    for section in linked.sections:
        if section["flags"] & 2 and section["size"]:
            require(section["name"] in generated or assembler_metadata(section),
                    f"extra linked allocation {section['name']}")
    audits = []
    for candidate in candidate_functions:
        for claim in candidate["functions"]:
            emitted = linked.named_symbol(claim["name"])
            expected = claim["original_symbol"]
            require(all(emitted[key] == expected[key] for key in ("name", "value", "size", "type", "binding", "other")),
                    f"complete-module compiler FUNC identity differs: {claim['name']}")
            audits.append(dict(name=claim["name"], offset=emitted["value"], size=emitted["size"],
                               type=emitted["type"], binding=emitted["binding"],
                               linked_sha256=sha(function_bytes(linked, emitted)),
                               original_sha256=sha(function_bytes(original, expected)),
                               exact=function_bytes(linked, emitted) == function_bytes(original, expected),
                               compiler_object=str(candidate["object"]),
                               compiler_assembly=str(candidate["assembly"]),
                               standalone_object_symbol=claim["object_symbol"],
                               linked_symbol=emitted, original_symbol=expected))
    # Build a new container from an explicit nonoverlapping layout. Bytes outside
    # sections are original uncredited container padding, never executable spans.
    h = original.header
    pieces = [(0, struct.pack("<16sHHIIIIIHHHHHH", *h), "original_header_metadata")]
    if h[10]:
        pieces.append((h[5], original.raw[h[5]:h[5]+h[9]*h[10]], "original_program_header_metadata"))
    for section in original.sections[1:]:
        if section["type"] != 8 and section["size"]:
            pieces.append((section["offset"], generated.get(section["name"], original.payload(section)),
                           "linked_section" if section["name"] in generated else "original_uncredited_metadata"))
    pieces.append((h[6], b"".join(struct.pack("<10I", *s["raw_header"]) for s in original.sections), "original_section_header_metadata"))
    pieces.sort(key=lambda p: p[0])
    out, cursor, manifest = bytearray(), 0, []
    for offset, payload, origin in pieces:
        require(offset >= cursor, "overlapping original ELF container extents")
        if offset > cursor:
            padding = original.raw[cursor:offset]
            out.extend(padding)
            manifest.append(dict(offset=cursor, size=len(padding), origin="original_container_padding", sha256=sha(padding)))
        out.extend(payload)
        manifest.append(dict(offset=offset, size=len(payload), origin=origin, sha256=sha(payload)))
        cursor = offset + len(payload)
    if cursor < len(original.raw):
        out.extend(original.raw[cursor:])
        manifest.append(dict(offset=cursor, size=len(original.raw)-cursor, origin="original_container_tail", sha256=sha(original.raw[cursor:])))
    require(len(out) == len(original.raw), "fresh container size differs")
    return bytes(out), manifest, audits


def build_module(root, unit, candidate_objects, build_dir, output, toolchain=None, *, reporting=False):
    root, build_dir, output = Path(root).resolve(), Path(build_dir).resolve(), Path(output).resolve()
    require(not build_dir.exists() and not output.exists(), "module build/output must be fresh")
    require(build_dir.is_relative_to(root / ".work") or build_dir.is_relative_to(root / "build") or build_dir.is_relative_to(Path("/tmp")), "build must be private")
    require(output.is_relative_to(root / ".work") or output.is_relative_to(root / "build") or output.is_relative_to(Path("/tmp")), "output must be private")
    references = iop_inventory.verify_iop(root)
    require(unit in references["units"], "unknown IOP module")
    module = references["units"][unit]
    original_path = root / "game/extracted-iso" / module["path"]
    original = Elf(original_path)
    candidates = normalize_candidates(candidate_objects, original)
    contract, tools, contract_evidence = load_contract(root)
    for name, override in (toolchain or {}).items():
        require(name in tools and sha(Path(override).resolve().read_bytes()) == sha(tools[name].read_bytes()),
                "module tool override differs from selected contract")
        tools[name] = override
    tools = {k: Path(v).resolve() for k, v in tools.items()}
    require(all(v.is_file() for v in tools.values()), "IOP module tool is missing")
    build_dir.mkdir(parents=True)
    output.parent.mkdir(parents=True, exist_ok=True)
    report = dict(schema_version=1, kind="iop_source_driven_complete_module_gate", unit=unit,
                  ok=False, gate_passed=False, build_passed=False,
                  mode='reporting' if reporting else 'exact',
                  output=str(output), linked_elf=str(build_dir / "linked.elf"),
                  source_acceptance=False, recovery_credit=False,
                  metadata_scaffolding=True, runtime_execution_proven=False,
                  relocation_emission="Actual linked fixups grouped HI16/LO16 and emitted at first instruction offset; complete raw comparison unchanged",
                  original_sha256=sha(original.raw), commands=[],
                  contract=contract_evidence,
                  backend=dict(path=str(Path(__file__).resolve()), sha256=sha(Path(__file__).read_bytes())),
                  compiler_inputs=[dict(object=str(c["object"]), object_sha256=sha(c["object"].read_bytes()),
                                        assembly=str(c["assembly"]), assembly_sha256=sha(c["assembly"].read_bytes()),
                                        functions=[dict(name=f["name"], offset=f["offset"], size=f["size"]) for f in c["functions"]],
                                        generated_data=c["data_maps"],
                                        namespace_transport=c["namespace_transport"]) for c in candidates],
                  toolchain={k: dict(path=str(v), sha256=sha(v.read_bytes())) for k,v in tools.items()})
    try:
        dislog = build_dir / "disassemble.log"
        report["commands"].append(run([tools["objdump"], "-D", "-z", "-j", ".text", "--no-show-raw-insn", "-M", "no-aliases,gpr-names=numeric", original_path], root, dislog))
        assembly, provenance, tables = generate_assembly(original, module, original.raw, candidates, dislog.read_text())
        source = build_dir / "module.s"
        source.write_text(assembly)
        script = build_dir / "module.ld"
        script.write_text(link_script(original))
        obj = build_dir / "module.o"
        report["commands"].append(run([tools["assembler"], *contract["assembler_flags"], "-o", obj, source], root, build_dir / "assemble.log"))
        report["commands"].append(run([tools["linker"], *contract["linker_flags"], "--emit-relocs", "-T", script, "-o", build_dir / "linked.elf", obj], root, build_dir / "link.log"))
        linked = Elf(build_dir / "linked.elf")
        require(linked.header[7] == 1, "linked module is not native MIPS-I flags=1")
        unresolved = [s["name"] for s in linked.symbols if s["name"] and s["section_index"] == 0 and s["binding"] != 2]
        require(not unresolved, f"unresolved complete-module symbols: {unresolved}")
        payload, layout, audits = serialize(original, linked, candidates)
        with output.open("xb") as stream:
            stream.write(payload)
        comparison = iop_compare.compare(unit, output, root)
        if reporting:
            reporting_comparison(comparison)
        report.update(ok=True if reporting else comparison["ok"], gate_passed=comparison["ok"],
                      build_passed=True,
                      comparison=comparison, functions=audits, loader_tables=tables,
                      assembly_provenance=provenance, container_layout=layout,
                      generated_source=dict(path=str(source), sha256=sha(source.read_bytes())),
                      object=dict(path=str(obj), sha256=sha(obj.read_bytes())),
                      linked_relocations=linked.relocations,
                      linked_sha256=sha(linked.raw), output_sha256=sha(payload))
    except (OSError, ValueError, KeyError, struct.error, subprocess.SubprocessError) as exc:
        report["error"] = str(exc)
    (build_dir / "module-report.json").write_text(json.dumps(report, sort_keys=True, indent=2) + "\n")
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--unit", choices=tuple(iop_inventory.UNIT_PATHS), required=True)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--candidates", type=Path, help="JSON list of object/assembly/function/data descriptors")
    args = parser.parse_args(argv)
    try:
        candidates = json.loads(args.candidates.read_text()) if args.candidates else []
        report = build_module(args.root, args.unit, candidates, args.build_dir, args.output)
        print(json.dumps({k: report.get(k) for k in ("ok", "gate_passed", "output", "linked_elf", "error")}))
        return 0 if report["gate_passed"] else 1
    except (OSError, ValueError, KeyError, struct.error, subprocess.SubprocessError) as exc:
        print(json.dumps(dict(ok=False, gate_passed=False, error=str(exc))))
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
