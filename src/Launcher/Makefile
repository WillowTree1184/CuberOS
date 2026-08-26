# src/Launcher/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

ARCH		?= x86_64
BUILD_ROOT	?= $(CURDIR)/../../build/$(ARCH)

SRC			?= $(CURDIR)
INC_ROOT	:= $(SRC)/Headers

CFLAGS		+= -target $(ARCH)-pc-win32-coff -I $(INC_ROOT)
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
build: $(TARGET_EFI)
	@mkdir -p "$(BUILD_ROOT)/Efi/Boot/"
	@cp "$(TARGET_EFI)" "$(BUILD_ROOT)/Efi/Boot/$(TARGET_EFI)"
	@cp "$(TARGET_LIB)" "$(BUILD_ROOT)/Efi/Boot/$(TARGET_LIB)"

$(TARGET_EFI): Launcher.o
	@$(LD) $(LDFLAGS) "$<" $(LD_OUT)"$@"

%.o: %.c
	@$(CC) $(CFLAGS) -c "$<" $(CC_OUT)"$@"

# Clean
clean:
	@rm -f "Launcher.o" "$(TARGET_EFI)" "$(TARGET_LIB)"