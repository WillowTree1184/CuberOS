// src/Launcher/Launcher.c

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include "./Headers/Efi/Efi.h"

unsigned long long EFIAPI Main(void *ImageHandle __attribute__((unused)), Efi_SyetemTable *SystemTable)
{
    SystemTable->ConsoleOutput->ClearScreen(SystemTable->ConsoleOutput);

    SystemTable->ConsoleOutput->OutputString(SystemTable->ConsoleOutput,
                                             L"CuberOS Launcher Beta v1.0\r\n");

    SystemTable->ConsoleOutput->OutputString(SystemTable->ConsoleOutput,
                                             L"Hello World!\r\n");
    while (1)
        ;
}