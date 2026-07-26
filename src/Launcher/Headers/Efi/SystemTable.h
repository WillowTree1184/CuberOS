// src/Launcher/Headers/Efi/SystemTable.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_SYSTEM_TABLE_H__
#define __COL_EFI_SYSTEM_TABLE_H__

#include "./Protocol/SimpleTextOutout.h"

typedef struct
{
    char _gap[60];
    Efi_Protocol_SimpleTextOutout *ConsoleOutput;
} Efi_SyetemTable;

#endif