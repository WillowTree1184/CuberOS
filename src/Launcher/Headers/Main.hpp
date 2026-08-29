// src/Launcher/Headers/Main.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#pragma once

#include <Efi/Efi.hpp>

efi::Status efiapi Main(efi::Handle imageHandle, efi::SystemTable *systemTable);