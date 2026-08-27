// src/CoafLinker/src/RelocProcessor.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"
#include "ElfTypes.h"

#include <iostream>
#include <cstring>
#include <algorithm>

namespace Linker
{

    using S32 = Elf::S32;
    using U32 = Elf::U32;
    using S16 = int16_t;
    using S8 = int8_t;

    static bool FindSymbolAddress(Context &ctx, size_t fileIdx, U64 symIdx, U64 &outAddr, bool &isLocal)
    {
        if (fileIdx >= ctx.InputFiles.size())
            return false;
        const auto &file = ctx.InputFiles[fileIdx];
        if (symIdx >= file.Symbols.size())
            return false;

        const auto &sym = file.Symbols[symIdx];
        std::string lookupName;
        if (sym.Binding == Elf::StbLocal)
        {
            lookupName = sym.Name + "@@" + std::to_string(fileIdx);
        }
        else
        {
            lookupName = sym.Name;
        }

        auto it = ctx.SymbolMap.find(lookupName);
        if (it == ctx.SymbolMap.end())
        {
            it = ctx.SymbolMap.find(sym.Name);
            if (it == ctx.SymbolMap.end())
            {
                std::cerr << "Error: symbol not found: " << sym.Name << std::endl;
                return false;
            }
        }

        outAddr = it->second.ImageOffset;
        isLocal = it->second.IsLocal;
        return true;
    }

    static bool FindOutputLocation(Context &ctx, size_t fileIdx, size_t secIdx, U64 offset,
                                   U64 &outImageOffset, uint8_t *&outPtr, size_t &outSegIdx)
    {
        for (size_t s = 0; s < ctx.Segments.size(); ++s)
        {
            auto &seg = ctx.Segments[s];
            for (const auto &src : seg.Sources)
            {
                if (src.FileIndex == fileIdx && src.SectionIndex == secIdx)
                {
                    U64 loc = src.OutputOffset + offset;
                    if (loc >= seg.Data.size() && !seg.Data.empty())
                    {
                        std::cerr << "Error: relocation out of segment bounds" << std::endl;
                        return false;
                    }
                    outImageOffset = seg.ImageOffset + loc;
                    if (!seg.Data.empty())
                    {
                        outPtr = seg.Data.data() + loc;
                    }
                    else
                    {
                        outPtr = nullptr;
                    }
                    outSegIdx = s;
                    return true;
                }
            }
        }
        std::cerr << "Error: cannot find output location for relocation" << std::endl;
        return false;
    }

    bool ProcessRelocations(Context &ctx)
    {
        for (size_t fidx = 0; fidx < ctx.InputFiles.size(); ++fidx)
        {
            const auto &file = ctx.InputFiles[fidx];
            for (const auto &rel : file.Relocations)
            {
                U64 symAddr = 0;
                bool isLocal = false;
                if (!FindSymbolAddress(ctx, rel.InputFileIndex, rel.SymbolIndex, symAddr, isLocal))
                {
                    return false;
                }

                U64 refImageOffset = 0;
                uint8_t *ptr = nullptr;
                size_t segIdx = 0;
                if (!FindOutputLocation(ctx, rel.InputFileIndex, rel.SectionIndex, rel.Offset,
                                        refImageOffset, ptr, segIdx))
                {
                    return false;
                }

                if (ptr == nullptr)
                {
                    std::cerr << "Error: relocation targets BSS (not supported in MVP)" << std::endl;
                    return false;
                }

                switch (rel.Type)
                {
                case Elf::R_X86_64_NONE:
                    break;

                case Elf::R_X86_64_64:
                {
                    U64 value = symAddr + static_cast<U64>(rel.Addend);
                    std::memcpy(ptr, &value, sizeof(U64));
                    ctx.CoafRelocations.push_back(refImageOffset);
                    break;
                }

                case Elf::R_X86_64_PC32:
                {
                    S64 value = static_cast<S64>(symAddr) + rel.Addend - static_cast<S64>(refImageOffset);
                    S32 value32 = static_cast<S32>(value);
                    std::memcpy(ptr, &value32, sizeof(S32));
                    break;
                }

                case Elf::R_X86_64_PLT32:
                {
                    S64 value = static_cast<S64>(symAddr) + rel.Addend - static_cast<S64>(refImageOffset);
                    S32 value32 = static_cast<S32>(value);
                    std::memcpy(ptr, &value32, sizeof(S32));
                    break;
                }

                case Elf::R_X86_64_32:
                {
                    U64 value = symAddr + static_cast<U64>(rel.Addend);
                    U32 value32 = static_cast<U32>(value);
                    std::memcpy(ptr, &value32, sizeof(U32));
                    ctx.CoafRelocations.push_back(refImageOffset);
                    break;
                }

                case Elf::R_X86_64_32S:
                {
                    S64 value = static_cast<S64>(symAddr) + rel.Addend;
                    S32 value32 = static_cast<S32>(value);
                    std::memcpy(ptr, &value32, sizeof(S32));
                    ctx.CoafRelocations.push_back(refImageOffset);
                    break;
                }

                case Elf::R_X86_64_16:
                {
                    U64 value = symAddr + static_cast<U64>(rel.Addend);
                    U16 value16 = static_cast<U16>(value);
                    std::memcpy(ptr, &value16, sizeof(U16));
                    ctx.CoafRelocations.push_back(refImageOffset);
                    break;
                }

                case Elf::R_X86_64_PC16:
                {
                    S64 value = static_cast<S64>(symAddr) + rel.Addend - static_cast<S64>(refImageOffset);
                    S16 value16 = static_cast<S16>(value);
                    std::memcpy(ptr, &value16, sizeof(S16));
                    break;
                }

                case Elf::R_X86_64_8:
                {
                    U64 value = symAddr + static_cast<U64>(rel.Addend);
                    U8 value8 = static_cast<U8>(value);
                    std::memcpy(ptr, &value8, sizeof(U8));
                    ctx.CoafRelocations.push_back(refImageOffset);
                    break;
                }

                case Elf::R_X86_64_PC8:
                {
                    S64 value = static_cast<S64>(symAddr) + rel.Addend - static_cast<S64>(refImageOffset);
                    S8 value8 = static_cast<S8>(value);
                    std::memcpy(ptr, &value8, sizeof(S8));
                    break;
                }

                default:
                    std::cerr << "Error: unsupported relocation type: " << rel.Type << std::endl;
                    return false;
                }
            }
        }

        std::sort(ctx.CoafRelocations.begin(), ctx.CoafRelocations.end());
        ctx.CoafRelocations.erase(
            std::unique(ctx.CoafRelocations.begin(), ctx.CoafRelocations.end()),
            ctx.CoafRelocations.end());

        return true;
    }

} // namespace Linker