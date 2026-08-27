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

    static U64 AppendU64(std::vector<uint8_t> &buf, U64 value)
    {
        return AppendData(buf, &value, sizeof(U64));
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

        // Build string table
        for (const auto &symPair : ctx.SymbolMap)
        {
            if (symPair.second.IsExported)
            {
                AddString(ctx, symPair.first);
            }
        }
        // Add special symbols if needed
        if (ctx.HasInitArray)
        {
            AddString(ctx, "__init_array_start");
            AddString(ctx, "__init_array_end");
        }

        // Build export symbol table
        for (const auto &symPair : ctx.SymbolMap)
        {
            if (!symPair.second.IsExported)
                continue;
            Coaf::ExportSymbol exp;
            exp.NameOffset = AddString(ctx, symPair.first);
            exp.ImageOffset = symPair.second.ImageOffset;
            ctx.CoafExports.push_back(exp);
        }

        // Add init_array symbols to exports
        if (ctx.HasInitArray)
        {
            Coaf::ExportSymbol startSym;
            startSym.NameOffset = AddString(ctx, "__init_array_start");
            startSym.ImageOffset = ctx.InitArrayStart;
            ctx.CoafExports.push_back(startSym);

            Coaf::ExportSymbol endSym;
            endSym.NameOffset = AddString(ctx, "__init_array_end");
            endSym.ImageOffset = ctx.InitArrayEnd;
            ctx.CoafExports.push_back(endSym);
        }

        // Sort exports by name
        std::sort(ctx.CoafExports.begin(), ctx.CoafExports.end(),
                  [](const Coaf::ExportSymbol &a, const Coaf::ExportSymbol &b)
                  {
                      return a.NameOffset < b.NameOffset;
                  });

        // Actually we need to sort by name bytes, but since we built the string table
        // in arbitrary order, let's sort by the actual string content
        std::sort(ctx.CoafExports.begin(), ctx.CoafExports.end(),
                  [&ctx](const Coaf::ExportSymbol &a, const Coaf::ExportSymbol &b)
                  {
                      const char *sa = reinterpret_cast<const char *>(ctx.StringTable.data() + a.NameOffset);
                      const char *sb = reinterpret_cast<const char *>(ctx.StringTable.data() + b.NameOffset);
                      return std::strcmp(sa, sb) < 0;
                  });

        // Calculate total image size
        U64 imageSize = 0;
        for (const auto &seg : ctx.Segments)
        {
            U64 end = seg.ImageOffset + seg.MemorySize;
            if (end > imageSize)
                imageSize = end;
        }
        imageSize = AlignUp(imageSize, Coaf::PageSize);

        // Build COAF structures
        ctx.MainTable.ArchId = ctx.TargetArchId;
        ctx.MainTable.ImageSize = imageSize;
        ctx.MainTable.SegmentCount = ctx.Segments.size();
        ctx.MainTable.ExportSymbolCount = ctx.CoafExports.size();
        ctx.MainTable.ImportModuleCount = 0;
        ctx.MainTable.ImportSymbolCount = 0;
        ctx.MainTable.RelocationCount = ctx.CoafRelocations.size();
        ctx.MainTable.StringTableSize = ctx.StringTable.size();
        ctx.MainTable.SignatureBlobOffset = 0;

        // Serialize to buffer
        std::vector<uint8_t> buf;

        // Header
        (void)0; // header starts at offset 0
        Coaf::Header hdr;
        hdr.Magic = Coaf::MagicImage;
        hdr.Version = 1;
        hdr.MainTableOffset = sizeof(Coaf::Header);
        AppendData(buf, &hdr, sizeof(Coaf::Header));

        // MainTable
        // MainTable starts at buf.size()
        AppendData(buf, &ctx.MainTable, sizeof(Coaf::MainTableV1));

        // Segment table
        U64 segTableOffset = 0;
        if (!ctx.Segments.empty())
        {
            segTableOffset = AlignUp(buf.size(), 8);
            buf.resize(segTableOffset);
            for (const auto &seg : ctx.Segments)
            {
                Coaf::Segment cs;
                cs.ImageOffset = seg.ImageOffset;
                cs.FileOffset = 0; // Will be patched later
                cs.FileSize = seg.Data.size();
                cs.MemorySize = seg.MemorySize;
                cs.Permissions = seg.Permissions;
                ctx.CoafSegments.push_back(cs);
            }
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
                AppendU64(buf, addr);
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

        // Re-serialize with patched offsets
        buf.clear();
        AppendData(buf, &hdr, sizeof(Coaf::Header));
        AppendData(buf, &ctx.MainTable, sizeof(Coaf::MainTableV1));

        if (segTableOffset > 0)
        {
            buf.resize(segTableOffset);
            AppendData(buf, ctx.CoafSegments.data(), ctx.CoafSegments.size() * sizeof(Coaf::Segment));
        }
        if (exportTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < exportTableOffset)
                buf.resize(exportTableOffset);
            AppendData(buf, ctx.CoafExports.data(), ctx.CoafExports.size() * sizeof(Coaf::ExportSymbol));
        }
        if (relocTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < relocTableOffset)
                buf.resize(relocTableOffset);
            for (U64 addr : ctx.CoafRelocations)
            {
                AppendU64(buf, addr);
            }
        }
        if (stringTableOffset > 0)
        {
            buf.resize(AlignUp(buf.size(), 8));
            if (buf.size() < stringTableOffset)
                buf.resize(stringTableOffset);
            AppendData(buf, ctx.StringTable.data(), ctx.StringTable.size());
        }

        // Segment data again with patched FileOffsets
        for (size_t i = 0; i < ctx.Segments.size(); ++i)
        {
            if (ctx.Segments[i].Data.empty())
                continue;
            U64 dataOffset = AlignUp(buf.size(), 8);
            buf.resize(dataOffset);
            AppendData(buf, ctx.Segments[i].Data.data(), ctx.Segments[i].Data.size());
            ctx.CoafSegments[i].FileOffset = dataOffset;
        }

        // Final patch of segment table with correct FileOffsets
        if (segTableOffset > 0)
        {
            std::memcpy(buf.data() + segTableOffset, ctx.CoafSegments.data(),
                        ctx.CoafSegments.size() * sizeof(Coaf::Segment));
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