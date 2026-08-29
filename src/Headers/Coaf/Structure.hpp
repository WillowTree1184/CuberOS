// src/Headers/Coaf/Structure.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on COAF Specification V1 (docs/CoafSpecification/V1.md). Independent implementation.

#pragma once

#include "Types.hpp"

namespace coaf
{
    enum class Magic : U64
    {
        Image = 0x00474D4946414F43ULL,
        Package = 0x00474B5046414F43ULL
    };

    struct Header
    {
        Magic Magic;         /* 映像魔数 CoafMagicImage */
        U64 Version;         /* 见 6.1 */
        U64 MainTableOffset; /* FileOffset（见 4.1），指向 MainTable（见 6.2） */
    };
    static_assert(sizeof(Header) == 24);
} // namespace coaf