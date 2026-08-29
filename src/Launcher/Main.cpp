// src/Launcher/Main.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include <Main.hpp>
#include <EfiStd.hpp>
#include <Memory.hpp>
#include <Booter.hpp>

inline void InitalizeGlobalSettings(efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    gImageHandle = imageHandle;
    gSystemTable = systemTable;
    gBootServices = systemTable->BootServices;
}

efi::Status efiapi Main(efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    InitalizeGlobalSettings(imageHandle, systemTable);
    efi::Status status;

    // Clear Screen
    efi::ResetTextColor();
    efi::ClearScreen();

    // Print data
    efi::PrintMessage(L"Launching");

    // Open volume
    efi::protocol::File *root;
    status = efi::OpenVolume(&root);
    if (efi::IsError(status))
    {
        efi::ExitByError(status, efi::ToU16(L"Can NOT open volume"));
    }

    Boot(imageHandle, systemTable, root);

    while (true)
    {
        efi::uint16 buffer[3];

        efi::Get(buffer);
        if (buffer[0] == efi::ToU16(L"q\r\n")[0])
        {
            efi::ExitByError(efi::Success, efi::ToU16(L"Quiet"));
        }
    }

    return efi::Success;
}