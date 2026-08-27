// src/CoafLinker/include/ElfTypes.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#pragma once

#include <cstdint>

namespace Elf
{

    using U8 = uint8_t;
    using U16 = uint16_t;
    using U32 = uint32_t;
    using U64 = uint64_t;
    using S32 = int32_t;
    using S64 = int64_t;

    constexpr U8 Mag0 = 0x7F;
    constexpr U8 Mag1 = 'E';
    constexpr U8 Mag2 = 'L';
    constexpr U8 Mag3 = 'F';
    constexpr U8 Class64 = 2;
    constexpr U8 Data2LSB = 1;
    constexpr U8 EvCurrent = 1;
    constexpr U16 EtRel = 1;
    constexpr U16 EtDyn = 3;
    constexpr U16 EmX86_64 = 62;
    constexpr U16 EmAarch64 = 183;
    constexpr U16 EmRiscv = 243;

    struct Header
    {
        U8 Ident[16];
        U16 Type;
        U16 Machine;
        U32 Version;
        U64 Entry;
        U64 Phoff;
        U64 Shoff;
        U32 Flags;
        U16 Ehsize;
        U16 Phentsize;
        U16 Phnum;
        U16 Shentsize;
        U16 Shnum;
        U16 Shstrndx;
    };

    struct SectionHeader
    {
        U32 Name;
        U32 Type;
        U64 Flags;
        U64 Addr;
        U64 Offset;
        U64 Size;
        U32 Link;
        U32 Info;
        U64 Addralign;
        U64 Entsize;
    };

    struct Symbol
    {
        U32 Name;
        U8 Info;
        U8 Other;
        U16 Shndx;
        U64 Value;
        U64 Size;
    };

    struct Rel
    {
        U64 Offset;
        U64 Info;
    };

    struct Rela
    {
        U64 Offset;
        U64 Info;
        S64 Addend;
    };

    constexpr U32 ShtNull = 0;
    constexpr U32 ShtProgbits = 1;
    constexpr U32 ShtSymtab = 2;
    constexpr U32 ShtStrtab = 3;
    constexpr U32 ShtRela = 4;
    constexpr U32 ShtNoBits = 8;
    constexpr U32 ShtRel = 9;
    constexpr U32 ShtDynSym = 11;
    constexpr U32 ShtInitArray = 14;
    constexpr U32 ShtFiniArray = 15;

    constexpr U64 ShfWrite = 0x1;
    constexpr U64 ShfAlloc = 0x2;
    constexpr U64 ShfExecInstr = 0x4;

    constexpr U8 StbLocal = 0;
    constexpr U8 StbGlobal = 1;
    constexpr U8 StbWeak = 2;

    constexpr U32 SttNotype = 0;
    constexpr U32 SttObject = 1;
    constexpr U32 SttFunc = 2;
    constexpr U32 SttSection = 3;
    constexpr U32 SttFile = 4;

    constexpr U32 R_X86_64_NONE = 0;
    constexpr U32 R_X86_64_64 = 1;
    constexpr U32 R_X86_64_PC32 = 2;
    constexpr U32 R_X86_64_GOT32 = 3;
    constexpr U32 R_X86_64_PLT32 = 4;
    constexpr U32 R_X86_64_COPY = 5;
    constexpr U32 R_X86_64_GLOB_DAT = 6;
    constexpr U32 R_X86_64_JUMP_SLOT = 7;
    constexpr U32 R_X86_64_RELATIVE = 8;
    constexpr U32 R_X86_64_GOTPCREL = 9;
    constexpr U32 R_X86_64_32 = 10;
    constexpr U32 R_X86_64_32S = 11;
    constexpr U32 R_X86_64_16 = 12;
    constexpr U32 R_X86_64_PC16 = 13;
    constexpr U32 R_X86_64_8 = 14;
    constexpr U32 R_X86_64_PC8 = 15;

    inline U32 GetRelType(U64 info) { return static_cast<U32>(info); }
    inline U32 GetRelSym(U64 info) { return static_cast<U32>(info >> 32); }

} // namespace Elf