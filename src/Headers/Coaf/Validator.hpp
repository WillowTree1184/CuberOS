// src/Headers/Coaf/Validator.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on COAF Specification V1 (docs/CoafSpecification/V1.md). Independent implementation.

#pragma once

#include "Types.hpp"
#include "Structure.hpp"

namespace coaf
{
    class IValidator
    {
    protected:
        char *fileBegin = nullptr;

    public:
        IValidator(char *fileBegin)
            : fileBegin(fileBegin) {}

        virtual bool Validate() = 0;

        virtual ~IValidator() = default;
    };

    class HeaderValidator : public IValidator
    {
    protected:
        CoafHeader *header = nullptr;

    public:
        HeaderValidator(char *fileBegin)
            : IValidator(fileBegin),
              header(reinterpret_cast<CoafHeader *>(fileBegin)) {}

        bool ValidateHeader()
        {
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

        bool Validate() override
        {
            return ValidateHeader();
        }
    };
} // namespace coaf