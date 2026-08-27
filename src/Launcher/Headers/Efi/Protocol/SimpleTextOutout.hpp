// src/Launcher/Headers/Efi/Protocol/SimpleTextOutout.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include "../Macros.hpp"

namespace efi
{
    struct SimpleTextOutputMode
    {
        int32 MaxMode;
        int32 Mode;
        int32 Attribute;
        int32 CursorColumn;
        int32 CursorRow;
        bool CursorVisible;
    };

    namespace protocol
    {
        struct SimpleTextOutput
        {
            Status(efiapi *Reset)(SimpleTextOutput *current, bool extendedVerification);
            Status(efiapi *OutputString)(SimpleTextOutput *current, uint16 *target);
            Status(efiapi *TestString)(SimpleTextOutput *current, uint16 *string);
            Status(efiapi *QueryMode)(SimpleTextOutput *current, uintn modeNumber, uintn *columns, uintn *rows);
            Status(efiapi *SetMode)(SimpleTextOutput *current, uintn modeNumber);
            Status(efiapi *SetAttribute)(SimpleTextOutput *current, uintn attribute);
            Status(efiapi *ClearScreen)(SimpleTextOutput *current);
            Status(efiapi *SetCursorPosition)(SimpleTextOutput *current, uintn column, uintn row);
            Status(efiapi *EnableCursor)(SimpleTextOutput *current, bool visible);

            SimpleTextOutputMode *mode;
        };
    }

    Status Print(protocol::SimpleTextOutput *simpleTextOutput, uint16 *target)
    {
        return simpleTextOutput->OutputString(simpleTextOutput, target);
    }

    Status ClearScreen(protocol::SimpleTextOutput *simpleTextOutput)
    {
        return simpleTextOutput->ClearScreen(simpleTextOutput);
    }
}