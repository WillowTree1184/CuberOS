# src/Run/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

OVMF_CODE	:= /usr/share/OVMF/OVMF_CODE_4M.fd
OVMF_VARS	:= /usr/share/OVMF/OVMF_VARS_4M.fd

SYSFS_DIR	?= $(CURDIR)/../System
QEMU		?= qemu-system-x86_64
QEMU_FLAGS	?=-m 512M -smp 2 -vga std

.PHONY: all
all:
	@$(QEMU) \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-drive format=raw,file=fat:rw:$(SYSFS_DIR) \
		$(QEMU_FLAGS)