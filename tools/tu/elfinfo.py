#!/usr/bin/env python3
"""Minimal read-only ELF32 LE parser for the TU-migration prototype (stdlib only)."""
import struct
from dataclasses import dataclass, field


@dataclass
class Section:
    index: int
    name: str
    type: int
    flags: int
    addr: int
    offset: int
    size: int
    link: int
    info: int
    align: int
    entsize: int
    name_off: int = 0


@dataclass
class Symbol:
    index: int
    name: str
    value: int
    size: int
    type: int
    bind: int
    other: int
    shndx: int


@dataclass
class Phdr:
    type: int
    offset: int
    vaddr: int
    paddr: int
    filesz: int
    memsz: int
    flags: int
    align: int


class Elf:
    def __init__(self, data: bytes):
        self.data = data
        if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
            raise ValueError("not ELF32 LE")
        (self.e_type, self.e_machine, self.e_version, self.entry, self.phoff, self.shoff,
         self.flags, self.ehsize, self.phentsize, self.phnum, self.shentsize, self.shnum,
         self.shstrndx) = struct.unpack_from("<HHIIIIIHHHHHH", data, 16)
        self.sections = []
        for i in range(self.shnum):
            f = struct.unpack_from("<IIIIIIIIII", data, self.shoff + i * self.shentsize)
            self.sections.append(Section(i, "", f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9], f[0]))
        strtab = self.sections[self.shstrndx]
        for s in self.sections:
            s.name = self._str(strtab.offset, s.name_off)
        self.phdrs = []
        for i in range(self.phnum):
            f = struct.unpack_from("<IIIIIIII", data, self.phoff + i * self.phentsize)
            self.phdrs.append(Phdr(*f))
        self.symbols = []
        for s in self.sections:
            if s.type == 2:  # SHT_SYMTAB
                names = self.sections[s.link]
                for i in range(s.size // 16):
                    n, v, sz, info, other, shndx = struct.unpack_from("<IIIBBH", data, s.offset + 16 * i)
                    self.symbols.append(Symbol(i, self._str(names.offset, n), v, sz, info & 15, info >> 4, other, shndx))

    def _str(self, base, off):
        end = self.data.index(b"\0", base + off)
        return self.data[base + off:end].decode("latin-1")

    def section(self, name):
        return next(s for s in self.sections if s.name == name)

    def section_bytes(self, s):
        return b"" if s.type == 8 else self.data[s.offset:s.offset + s.size]

    @property
    def gp(self):
        for s in self.symbols:
            if s.name == "_gp":
                return s.value
        return None
