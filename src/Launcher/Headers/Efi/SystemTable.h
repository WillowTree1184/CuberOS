// src/Launcher/Headers/Efi/SystemTable.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_SYSTEM_TABLE_H__
#define __COL_EFI_SYSTEM_TABLE_H__

#include "./Efi.h"

typedef struct
{
    char _buf[44];
    Efi_Protocol_SimpleTextInput *ConsoleInput;
    unsigned long long _buf2;
    Efi_Protocol_SimpleTextOutput *ConsoleOutput;
} Efi_SyetemTable;

#endif