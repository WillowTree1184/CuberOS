// src/Headers/Coaf/V1/Structure.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on COAF Specification V1 (docs/CoafSpecification/V1.md). Independent implementation.

#pragma once

#include "../Types.hpp"

namespace coaf::v1
{
    // Main Table
    enum class ArchId : U64
    {
        x86_64 = 1ULL,
        AArch64 = 2ULL,
        RiscV64 = 3ULL,
        LoongArch64 = 4ULL
    };

    struct MainTable
    {
        ArchId ArchId;          /* 见 6.3 */
        U64 ImageSize;          /* 映像内存总字节数；约束见 7.1 */
        U64 SegmentTableOffset; /* FileOffset（见 4.1），见 7.1 */
        U64 SegmentCount;
        U64 ExportSymbolTableOffset; /* 见 9.1 */
        U64 ExportSymbolCount;
        U64 ImportModuleTableOffset; /* 见 9.2 */
        U64 ImportModuleCount;
        U64 ImportSymbolTableOffset; /* 见 9.3 */
        U64 ImportSymbolCount;
        U64 RelocationTableOffset; /* 见 9.4 */
        U64 RelocationCount;
        U64 StringTableOffset;   /* 见 8 */
        U64 StringTableSize;     /* 字节数 */
        U64 SignatureBlobOffset; /* FileOffset（见 4.1）；0 表示无签名，见 12 */
    };
    static_assert(sizeof(MainTable) == 120);

    // Segment
    constexpr inline U64 PageSize = 4096ULL;

    enum class Permission : U64
    {
        Read = 1ULL,
        Write = 2ULL,
        Execute = 4ULL
    };

    constexpr Permission operator|(Permission former, Permission latter) noexcept
    {
        return static_cast<Permission>(static_cast<U64>(former) | static_cast<U64>(latter));
    }

    constexpr Permission operator&(Permission former, Permission latter) noexcept
    {
        return static_cast<Permission>(static_cast<U64>(former) & static_cast<U64>(latter));
    }

    constexpr Permission &operator|=(Permission &source, Permission target) noexcept
    {
        source = source | target;
        return source;
    }

    constexpr bool Any(Permission flags, Permission mask) noexcept
    {
        return (static_cast<U64>(flags) & static_cast<U64>(mask)) != 0;
    }

    struct Segment
    {
        U64 ImageOffset;        /* 见 4.2；段在映像中的起始 */
        U64 FileOffset;         /* 见 4.1；段内容在文件中的起始 */
        U64 FileSize;           /* 段内容在文件中的字节数 */
        U64 MemorySize;         /* 段在内存中的字节数 */
        Permission Permissions; /* 见 7.2 */
    };
    static_assert(sizeof(Segment) == 40);

    // ExportSymbol
    struct ExportSymbol
    {
        U64 NameOffset;  /* FileOffset（见 8）；导出名称 */
        U64 ImageOffset; /* 见 4.2；符号所指位置 */
    };
    static_assert(sizeof(ExportSymbol) == 16);

    // ImportModule
    struct ImportModule
    {
        U64 NameOffset;
    };
    static_assert(sizeof(ImportModule) == 8);

    // ImportSymbol
    struct ImportSymbol
    {
        U64 ModuleIndex;     /* 导入模块表（见 9.2）的下标 */
        U64 NameOffset;      /* FileOffset（见 8）；导入名称 */
        U64 CellImageOffset; /* 见 4.2；导入单元的位置 */
    };
    static_assert(sizeof(ImportSymbol) == 24);

    // Multi-Arch Package
    struct PackageHeader
    {
        U64 Magic;      /* 包魔数 CoafMagicPackage（见 5） */
        U64 Version;    /* 包结构版本；规则同 6.1，自 1 起 */
        U64 EntryCount; /* CoafPackageEntry 项数 */
    };
    static_assert(sizeof(PackageHeader) == 24);

    struct PackageEntry
    {
        U64 ArchId;     /* 见 6.3 */
        U64 FileOffset; /* 见 4.1；内嵌映像的起始 */
        U64 Size;       /* 内嵌映像的字节数 */
    };
    static_assert(sizeof(PackageEntry) == 24);

    // Signature
    enum class SignatureAlgorithm : U64
    {
        Ed25519 = 1ULL
    };

    struct SignatureBlob
    {
        U64 SignatureAlgorithm; /* 签名算法注册表的值，见下注 */
        U64 SignatureSize;      /* Signature 的字节数 */
        uint8_t KeyId[32];      /* 公钥标识，见下注 */
        uint8_t Signature[];    /* 签名值 */
    };
    static_assert(sizeof(SignatureBlob) == 48);
} // namespace coaf::v1