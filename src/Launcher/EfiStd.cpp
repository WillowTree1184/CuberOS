#include <EfiStd.hpp>
#include <Memory.hpp>

efi::SystemTable *gSystemTable = nullptr;
efi::BootServices *gBootServices = nullptr;
efi::Handle gImageHandle = nullptr;

efi::char16 *launcherTitle = efi::ToU16(L"CuberOS Launcher Beta v1.0");

namespace efi
{
    // Types
    char16 *ToU16(const wchar_t *string) noexcept
    {
        return reinterpret_cast<uint16 *>(const_cast<wchar_t *>(string));
    }

    // String
    uintn LengthOf(char16 *string)
    {
        uintn length = 0;
        while (string[length] != L'\0')
        {
            length++;
        }
        return length;
    }

    uintn WidthOf(char16 *string)
    {
        uintn maxWidth = 0;
        uintn currentWidth = 0;
        for (uintn i = 0; string[i] != L'\0'; i++)
        {
            if (string[i] == L'\r' || string[i] == L'\n')
            {
                if (currentWidth > maxWidth)
                {
                    maxWidth = currentWidth;
                }

                currentWidth = 0;
            }
            else
            {
                currentWidth++;
            }
        }
        if (currentWidth > maxWidth)
        {
            maxWidth = currentWidth;
        }
        return maxWidth;
    }

    uintn HeightOf(char16 *string)
    {
        uintn height = 1;
        for (uintn i = 0; string[i] != L'\0'; i++)
        {
            if (string[i] == L'\n')
            {
                height++;
            }
        }
        return height;
    }

    // Input
    Status TryGet(char16 output[3])
    {
        InputKey key;

        Status status;
        status = gSystemTable->ConsoleInput->ReadKeyStroke(gSystemTable->ConsoleInput, &key);
        if (IsError(status))
        {
            return status;
        }

        if (!key.UnicodeChar)
        {
            return error::Unsupported;
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

        return Success;
    }

    Status Get(char16 output[3])
    {
        while (IsError(TryGet(output)))
            ;
        return Success;
    }

    Status WaitAnyKey()
    {
        uint16 output[3];
        while (IsError(TryGet(output)))
            ;
        return Success;
    }

    // Output
    Status Print(char16 *target)
    {
        return gSystemTable->ConsoleOutput->OutputString(gSystemTable->ConsoleOutput, target);
    }

    Status PrintInCenter(char16 *target, uintn minWidth, long long heightOffset)
    {
        Status status;

        uintn columns, rows;
        status = gSystemTable->ConsoleOutput->QueryMode(gSystemTable->ConsoleOutput,
                                                        gSystemTable->ConsoleOutput->mode->Mode,
                                                        &columns, &rows);
        if (IsError(status))
        {
            return status;
        }

        uintn targetWidth = WidthOf(target);
        if (targetWidth < minWidth)
        {
            targetWidth = minWidth;
        }

        uintn diaplayColumn = targetWidth >= columns ? 0 : (columns - targetWidth) / 2;
        uintn displayRow = rows / 2 - heightOffset;
        gSystemTable->ConsoleOutput->SetCursorPosition(gSystemTable->ConsoleOutput, diaplayColumn, displayRow);

        return gSystemTable->ConsoleOutput->OutputString(gSystemTable->ConsoleOutput, target);
    }

    Status PrintMessage(char16 *message)
    {
        Status status;

        status = ClearScreen();
        if (IsError(status))
        {
            return status;
        }

        status = PrintInCenter(launcherTitle, 0, 0);
        if (IsError(status))
        {
            return status;
        }

        status = PrintInCenter(message, 0, -1);
        if (IsError(status))
        {
            return status;
        }

        return Success;
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

        protocol::SimpleFileSystem *simpleFileSystem;
        status = gSystemTable->BootServices->LocateProtocol(&guid::SimpleFileSystem, nullptr, (void **)&simpleFileSystem);
        if (IsError(status))
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
        if (IsError(status))
        {
            return status;
        }

        // Get size
        uintn infoSize = 0;
        status = file->GetInfo(file, &guid::FileInfo, &infoSize, nullptr);
        if (status != error::BufferTooSmall)
        {
            file->Close(file);
            return status;
        }

        auto *infoBuffer = new (nothrow) uint8[infoSize];
        if (infoBuffer == nullptr)
        {
            file->Close(file);
            return error::OutOfResources;
        }

        status = file->GetInfo(file, &guid::FileInfo, &infoSize, infoBuffer);
        if (IsError(status))
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
            return error::OutOfResources;
        }

        uintn readSize = actualSize;
        status = file->Read(file, &readSize, data);
        file->Close(file);

        if (IsError(status))
        {
            delete[] data;
            *buffer = nullptr;
            return status;
        }

        if (readSize != actualSize)
        {
            delete[] data;
            *buffer = nullptr;
            return error::DeviceError;
        }

        // Null terminate for UTF-16
        data[actualSize] = 0;
        data[actualSize + 1] = 0;

        *buffer = data;
        *fileSize = actualSize;

        return Success;
    }

    // Error Handling
    void noreturn efiapi ExitByError(Status exitStatus, uint16 *message)
    {

        PrintInCenter(launcherTitle, 0, 0);
        SetTextColor(TextColor::White, TextColor::LightRed);
        PrintInCenter(message, 0, -1);
        ResetTextColor();

        PrintInCenter(ToU16(L"You can Press: [Q] to shutdown, [R] to reboot, [ESC] to Firmware UI, [Enter] to fall-through next."), 0, -2);
        while (true)
        {
            InputKey key;
            gSystemTable->ConsoleInput->ReadKeyStroke(gSystemTable->ConsoleInput, &key);
            if (key.UnicodeChar == L'q' || key.UnicodeChar == L'Q')
            {
                gSystemTable->RuntimeServices->ResetSystem(ResetType::Shutdown, exitStatus, 0, nullptr);
            }
            else if (key.UnicodeChar == L'r' || key.UnicodeChar == L'R')
            {
                gSystemTable->RuntimeServices->ResetSystem(ResetType::Warm, exitStatus, 0, nullptr);
            }
            else if (key.ScanCode == KeyScanCode::Escape)
            {
                enum class OsIndication : uint64
                {
                    BootToFirmwareUI = 0x0000000000000001
                };
                OsIndication indications = OsIndication::BootToFirmwareUI;

                gSystemTable->RuntimeServices->SetVariable(
                    ToU16(L"OsIndications"),
                    &guid::GlobalVariableGuid,
                    VariableAttribute::NonVolatile | VariableAttribute::BootServiceAccess | VariableAttribute::RuntimeAccess, // 0x07, // NonVolatile | BootServiceAccess | RuntimeAccess
                    sizeof(indications),
                    &indications);

                gSystemTable->RuntimeServices->ResetSystem(ResetType::Warm, exitStatus, 0, nullptr);
            }
            else if (key.UnicodeChar == L'\r')
            {
                gSystemTable->BootServices->Exit(gImageHandle, exitStatus, 0, nullptr);
            }
        }
    }
}