// src/Headers/Efi/Types.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#pragma once

#include <Macros.hpp>

#if !defined(__clang__) && !defined(__GNUC__)
#error "Unsupported compiler"
#endif

#define efiapi __attribute__((ms_abi))

// #ifndef in
// #define in
// #endif
// #ifndef out
// #define out
// #endif
// #ifndef optional
// #define optional
// #endif

namespace efi
{

    using uint8 = unsigned char;
    using uint16 = unsigned short;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    using int8 = signed char;
    using int16 = signed short;
    using int32 = signed int;
    using int64 = signed long long;

#if defined(__x86_64__) || defined(__aarch64__) || defined(__riscv) || defined(__loongarch64)
    using uintn = unsigned long long;
    using intn = signed long long;
#else
#error "Unsupported architecture."
#endif

    using char8 = char;
    using char16 = uint16;

    using Status = uintn;
    using Handle = void *;
    using Event = void *;
    using LBA = uint64;
    using TPL = uintn;

    constexpr Status MakeError(uintn code) noexcept
    {
        return static_cast<Status>((static_cast<uintn>(1) << (sizeof(uintn) * 8 - 1)) | code);
    }

    constexpr Status Success = static_cast<Status>(0);

    namespace error
    {
        constexpr Status LoadError = MakeError(1);
        constexpr Status InvalidParameter = MakeError(2);
        constexpr Status Unsupported = MakeError(3);
        constexpr Status BadBufferSize = MakeError(4);
        constexpr Status BufferTooSmall = MakeError(5);
        constexpr Status NotReady = MakeError(6);
        constexpr Status DeviceError = MakeError(7);
        constexpr Status WriteProtected = MakeError(8);
        constexpr Status OutOfResources = MakeError(9);
        constexpr Status VolumeCorrupted = MakeError(10);
        constexpr Status VolumeFull = MakeError(11);
        constexpr Status NoMedia = MakeError(12);
        constexpr Status MediaChanged = MakeError(13);
        constexpr Status NotFound = MakeError(14);
        constexpr Status AccessDenied = MakeError(15);
        constexpr Status NoResponse = MakeError(16);
        constexpr Status NoMapping = MakeError(17);
        constexpr Status Timeout = MakeError(18);
        constexpr Status NotStarted = MakeError(19);
        constexpr Status AlreadyStarted = MakeError(20);
        constexpr Status Aborted = MakeError(21);
        constexpr Status IcmpError = MakeError(22);
        constexpr Status TftpError = MakeError(23);
        constexpr Status ProtocolError = MakeError(24);
        constexpr Status IncompatibleVersion = MakeError(25);
        constexpr Status SecurityViolation = MakeError(26);
        constexpr Status CrcError = MakeError(27);
        constexpr Status EndOfMedia = MakeError(28);
        constexpr Status EndOfFile = MakeError(31);
        constexpr Status InvalidLanguage = MakeError(32);
        constexpr Status CompromisedData = MakeError(33);
        constexpr Status IpAddressConflict = MakeError(34);
        constexpr Status HttpError = MakeError(35);
    }

    namespace warning
    {
        constexpr Status UnknownGlyph = static_cast<Status>(1);
        constexpr Status DeleteFailure = static_cast<Status>(2);
        constexpr Status WriteFailure = static_cast<Status>(3);
        constexpr Status BufferTooSmall = static_cast<Status>(4);
        constexpr Status StaleData = static_cast<Status>(5);
        constexpr Status FileSystem = static_cast<Status>(6);
        constexpr Status ResetRequired = static_cast<Status>(7);
    }

    constexpr bool IsError(Status s) noexcept
    {
        return static_cast<intn>(s) < 0;
    }

    inline uint16 *ToU16(const wchar_t *s) noexcept
    {
        return reinterpret_cast<uint16 *>(const_cast<wchar_t *>(s));
    }

    inline uint16 ToC16(wchar_t c) noexcept
    {
        return static_cast<uint16>(c);
    }

    struct compact Guid
    {
        uint32 Data1;
        uint16 Data2;
        uint16 Data3;
        uint8 Data4[8];
    };

    struct Time
    {
        uint16 Year;
        uint8 Month;
        uint8 Day;
        uint8 Hour;
        uint8 Minute;
        uint8 Second;
        uint8 Pad1;
        uint32 Nanosecond;
        int16 TimeZone;
        uint8 Daylight;
        uint8 Pad2;
    };
} // namespace coaf