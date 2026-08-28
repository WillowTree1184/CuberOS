#pragma once

#include <Efi/Efi.hpp>

extern efi::SystemTable *gSystemTable;
extern efi::BootServices *gBootServices;
extern efi::Handle gImageHandle;

namespace efi
{
    // Input
    Status TryGet(uint16 output[3]);
    Status Get(uint16 output[3]);
    Status WaitAnyKey();

    // Output
    Status Print(uint16 *target);
    inline Status Print(const wchar_t *target)
    {
        return Print(ToU16(target));
    }
    Status ClearScreen();
    Status SetTextColor(TextColor foreground, TextColor background);
    Status ResetTextColor();

    // File System
    Status OpenVolume(protocol::File **root);
    Status ReadFile(protocol::File *root, char16 *fileName, void **buffer, uintn *fileSize);

    // Error handling
    void noreturn efiapi ExitByError(efi::Status exitStatus, efi::uint16 *message);
}