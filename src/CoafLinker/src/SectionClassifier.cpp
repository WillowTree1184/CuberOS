// src/CoafLinker/src/SectionClassifier.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <iostream>

namespace Linker
{

    static U64 ClassifyPermissions(U64 elfFlags, U64)
    {
        if ((elfFlags & Elf::ShfAlloc) == 0)
        {
            return 0;
        }

        bool isWrite = (elfFlags & Elf::ShfWrite) != 0;
        bool isExec = (elfFlags & Elf::ShfExecInstr) != 0;

        if (isExec)
        {
            return Coaf::PermRead | Coaf::PermExec;
        }
        if (isWrite)
        {
            return Coaf::PermRead | Coaf::PermWrite;
        }
        return Coaf::PermRead;
    }

    bool ClassifySections(Context &ctx)
    {
        for (auto &file : ctx.InputFiles)
        {
            for (auto &sec : file.Sections)
            {
                sec.CoafPermissions = ClassifyPermissions(sec.Flags, sec.Type);
            }
        }
        return true;
    }

} // namespace Linker