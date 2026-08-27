// src/CoafLinker/src/ElfReader.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include "ElfTypes.h"
#include "LinkerContext.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

namespace Linker
{

    static bool ReadFile(const std::string &path, std::vector<uint8_t> &outData)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            std::cerr << "Error: cannot open file: " << path << std::endl;
            return false;
        }
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        outData.resize(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char *>(outData.data()), size))
        {
            std::cerr << "Error: cannot read file: " << path << std::endl;
            return false;
        }
        return true;
    }

    static bool ParseElfHeader(const std::vector<uint8_t> &data, Elf::Header &hdr)
    {
        if (data.size() < sizeof(Elf::Header))
        {
            std::cerr << "Error: file too small for ELF header" << std::endl;
            return false;
        }
        std::memcpy(&hdr, data.data(), sizeof(Elf::Header));
        if (hdr.Ident[0] != Elf::Mag0 || hdr.Ident[1] != Elf::Mag1 ||
            hdr.Ident[2] != Elf::Mag2 || hdr.Ident[3] != Elf::Mag3)
        {
            std::cerr << "Error: invalid ELF magic" << std::endl;
            return false;
        }
        if (hdr.Ident[4] != Elf::Class64)
        {
            std::cerr << "Error: not ELF64" << std::endl;
            return false;
        }
        if (hdr.Ident[5] != Elf::Data2LSB)
        {
            std::cerr << "Error: not little-endian" << std::endl;
            return false;
        }
        if (hdr.Type != Elf::EtRel)
        {
            std::cerr << "Error: not relocatable object file (type=" << hdr.Type << ")" << std::endl;
            return false;
        }
        return true;
    }

    static bool ParseSections(InputFile &file)
    {
        const auto &hdr = file.ElfHdr;
        if (hdr.Shoff == 0 || hdr.Shnum == 0)
            return true;

        file.SecHeaders.resize(hdr.Shnum);
        for (U64 i = 0; i < hdr.Shnum; ++i)
        {
            U64 off = hdr.Shoff + i * sizeof(Elf::SectionHeader);
            if (off + sizeof(Elf::SectionHeader) > file.RawData.size())
            {
                std::cerr << "Error: section header out of bounds" << std::endl;
                return false;
            }
            std::memcpy(&file.SecHeaders[i], file.RawData.data() + off, sizeof(Elf::SectionHeader));
        }

        // Read section name string table
        if (hdr.Shstrndx < hdr.Shnum)
        {
            const auto &shstrSh = file.SecHeaders[hdr.Shstrndx];
            if (shstrSh.Offset + shstrSh.Size <= file.RawData.size())
            {
                file.ShStrTab.assign(
                    reinterpret_cast<const char *>(file.RawData.data() + shstrSh.Offset),
                    shstrSh.Size);
            }
        }

        // Parse each section
        for (U64 i = 0; i < hdr.Shnum; ++i)
        {
            const auto &sh = file.SecHeaders[i];
            InputSection sec;
            sec.Name = (sh.Name < file.ShStrTab.size())
                           ? std::string(file.ShStrTab.data() + sh.Name)
                           : "";
            sec.Flags = sh.Flags;
            sec.AddrAlign = sh.Addralign;
            sec.Type = sh.Type;
            sec.OffsetInFile = sh.Offset;
            sec.IsBss = (sh.Type == Elf::ShtNoBits);

            if (!sec.IsBss && sh.Size > 0 && sh.Offset + sh.Size <= file.RawData.size())
            {
                sec.Data.assign(file.RawData.data() + sh.Offset, file.RawData.data() + sh.Offset + sh.Size);
            }

            file.Sections.push_back(std::move(sec));
        }

        return true;
    }

    static bool ParseSymbols(InputFile &file)
    {
        // Find symbol table and string table
        const Elf::SectionHeader *symtabSh = nullptr;
        const Elf::SectionHeader *strtabSh = nullptr;

        for (U64 i = 0; i < file.SecHeaders.size(); ++i)
        {
            if (file.SecHeaders[i].Type == Elf::ShtSymtab)
            {
                symtabSh = &file.SecHeaders[i];
                U64 strtabIdx = file.SecHeaders[i].Link;
                if (strtabIdx < file.SecHeaders.size())
                {
                    strtabSh = &file.SecHeaders[strtabIdx];
                }
                break;
            }
        }

        if (!symtabSh || !strtabSh)
            return true;

        // Read string table
        if (strtabSh->Offset + strtabSh->Size <= file.RawData.size())
        {
            file.StrTab.assign(
                reinterpret_cast<const char *>(file.RawData.data() + strtabSh->Offset),
                strtabSh->Size);
        }

        U64 numSymbols = symtabSh->Size / sizeof(Elf::Symbol);
        for (U64 i = 0; i < numSymbols; ++i)
        {
            U64 off = symtabSh->Offset + i * sizeof(Elf::Symbol);
            if (off + sizeof(Elf::Symbol) > file.RawData.size())
                break;

            Elf::Symbol sym;
            std::memcpy(&sym, file.RawData.data() + off, sizeof(Elf::Symbol));

            InputSymbol isym;
            isym.Value = sym.Value;
            isym.Size = sym.Size;
            isym.Binding = sym.Info >> 4;
            isym.SectionIndex = sym.Shndx;
            isym.InputFileIndex = 0; // set later
            isym.SectionIndex = 0;   // set later

            if (sym.Name < file.StrTab.size())
            {
                isym.Name = std::string(file.StrTab.data() + sym.Name);
            }

            // Map section index
            if (sym.Shndx > 0 && sym.Shndx < file.SecHeaders.size())
            {
                isym.SectionIndex = sym.Shndx;
            }

            file.Symbols.push_back(std::move(isym));
        }

        return true;
    }

    static bool ParseRelocations(InputFile &file)
    {
        for (U64 i = 0; i < file.SecHeaders.size(); ++i)
        {
            const auto &sh = file.SecHeaders[i];
            if (sh.Type != Elf::ShtRel && sh.Type != Elf::ShtRela)
                continue;

            U64 targetSecIdx = sh.Info;
            if (targetSecIdx >= file.SecHeaders.size())
                continue;

            if (sh.Type == Elf::ShtRela)
            {
                U64 numRela = sh.Size / sizeof(Elf::Rela);
                for (U64 j = 0; j < numRela; ++j)
                {
                    U64 off = sh.Offset + j * sizeof(Elf::Rela);
                    if (off + sizeof(Elf::Rela) > file.RawData.size())
                        break;

                    Elf::Rela rela;
                    std::memcpy(&rela, file.RawData.data() + off, sizeof(Elf::Rela));

                    InputRelocation ir;
                    ir.Offset = rela.Offset;
                    ir.Type = Elf::GetRelType(rela.Info);
                    ir.SymbolIndex = Elf::GetRelSym(rela.Info);
                    ir.Addend = rela.Addend;
                    ir.SectionIndex = targetSecIdx;
                    file.Relocations.push_back(std::move(ir));
                }
            }
            else
            {
                U64 numRel = sh.Size / sizeof(Elf::Rel);
                for (U64 j = 0; j < numRel; ++j)
                {
                    U64 off = sh.Offset + j * sizeof(Elf::Rel);
                    if (off + sizeof(Elf::Rel) > file.RawData.size())
                        break;

                    Elf::Rel rel;
                    std::memcpy(&rel, file.RawData.data() + off, sizeof(Elf::Rel));

                    InputRelocation ir;
                    ir.Offset = rel.Offset;
                    ir.Type = Elf::GetRelType(rel.Info);
                    ir.SymbolIndex = Elf::GetRelSym(rel.Info);
                    // Read addend from target location
                    U64 targetSec = targetSecIdx;
                    if (targetSec < file.Sections.size() && ir.Offset + 8 <= file.Sections[targetSec].Data.size())
                    {
                        std::memcpy(&ir.Addend, file.Sections[targetSec].Data.data() + ir.Offset, sizeof(S64));
                    }
                    else
                    {
                        ir.Addend = 0;
                    }
                    ir.SectionIndex = targetSecIdx;
                    file.Relocations.push_back(std::move(ir));
                }
            }
        }
        return true;
    }

    bool LoadElfFile(const std::string &path, Context &ctx)
    {
        InputFile file;
        file.Path = path;

        if (!ReadFile(path, file.RawData))
            return false;
        if (!ParseElfHeader(file.RawData, file.ElfHdr))
            return false;
        if (!ParseSections(file))
            return false;
        if (!ParseSymbols(file))
            return false;
        if (!ParseRelocations(file))
            return false;

        size_t fileIdx = ctx.InputFiles.size();
        for (auto &sym : file.Symbols)
        {
            sym.InputFileIndex = fileIdx;
        }
        for (auto &rel : file.Relocations)
        {
            rel.InputFileIndex = fileIdx;
        }

        ctx.InputFiles.push_back(std::move(file));
        return true;
    }

} // namespace Linker