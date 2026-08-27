// src/CoafLinker/src/SymbolTable.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <iostream>

namespace Linker
{

    bool BuildSymbolTable(Context &ctx)
    {
        // First pass: collect all defined global symbols
        for (size_t fidx = 0; fidx < ctx.InputFiles.size(); ++fidx)
        {
            const auto &file = ctx.InputFiles[fidx];
            for (const auto &sym : file.Symbols)
            {
                if (sym.Binding == Elf::StbGlobal && sym.SectionIndex != 0)
                {
                    auto it = ctx.SymbolMap.find(sym.Name);
                    if (it != ctx.SymbolMap.end() && !it->second.IsLocal)
                    {
                        std::cerr << "Error: multiple definition of symbol: " << sym.Name << std::endl;
                        return false;
                    }
                    ResolvedSymbol rs;
                    rs.Name = sym.Name;
                    rs.ImageOffset = 0; // Will be set after layout
                    rs.IsExported = true;
                    rs.IsLocal = false;
                    rs.DefFileIndex = fidx;
                    rs.DefSectionIndex = sym.SectionIndex;
                    ctx.SymbolMap[sym.Name] = rs;
                }
            }
        }

        // Second pass: collect undefined globals (should not exist in static link)
        for (size_t fidx = 0; fidx < ctx.InputFiles.size(); ++fidx)
        {
            const auto &file = ctx.InputFiles[fidx];
            for (const auto &sym : file.Symbols)
            {
                if (sym.Binding == Elf::StbGlobal && sym.SectionIndex == 0)
                {
                    auto it = ctx.SymbolMap.find(sym.Name);
                    if (it == ctx.SymbolMap.end())
                    {
                        std::cerr << "Error: undefined symbol: " << sym.Name << std::endl;
                        return false;
                    }
                }
            }
        }

        // Add local symbols to map for relocation resolution
        for (size_t fidx = 0; fidx < ctx.InputFiles.size(); ++fidx)
        {
            const auto &file = ctx.InputFiles[fidx];
            for (const auto &sym : file.Symbols)
            {
                if (sym.Binding == Elf::StbLocal && sym.SectionIndex != 0 && !sym.Name.empty())
                {
                    std::string localName = sym.Name + "@@" + std::to_string(fidx);
                    ResolvedSymbol rs;
                    rs.Name = localName;
                    rs.ImageOffset = 0;
                    rs.IsExported = false;
                    rs.IsLocal = true;
                    rs.DefFileIndex = fidx;
                    rs.DefSectionIndex = sym.SectionIndex;
                    ctx.SymbolMap[localName] = rs;
                }
            }
        }

        return true;
    }

} // namespace Linker