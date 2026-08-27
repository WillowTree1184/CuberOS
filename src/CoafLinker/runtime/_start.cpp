// src/CoafLinker/runtime/_start.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

extern "C" int main();

typedef void (*InitFunc)();

extern "C" InitFunc __init_array_start[];
extern "C" InitFunc __init_array_end[];

extern "C" unsigned long long Main()
{
    for (InitFunc *p = __init_array_start; p < __init_array_end; ++p)
    {
        if (*p)
            (*p)();
    }
    return static_cast<unsigned long long>(main());
}