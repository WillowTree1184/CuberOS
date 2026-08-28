// src/CoafLinker/src/CoafWriter.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <fstream>

namespace Linker
{

    static U64 AlignUp(U64 value, U64 align)
    {
        if (align == 0)
            return value;
        return (value + align - 1) & ~(align - 1);
    }

    static U64 AppendData(std::vector<uint8_t> &buf, const void *data, size_t size)
    {
        U64 offset = buf.size();
        if (size > 0)
        {
            buf.resize(offset + size);
            std::memcpy(buf.data() + offset, data, size);
        }
        return offset;
    }

    static U64 AddString(Context &ctx, const std::string &str)
    {
        auto it = ctx.StringOffsets.find(str);
        if (it != ctx.StringOffsets.end())
        {
            return it->second;
        }
        U64 offset = ctx.StringTable.size();
        ctx.StringTable.insert(ctx.StringTable.end(), str.begin(), str.end());
        ctx.StringTable.push_back(0);
        ctx.StringOffsets[str] = offset;
        return offset;
    }

    // Try to find the ELF symbol that corresponds to the user-specified entry name.
    // Handles both C linkage (exact match) and simple Itanium C++ mangling
    // for a zero-argument function: _Z<N><name>v
    static std::string ResolveEntryElfName(const Context &ctx)
    {
        const std::string &want = ctx.EntrySymbolName;

        // 1. Exact match (C linkage or already mangled)
        if (ctx.SymbolMap.count(want))
        {
            return want;
        }

        // 2. Simple Itanium mangling for void(void): _Z<N><name>v
        //    e.g. "main" -> "_Z4mainv"
        std::string mangled = "_Z" + std::to_string(want.size()) + want + "v";
        if (ctx.SymbolMap.count(mangled))
        {
            return mangled;
        }

        return "";
    }

    bool WriteCoaf(Context &ctx)
    {
        // Determine architecture from first input file
        if (!ctx.InputFiles.empty())
        {
            switch (ctx.InputFiles[0].ElfHdr.Machine)
            {
            case Elf::EmX86_64:
                ctx.TargetArchId = Coaf::ArchX86_64;
                break;
            case Elf::EmAarch64:
                ctx.TargetArchId = Coaf::ArchAArch64;
                break;
            case Elf::EmRiscv:
                ctx.TargetArchId = Coaf::ArchRiscv64;
                break;
            default:
                std::cerr << "Error: unsupported architecture" << std::endl;
                return false;
            }
        }
        else
        {
            ctx.TargetArchId = Coaf::ArchX86_64;
        }

        // Resolve entry symbol
        std::string entryElfName = ResolveEntryElfName(ctx);
        bool hasEntry = !entryElfName.empty();

        // Collect exported symbols
        struct ExportEntry
        {
            std::string Name;
            U64 ImageOffset;
        };
        std::vector<ExportEntry> exports;

        for (const auto &symPair : ctx.SymbolMap)
        {
            if (!symPair.second.IsExported)
                continue;

            std::string exportName = symPair.first;

            // If this is the entry symbol, rename it to "Main" per COAF spec
            if (hasEntry && symPair.first == entryElfName)
            {
                exportName = "Main";
            }

            exports.push_back({exportName, symPair.second.ImageOffset});
        }

        if (ctx.HasInitArray)
        {
            exports.push_back({"__init_array_start", ctx.InitArrayStart});
            exports.push_back({"__init_array_end", ctx.InitArrayEnd});
        }

        // Sort by name
        std::sort(exports.begin(), exports.end(),
                  [](const ExportEntry &a, const ExportEntry &b)
                  {
                      return a.Name < b.Name;
                  });

        // Check for duplicates
        for (size_t i = 1; i < exports.size(); ++i)
        {
            if (exports[i].Name == exports[i - 1].Name)
            {
                std::cerr << "Error: duplicate export symbol: "
                          << exports[i].Name << std::endl;
                return false;
            }
        }

        // Build string table and COAF export table
        for (const auto &entry : exports)
        {
            Coaf::ExportSymbol exp;
            exp.NameOffset = AddString(ctx, entry.Name);
            exp.ImageOffset = entry.ImageOffset;
            ctx.CoafExports.push_back(exp);
        }

        // Calculate total image size
        U64 imageSize = 0;
        for (const auto &seg : ctx.Segments)
        {
            U64 end = seg.ImageOffset + seg.MemorySize;
            if (end > imageSize)
                imageSize = end;
        }
        imageSize = AlignUp(imageSize, Coaf::PageSize);

        // Build COAF segment descriptors
        for (const auto &seg : ctx.Segments)
        {
            Coaf::Segment cs;
            cs.ImageOffset = seg.ImageOffset;
            cs.FileOffset = 0;
            cs.FileSize = seg.Data.size();
            cs.MemorySize = seg.MemorySize;
            cs.Permissions = seg.Permissions;
            ctx.CoafSegments.push_back(cs);
        }

        // Fill MainTable
        ctx.MainTable.ArchId = ctx.TargetArchId;
        ctx.MainTable.ImageSize = imageSize;
        ctx.MainTable.SegmentCount = ctx.Segments.size();
        ctx.MainTable.ExportSymbolCount = ctx.CoafExports.size();
        ctx.MainTable.ImportModuleCount = 0;
        ctx.MainTable.ImportSymbolCount = 0;
        ctx.MainTable.RelocationCount = ctx.CoafRelocations.size();
        ctx.MainTable.StringTableSize = ctx.StringTable.size();
        ctx.MainTable.SignatureBlobOffset = 0;

        // === Layout file contents ===
        std::vector<uint8_t> buf;

        // Header at offset 0
        Coaf::Header hdr;
        hdr.Magic = Coaf::MagicImage;
        hdr.Version = 1;
        hdr.MainTableOffset = sizeof(Coaf::Header);
        AppendData(buf, &hdr, sizeof(Coaf::Header));

        // MainTable
        AppendData(buf, &ctx.MainTable, sizeof(Coaf::MainTableV1));

        // Segment table
        U64 segTableOffset = 0;
        if (!ctx.CoafSegments.empty())
        {
            segTableOffset = AlignUp(buf.size(), 8);
            buf.resize(segTableOffset);
            AppendData(buf, ctx.CoafSegments.data(), ctx.CoafSegments.size() * sizeof(Coaf::Segment));
        }

        // Export symbol table
        U64 exportTableOffset = 0;
        if (!ctx.CoafExports.empty())
        {
            exportTableOffset = AlignUp(buf.size(), 8);
            buf.resize(exportTableOffset);
            AppendData(buf, ctx.CoafExports.data(), ctx.CoafExports.size() * sizeof(Coaf::ExportSymbol));
        }

        // Import module table (empty)
        U64 importModuleOffset = 0;

        // Import symbol table (empty)
        U64 importSymbolOffset = 0;

        // Relocation table
        U64 relocTableOffset = 0;
        if (!ctx.CoafRelocations.empty())
        {
            relocTableOffset = AlignUp(buf.size(), 8);
            buf.resize(relocTableOffset);
            for (U64 addr : ctx.CoafRelocations)
            {
                AppendData(buf, &addr, sizeof(U64));
            }
        }

        // String table
        U64 stringTableOffset = 0;
        if (!ctx.StringTable.empty())
        {
            stringTableOffset = AlignUp(buf.size(), 8);
            buf.resize(stringTableOffset);
            AppendData(buf, ctx.StringTable.data(), ctx.StringTable.size());
        }

        // Segment data
        for (size_t i = 0; i < ctx.Segments.size(); ++i)
        {
            if (ctx.Segments[i].Data.empty())
                continue;
            U64 dataOffset = AlignUp(buf.size(), 8);
            buf.resize(dataOffset);
            AppendData(buf, ctx.Segments[i].Data.data(), ctx.Segments[i].Data.size());
            ctx.CoafSegments[i].FileOffset = dataOffset;
        }

        // Patch MainTable offsets
        ctx.MainTable.SegmentTableOffset = segTableOffset;
        ctx.MainTable.ExportSymbolTableOffset = exportTableOffset;
        ctx.MainTable.ImportModuleTableOffset = importModuleOffset;
        ctx.MainTable.ImportSymbolTableOffset = importSymbolOffset;
        ctx.MainTable.RelocationTableOffset = relocTableOffset;
        ctx.MainTable.StringTableOffset = stringTableOffset;

        // Patch export symbol NameOffsets to absolute FileOffsets
        for (auto &exp : ctx.CoafExports)
        {
            exp.NameOffset += stringTableOffset;
        }

        // === Re-serialize with all patches applied ===
        buf.clear();

        // Header
        AppendData(buf, &hdr, sizeof(Coaf::Header));

        // MainTable
        AppendData(buf, &ctx.MainTable, sizeof(Coaf::MainTableV1));

        // Segment table
        if (segTableOffset > 0)
        {
            buf.resize(segTableOffset);
            AppendData(buf, ctx.CoafSegments.data(), ctx.CoafSegments.size() * sizeof(Coaf::Segment));
        }

        // Export symbol table
        if (exportTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < exportTableOffset)
                buf.resize(exportTableOffset);
            AppendData(buf, ctx.CoafExports.data(), ctx.CoafExports.size() * sizeof(Coaf::ExportSymbol));
        }

        // Relocation table
        if (relocTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < relocTableOffset)
                buf.resize(relocTableOffset);
            for (U64 addr : ctx.CoafRelocations)
            {
                AppendData(buf, &addr, sizeof(U64));
            }
        }

        // String table
        if (stringTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < stringTableOffset)
                buf.resize(stringTableOffset);
            AppendData(buf, ctx.StringTable.data(), ctx.StringTable.size());
        }

        // Segment data
        for (size_t i = 0; i < ctx.Segments.size(); ++i)
        {
            if (ctx.Segments[i].Data.empty())
                continue;
            U64 dataOffset = AlignUp(buf.size(), 8);
            buf.resize(dataOffset);
            AppendData(buf, ctx.Segments[i].Data.data(), ctx.Segments[i].Data.size());
        }

        ctx.OutputBuffer = std::move(buf);

        // Write to file
        std::ofstream out(ctx.OutputPath, std::ios::binary);
        if (!out)
        {
            std::cerr << "Error: cannot create output file: " << ctx.OutputPath << std::endl;
            return false;
        }
        out.write(reinterpret_cast<const char *>(ctx.OutputBuffer.data()), ctx.OutputBuffer.size());
        if (!out)
        {
            std::cerr << "Error: failed to write output file" << std::endl;
            return false;
        }

        return true;
    }

} // namespace Linker