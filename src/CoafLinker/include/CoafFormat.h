// src/CoafLinker/include/CoafFormat.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#pragma once

#include <cstdint>

namespace Coaf
{

    using U64 = uint64_t;

    constexpr U64 MagicImage = 0x00474D4946414F43ULL;
    constexpr U64 MagicPackage = 0x00474B5046414F43ULL;
    constexpr U64 PageSize = 4096ULL;

    constexpr U64 PermRead = 1ULL;
    constexpr U64 PermWrite = 2ULL;
    constexpr U64 PermExec = 4ULL;

    constexpr U64 ArchX86_64 = 1ULL;
    constexpr U64 ArchAArch64 = 2ULL;
    constexpr U64 ArchRiscv64 = 3ULL;
    constexpr U64 ArchLoongArch64 = 4ULL;

    struct Header
    {
        U64 Magic;
        U64 Version;
        U64 MainTableOffset;
    };

    struct MainTableV1
    {
        U64 ArchId;
        U64 ImageSize;
        U64 SegmentTableOffset;
        U64 SegmentCount;
        U64 ExportSymbolTableOffset;
        U64 ExportSymbolCount;
        U64 ImportModuleTableOffset;
        U64 ImportModuleCount;
        U64 ImportSymbolTableOffset;
        U64 ImportSymbolCount;
        U64 RelocationTableOffset;
        U64 RelocationCount;
        U64 StringTableOffset;
        U64 StringTableSize;
        U64 SignatureBlobOffset;
    };

    struct Segment
    {
        U64 ImageOffset;
        U64 FileOffset;
        U64 FileSize;
        U64 MemorySize;
        U64 Permissions;
    };

    struct ExportSymbol
    {
        U64 NameOffset;
        U64 ImageOffset;
    };

    struct ImportModule
    {
        U64 NameOffset;
    };

    struct ImportSymbol
    {
        U64 ModuleIndex;
        U64 NameOffset;
        U64 CellImageOffset;
    };

    struct PackageHeader
    {
        U64 Magic;
        U64 Version;
        U64 EntryCount;
    };

    struct PackageEntry
    {
        U64 ArchId;
        U64 FileOffset;
        U64 Size;
    };

} // namespace Coaf