#include <EfiStd.hpp>
#include <Memory.hpp>

efi::SystemTable *gSystemTable = nullptr;
efi::BootServices *gBootServices = nullptr;
efi::Handle gImageHandle = nullptr;

namespace efi
{
    // Input
    Status TryGet(uint16 output[3])
    {
        InputKey key;

        Status status;
        status = gSystemTable->ConsoleInput->ReadKeyStroke(gSystemTable->ConsoleInput, &key);
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

    Status Get(uint16 output[3])
    {
        while (efi::IsError(TryGet(output)))
            ;
        return efi::Success;
    }

    Status WaitAnyKey()
    {
        uint16 output[3];
        while (efi::IsError(TryGet(output)))
            ;
        return efi::Success;
    }

    // Output
    Status Print(uint16 *target)
    {
        return gSystemTable->ConsoleOutput->OutputString(gSystemTable->ConsoleOutput, target);
    }

    Status ClearScreen()
    {
        return gSystemTable->ConsoleOutput->ClearScreen(gSystemTable->ConsoleOutput);
    }

    Status SetTextColor(TextColor foreground, TextColor background)
    {
        return gSystemTable->ConsoleOutput->SetAttribute(gSystemTable->ConsoleOutput, MakeTextOutputAttribute(foreground, background));
    }

    Status ResetTextColor()
    {
        return gSystemTable->ConsoleOutput->SetAttribute(gSystemTable->ConsoleOutput, MakeTextOutputAttribute(TextColor::White, TextColor::Black));
    }

    // File System
    Status OpenVolume(protocol::File **root)
    {
        Status status;

        efi::protocol::SimpleFileSystem *simpleFileSystem;
        status = gSystemTable->BootServices->LocateProtocol(&efi::guid::SimpleFileSystem, nullptr, (void **)&simpleFileSystem);
        if (efi::IsError(status))
        {
            Print(ToU16(L"LOCATE ERROR\r\n"));
            return status;
        }

        return simpleFileSystem->OpenVolume(simpleFileSystem, root);
    }

    Status ReadFile(protocol::File *root, char16 *fileName, void **buffer, uintn *fileSize)
    {
        Status status;
        protocol::File *file = nullptr;

        // Open
        status = root->Open(root, &file, fileName, FileMode::Read, FileAttribute::None);
        if (efi::IsError(status))
        {
            return status;
        }

        // Get size
        uintn infoSize = 0;
        status = file->GetInfo(file, &efi::guid::FileInfo, &infoSize, nullptr);
        if (status != efi::error::BufferTooSmall)
        {
            file->Close(file);
            return status;
        }

        auto *infoBuffer = new (nothrow) uint8[infoSize];
        if (infoBuffer == nullptr)
        {
            file->Close(file);
            return efi::error::OutOfResources;
        }

        status = file->GetInfo(file, &efi::guid::FileInfo, &infoSize, infoBuffer);
        if (efi::IsError(status))
        {
            delete[] infoBuffer;
            file->Close(file);
            return status;
        }

        uintn actualSize = static_cast<uintn>(reinterpret_cast<FileInfo *>(infoBuffer)->FileSize);
        delete[] infoBuffer;

        // Read
        auto *data = new (nothrow) uint8[actualSize + sizeof(char16)];
        if (data == nullptr)
        {
            file->Close(file);
            return efi::error::OutOfResources;
        }

        uintn readSize = actualSize;
        status = file->Read(file, &readSize, data);
        file->Close(file);

        if (efi::IsError(status))
        {
            delete[] data;
            *buffer = nullptr;
            return status;
        }

        if (readSize != actualSize)
        {
            delete[] data;
            *buffer = nullptr;
            return efi::error::DeviceError;
        }

        // Null terminate for UTF-16
        data[actualSize] = 0;
        data[actualSize + 1] = 0;

        *buffer = data;
        *fileSize = actualSize;

        return efi::Success;
    }

    // Error Handling
    void noreturn efiapi ExitByError(efi::Status exitStatus, efi::uint16 *message)
    {
        efi::SetTextColor(efi::TextColor::White, efi::TextColor::LightRed);

        efi::Print(L"\r\nError: ");
        efi::Print(message);

        efi::ResetTextColor();

        efi::Print(efi::ToU16(L"\r\nYou can Press:\r\n- [Q] to shutdown,\r\n- [R] to reboot,\r\n- [ESC] to reboot to Firmware UI,\r\n- [Enter] to exit Launcher.\r\n"));
        while (true)
        {
            efi::InputKey key;
            gSystemTable->ConsoleInput->ReadKeyStroke(gSystemTable->ConsoleInput, &key);
            if (key.UnicodeChar == L'q' || key.UnicodeChar == L'Q')
            {
                gSystemTable->RuntimeServices->ResetSystem(efi::ResetType::Shutdown, exitStatus, 0, nullptr);
            }
            else if (key.UnicodeChar == L'r' || key.UnicodeChar == L'R')
            {
                gSystemTable->RuntimeServices->ResetSystem(efi::ResetType::Warm, exitStatus, 0, nullptr);
            }
            else if (key.ScanCode == efi::KeyScanCode::Escape)
            {
                enum class OsIndication : efi::uint64
                {
                    BootToFirmwareUI = 0x0000000000000001
                };
                OsIndication indications = OsIndication::BootToFirmwareUI;

                gSystemTable->RuntimeServices->SetVariable(
                    efi::ToU16(L"OsIndications"),
                    &efi::guid::GlobalVariableGuid,
                    efi::VariableAttribute::NonVolatile | efi::VariableAttribute::BootServiceAccess | efi::VariableAttribute::RuntimeAccess, // 0x07, // NonVolatile | BootServiceAccess | RuntimeAccess
                    sizeof(indications),
                    &indications);

                gSystemTable->RuntimeServices->ResetSystem(efi::ResetType::Warm, exitStatus, 0, nullptr);
            }
            else if (key.UnicodeChar == L'\r')
            {
                gSystemTable->BootServices->Exit(gImageHandle, exitStatus, 0, nullptr);
            }
        }
    }
}