// src/Launcher/Headers/Efi/Protocol/SimpleTextOutout.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_PROTOCOL_SIMPLE_TEXT_OUTPUT_H__
#define __COL_EFI_PROTOCOL_SIMPLE_TEXT_OUTPUT_H__

#include "../EfiApi.h"

typedef struct Efi_Protocol_SimpleTextOutout
{
    unsigned long long _gap;

    unsigned long long(EFIAPI *OutputString)(
        struct Efi_Protocol_SimpleTextOutout *current,
        unsigned short *target);

    unsigned long long _gap2[4];

    unsigned long long(EFIAPI *ClearScreen)(
        struct Efi_Protocol_SimpleTextOutout *current);
} Efi_Protocol_SimpleTextOutout;

#endif