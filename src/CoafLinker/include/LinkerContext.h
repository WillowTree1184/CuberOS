// src/CoafLinker/include/LinkerContext.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#pragma once

#include "CoafFormat.h"
#include "ElfTypes.h"

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Linker
{

    using U64 = Coaf::U64;
    using U8 = Elf::U8;
    using U16 = Elf::U16;
    using S64 = Elf::S64;

    struct InputSection
    {
        std::string Name;
        U64 Flags;
        std::vector<uint8_t> Data;
        U64 AddrAlign;
        U64 Type;
        U64 OffsetInFile;
        U64 Size;
        bool IsBss;
        U64 CoafPermissions;
    };

    struct InputSymbol
    {
        std::string Name;
        U64 Value;
        U64 Size;
        U8 Binding;
        U16 SectionIndex;
        size_t InputFileIndex;
    };

    struct InputRelocation
    {
        U64 Offset;
        U64 Type;
        U64 SymbolIndex;
        S64 Addend;
        size_t InputFileIndex;
        size_t SectionIndex;
    };

    struct InputFile
    {
        std::string Path;
        std::vector<uint8_t> RawData;
        Elf::Header ElfHdr;
        std::vector<Elf::SectionHeader> SecHeaders;
        std::vector<InputSection> Sections;
        std::vector<InputSymbol> Symbols;
        std::vector<InputRelocation> Relocations;
        std::string ShStrTab;
        std::string StrTab;
    };

    struct SourceRef
    {
        size_t FileIndex;
        size_t SectionIndex;
        U64 OutputOffset;
    };

    struct OutputSegment
    {
        U64 Permissions;
        U64 ImageOffset;
        std::vector<uint8_t> Data;
        U64 MemorySize;
        std::vector<SourceRef> Sources;
    };

    struct ResolvedSymbol
    {
        std::string Name;
        U64 ImageOffset;
        bool IsExported;
        bool IsLocal;
        size_t DefFileIndex;
        size_t DefSectionIndex;
    };

    struct Context
    {
        std::vector<InputFile> InputFiles;
        std::vector<std::string> LibraryPaths;
        std::vector<std::string> Libraries;
        std::string EntrySymbolName;
        std::string OutputPath;
        U64 TargetArchId;

        std::vector<OutputSegment> Segments;
        std::map<std::string, ResolvedSymbol> SymbolMap;

        std::vector<uint8_t> StringTable;
        std::map<std::string, U64> StringOffsets;

        Coaf::Header CoafHdr;
        Coaf::MainTableV1 MainTable;
        std::vector<Coaf::Segment> CoafSegments;
        std::vector<Coaf::ExportSymbol> CoafExports;
        std::vector<U64> CoafRelocations;

        std::vector<uint8_t> OutputBuffer;

        U64 InitArrayStart;
        U64 InitArrayEnd;
        bool HasInitArray;
    };

} // namespace Linker