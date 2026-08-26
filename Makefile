# Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

# Target arch
# Supported: x86_64, aarch64
ARCH		?= x86_64
VAILD_ARCHS	:= x86_64 aarch64
ifneq ($(filter $(ARCH), $(VAILD_ARCHS)), $(ARCH))
$(error Unsupported architecture: $(ARCH). Supported: $(VAILD_ARCHS))
endif
export ARCH

# Global directories
BUILD_DIR			:= $(CURDIR)/Build
export BUILD_ROOT	:= $(BUILD_DIR)/$(ARCH)
export PROJECT_SRC	:= $(CURDIR)/src

# Global tool chain
export CC		:= clang
export CC_OUT	:= -o 
export CFLAGS	:= -ffreestanding -fshort-wchar -mno-red-zone -fno-stack-protector -fno-builtin -Wall -Wextra -O2
export LD		:= lld-link
export LD_OUT	:= -out:

# QEMU
QEMU		?= qemu-system-$(ARCH)
ifeq ($(ARCH), x86_64)
	OVMF_CODE	?= $(firstword $(foreach f,\
		/usr/share/OVMF/OVMF_CODE_4M.fd \
		/usr/share/edk2-ovmf/x64/OVMF_CODE_4M.fd \
		/usr/share/OVMF/OVMF_CODE.fd \
		/usr/share/edk2-ovmf/x64/OVMF_CODE.fd,$(wildcard $f)))
	OVMF_VARS	?= $(firstword $(foreach f,\
		/usr/share/OVMF/OVMF_VARS_4M.fd \
		/usr/share/edk2-ovmf/x64/OVMF_VARS_4M.fd \
		/usr/share/OVMF/OVMF_VARS.fd \
		/usr/share/edk2-ovmf/x64/OVMF_VARS.fd,$(wildcard $f)))
else ifeq ($(ARCH), aarch64)
	AAVMF_CODE := $(firstword $(foreach f,\
        /usr/share/AAVMF/AAVMF_CODE_4M.fd \
        /usr/share/AAVMF/AAVMF_CODE.fd,$(wildcard $f)))
    AAVMF_VARS := $(firstword $(foreach f,\
        /usr/share/AAVMF/AAVMF_VARS_4M.fd \
        /usr/share/AAVMF/AAVMF_VARS.fd,$(wildcard $f)))
endif

# Modules
MODULES	:= $(notdir $(patsubst %/Makefile,%,$(wildcard $(PROJECT_SRC)/*/Makefile)))

.PHONY: all build $(MODULES)

all: build run clean

# Build module
build: $(MODULES)
$(MODULES):
	@$(MAKE) -C "$(PROJECT_SRC)/$@" build SRC="$(PROJECT_SRC)/$@"

# Run
run: | $(BUILD_ROOT)
ifeq ($(ARCH), x86_64)
	$(QEMU) \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-drive format=raw,file=fat:rw:$(BUILD_ROOT) \
		-m 512M -smp 2 -vga std
else ifeq ($(ARCH), aarch64)
	@$(QEMU) \
		-drive if=pflash,format=raw,readonly=on,file=$(AAVMF_CODE) \
		-drive if=pflash,format=raw,file=$(AAVMF_VARS) \
		-drive format=raw,file=fat:rw:$(BUILD_ROOT) \
		-m 512M -smp 2 -vga std
endif

# clean
clean:
	@$(foreach i, $(MODULES), $(MAKE) -C "$(PROJECT_SRC)/$i" clean SRC="$(PROJECT_SRC)/$@")
	@rm -rf "$(BUILD_DIR)"