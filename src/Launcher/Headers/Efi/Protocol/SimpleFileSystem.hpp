// src/Launcher/Headers/Efi/Protocol/SimpleFileSystem.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include "File.hpp"
#include "../SystemTable.hpp"
#include "../Macros.hpp"

namespace efi
{
    namespace protocol
    {
        struct SimpleFileSystem
        {
            uint64 Revision;
            Status(efiapi *OpenVolume)(SimpleFileSystem *current, File **root);
        };
    }

    namespace guid
    {
        Guid SimpleFileSystem = {0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
    }

    Status OpenVolume(protocol::File **root, SystemTable *systemTable)
    {
        Status status;

        efi::protocol::SimpleFileSystem *simpleFileSystem;
        status = systemTable->BootServices->LocateProtocol(&efi::guid::SimpleFileSystem, nullptr, (void **)&simpleFileSystem);
        if (efi::IsError(status))
        {
            return status;
        }

        return simpleFileSystem->OpenVolume(simpleFileSystem, root);
    }
}