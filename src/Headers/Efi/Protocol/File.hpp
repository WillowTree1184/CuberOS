// src/Headers/Efi/Protocol/File.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#pragma once

#include "../SystemTable.hpp"
#include "../Types.hpp"

namespace efi
{
    // FileMode
    enum class FileMode : uint64
    {
        Read = 0x0000000000000001,
        Write = 0x0000000000000002,
        Create = 0x8000000000000000,
    };

    constexpr FileMode operator|(FileMode a, FileMode b) noexcept
    {
        return static_cast<FileMode>(static_cast<uint64>(a) | static_cast<uint64>(b));
    }

    constexpr FileMode operator&(FileMode a, FileMode b) noexcept
    {
        return static_cast<FileMode>(static_cast<uint64>(a) & static_cast<uint64>(b));
    }

    constexpr FileMode &operator|=(FileMode &a, FileMode b) noexcept
    {
        a = a | b;
        return a;
    }

    constexpr bool Any(FileMode flags, FileMode mask) noexcept
    {
        return (static_cast<uint64>(flags) & static_cast<uint64>(mask)) != 0;
    }

    // FileAttribute
    enum class FileAttribute : uint64
    {
        None = 0,
        ReadOnly = 0x0000000000000001,
        Hidden = 0x0000000000000002,
        System = 0x0000000000000004,
        Reserved = 0x0000000000000008,
        Directory = 0x0000000000000010,
        Archive = 0x0000000000000020,
        ValidMask = 0x0000000000000037,
    };

    constexpr FileAttribute operator|(FileAttribute a, FileAttribute b) noexcept
    {
        return static_cast<FileAttribute>(static_cast<uint64>(a) | static_cast<uint64>(b));
    }

    constexpr FileAttribute operator&(FileAttribute a, FileAttribute b) noexcept
    {
        return static_cast<FileAttribute>(static_cast<uint64>(a) & static_cast<uint64>(b));
    }

    constexpr FileAttribute &operator|=(FileAttribute &a, FileAttribute b) noexcept
    {
        a = a | b;
        return a;
    }

    constexpr bool Any(FileAttribute flags, FileAttribute mask) noexcept
    {
        return (static_cast<uint64>(flags) & static_cast<uint64>(mask)) != 0;
    }

    namespace guid
    {
        inline constexpr Guid FileInfo = {0x9576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
    } // namespace coaf::guid

    struct FileInfo
    {
        uint64 Size;
        uint64 FileSize;
        uint64 PhysicalSize;
        Time CreateTime;
        Time LastAccessTime;
        Time ModificationTime;
        FileAttribute Attribute;
        char16 *FileName;
    };

    // protocol File
    namespace protocol
    {
        struct File
        {
            uint64 Revision;
            Status(efiapi *Open)(File *current, File **NewHandle, unsigned short *FileName, FileMode OpenMode, FileAttribute Attributes);
            Status(efiapi *Close)(File *current);
            Status(efiapi *Delete)(File *current);
            Status(efiapi *Read)(File *current, uintn *BufferSize, void *Buffer);
            Status(efiapi *Write)(File *current, uintn *BufferSize, void *Buffer);
            Status(efiapi *GetPosition)(File *current, uint64 *Position);
            Status(efiapi *SetPosition)(File *current, uint64 Position);
            Status(efiapi *GetInfo)(File *current, const Guid *InformationType, uintn *BufferSize, void *Buffer);
            Status(efiapi *SetInfo)(File *current, const Guid *InformationType, uintn BufferSize, void *Buffer);
            Status(efiapi *Flush)(File *current);
        };
    } // namespace coaf::protocol
} // namespace coaf