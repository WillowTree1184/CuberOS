#pragma once

#include "IValidator.hpp"
#include "V1/Validator.hpp"

namespace coaf
{
    class HeaderValidator : public IValidator
    {
    private:
        char *fileBegin = nullptr;
        U64 fileSize = 0;

        bool ValidateHeader()
        {
            Header *header = reinterpret_cast<Header *>(fileBegin);

            // Magic must be Magic::Image or Magic::Package
            if (header->Magic != Magic::Image && header->Magic != Magic::Package)
            {
                return false;
            }

            // Version must be 1
            if (header->Version < 1ULL || header->Version > 1ULL)
            {
                return false;
            }

            // MainTableOffset must be multiples of 8
            if (header->MainTableOffset % 8 != 0)
            {
                return false;
            }

            return true;
        }

    public:
        HeaderValidator(char *fileBegin, U64 fileSize)
            : fileBegin(fileBegin),
              fileSize(fileSize) {}

        bool Validate() override
        {
            return ValidateHeader();
        }

        IValidator GetSuitableValidator()
        {
            Header *header = reinterpret_cast<Header *>(fileBegin);
            if (header->Version == 1ULL)
            {
                if (header->Magic == Magic::Image)
                {
                    return v1::ImageValidator(fileBegin, fileSize);
                }
            }

            return *this;
        }
    };
}