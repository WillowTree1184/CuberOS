#pragma once

#include <Efi/Efi.hpp>

extern efi::SystemTable *gSystemTable;
extern efi::BootServices *gBootServices;
extern efi::Handle gImageHandle;

extern efi::char16 *launcherTitle;

namespace efi
{
    // Types
    char16 *ToU16(const wchar_t *string) noexcept;

    // String
    uintn LengthOf(char16 *string);
    uintn WidthOf(char16 *string);
    uintn HeightOf(char16 *string);

    // Input
    Status TryGet(char16 output[3]);
    Status Get(char16 output[3]);
    Status WaitAnyKey();

    // Output
    Status Print(char16 *target);
    inline Status Print(const wchar_t *target)
    {
        return Print(ToU16(target));
    }
    Status PrintInCenter(char16 *target, uintn minWidth = 0, long long heightOffset = 0);
    inline Status PrintInCenter(const wchar_t *target, uintn minWidth = 0, long long heightOffset = 0)
    {
        return PrintInCenter(ToU16(target), minWidth, heightOffset);
    }
    Status PrintMessage(char16 *message);
    inline Status PrintMessage(const wchar_t *message)
    {
        return PrintMessage(ToU16(message));
    }
    Status ClearScreen();
    Status SetTextColor(TextColor foreground, TextColor background);
    Status ResetTextColor();

    // File System
    Status OpenVolume(protocol::File **root);
    Status ReadFile(protocol::File *root, char16 *fileName, void **buffer, uintn *fileSize);

    // Error handling
    void noreturn efiapi ExitByError(Status exitStatus, char16 *message);
}