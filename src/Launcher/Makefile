# src/Launcher/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

ARCH		?= x86_64
BUILD_ROOT	?= $(CURDIR)/../../build/$(ARCH)

INC_ROOT	:= $(CURDIR)/Headers

L_CXX			:= clang
L_CXXFLAGS		:= -ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -mno-red-zone -fshort-wchar -fno-stack-protector -fno-builtin -Wall -Wextra -O2 -std=c++20 -I $(GLOBAL_HEADERS) -target $(ARCH)-pc-win32-coff -I $(INC_ROOT)

LD		:= lld-link
LDFLAGS	:= -subsystem:efi_application -entry:Main -nodefaultlib -dll

ifeq ($(ARCH),x86_64)
    TARGET_EFI	:= BootX64.efi
    TARGET_LIB	:= BootX64.lib
else ifeq ($(ARCH),aarch64)
    TARGET_EFI	:= BootAA64.efi
    TARGET_LIB	:= BootAA64.lib
endif

.PHONY: all build clean

all: build clean

# Build
build: Build/$(TARGET_EFI)
	@mkdir -p "$(BUILD_ROOT)/Efi/Boot/"
	@cp -r Build/* "$(BUILD_ROOT)/Efi/Boot/"

Build/$(TARGET_EFI): Bin/Launcher.o
	@mkdir -p Build
	$(LD) $(LDFLAGS) "$<" -out:"$@"

Bin/%.o: %.cpp
	@mkdir -p Bin
	$(L_CXX) $(L_CXXFLAGS) -c "$<" -o "$@"

# Clean
clean:
	@rm -rf Bin Build