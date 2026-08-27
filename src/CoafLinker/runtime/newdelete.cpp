// src/CoafLinker/runtime/newdelete.cpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
// AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

#include <cstddef>

static char heap[1024 * 1024];
static char *heapPtr = heap;

extern "C" void *malloc(unsigned long size)
{
    void *p = heapPtr;
    heapPtr += size;
    if (heapPtr > heap + sizeof(heap))
    {
        heapPtr = heap + sizeof(heap);
        return nullptr;
    }
    return p;
}

extern "C" void free(void *) {}

void *operator new(std::size_t size)
{
    return malloc(size);
}

void *operator new[](std::size_t size)
{
    return malloc(size);
}

void operator delete(void *p) noexcept
{
    free(p);
}

void operator delete[](void *p) noexcept
{
    free(p);
}