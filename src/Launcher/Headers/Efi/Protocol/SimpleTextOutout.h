// src/Launcher/Headers/Efi/Protocol/SimpleTextOutout.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_PROTOCOL_SIMPLE_TEXT_OUTPUT_H__
#define __COL_EFI_PROTOCOL_SIMPLE_TEXT_OUTPUT_H__

#include "../EfiMacros.h"

/* 如果你还没有这个结构体，它需要作为 Protocol 的最后一个字段 */
typedef struct Efi_SimpleTextOutputMode
{
    int32 MaxMode;
    int32 Mode;
    int32 Attribute;
    int32 CursorColumn;
    int32 CursorRow;
    bool CursorVisible;
} Efi_SimpleTextOutputMode;

typedef struct Efi_Protocol_SimpleTextOutput
{
    Efi_Status(efiapi *Reset)(
        struct Efi_Protocol_SimpleTextOutput *current,
        bool extended_verification);

    Efi_Status(efiapi *OutputString)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uint16 *target);

    Efi_Status(efiapi *TestString)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uint16 *string);

    Efi_Status(efiapi *QueryMode)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uintn mode_number,
        uintn *columns,
        uintn *rows);

    Efi_Status(efiapi *SetMode)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uintn mode_number);

    Efi_Status(efiapi *SetAttribute)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uintn attribute);

    Efi_Status(efiapi *ClearScreen)(
        struct Efi_Protocol_SimpleTextOutput *current);

    Efi_Status(efiapi *SetCursorPosition)(
        struct Efi_Protocol_SimpleTextOutput *current,
        uintn column,
        uintn row);

    Efi_Status(efiapi *EnableCursor)(
        struct Efi_Protocol_SimpleTextOutput *current,
        bool visible);

    Efi_SimpleTextOutputMode *mode;
} Efi_Protocol_SimpleTextOutput;

Efi_Status Efi_Print(Efi_Protocol_SimpleTextOutput *protocol, uint16 *target)
{
    return protocol->OutputString(protocol, target);
}

Efi_Status Efi_ClearScreen(Efi_Protocol_SimpleTextOutput *protocol)
{
    return protocol->ClearScreen(protocol);
}

#endif