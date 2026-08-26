# src/Launcher/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

SRC_DIR ?= $(CURDIR)/..

MAKE_PLUGIN_DIR := $(SRC_DIR)/Tools/MakefilePlugin
include $(MAKE_PLUGIN_DIR)/rwildcard.mk

BUILDED_DIR_NAME	?= Builded
BUILDED_DIR 		:= $(CURDIR)/$(BUILDED_DIR_NAME)

CC			?= clang
CC_FLAGS	:= -target x86_64-pc-win32-coff -ffreestanding -fshort-wchar -mno-red-zone -fno-stack-protector -fno-builtin

LD			?= lld-link
LD_FLAGS	:= -subsystem:efi_application -entry:Main -nodefaultlib -dll

HEADERS_DIR		:= $(CURDIR)/Headers
HEADER_FILES	:= $(call rwildcard, $(HEADERS_DIR)/, *.h)

.PHONY: all
all: $(BUILDED_DIR) $(BUILDED_DIR)/BootX64.efi

.PHONY: $(BUILDED_DIR)
$(BUILDED_DIR):
	@mkdir -p $@

%.efi: Launcher.o
	@$(LD) $(LD_FLAGS) "$<" -out:"$@"

%.o: %.c $(HEADER_FILES)
	@$(CC) $(CC_FLAGS) -c "$<" -o "$@"