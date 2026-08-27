// src/Launcher/Launcher.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include <Efi/Efi.hpp>

void noreturn efiapi ExitByError(efi::Status exitStatus, efi::uint16 *message, efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Error: "));
    efi::Print(systemTable->ConsoleOutput, message);

    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"\r\nPress [Q] to shutdown, [R] to reboot, [Enter] to exit Launcher.\r\n"));
    while (true)
    {
        efi::InputKey key;
        systemTable->ConsoleInput->ReadKeyStroke(systemTable->ConsoleInput, &key);
        if (key.UnicodeChar == L'q' || key.UnicodeChar == L'Q')
        {
            systemTable->RuntimeServices->ResetSystem(efi::ResetType::Shutdown, exitStatus, 0, nullptr);
        }
        else if (key.UnicodeChar == L'r' || key.UnicodeChar == L'R')
        {
            systemTable->RuntimeServices->ResetSystem(efi::ResetType::Warm, exitStatus, 0, nullptr);
        }
        else if (key.UnicodeChar == L'\r')
        {
            systemTable->BootServices->Exit(imageHandle, exitStatus, 0, nullptr);
        }
    }
}

efi::Status efiapi Main(efi::Handle imageHandle unused, efi::SystemTable *systemTable)
{
    efi::Status status;

    efi::ClearScreen(systemTable->ConsoleOutput);

    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"CuberOS Launcher Beta v1.0\r\n\r\n"));
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Reading profile\r\n"));

    efi::protocol::File *root;
    status = efi::OpenVolume(&root, systemTable);
    if (efi::IsError(status))
    {
        ExitByError(status, efi::ToU16(L"Can NOT open volume\r\n"), imageHandle, systemTable);
        return status;
    }
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Success: Open volume\r\n"));

    efi::uint16 *buffer;
    efi::uintn fileSize;
    status = efi::ReadFile(root, efi::ToU16(L"Efi\\Boot\\Profile"), (void **)&buffer, &fileSize, systemTable);
    if (efi::IsError(status))
    {
        ExitByError(status, efi::ToU16(L"Can NOT read file\r\n"), imageHandle, systemTable);
        return status;
    }
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Success: Read file\r\n"));

    while (true)
    {
        efi::uint16 buffer[3];

        efi::Get(systemTable->ConsoleInput, buffer);
        if (buffer[0] == efi::ToU16(L"q\r\n")[0])
        {
            return efi::Success;
        }
    }

    return efi::Success;
}