#include <Booter.hpp>
#include <EfiStd.hpp>
#include <Coaf/Coaf.hpp>

efi::Status efiapi Boot(efi::Handle imageHandle unused, efi::SystemTable *systemTable unused, efi::protocol::File *root)
{
    efi::Status status;

    efi::uint16 *buffer;
    efi::uintn fileSize;
    status = efi::ReadFile(root, efi::ToU16(L"Preloader\\Preloader.app"), (void **)&buffer, &fileSize);
    if (efi::IsError(status))
    {
        efi::ExitByError(status, efi::ToU16(L"Can NOT load preloader"));
        return status;
    }

    // Check Preloader
    char *fileOffset = reinterpret_cast<char *>(buffer);
    coaf::HeaderValidator headerValidator(fileOffset, fileSize);
    if (!headerValidator.Validate())
    {
        efi::ExitByError(efi::error::Aborted, efi::ToU16(L"The CoafHeader of preloader is invaild."));
        return efi::error::Aborted;
    }
    // coaf::IValidator validator = headerValidator.GetSuitableValidator();
    coaf::v1::ImageValidator validator = coaf::v1::ImageValidator(fileOffset, fileSize);
    if (!validator.Validate())
    {
        efi::ExitByError(efi::error::Aborted, efi::ToU16(L"The Coaf format of preloader is invaild."));
        return efi::error::Aborted;
    }

    coaf::Header *header = reinterpret_cast<coaf::Header *>(fileOffset);
    coaf::v1::MainTable *mainTable = reinterpret_cast<coaf::v1::MainTable *>(fileOffset + header->MainTableOffset);
    coaf::v1::ExportSymbol *exportSymbolTable = reinterpret_cast<coaf::v1::ExportSymbol *>(fileOffset + mainTable->ExportSymbolTableOffset);
    for (coaf::U64 i = 0; i < mainTable->ExportSymbolCount; i++)
    {
        const char *symbolName = reinterpret_cast<const char *>(fileOffset + exportSymbolTable[i].NameOffset);
        if (symbolName[0] == 'M' && symbolName[1] == 'a' && symbolName[2] == 'i' && symbolName[3] == 'n' && symbolName[4] == '\0')
        {
            efi::PrintMessage(L"Found!");
            void (*Intro)() = reinterpret_cast<void (*)()>(fileOffset + exportSymbolTable[i].ImageOffset);
            Intro();
        }
    }

    return efi::Success;
}