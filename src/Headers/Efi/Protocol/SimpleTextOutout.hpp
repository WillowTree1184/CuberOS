// src/Headers/Efi/Protocol/SimpleTextOutout.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include "../Types.hpp"

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

    enum class TextColor : uint8
    {
        Black = 0x00,
        Blue = 0x01,
        Green = 0x02,
        Cyan = 0x03,
        Red = 0x04,
        Magenta = 0x05,
        Brown = 0x06,
        LightGray = 0x07,
        DarkGray = 0x08,
        LightBlue = 0x09,
        LightGreen = 0x0A,
        LightCyan = 0x0B,
        LightRed = 0x0C,
        LightMagenta = 0x0D,
        Yellow = 0x0E,
        White = 0x0F
    };

    constexpr uintn MakeTextOutputAttribute(TextColor foreground, TextColor background) noexcept
    {
        return (static_cast<uintn>(background) << 4) | static_cast<uintn>(foreground);
    }

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
    } // namespace coaf::protocol
} // namespace coaf