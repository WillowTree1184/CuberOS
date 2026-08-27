// src/Launcher/Launcher.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include <Launcher.hpp>

void noreturn efiapi ExitByError(efi::Status exitStatus, efi::uint16 *message, efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    efi::SetTextColor(systemTable->ConsoleOutput, efi::TextColor::White, efi::TextColor::LightRed);

    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"\r\nError: "));
    efi::Print(systemTable->ConsoleOutput, message);

    efi::ResetTextColor(systemTable->ConsoleOutput);

    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"\r\nYou can Press:\r\n- [Q] to shutdown,\r\n- [R] to reboot,\r\n- [ESC] to reboot to Firmware UI,\r\n- [Enter] to exit Launcher.\r\n"));
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
        else if (key.ScanCode == efi::KeyScanCode::Escape)
        {
            enum class OsIndication : efi::uint64
            {
                BootToFirmwareUI = 0x0000000000000001
            };
            OsIndication indications = OsIndication::BootToFirmwareUI;

            systemTable->RuntimeServices->SetVariable(
                efi::ToU16(L"OsIndications"),
                &efi::guid::GlobalVariableGuid,
                efi::VariableAttribute::NonVolatile | efi::VariableAttribute::BootServiceAccess | efi::VariableAttribute::RuntimeAccess, // 0x07, // NonVolatile | BootServiceAccess | RuntimeAccess
                sizeof(indications),
                &indications);

            systemTable->RuntimeServices->ResetSystem(efi::ResetType::Warm, exitStatus, 0, nullptr);
        }
        else if (key.UnicodeChar == L'\r')
        {
            systemTable->BootServices->Exit(imageHandle, exitStatus, 0, nullptr);
        }
    }
}

efi::Status efiapi Main(efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    efi::Status status;

    // Clear Screen
    efi::ClearScreen(systemTable->ConsoleOutput);

    // Print data
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"CuberOS Launcher Beta v1.0\r\n\r\n"));
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Reading profile\r\n"));

    // Open volume
    efi::protocol::File *root;
    status = efi::OpenVolume(&root, systemTable);
    if (efi::IsError(status))
    {
        ExitByError(status, efi::ToU16(L"Can NOT open volume"), imageHandle, systemTable);
        return status;
    }
    efi::Print(systemTable->ConsoleOutput, efi::ToU16(L"Success: Open volume.\r\n"));

    // Read profile
    efi::uint16 *buffer;
    efi::uintn fileSize;
    status = efi::ReadFile(root, efi::ToU16(L"Preloader\\Preloader.app"), (void **)&buffer, &fileSize, systemTable);
    if (efi::IsError(status))
    {
        ExitByError(status, efi::ToU16(L"Can NOT load preloader"), imageHandle, systemTable);
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