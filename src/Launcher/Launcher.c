// src/Launcher/Launcher.c

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include "./Headers/Efi/Efi.h"

Efi_Status efiapi Main(void *ImageHandle unused, Efi_SyetemTable *SystemTable)
{
    Efi_ClearScreen(SystemTable->ConsoleOutput);

    Efi_Print(SystemTable->ConsoleOutput, L"CuberOS Launcher Beta v1.0\r\n");
    Efi_Print(SystemTable->ConsoleOutput, L"Hello World!\r\n");

    while (true)
    {
        uint16 buffer[3];

        Efi_Get(SystemTable->ConsoleInput, buffer);

        Efi_Print(SystemTable->ConsoleOutput, buffer);
    }

    return Efi_Success;
}