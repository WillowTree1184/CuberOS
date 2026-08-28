// src/CoafLinker/src/main.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "LinkerContext.h"
#include "CoafFormat.h"

#include <iostream>
#include <string>
#include <vector>

namespace Linker
{

    extern bool LoadElfFile(const std::string &path, Context &ctx);
    extern bool ClassifySections(Context &ctx);
    extern bool BuildSymbolTable(Context &ctx);
    extern bool LayoutSegments(Context &ctx);
    extern bool ProcessRelocations(Context &ctx);
    extern bool WriteCoaf(Context &ctx);
    extern bool Validate(Context &ctx);

} // namespace Linker

static void PrintUsage(const char *prog)
{
    std::cerr << "Usage: " << prog << " [options] -o <output.coaf> <input1.o> [input2.o ...]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  --entry <name>    Entry symbol name (default: main)" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        PrintUsage(argv[0]);
        return 1;
    }

    Linker::Context ctx;
    ctx.EntrySymbolName = "main";
    std::vector<std::string> inputs;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc)
        {
            ctx.OutputPath = argv[++i];
        }
        else if (arg == "--entry" && i + 1 < argc)
        {
            ctx.EntrySymbolName = argv[++i];
        }
        else if (arg.substr(0, 2) == "-L")
        {
            ctx.LibraryPaths.push_back(arg.substr(2));
        }
        else if (arg.substr(0, 2) == "-l")
        {
            ctx.Libraries.push_back(arg.substr(2));
        }
        else if (arg.ends_with(".o") || arg.ends_with(".a"))
        {
            inputs.push_back(arg);
        }
        else
        {
            std::cerr << "Warning: ignoring argument: " << arg << std::endl;
        }
    }

    if (ctx.OutputPath.empty())
    {
        std::cerr << "Error: no output file specified" << std::endl;
        PrintUsage(argv[0]);
        return 1;
    }

    for (const auto &path : inputs)
    {
        if (!Linker::LoadElfFile(path, ctx))
        {
            std::cerr << "Error: failed to load " << path << std::endl;
            return 1;
        }
    }

    if (ctx.InputFiles.empty())
    {
        std::cerr << "Error: no input files" << std::endl;
        return 1;
    }

    if (!Linker::ClassifySections(ctx))
    {
        std::cerr << "Error: section classification failed" << std::endl;
        return 1;
    }

    if (!Linker::BuildSymbolTable(ctx))
    {
        std::cerr << "Error: symbol table build failed" << std::endl;
        return 1;
    }

    if (!Linker::LayoutSegments(ctx))
    {
        std::cerr << "Error: layout failed" << std::endl;
        return 1;
    }

    if (!Linker::ProcessRelocations(ctx))
    {
        std::cerr << "Error: relocation processing failed" << std::endl;
        return 1;
    }

    if (!Linker::WriteCoaf(ctx))
    {
        std::cerr << "Error: COAF write failed" << std::endl;
        return 1;
    }

    if (!Linker::Validate(ctx))
    {
        std::cerr << "Error: validation failed" << std::endl;
        return 1;
    }

    std::cout << "Linked: " << ctx.OutputPath << std::endl;
    std::cout << "  ArchId: " << ctx.TargetArchId << std::endl;
    std::cout << "  ImageSize: " << ctx.MainTable.ImageSize << std::endl;
    std::cout << "  Segments: " << ctx.MainTable.SegmentCount << std::endl;
    std::cout << "  Exports: " << ctx.MainTable.ExportSymbolCount << std::endl;
    std::cout << "  Relocations: " << ctx.MainTable.RelocationCount << std::endl;

    return 0;
}