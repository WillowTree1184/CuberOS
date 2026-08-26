// src/Launcher/Headers/Efi/EfiMacros.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_MACROS_H__
#define __COL_EFI_MACROS_H__

#if defined(__clang__) || defined(__GNUC__)

// [WillowTree1184] Some macros semantically a keyword, so use lowercase specifically.

#define efiapi __attribute__((ms_abi))
#define compact __attribute__((packed)) // [WillowTree1184] The name "compact" is just personal preference.
#define noreturn __attribute__((noreturn))
#define unused __attribute__((unused))
#define noinline __attribute__((noinline))
#define aligned(x) __attribute__((aligned(x)))
#define weak __attribute__((weak))

#ifndef in
#define in
#endif

#ifndef out
#define out
#endif

#ifndef optional
#define optional
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

#if defined(__x86_64__) || defined(__aarch64__) || defined(__riscv) || defined(__loongarch64)
typedef unsigned long long uintn;
typedef signed long long intn;
#define EFI_ERROR(a) ((Efi_Status)(0x8000000000000000 | (a)))
#elif defined(__i386__) || defined(__arm__) || defined(__riscv32)
typedef unsigned int uintn;
typedef signed int intn;
#define EFI_ERROR(a) ((Efi_Status)(0x80000000 | (a)))
#else
#error "Unsupported architecture."
#endif

typedef unsigned char bool;

#ifndef true
#define true ((bool)1)
#endif

#ifndef false
#define false ((bool)0)
#endif

typedef char char8;
typedef uint16 char16;

typedef uintn Efi_Status;

#define Efi_Success ((Efi_Status)0x0000000000000000)
#define Efi_Error_LoadError EFI_ERROR(1)
#define Efi_Error_InvalidParameter EFI_ERROR(2)
#define Efi_Error_Unsupported EFI_ERROR(3)
#define Efi_Error_BadBufferSize EFI_ERROR(4)
#define Efi_Error_BufferTooSmall EFI_ERROR(5)
#define Efi_Error_NotReady EFI_ERROR(6)
#define Efi_Error_DeviceError EFI_ERROR(7)
#define Efi_Error_WriteProtected EFI_ERROR(8)
#define Efi_Error_OutOfResources EFI_ERROR(9)
#define Efi_Error_VolumeCorrupted EFI_ERROR(10)
#define Efi_Error_VolumeFull EFI_ERROR(11)
#define Efi_Error_NoMedia EFI_ERROR(12)
#define Efi_Error_MediaChanged EFI_ERROR(13)
#define Efi_Error_NotFound EFI_ERROR(14)
#define Efi_Error_AccessDenied EFI_ERROR(15)
#define Efi_Error_NoResponse EFI_ERROR(16)
#define Efi_Error_NoMapping EFI_ERROR(17)
#define Efi_Error_Timeout EFI_ERROR(18)
#define Efi_Error_NotStarted EFI_ERROR(19)
#define Efi_Error_AlreadyStarted EFI_ERROR(20)
#define Efi_Error_Aborted EFI_ERROR(21)
#define Efi_Error_IcmpError EFI_ERROR(22)
#define Efi_Error_TftpError EFI_ERROR(23)
#define Efi_Error_ProtocolError EFI_ERROR(24)
#define Efi_Error_IncompatibleVersion EFI_ERROR(25)
#define Efi_Error_SecurityViolation EFI_ERROR(26)
#define Efi_Error_CrcError EFI_ERROR(27)
#define Efi_Error_EndOfMedia EFI_ERROR(28)
#define Efi_Error_EndOfFile EFI_ERROR(31)
#define Efi_Error_InvalidLanguage EFI_ERROR(32)
#define Efi_Error_CompromisedData EFI_ERROR(33)
#define Efi_Error_IpAddressConflict EFI_ERROR(34)
#define Efi_Error_HttpError EFI_ERROR(35)

#define Efi_Warn_UnknownGlyph ((Efi_Status)1)
#define Efi_Warn_DeleteFailure ((Efi_Status)2)
#define Efi_Warn_WriteFailure ((Efi_Status)3)
#define Efi_Warn_BufferTooSmall ((Efi_Status)4)
#define Efi_Warn_StaleData ((Efi_Status)5)
#define Efi_Warn_FileSystem ((Efi_Status)6)
#define Efi_Warn_ResetRequired ((Efi_Status)7)

#define EFI_IsError(Status) (((intn)(Status)) < 0)

typedef void *Efi_Handle;
typedef void *Efi_Event;
typedef uint64 Efi_LBA;
typedef uintn Efi_TPL;

typedef struct
{
    uint32 Data1;
    uint16 Data2;
    uint16 Data3;
    uint8 Data4[8];
} compact Efi_Guid;

#ifndef null
#define null ((void *)0)
#endif

#else
#error "Unsupported complier"
#endif

#endif