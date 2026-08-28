#include <Memory.hpp>
#include <EfiStd.hpp>

namespace efi
{
    void *Allocate(uintn size, MemoryType type)
    {
        void *buffer = nullptr;
        if (gBootServices != nullptr)
        {
            gBootServices->AllocatePool(type, size, (void **)&buffer);
        }
        return buffer;
    }

    void Free(void *buffer)
    {
        if (gBootServices != nullptr && buffer != nullptr)
        {
            gBootServices->FreePool(buffer);
        }
    }

} // namespace efi

void *operator new(size_t size)
{
    return efi::Allocate(size);
}

void *operator new[](size_t size)
{
    return efi::Allocate(size);
}

void *operator new(size_t size, nothrow_t const &) noexcept
{
    return efi::Allocate(size);
}

void *operator new[](size_t size, nothrow_t const &) noexcept
{
    return efi::Allocate(size);
}

void operator delete(void *buffer) noexcept
{
    efi::Free(buffer);
}

void operator delete[](void *buffer) noexcept
{
    efi::Free(buffer);
}

void operator delete(void *buffer, size_t) noexcept
{
    efi::Free(buffer);
}

void operator delete[](void *buffer, size_t) noexcept
{
    efi::Free(buffer);
}