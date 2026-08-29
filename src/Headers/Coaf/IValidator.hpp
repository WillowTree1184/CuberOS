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
    public:
        virtual bool Validate()
        {
            return false;
        }

        virtual ~IValidator() = default;
    };
} // namespace coaf