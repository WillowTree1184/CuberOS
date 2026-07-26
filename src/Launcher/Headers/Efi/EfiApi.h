// src/Launcher/Headers/Efi/EfiApi.h

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on UEFI Specification 2.10. Independent implementation.

#ifndef __COL_EFI_EFIAPI_H__
#define __COL_EFI_EFIAPI_H__

#if defined(__clang__) || defined(__GNUC__)
#define EFIAPI __attribute__((ms_abi))
#else
#define EFIAPI
#endif

#endif