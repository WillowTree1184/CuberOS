// src/Headers/Macros.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

#pragma once

#define compact __attribute__((packed))
#define noreturn __attribute__((noreturn))
#define unused __attribute__((unused))
#define noinline __attribute__((noinline))
#define aligned(x) __attribute__((aligned(x)))
#define weak __attribute__((weak))