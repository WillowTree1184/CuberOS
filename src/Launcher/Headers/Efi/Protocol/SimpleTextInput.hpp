// src/Launcher/Headers/Efi/Protocol/SimpleTextInput.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include "../Macros.hpp"

namespace efi
{
    enum class KeyScanCode : uint16
    {
        Null = 0x0000,
        Up = 0x0001,
        Down = 0x0002,
        Right = 0x0003,
        Left = 0x0004,
        Home = 0x0005,
        End = 0x0006,
        Insert = 0x0007,
        Delete = 0x0008,
        PageUp = 0x0009,
        PageDown = 0x000A,
        F1 = 0x000B,
        F2 = 0x000C,
        F3 = 0x000D,
        F4 = 0x000E,
        F5 = 0x000F,
        F6 = 0x0010,
        F7 = 0x0011,
        F8 = 0x0012,
        F9 = 0x0013,
        F10 = 0x0014,
        F11 = 0x0015,
        F12 = 0x0016,
        Escape = 0x0017,
        // 特殊输入设备
        Mute = 0x001C,
        VolumeUp = 0x001D,
        VolumeDown = 0x001E,
        BrightnessUp = 0x001F,
        BrightnessDown = 0x0020,
        Suspend = 0x0021,
        Hibernate = 0x0022,
        ToggleDisplay = 0x0023,
        Recovery = 0x0024,
        Eject = 0x0025,
        // 扫描码最大值
        Maximum = 0x0026
    };

    struct InputKey
    {
        KeyScanCode ScanCode;
        uint16 UnicodeChar;
    };

    namespace protocol
    {
        struct SimpleTextInput
        {
            uint64 Revision;
            Status(efiapi *ReadKeyStroke)(struct SimpleTextInput *current, InputKey *key);
        };
    }

    Status TryGet(protocol::SimpleTextInput *simpleTextInput, uint16 output[3])
    {
        InputKey key;

        Status status;
        status = simpleTextInput->ReadKeyStroke(simpleTextInput, &key);
        if (efi::IsError(status))
        {
            return status;
        }

        if (!key.UnicodeChar)
        {
            return efi::error::Unsupported;
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

        return efi::Success;
    }

    Status Get(protocol::SimpleTextInput *simpleTextInput, uint16 output[3])
    {
        while (efi::IsError(TryGet(simpleTextInput, output)))
            ;
        return efi::Success;
    }

    Status WaitAnyKey(protocol::SimpleTextInput *simpleTextInput)
    {
        uint16 output[3];
        while (efi::IsError(TryGet(simpleTextInput, output)))
            ;
        return efi::Success;
    }

}