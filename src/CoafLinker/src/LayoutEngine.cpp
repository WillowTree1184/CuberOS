// src/CoafLinker/src/LayoutEngine.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <algorithm>
#include <iostream>
#include <cstring>

namespace Linker
{

    static U64 AlignUp(U64 value, U64 align)
    {
        if (align == 0)
            return value;
        return (value + align - 1) & ~(align - 1);
    }

    static bool IsSpecialSection(const std::string &name)
    {
        return name == ".comment" || name == ".note" || name == ".symtab" ||
               name == ".strtab" || name == ".shstrtab" || name == ".rel" ||
               name == ".rela" || name.substr(0, 4) == ".rel" || name.substr(0, 5) == ".rela";
    }

    bool LayoutSegments(Context &ctx)
    {
        struct Group
        {
            U64 Permissions;
            std::vector<std::pair<size_t, size_t>> Sections;
        };

        std::map<U64, Group> groups;

        for (size_t fidx = 0; fidx < ctx.InputFiles.size(); ++fidx)
        {
            const auto &file = ctx.InputFiles[fidx];
            for (size_t sidx = 0; sidx < file.Sections.size(); ++sidx)
            {
                const auto &sec = file.Sections[sidx];
                if (sec.CoafPermissions == 0)
                    continue;
                if (IsSpecialSection(sec.Name))
                    continue;

                auto &g = groups[sec.CoafPermissions];
                g.Permissions = sec.CoafPermissions;
                g.Sections.push_back({fidx, sidx});
            }
        }

        std::vector<U64> permOrder = {
            Coaf::PermRead | Coaf::PermExec,
            Coaf::PermRead,
            Coaf::PermRead | Coaf::PermWrite};

        U64 currentImageOffset = 0;

        for (U64 perm : permOrder)
        {
            auto it = groups.find(perm);
            if (it == groups.end())
                continue;

            const auto &g = it->second;

            std::vector<std::pair<size_t, size_t>> dataSecs;
            std::vector<std::pair<size_t, size_t>> bssSecs;
            for (const auto &p : g.Sections)
            {
                const auto &sec = ctx.InputFiles[p.first].Sections[p.second];
                if (sec.IsBss)
                {
                    bssSecs.push_back(p);
                }
                else
                {
                    dataSecs.push_back(p);
                }
            }

            if (!dataSecs.empty())
            {
                OutputSegment seg;
                seg.Permissions = perm;
                seg.ImageOffset = AlignUp(currentImageOffset, Coaf::PageSize);

                U64 offset = 0;
                for (const auto &p : dataSecs)
                {
                    const auto &sec = ctx.InputFiles[p.first].Sections[p.second];
                    U64 align = sec.AddrAlign;
                    if (align == 0)
                        align = 1;
                    offset = AlignUp(offset, align);

                    SourceRef ref;
                    ref.FileIndex = p.first;
                    ref.SectionIndex = p.second;
                    ref.OutputOffset = offset;
                    seg.Sources.push_back(ref);

                    if (!sec.Data.empty())
                    {
                        if (offset + sec.Data.size() > seg.Data.size())
                        {
                            seg.Data.resize(offset + sec.Data.size(), 0);
                        }
                        std::memcpy(seg.Data.data() + offset, sec.Data.data(), sec.Data.size());
                    }
                    offset += sec.Data.size();
                }

                seg.MemorySize = AlignUp(offset, Coaf::PageSize);
                // Discard empty segments (both file and memory are empty)
                if (seg.MemorySize == 0)
                {
                    continue;
                }
                if (seg.Data.size() < seg.MemorySize)
                {
                    seg.Data.resize(seg.MemorySize, 0);
                }

                currentImageOffset = seg.ImageOffset + seg.MemorySize;
                ctx.Segments.push_back(std::move(seg));
            }

            if (!bssSecs.empty())
            {
                OutputSegment seg;
                seg.Permissions = perm;
                seg.ImageOffset = AlignUp(currentImageOffset, Coaf::PageSize);

                U64 offset = 0;
                for (const auto &p : bssSecs)
                {
                    const auto &sec = ctx.InputFiles[p.first].Sections[p.second];
                    U64 align = sec.AddrAlign;
                    if (align == 0)
                        align = 1;
                    offset = AlignUp(offset, align);

                    SourceRef ref;
                    ref.FileIndex = p.first;
                    ref.SectionIndex = p.second;
                    ref.OutputOffset = offset;
                    seg.Sources.push_back(ref);

                    offset += sec.Size;
                }

                seg.MemorySize = AlignUp(offset, Coaf::PageSize);
                // Discard empty segments
                if (seg.MemorySize == 0)
                {
                    continue;
                }
                seg.Data.clear();

                currentImageOffset = seg.ImageOffset + seg.MemorySize;
                ctx.Segments.push_back(std::move(seg));
            }
        }

        // A valid COAF must have at least one segment
        if (ctx.Segments.empty())
        {
            std::cerr << "Error: module has no loadable content; all segments are empty." << std::endl;
            std::cerr << "       This typically happens with -O2 when the compiler optimizes away" << std::endl;
            std::cerr << "       all code. Add side effects (e.g., asm volatile) or use -O0 for testing." << std::endl;
            return false;
        }

        // Calculate symbol addresses
        for (auto &symPair : ctx.SymbolMap)
        {
            auto &sym = symPair.second;
            if (sym.DefFileIndex >= ctx.InputFiles.size())
                continue;
            const auto &file = ctx.InputFiles[sym.DefFileIndex];
            if (sym.DefSectionIndex >= file.Sections.size())
                continue;

            bool found = false;
            for (const auto &seg : ctx.Segments)
            {
                for (const auto &src : seg.Sources)
                {
                    if (src.FileIndex == sym.DefFileIndex && src.SectionIndex == sym.DefSectionIndex)
                    {
                        for (const auto &isym : file.Symbols)
                        {
                            std::string symName = isym.Name;
                            if (isym.Binding == Elf::StbLocal)
                            {
                                symName = isym.Name + "@@" + std::to_string(sym.DefFileIndex);
                            }
                            if (symName == sym.Name)
                            {
                                sym.ImageOffset = seg.ImageOffset + src.OutputOffset + isym.Value;
                                found = true;
                                break;
                            }
                        }
                        break;
                    }
                }
                if (found)
                    break;
            }

            if (!found && sym.IsExported)
            {
                std::cerr << "Error: exported symbol "
                          << sym.Name
                          << " belongs to a discarded empty segment " << std::endl;
                return false;
            }
        }

        ctx.HasInitArray = false;
        ctx.InitArrayStart = 0;
        ctx.InitArrayEnd = 0;

        for (const auto &seg : ctx.Segments)
        {
            if ((seg.Permissions & Coaf::PermWrite) == 0)
                continue;
            U64 initArrayStart = 0;
            U64 initArrayEnd = 0;
            bool found = false;

            for (const auto &src : seg.Sources)
            {
                const auto &sec = ctx.InputFiles[src.FileIndex].Sections[src.SectionIndex];
                if (sec.Name == ".init_array")
                {
                    if (!found)
                    {
                        initArrayStart = seg.ImageOffset + src.OutputOffset;
                        found = true;
                    }
                    initArrayEnd = seg.ImageOffset + src.OutputOffset + sec.Data.size();
                }
            }

            if (found)
            {
                ctx.HasInitArray = true;
                ctx.InitArrayStart = initArrayStart;
                ctx.InitArrayEnd = initArrayEnd;
                break;
            }
        }

        return true;
    }

} // namespace Linker