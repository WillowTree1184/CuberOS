// src/Launcher/Launcher.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#include <Launcher.hpp>
#include <EfiStd.hpp>
#include <Memory.hpp>
#include <Coaf/Coaf.hpp>

// 返回写入的 UTF-16 码元数量（不含终止符），自动识别 UTF-8 多字节序列
size_t Utf8ToU16(const char *utf8, efi::char16 *result, size_t outMax)
{
    size_t j = 0;
    while (*utf8 && j + 1 < outMax)
    {
        uint8_t c = static_cast<uint8_t>(*utf8);

        if (c < 0x80)
        {
            // 1-byte: 0xxxxxxx
            result[j++] = c;
            ++utf8;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte: 110xxxxx 10xxxxxx
            if (!utf8[1])
                break;
            uint16_t code = ((c & 0x1F) << 6) | (static_cast<uint8_t>(utf8[1]) & 0x3F);
            result[j++] = code;
            utf8 += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
            if (!utf8[1] || !utf8[2])
                break;
            uint16_t code = ((c & 0x0F) << 12) | ((static_cast<uint8_t>(utf8[1]) & 0x3F) << 6) | (static_cast<uint8_t>(utf8[2]) & 0x3F);
            result[j++] = code;
            utf8 += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx → surrogate pair
            if (!utf8[1] || !utf8[2] || !utf8[3])
                break;

            uint32_t cp = ((c & 0x07) << 18) | ((static_cast<uint8_t>(utf8[1]) & 0x3F) << 12) | ((static_cast<uint8_t>(utf8[2]) & 0x3F) << 6) | (static_cast<uint8_t>(utf8[3]) & 0x3F);

            cp -= 0x10000;
            if (j + 2 < outMax)
            {
                result[j++] = 0xD800 | ((cp >> 10) & 0x3FF); // high surrogate
                result[j++] = 0xDC00 | (cp & 0x3FF);         // low surrogate
            }
            else
            {
                result[j++] = L'?'; // 空间不够，降级
            }
            utf8 += 4;
        }
        else
        {
            // 非法起始字节
            result[j++] = L'?';
            ++utf8;
        }
    }
    result[j] = 0;
    return j;
}

efi::Status efiapi Main(efi::Handle imageHandle, efi::SystemTable *systemTable)
{
    gSystemTable = systemTable;
    gBootServices = systemTable->BootServices;
    gImageHandle = imageHandle;

    efi::Status status;

    // Clear Screen
    efi::ClearScreen();

    // Print data
    efi::Print(L"CuberOS Launcher Beta v1.0\r\n\r\n");
    efi::Print(L"Launching\r\n");

    // Open volume
    efi::protocol::File *root;
    status = efi::OpenVolume(&root);
    if (efi::IsError(status))
    {
        efi::ExitByError(status, efi::ToU16(L"Can NOT open volume"));
    }
    efi::Print(L"Success: Open volume.\r\n");

    // Read profile
    efi::uint16 *buffer;
    efi::uintn fileSize;
    status = efi::ReadFile(root, efi::ToU16(L"Preloader\\Preloader.app"), (void **)&buffer, &fileSize);
    if (efi::IsError(status))
    {
        efi::ExitByError(status, efi::ToU16(L"Can NOT load preloader"));
        return status;
    }
    efi::Print(L"Success: Read file\r\n");

    // Check Preloader
    char *fileOffset = reinterpret_cast<char *>(buffer);
    coaf::HeaderValidator headerValidator(fileOffset);
    if (!headerValidator.Validate())
    {
        efi::ExitByError(status, efi::ToU16(L"The CoafHeader of preloader is invaild."));
        return status;
    }
    coaf::CoafHeader *header = reinterpret_cast<coaf::CoafHeader *>(fileOffset);
    coaf::v1::MainTable *mainTable = reinterpret_cast<coaf::v1::MainTable *>(fileOffset + header->MainTableOffset);
    coaf::v1::ExportSymbol *exportSymbolTable = reinterpret_cast<coaf::v1::ExportSymbol *>(fileOffset + mainTable->ExportSymbolTableOffset);
    for (coaf::U64 i = 0; i < mainTable->ExportSymbolCount; i++)
    {
        const char *symbolName = reinterpret_cast<const char *>(fileOffset + exportSymbolTable[i].NameOffset);
        if (symbolName[0] == 'M' && symbolName[1] == 'a' && symbolName[2] == 'i' && symbolName[3] == 'n' && symbolName[4] == '\0')
        {
            efi::Print(L"Found!\r\n");
            void (*Intro)() = reinterpret_cast<void (*)()>(fileOffset + exportSymbolTable[i].ImageOffset);
            Intro();
        }
    }
    efi::Print(efi::ToU16(L"Exports End\r\n"));

    while (true)
    {
        efi::uint16 buffer[3];

        efi::Get(buffer);
        if (buffer[0] == efi::ToU16(L"q\r\n")[0])
        {
            return efi::Success;
        }
    }

    return efi::Success;
}