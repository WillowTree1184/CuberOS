#pragma once

#include "../SystemTable.hpp"
#include "../Macros.hpp"

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
    }

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
    }

    Status ReadFile(protocol::File *root, char16 *fileName, void **buffer, uintn *fileSize, SystemTable *systemTable)
    {
        Status status;
        protocol::File *file = nullptr;

        // Open
        status = root->Open(root, &file, fileName, FileMode::Read, FileAttribute::None);
        if (efi::IsError(status))
        {
            return status;
        }

        // Get size
        uintn infoSize = 0;
        status = file->GetInfo(file, &efi::guid::FileInfo, &infoSize, nullptr);
        if (status != efi::error::BufferTooSmall)
        {
            file->Close(file);
            return status;
        }

        void *infoBuffer = nullptr;
        status = systemTable->BootServices->AllocatePool(efi::MemoryType::LoaderData, infoSize, &infoBuffer);
        if (efi::IsError(status))
        {
            file->Close(file);
            return status;
        }

        status = file->GetInfo(file, &efi::guid::FileInfo, &infoSize, infoBuffer);
        if (efi::IsError(status))
        {
            systemTable->BootServices->FreePool(infoBuffer);
            file->Close(file);
            return status;
        }

        uintn actualSize = static_cast<uintn>(static_cast<FileInfo *>(infoBuffer)->FileSize);
        systemTable->BootServices->FreePool(infoBuffer);

        // Read
        status = systemTable->BootServices->AllocatePool(efi::MemoryType::LoaderData, actualSize + sizeof(char16), buffer);
        if (efi::IsError(status))
        {
            file->Close(file);
            return status;
        }

        uintn readSize = actualSize;
        status = file->Read(file, &readSize, *buffer);
        file->Close(file);

        if (efi::IsError(status))
        {
            systemTable->BootServices->FreePool(*buffer);
            *buffer = nullptr;
            return status;
        }

        if (readSize != actualSize)
        {
            systemTable->BootServices->FreePool(*buffer);
            *buffer = nullptr;
            return efi::error::DeviceError;
        }

        *fileSize = actualSize;

        return efi::Success;
    }
}