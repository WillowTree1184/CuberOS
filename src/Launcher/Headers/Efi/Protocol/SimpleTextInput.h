// src/Launcher/Headers/Efi/Protocol/SimpleTextInput.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_PROTOCOL_SIMPLE_TEXT_INPUT_H__
#define __COL_EFI_PROTOCOL_SIMPLE_TEXT_INPUT_H__

#include "../EfiMacros.h"

typedef struct Efi_InputKey
{
    uint16 ScanCode;
    uint16 UnicodeChar;
} Efi_InputKey;

typedef struct Efi_Protocol_SimpleTextInput
{
    Efi_Status _buf;
    Efi_Status(efiapi *ReadKeyStroke)(
        struct Efi_Protocol_SimpleTextInput *current,
        Efi_InputKey *key);
} Efi_Protocol_SimpleTextInput;

Efi_Status Efi_TryGet(Efi_Protocol_SimpleTextInput *protocol, uint16 output[3])
{
    Efi_InputKey key;

    Efi_Status status;
    status = protocol->ReadKeyStroke(protocol, &key);
    if (EFI_IsError(status))
    {
        return status;
    }

    if (!key.UnicodeChar)
    {
        return Efi_Error_Unsupported;
    }
    else if (key.UnicodeChar == L'\r')
    {
        output[0] = L'\r';
        output[1] = L'\n';
        output[2] = L'\0';
    }
    else
    {
        output[0] = key.UnicodeChar;
        output[1] = L'\0';
    }

    return Efi_Success;
}

Efi_Status Efi_Get(Efi_Protocol_SimpleTextInput *protocol, uint16 output[3])
{
    while (EFI_IsError(Efi_TryGet(protocol, output)))
        ;
    return Efi_Success;
}

#endif