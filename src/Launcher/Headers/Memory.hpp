#pragma once

#include <Efi/Efi.hpp>

namespace efi
{
    void *Allocate(uintn size, MemoryType type = MemoryType::LoaderData);
    void Free(void *buffer);
}

struct nothrow_t
{
};

inline constexpr nothrow_t nothrow;
using size_t = unsigned long long;

void *operator new(size_t size);
void *operator new[](size_t size);
void *operator new(size_t size, nothrow_t const &) noexcept;
void *operator new[](size_t size, nothrow_t const &) noexcept;

void operator delete(void *buffer) noexcept;
void operator delete[](void *buffer) noexcept;
void operator delete(void *buffer, size_t) noexcept;
void operator delete[](void *buffer, size_t) noexcept;