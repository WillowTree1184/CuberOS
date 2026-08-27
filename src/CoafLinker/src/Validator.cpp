// src/CoafLinker/src/Validator.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <iostream>
#include <set>

namespace Linker
{

    static bool CheckOverlaps(const std::vector<std::pair<U64, U64>> &intervals)
    {
        for (size_t i = 0; i < intervals.size(); ++i)
        {
            for (size_t j = i + 1; j < intervals.size(); ++j)
            {
                if (intervals[i].first < intervals[j].second && intervals[j].first < intervals[i].second)
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool Validate(Context &ctx)
    {
        // Check alignment
        if (ctx.MainTable.SegmentTableOffset % 8 != 0 && ctx.MainTable.SegmentTableOffset != 0)
        {
            std::cerr << "Validation error: SegmentTableOffset not aligned" << std::endl;
            return false;
        }
        if (ctx.MainTable.ExportSymbolTableOffset % 8 != 0 && ctx.MainTable.ExportSymbolTableOffset != 0)
        {
            std::cerr << "Validation error: ExportSymbolTableOffset not aligned" << std::endl;
            return false;
        }
        if (ctx.MainTable.RelocationTableOffset % 8 != 0 && ctx.MainTable.RelocationTableOffset != 0)
        {
            std::cerr << "Validation error: RelocationTableOffset not aligned" << std::endl;
            return false;
        }
        if (ctx.MainTable.StringTableOffset % 8 != 0 && ctx.MainTable.StringTableOffset != 0)
        {
            std::cerr << "Validation error: StringTableOffset not aligned" << std::endl;
            return false;
        }

        // Check segment constraints
        if (ctx.Segments.empty())
        {
            std::cerr << "Validation error: no segments" << std::endl;
            return false;
        }

        U64 maxEnd = 0;
        std::vector<std::pair<U64, U64>> memIntervals;
        std::vector<std::pair<U64, U64>> fileIntervals;

        for (const auto &seg : ctx.Segments)
        {
            if (seg.ImageOffset % Coaf::PageSize != 0)
            {
                std::cerr << "Validation error: segment ImageOffset not page-aligned" << std::endl;
                return false;
            }
            if (seg.MemorySize == 0)
            {
                std::cerr << "Validation error: segment MemorySize is zero" << std::endl;
                return false;
            }
            if ((seg.Permissions & Coaf::PermRead) == 0)
            {
                std::cerr << "Validation error: segment missing read permission" << std::endl;
                return false;
            }
            if ((seg.Permissions & Coaf::PermWrite) && (seg.Permissions & Coaf::PermExec))
            {
                std::cerr << "Validation error: segment has both write and execute" << std::endl;
                return false;
            }

            memIntervals.push_back({seg.ImageOffset, seg.ImageOffset + seg.MemorySize});

            U64 end = seg.ImageOffset + seg.MemorySize;
            if (end > maxEnd)
                maxEnd = end;
        }

        if (!CheckOverlaps(memIntervals))
        {
            std::cerr << "Validation error: segments overlap in memory" << std::endl;
            return false;
        }

        U64 expectedImageSize = ((maxEnd + Coaf::PageSize - 1) / Coaf::PageSize) * Coaf::PageSize;
        if (ctx.MainTable.ImageSize != expectedImageSize)
        {
            std::cerr << "Validation error: ImageSize mismatch (expected " << expectedImageSize
                      << ", got " << ctx.MainTable.ImageSize << ")" << std::endl;
            return false;
        }

        // Check export symbols
        std::set<std::string> exportNames;
        for (const auto &exp : ctx.CoafExports)
        {
            if (exp.NameOffset >= ctx.StringTable.size())
            {
                std::cerr << "Validation error: export symbol name out of bounds" << std::endl;
                return false;
            }
            const char *name = reinterpret_cast<const char *>(ctx.StringTable.data() + exp.NameOffset);
            if (exportNames.count(name))
            {
                std::cerr << "Validation error: duplicate export symbol: " << name << std::endl;
                return false;
            }
            exportNames.insert(name);

            bool inSegment = false;
            for (const auto &seg : ctx.Segments)
            {
                if (exp.ImageOffset >= seg.ImageOffset && exp.ImageOffset < seg.ImageOffset + seg.MemorySize)
                {
                    inSegment = true;
                    break;
                }
            }
            if (!inSegment)
            {
                std::cerr << "Validation error: export symbol not in any segment: " << name << std::endl;
                return false;
            }
        }

        // Check relocations
        for (U64 addr : ctx.CoafRelocations)
        {
            if (addr % 8 != 0)
            {
                std::cerr << "Validation error: relocation not 8-byte aligned: " << addr << std::endl;
                return false;
            }
            bool inWritable = false;
            for (const auto &seg : ctx.Segments)
            {
                if (addr >= seg.ImageOffset && addr < seg.ImageOffset + seg.MemorySize)
                {
                    if (seg.Permissions & Coaf::PermWrite)
                    {
                        inWritable = true;
                    }
                    break;
                }
            }
            if (!inWritable)
            {
                std::cerr << "Validation error: relocation not in writable segment: " << addr << std::endl;
                return false;
            }
        }

        return true;
    }

} // namespace Linker